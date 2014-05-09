#include <clocale>
#include <string>
#include <thread>
#include <mutex>
#include <chrono>
#include <iostream>
#include <condition_variable>
#include <sstream>
#include <algorithm>
#include <cmath>

#include <QApplication>
#include <QLabel>
#include <QTextCodec>
#include <QWidget>
#include <QPainter>
#include <QEvent>

#include "ipc.h"
#include "fieldview.h"

void network_client(const std::string& server_host, uint16_t server_port, ab::ui::UiGameClientIpc& ipc);
void ui_ipc_listener(ab::ui::UiGameClientIpc& ipc);

GameFieldView::GameFieldView(ab::ui::UiGameClientIpc& ipc, QWidget *parent) : QWidget(parent), ipc(ipc) {}
GameFieldView::~GameFieldView() { }

void GameFieldView::paintEvent(QPaintEvent *event) {
  Q_UNUSED(event);
  QPainter qp(this);
  qp.setRenderHint(QPainter::Antialiasing, true);

  std::unique_lock<std::mutex> ipc_lock(ipc.mutex);


  double field_size = std::min(width() - 2, height() - 2);
  double field_dx = 1 + (width() - 2 - field_size) / 2;
  double field_dy = 1 + (height() - 2 - field_size) / 2;
  double field_to_px_coef = field_size / ipc.current_field.radius;
    
  QRectF r = QRectF(field_dx, field_dy, field_size, field_size);

  QBrush fieldBrush(QColor(200, 150, 150));

  qp.setBrush(fieldBrush);
  qp.drawEllipse(r);

  {
    std::ostringstream fmt_stream;
    fmt_stream << "state: ";
    switch (ipc.connection_state) {
    case ab::ui::ConnectionState::connecting: fmt_stream << "connecting"; break;
    case ab::ui::ConnectionState::connected: fmt_stream << "connected"; break;
    case ab::ui::ConnectionState::disconnected: fmt_stream << "disconnected"; break;
    default: break;
    }
    fmt_stream << ", tick: " << ipc.field_counter;
    qp.drawText(QRect(0, 0, width(), height()), QString(fmt_stream.str().c_str()));
  }

  QBrush playerBrush(QColor(120, 150, 200));
  QPen velocity_pen(QColor(50, 200, 60));
  velocity_pen.setWidth(5);
  qp.setBrush(playerBrush);
  for (const ab::Player& player: ipc.current_field.players) {
    QRectF playerRect = QRectF(field_dx + (player.center.x - player.radius) * field_to_px_coef,
                               field_dy + (player.center.y - player.radius) * field_to_px_coef,
                               2 * player.radius * field_to_px_coef,
                               2 * player.radius * field_to_px_coef);
    qp.drawEllipse(playerRect);
    qp.save();
    qp.setPen(velocity_pen);
    qp.drawLine(field_dx + player.center.x * field_to_px_coef,
                field_dy + player.center.y * field_to_px_coef,
                field_dx + (player.center.x + player.velocity.x) * field_to_px_coef,
                field_dy + (player.center.y + player.velocity.y) * field_to_px_coef);
    qp.restore();
    {
      std::ostringstream fmt_stream;
      fmt_stream << player.id;
      qp.drawText(playerRect, Qt::AlignCenter, QString(fmt_stream.str().c_str()));
    }
  }

  QBrush coinBrush(QColor(220, 240, 150));
  qp.setBrush(coinBrush);
  for (const ab::Coin& coin: ipc.current_field.coins) {
    QRectF coinRect = QRectF(field_dx + (coin.center.x - coin.radius) * field_to_px_coef,
                             field_dy + (coin.center.y - coin.radius) * field_to_px_coef,
                             2 * coin.radius * field_to_px_coef,
                             2 * coin.radius * field_to_px_coef);
    qp.drawEllipse(coinRect);
  }
}

void GameFieldView::updateFieldView() {
  //std::cerr << "got update" << std::endl;
  repaint();
}

int main(int argc, char * argv[]) {
  setlocale(LC_ALL, "");

  ab::ui::UiGameClientIpc ipc;

  QTextCodec::setCodecForCStrings(QTextCodec::codecForName("UTF-8"));
  QApplication app(argc, argv);
  GameFieldView fieldView(ipc);
  
  std::thread network_client_thread(network_client, "localhost", 1234, std::ref(ipc));
  std::thread ui_updater_thread([&]() {
      ab::ui::ConnectionState last_state = static_cast<ab::ui::ConnectionState>(-1);
      std::unique_lock<std::mutex> ipc_lock(ipc.mutex);
      while (true) {
        if (last_state != ipc.connection_state) {
          std::cerr << "state: " << ipc.connection_state << std::endl;
          last_state = ipc.connection_state;
        }
        QMetaObject::invokeMethod(&fieldView, "updateFieldView");
        if (ipc.connection_state == ab::ui::ConnectionState::disconnected) {
          return;
        }
        ipc.cv.wait(ipc_lock);
      }
    });

  fieldView.show();
  app.exec();

  ui_updater_thread.join();
  network_client_thread.join();
  return 0;
}
