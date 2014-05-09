#include <clocale>
#include <string>
#include <thread>
#include <mutex>
#include <chrono>
#include <iostream>
#include <condition_variable>
#include <sstream>

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

  QRectF r(0, 0, width(), height());
  std::ostringstream fmt_stream;
  fmt_stream << "state: ";
  switch (ipc.connection_state) {
  case ab::ui::ConnectionState::connecting: fmt_stream << "connecting"; break;
  case ab::ui::ConnectionState::connected: fmt_stream << "connected"; break;
  case ab::ui::ConnectionState::disconnected: fmt_stream << "disconnected"; break;
  default: break;
  }
  fmt_stream << ", tick: " << ipc.field_counter;
  qp.drawText(r, QString(fmt_stream.str().c_str()));
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
        ipc_lock.unlock();
        QMetaObject::invokeMethod(&fieldView, "updateFieldView");
        if (ipc.connection_state == ab::ui::ConnectionState::disconnected) {
          return;
        }
        ipc_lock.lock();
        ipc.cv.wait(ipc_lock);
      }
    });

  fieldView.show();
  app.exec();

  ui_updater_thread.join();
  network_client_thread.join();
  return 0;
}
