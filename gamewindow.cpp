#include "gamewindow.h"
#include <QPainter>
#include <QDebug>

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
    loadBackground();
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

    // 限制 Kirby 不超出 Stage 1 左右邊界
    if (kirby.x < 0) kirby.x = 0;
    if (kirby.x > 4860 - kirby.KIRBY_W) kirby.x = 4860 - kirby.KIRBY_W;

    update();  // 呼叫 paintEvent 重新繪製畫面
}

void GameWindow::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event);
    QPainter painter(this);

    // 1. 天空背景
    painter.fillRect(0, 0, width(), height(), QColor(135, 206, 235));

    // 2. 畫平台（會被草地蓋住）
    for (auto &p : platforms) {
        p.draw(painter, cameraX);
    }

    // 貼地形圖（貼在畫面底部，保持原始比例）
    for (int i = 0; i < 3; i++) {
        if (bgStage1[i].isNull()) continue;

        int drawX = (int)(i * 1620 - cameraX);
        if (drawX >= width() || drawX + 1620 <= 0) continue;

        // 寬度固定 1620，高度自動等比例計算
        float scale = 1620.0f / bgStage1[i].width();
        int drawH = (int)(bgStage1[i].height() * scale);
        int drawY = height() - drawH;  // 貼在底部

        painter.drawPixmap(drawX, drawY, 1620, drawH, bgStage1[i]);
    }

    // 把攝影機偏移傳給 Kirby 的 draw
    kirby.draw(painter, cameraX);

    // 畫碰撞框（debug 用，之後刪掉）
    painter.setPen(QPen(Qt::red, 2));
    painter.setBrush(Qt::NoBrush);
    for (auto &p : platforms) {
        QRectF r = p.getRect();
        painter.drawRect((int)(r.x() - cameraX), (int)r.y(), (int)r.width(), (int)r.height());
    }

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
{ //建立很多 Platform 物件，放進 platforms 這個列表
    platforms.clear();

    // === Frame 1 (x: 0 ~ 1620) ===
    // 地板
    platforms.append(Platform(0,    800, 2170, 100, PlatformType::Invisible));
    platforms.append(Platform(2170, 900, 2660-2170, 100, PlatformType::Invisible));
    platforms.append(Platform(2660, 815, 3675-2660, 100, PlatformType::Invisible));
    platforms.append(Platform(3675, 390, 3860-3675, 500, PlatformType::Invisible));
    platforms.append(Platform(3860, 600, 3960-3860, 500, PlatformType::Invisible));
    platforms.append(Platform(3960, 815, 4860, 100, PlatformType::Invisible));
    /* 平台
    platforms.append(Platform(300,  750,  200, 40, PlatformType::Floor));
    platforms.append(Platform(700,  650,  200, 40, PlatformType::Floor));
    // 磚頭
    platforms.append(Platform(500,  700,   80, 40, PlatformType::Brick));
*/
}

void GameWindow::checkCollisions()
{
    //掃描每個 Platform，看 Kirby 的矩形有沒有跟它重疊
    //重疊了就把 Kirby 推回去（站在上面或撞牆）

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

        if (p.type == PlatformType::Floor) {
            // Floor：只處理從上方落下，忽略其他方向
            // 條件：Kirby 往下移動，且上一幀底部在平台上方
            if (kirby.vy >= 0 && minOverlap == overlapTop) {
                kirby.y = pRect.top() - kirby.getRect().height();
                kirby.vy = 0;
                kirby.onGround = true;
            }
            // 從下往上穿越：不做任何處理（直接忽略）

        } else {
            // Brick：四個方向都碰撞
            if (minOverlap == overlapTop && kirby.vy >= 0) {
                kirby.y = pRect.top() - kirby.getRect().height();
                kirby.vy = 0;
                kirby.onGround = true;
            } else if (minOverlap == overlapBottom && kirby.vy < 0) {
                kirby.y = pRect.bottom();
                kirby.vy = 0;
            } else if (minOverlap == overlapLeft) {
                kirby.x = pRect.left() - kirby.getRect().width();
            } else if (minOverlap == overlapRight) {
                kirby.x = pRect.right();
            }
        }

        // 更新碰撞框
        kirbyRect = kirby.getRect();
    }
}

void GameWindow::loadBackground()
{
    bgStage1[0] = QPixmap(":/Image/background/Stage1(1).png");
    bgStage1[1] = QPixmap(":/Image/background/Stage1(2).png");
    bgStage1[2] = QPixmap(":/Image/background/Stage1(4).png");
}


void GameWindow::mousePressEvent(QMouseEvent *event)
{
    // 點擊畫面時印出世界座標
    float worldX = event->x() + cameraX;
    float worldY = event->y();
    qDebug() << "x:" << worldX << "y:" << worldY;
}
