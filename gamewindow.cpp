#include "gamewindow.h"
#include <QPainter>

GameWindow::GameWindow(QWidget *parent)
    : QMainWindow(parent)
{
    // 設定視窗大小為 1620x1080
    setFixedSize(1620, 1080);
    setWindowTitle("Kirby's Adventure");

    // 建立遊戲主迴圈，每 16ms 觸發一次（約 60fps）
    timer = new QTimer(this);
    connect(timer, &QTimer::timeout, this, &GameWindow::gameLoop);
    timer->start(16);
}

GameWindow::~GameWindow()
{
}

void GameWindow::gameLoop()
{
    // 之後這裡會放：更新物理、移動、碰撞判定等邏輯
    kirby.update(keysHeld);   // ← 每幀更新 Kirby
    update();  // 呼叫 paintEvent 重新繪製畫面
}

void GameWindow::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event);
    QPainter painter(this);

    // 暫時畫黑色背景
    painter.fillRect(0, 0, width(), height(), Qt::black);

    // 畫 Kirby
    kirby.draw(painter);
}

void GameWindow::keyPressEvent(QKeyEvent *event)
{
    keysHeld.insert(event->key());
}

void GameWindow::keyReleaseEvent(QKeyEvent *event)
{
    keysHeld.remove(event->key());
}
