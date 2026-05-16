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

    cameraX = 0;
}

GameWindow::~GameWindow()
{
}

void GameWindow::gameLoop()
{
    // 之後這裡會放：更新物理、移動、碰撞判定等邏輯
    kirby.update(keysHeld);   // ← 每幀更新 Kirby

    const float MAP_WIDTH = 4860.0f;

    // 鏡頭始終讓 Kirby 保持在畫面中間
    cameraX = kirby.x - width() / 2.0f;

    // 鏡頭不超出地圖邊界
    if (cameraX < 0) cameraX = 0;
    if (cameraX > MAP_WIDTH - width()) cameraX = MAP_WIDTH - width();

    update();  // 呼叫 paintEvent 重新繪製畫面
}

void GameWindow::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event);
    QPainter painter(this);

    // 暫時畫黑色背景
    painter.fillRect(0, 0, width(), height(), Qt::black);

    // 把攝影機偏移傳給 Kirby 的 draw
    kirby.draw(painter, cameraX);

    // 暫時畫 frame 邊界線（之後換成真正地圖）
    painter.setPen(QPen(Qt::white, 2));
    for (int i = 1; i <= 3; i++) {
        int lineX = (int)(i * 1620 - cameraX);
        painter.drawLine(lineX, 0, lineX, height());
    }
}

void GameWindow::keyPressEvent(QKeyEvent *event)
{
    keysHeld.insert(event->key());
}

void GameWindow::keyReleaseEvent(QKeyEvent *event)
{
    keysHeld.remove(event->key());
}
