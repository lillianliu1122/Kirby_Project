#include "gamewindow.h"
#include <QPainter>
#include <QDebug>

GameWindow::GameWindow(QWidget *parent)
    : QMainWindow(parent)
{
    // 設定視窗大小為 1620x1080
    setFixedSize(1620, 1080);
    setWindowTitle("Kirby's Adventure");
    gameState = GameState::StartMenu;

    // 建立遊戲主迴圈，每 16ms 觸發一次（約 60fps）
    timer = new QTimer(this);
    connect(timer, &QTimer::timeout, this, &GameWindow::gameLoop);
    timer->start(16);

    cameraX = 0;

     currentStage = 1;
    loadStage1();
    loadBackground();

    //currentStage = 2; //暫時
    //loadStage2();

    portals.append(Portal(4650, 550, PortalType::ToStage2));  // Stage1 終點
}

GameWindow::~GameWindow()
{
}

void GameWindow::gameLoop() //更新每幀畫面 形成動態效果
{
    // 不同stage的畫面寬度不同
    float mapWidth = (currentStage == 1) ? 4860.0f : 8100.0f;

    switch (gameState) {
        case GameState::StartMenu:
            // 開始畫面只等待按鍵，不更新遊戲邏輯
            break;

        case GameState::Playing:
            kirby.update(keysHeld); // 每幀更新 Kirby
            checkCollisions();
            // 之後新增 checkHole();
            checkPortal();

            // 鏡頭始終讓 Kirby 保持在畫面中間
            cameraX = kirby.x - width() / 2.0f;

            // 鏡頭不超出地圖邊界
            if (cameraX < 0) cameraX = 0;
            if (cameraX > mapWidth - width()) cameraX = mapWidth - width();

            // 限制 Kirby 不超出 Stage 1 左右邊界
            if (kirby.x < 0) kirby.x = 0;
            if (kirby.x > mapWidth - kirby.KIRBY_W) kirby.x = mapWidth - kirby.KIRBY_W;

            break;

        case GameState::GameOver:
            break;

        case GameState::StageClear:
            break;
    }

    update();  // 呼叫 paintEvent 重新繪製畫面
}

void GameWindow::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event);
    QPainter painter(this);

    switch (gameState) {
        case GameState::StartMenu:
            drawStartMenu(painter);   // 之後新增
            break;

        case GameState::Playing:
            drawGame(painter);        // 把原本的遊戲繪製移進來
            break;

        case GameState::GameOver:
            drawGameOver(painter);    // 之後新增
            break;

        case GameState::StageClear:
            break;
    }

    // 畫碰撞框（debug 用，之後刪掉）
    painter.setPen(QPen(Qt::red, 2));
    painter.setBrush(Qt::NoBrush);
    for (auto &p : platforms) {
        QRectF r = p.getRect();
        painter.drawRect((int)(r.x() - cameraX), (int)r.y(), (int)r.width(), (int)r.height());
    }
}

void GameWindow::drawGame(QPainter &painter) //畫出所有要顯示的物件
{
    // 後面畫的會覆蓋前面畫的
    // 1. 天空背景
    painter.fillRect(0, 0, width(), height(), QColor(135, 206, 235));

    // 2. 畫平台（會被草地蓋住）
    for (auto &p : platforms) {
        p.draw(painter, cameraX);
    }

    // 3. 貼地形圖（貼在畫面底部，保持原始比例）

    // Stage1
    if (currentStage == 1) {
        for (int i = 0; i < 3; i++) {
            if (bgStage1[i].isNull()) continue;
            float scale = 1620.0f / bgStage1[i].width(); //寬度固定 1620，高度等比例計算
            int drawH = (int)(bgStage1[i].height() * scale); //圖片高度
            int drawY = height() - drawH; //貼在畫面底部
            int drawX = (int)(i * 1620 - cameraX);
            if (drawX < width() && drawX + 1620 > 0)
                painter.drawPixmap(drawX, drawY, 1620, drawH, bgStage1[i]);
        }
    }
    // Stage 2
    else if (currentStage == 2) {
        for (int i = 0; i < 5; i++) {
            //if (bgStage2[i].isNull()) continue;
            float scale = 1620.0f / bgStage2[i].width();
            int drawH = (int)(bgStage2[0].height() * scale);
            int drawY = height() - drawH;
            int drawX = (int)(i * 1620 - cameraX);
            if (drawX < width() && drawX + 4050 > 0)
                painter.drawPixmap(drawX, drawY, 4050, drawH, bgStage2[0]);
        }
    }

    //  4. 畫傳送門
    for (auto &p : portals) {
        p.draw(painter, cameraX);
    }

    // 5. 畫kirby
    kirby.draw(painter, cameraX); //把攝影機偏移傳給 Kirby 的 draw
}

