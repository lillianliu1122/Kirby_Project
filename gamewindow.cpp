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

    loadStage1();
}

GameWindow::~GameWindow()
{
}

void GameWindow::gameLoop()
{
    // 之後這裡會放：更新物理、移動、碰撞判定等邏輯
    kirby.update(keysHeld);   // ← 每幀更新 Kirby
    checkCollisions();

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

    // 畫所有平台
    for (auto &p : platforms) {
        p.draw(painter, cameraX);
    }

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

void GameWindow::loadStage1()
{
    platforms.clear();

    // === Frame 1 (x: 0 ~ 1620) ===
    // 地板
    platforms.append(Platform(0,    900, 1300, 40, PlatformType::Floor));
    // 平台
    platforms.append(Platform(300,  750,  200, 40, PlatformType::Floor));
    platforms.append(Platform(700,  650,  200, 40, PlatformType::Floor));
    // 磚頭
    platforms.append(Platform(500,  700,   80, 40, PlatformType::Brick));

    // === Frame 2 (x: 1620 ~ 3240) ===
    // 地板
    platforms.append(Platform(1620, 900, 1620, 40, PlatformType::Floor));
    // 平台
    platforms.append(Platform(1800, 700,  200, 40, PlatformType::Floor));
    platforms.append(Platform(2200, 600,  200, 40, PlatformType::Floor));
    // 磚頭
    platforms.append(Platform(2000, 750,   80, 40, PlatformType::Brick));

    // === Frame 3 (x: 3240 ~ 4860) ===
    // 地板
    platforms.append(Platform(3240, 900, 1620, 40, PlatformType::Floor));
    // 平台
    platforms.append(Platform(3400, 700,  200, 40, PlatformType::Floor));
    platforms.append(Platform(3800, 600,  200, 40, PlatformType::Floor));
    // 磚頭
    platforms.append(Platform(3600, 750,   80, 40, PlatformType::Brick));
}

void GameWindow::checkCollisions()
{
    QRectF kirbyRect = kirby.getRect();
    kirby.onGround = false;

    for (auto &p : platforms) {
        if (!p.visible) continue;

        QRectF pRect = p.getRect();
        if (!kirbyRect.intersects(pRect)) continue;

        // 計算重疊量
        float overlapLeft   = kirbyRect.right()  - pRect.left();
        float overlapRight  = pRect.right()  - kirbyRect.left();
        float overlapTop    = kirbyRect.bottom() - pRect.top();
        float overlapBottom = pRect.bottom() - kirbyRect.top();

        // 找最小重疊方向
        float minOverlap = qMin(qMin(overlapLeft, overlapRight), qMin(overlapTop, overlapBottom));

        if (minOverlap == overlapTop && kirby.vy >= 0) {
            // 從上方落下，站在平台上
            kirby.y = pRect.top() - kirby.getRect().height();
            kirby.vy = 0;
            kirby.onGround = true;
        } else if (minOverlap == overlapBottom && kirby.vy < 0) {
            // 從下方撞到
            kirby.y = pRect.bottom();
            kirby.vy = 0;
        } else if (minOverlap == overlapLeft) {
            kirby.x = pRect.left() - kirby.getRect().width();
        } else if (minOverlap == overlapRight) {
            kirby.x = pRect.right();
        }

        // 更新碰撞框
        kirbyRect = kirby.getRect();
    }
}
