#pragma once

#include <QWidget>
#include <QEvent>

#include "ipc.hpp"

class GameFieldView : public QWidget
{
    Q_OBJECT

    ab::ui::UiGameClientIpc& ipc;

public:
    GameFieldView(ab::ui::UiGameClientIpc& ipc, QWidget *parent = nullptr);
    ~GameFieldView();

public slots:
    void updateFieldView();

protected:
    void paintEvent(QPaintEvent *event);
};