void GameWindow::drawStartMenu(QPainter &painter)
{
    // 貼 start.png 背景
    QPixmap startBg(":/Image/background/start.png");
    if (!startBg.isNull()) {
        painter.drawPixmap(0, 0, width(), height(), startBg);
    }

    /* 提示文字
    painter.setPen(Qt::white);
    painter.setFont(QFont("Arial", 24, QFont::Bold));
    painter.drawText(rect(), Qt::AlignHCenter | Qt::AlignBottom,
                     "Press ENTER or SPACE to Start");
    */
}

void GameWindow::drawGameOver(QPainter &painter)
{
    // 之後實作
    painter.fillRect(0, 0, width(), height(), Qt::black);
}


void GameWindow::keyPressEvent(QKeyEvent *event)
{
    keysHeld.insert(event->key());

    switch (gameState) {
        case GameState::StartMenu:
            // 任意鍵進入遊戲
            if (event->key() == Qt::Key_Return || event->key() == Qt::Key_Space) {
                gameState = GameState::Playing;
            }
            break;

        case GameState::GameOver:
            break;

        default:
            break;
    }
}

void GameWindow::keyReleaseEvent(QKeyEvent *event)
{
    keysHeld.remove(event->key());
}

void GameWindow::loadStage1()//布置platform位置
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
    // 平台
    platforms.append(Platform(700,  450,  215, 40, PlatformType::Floor));
    platforms.append(Platform(2400, 400,  320, 40, PlatformType::Floor));
    // 磚頭
    // platforms.append(Platform(500,  700,   80, 40, PlatformType::Brick));
}

void GameWindow::loadStage2()
{
    platforms.clear();
    portals.clear();

    // Stage 2 地板（暫時）
    platforms.append(Platform(0, 900, 8100, 40, PlatformType::Invisible));

    // Stage 2 終點門
    portals.append(Portal(7900, 800, PortalType::Goal));
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


void GameWindow::checkPortal() //由傳送門切換stage
{
    for (auto &p : portals) {
        if (!kirby.getRect().intersects(p.getRect())) continue;

        // 接觸 Portal 且按 Up
        if (keysHeld.contains(Qt::Key_Up)) {
            if (p.type == PortalType::ToStage2 && currentStage == 1) {
                currentStage = 2;
                loadStage2();
                // 重置 Kirby 位置到 Stage2 起點
                kirby.x = 100;
                kirby.y = 400;
                kirby.vx = 0;
                kirby.vy = 0;
                cameraX = 0;
            } else if (p.type == PortalType::Goal) {
                gameState = GameState::StageClear;
            }
        }
    }
}

void GameWindow::loadBackground() //載入背景圖
{
    bgStage1[0] = QPixmap(":/Image/background/Stage1(1).png");
    bgStage1[1] = QPixmap(":/Image/background/Stage1(2).png");
    bgStage1[2] = QPixmap(":/Image/background/Stage1(4).png");
    bgStage2[0] = QPixmap(":/Image/background/Stage2(1).png");
    bgStage2[1] = QPixmap(":/Image/background/Stage2(2).png");
}

void GameWindow::mousePressEvent(QMouseEvent *event)
{
    // 點擊畫面時印出世界座標
    float worldX = event->x() + cameraX;
    float worldY = event->y();
    qDebug() << "x:" << worldX << "y:" << worldY;
}

