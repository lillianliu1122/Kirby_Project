#include "gamewindow.h"
#include "waddledee.h"
#include "gordo.h"
#include "hothead.h"
#include "sparky.h"
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
    float mapWidth = (currentStage == 1) ? 4860.0f : 8100.0f;

    switch (gameState) {
        case GameState::StartMenu:
            break;

        case GameState::Playing:
            kirby.update(keysHeld, keysJustPressed);
            checkCollisions();
            checkSlopeCollisions();
            checkPortal();
            checkInhale();

            // 生成星星彈
            if (kirby.wantsToSpitStar) {
                kirby.wantsToSpitStar = false;
                float starX = kirby.facingRight ? kirby.x + kirby.KIRBY_W : kirby.x - 60;
                float starY = kirby.y + kirby.KIRBY_H / 2.0f - 30;
                starBullets.append(StarBullet(starX, starY, kirby.facingRight));
            }
            updateStarBullets(cameraX);

            // 敵人行為更新與戰鬥判定區塊
            for (Enemy* enemy : enemies) {
                // 1. 更新敵人的移動邏輯與狀態（傳入卡比座標，供怪判斷攻擊時機）
                enemy->updateBehavior(kirby.x, kirby.y);
                enemy->checkWallCollision(platforms); // 這是關鍵的一步，防止穿牆！
                // 2. 檢查怪物是否離開鏡頭太遠，離開就暫時移除，卡比回頭時重新生成
                enemy->checkRespawn(cameraX, 1620);

                if (HotHead* h = dynamic_cast<HotHead*>(enemy)) {
                    // 傳入 kirby.getRect() 進行碰撞判定
                    if (h->fireBall.hitKirby) {
                        QRect fireRect(h->fireBall.fx, h->fireBall.fy, 24, 24);
                        if (fireRect.intersects(kirby.getRect().toRect()) && !kirby.isInvincible) { // 在 GameWindow 這裡檢查無敵狀態
                            if (!kirby.isInvincible) {
                                kirby.takeDamage();
                                if (kirby.hp <= 0) handleLifeLost();
                                h->fireBall.active = false; // 擊中後火球消失
                            }
                        }
                    }
                }

                // 身體碰撞
                if (!enemy->getIsDead() && kirby.getRect().intersects(enemy->getCollisionBox())) {
                    if (!kirby.isInvincible) {
                        kirby.takeDamage();
                        if (kirby.hp <= 0) handleLifeLost();
                    }
                }

                // 偵測電流攻擊
                if (Sparky* s = dynamic_cast<Sparky*>(enemy)) {
                    if (s->isDischarging) {
                        int radius = 60; // 跟你 draw 的數值一樣
                        QRectF electricRect(s->x - radius, s->y - radius, s->width + (radius * 2), s->height + (radius * 2));
                        if (kirby.getRect().intersects(electricRect)) {
                            if (!kirby.isInvincible) {
                                kirby.takeDamage();
                                    if (kirby.hp <= 0) handleLifeLost();
                            }
                        }
                    }
                }


            }

            // tomato碰撞
            for (auto &t : tomatoes) {
                if (t.isActive && kirby.getRect().intersects(t.getRect())) {
                    kirby.hp = 3;          // 補滿 HP
                    t.isActive = false;     // 番茄消失
                }
            }

            // 1UP碰撞
            for (auto &life : oneUps) {
                if (!life.isCollected && kirby.getRect().intersects(life.getRect())) {
                    life.isCollected = true; // 隱藏道具
                    if (kirby.lives < 3) { // 假設你希望生命值上限是 3
                        kirby.lives += 1;
                    }
                }
            }
            // hole判斷
            // 如果卡比的 y 座標大於視窗高度 (1080)，代表掉進了 Hole
            if (kirby.y > height()) {
                handleLifeLost();
            }

            cameraX = kirby.x - width() / 2.0f;
            if (cameraX < 0) cameraX = 0;
            if (cameraX > mapWidth - width()) cameraX = mapWidth - width();

            if (kirby.x < 0) kirby.x = 0;
            if (kirby.x > mapWidth - kirby.KIRBY_W) kirby.x = mapWidth - kirby.KIRBY_W;

            break;

        case GameState::GameOver:
            break;

        case GameState::StageClear:
            break;
    }

    keysJustPressed.clear();
    update();
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
            drawStageClear(painter);
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
    // 1. 天空背景（最底層）
    QPixmap &sky = (currentStage == 1) ? bgSky1 : bgSky2;
    if (!sky.isNull()) {
        painter.drawPixmap(0, 0, 1620, 1080, sky);
    } else {
        painter.fillRect(0, 0, 1620, 1080, QColor(135, 206, 235));
    }

    // 2. 畫平台
    for (auto &p : platforms) {
        p.draw(painter, cameraX);
    }


    // 3. 貼地形圖
    if (currentStage == 1) {
        for (int i = 0; i < 3; i++) {
            if (bgStage1[i].isNull()) continue;
            float scale = 1620.0f / bgStage1[i].width();
            int drawH = (int)(bgStage1[i].height() * scale);
            int drawY = height() - drawH;
            int drawX = (int)(i * 1620 - cameraX);
            if (drawX < width() && drawX + 1620 > 0)
                painter.drawPixmap(drawX, drawY, 1620, drawH, bgStage1[i]);
        }
    }
    else if (currentStage == 2) {
        for (int i = 0; i < 5; i++) {
            float scale = 1620.0f / bgStage2[i].width();
            int drawH = (int)(bgStage2[i].height() * scale);
            int drawY = height() - drawH;
            int drawX = (int)(i * 1620 - cameraX);
            if (drawX < width() && drawX + 1620 > 0)
                painter.drawPixmap(drawX, drawY, 1620, drawH, bgStage2[i]);
        }
    }

    // 4. 畫傳送門
    for (auto &p : portals) {
        p.draw(painter, cameraX);
    }

    // 敵人繪製區塊
    // 將畫布往左偏移，確保怪物座標與捲軸鏡頭對齊
    painter.translate(-cameraX, 0);

    for (Enemy* enemy : enemies) {
        enemy->draw(painter);       // 叫每一隻怪物繪製自己
    }

    painter.translate(cameraX, 0);  // 畫完怪物後移回畫布

    // 5. 畫星星彈
    for (auto &s : starBullets) {
        s.draw(painter, cameraX);
    }

    // 6. 畫kirby
    kirby.draw(painter, cameraX);

    // 畫slope
    for (auto &s : slopes) {
        s.draw(painter, cameraX);
    }

    // 畫 HP 數字與 Lives
    painter.resetTransform(); // 重置畫布偏移，確保 UI 固定在右下角

    // UI 整體起始位置 (設定在螢幕右下角)
    int uiStartX = width() - 350; // 調整這個值可以左右移動整個 UI 區塊
    int uiStartY = height() - 80;
    int iconSize = 40;

    // 畫 Lives 圖示與數字
    QPixmap lifeIcon(":/Image/item/life.png");
    painter.drawPixmap(uiStartX-30, uiStartY, iconSize, iconSize, lifeIcon);

    painter.setFont(QFont("Arial", 28, QFont::Bold));
    // 1. 先畫粉紅色的 "x"
    painter.setPen(QColor(255, 162, 222));
    painter.drawText(uiStartX + iconSize -20, uiStartY + 35, "x");
    // 2. 再畫黃色的數字 (計算位置稍微向右偏移)
    painter.setPen(Qt::yellow);
    QString livesStr = QString("%1").arg(kirby.lives, 2, 10, QChar('0'));
    painter.drawText(uiStartX + iconSize + 10, uiStartY + 35, livesStr);

    // 3. 畫 HP 圖片 (如果 HP 滿的顯示滿血圖，否則顯示空血圖)
    int hpStartX = uiStartX + 140;
    for (int i = 0; i < 3; ++i) {
        QPixmap hpImg = (i < kirby.hp) ? QPixmap(":/Image/item/HP_1.png") : QPixmap(":/Image/item/HP_0.png");
        // 放在 Lives 圖示的上方
        painter.drawPixmap(hpStartX + (i * 45), uiStartY, iconSize, iconSize, hpImg);
    }
    // 畫tomato
    for (auto &t : tomatoes) {
        t.draw(painter, cameraX);
    }
    // 畫1UP
    for (auto &life : oneUps) {
        life.draw(painter, cameraX);
    }
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
    QPixmap bg(":/Image/background/game_over_continue.png");
    painter.drawPixmap(0, 0, width(), height(), bg);
}

void GameWindow::drawStageClear(QPainter &painter)
{
    QPixmap bg(":/Image/background/cleared.png");
    painter.drawPixmap(0, 0, width(), height(), bg);
}

void GameWindow::keyPressEvent(QKeyEvent *event)
{
    if (!event->isAutoRepeat()) {
        keysJustPressed.insert(event->key());
    }
    keysHeld.insert(event->key());

    switch (gameState) {
        case GameState::StartMenu:
            // 任意鍵進入遊戲
            if (event->key() == Qt::Key_Return || event->key() == Qt::Key_Space) {
                gameState = GameState::Playing;
            }
            break;

        case GameState::GameOver:
            // 1. 處理指標上下移動 (Key_Up / Key_Down)
            if (event->key() == Qt::Key_Up || event->key() == Qt::Key_Down) {
                gameOverOption = 1 - gameOverOption; // 0 切換到 1，1 切換到 0
            }
            // 2. 處理確認選擇 (Key_Return / Key_Space)
            else if (event->key() == Qt::Key_Return || event->key() == Qt::Key_Space) {
                if (gameOverOption == 0) {
                // 選 Continue: 重置遊戲並回到 Playing 狀態
                gameState = GameState::Playing;
                kirby.lives = 3;
                kirby.hp = 3;
                kirby.x = 100;
                kirby.y = 400;
                cameraX = 0;
                currentStage = 1;
                loadStage1(); // 重新載入關卡
                }
                    else {
                        // 選 Quit: 關閉程式
                        close();
                    }
                }
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
    initStageEnemies(1);
}

void GameWindow::loadStage2()
{
    platforms.clear();
    portals.clear();

    // Stage 2 地板
    platforms.append(Platform(0, 900, 2630, 40, PlatformType::Invisible));
    platforms.append(Platform(2860, 800, 3653-2820, 5, PlatformType::Invisible));
    platforms.append(Platform(3864, 725, 3950-3864, 400, PlatformType::Invisible));
    platforms.append(Platform(5761, 725, 3960-3864, 400, PlatformType::Invisible));
    platforms.append(Platform(6072, 800, 6867-6072, 5, PlatformType::Invisible));
    platforms.append(Platform(7116, 900, 2000, 5, PlatformType::Invisible));

    //斜坡
    slopes.append(SlopePlatform(2630, 900, 2820, 795));
    slopes.append(SlopePlatform(3691, 800, 3824, 720));
    slopes.append(SlopePlatform(5903, 720, 6072, 800));
    slopes.append(SlopePlatform(6907, 795, 7085, 900));

    //平台
    //(floor)
    platforms.append(Platform(4094, 550, 215, 40, PlatformType::Floor));
    platforms.append(Platform(4564, 434, 215, 40, PlatformType::Floor));
    platforms.append(Platform(4960, 550, 215, 40, PlatformType::Floor));
    platforms.append(Platform(5388, 434, 215, 40, PlatformType::Floor));
    platforms.append(Platform(1922, 450, 320, 40, PlatformType::Floor));
    platforms.append(Platform(587, 450, 215, 40, PlatformType::Floor));

    //(brick)
    platforms.append(Platform(1160, 800, 320, 100, PlatformType::Brick));
    platforms.append(Platform(1260, 700, 220, 100, PlatformType::Brick));
    platforms.append(Platform(1365, 600, 115, 100, PlatformType::Brick));

    // 斷開的地板：左半邊
    //platforms.append(Platform(0, 900, 2000, 40, PlatformType::Invisible));
    // 斷開的地板：右半邊
    //platforms.append(Platform(2200, 900, 5900, 40, PlatformType::Invisible));

    // Stage 2 終點門
    portals.append(Portal(7800, 620, PortalType::Goal));
    initStageEnemies(2);
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

void GameWindow::checkSlopeCollisions()
{
    for (auto &s : slopes) {
        // Kirby 底部中心的 x 位置
        float kirbyBottomX = kirby.x + kirby.KIRBY_W / 2.0f;
        float kirbyBottom  = kirby.y + kirby.KIRBY_H;

        if (!s.containsX(kirbyBottomX)) continue;

        float slopeY = s.getYAtX(kirbyBottomX);

        // Kirby 往下且底部碰到斜面
        if (kirby.vy >= 0 && kirbyBottom >= slopeY) {
            kirby.y = slopeY - kirby.KIRBY_H;
            kirby.vy = 0;
            kirby.onGround = true;
        }
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
    bgStage2[0] = QPixmap(":/Image/background/Stage2(3).png");
    bgStage2[1] = QPixmap(":/Image/background/Stage2(4).png");
    bgStage2[2] = QPixmap(":/Image/background/Stage2(6).png");
    bgStage2[3] = QPixmap(":/Image/background/Stage2(7).png");
    bgStage2[4] = QPixmap(":/Image/background/Stage2(8).png");
    bgSky1 = QPixmap(":/Image/background/Stage1(bg).png");
    bgSky2 = QPixmap(":/Image/background/Stage2(bg).png");
}

void GameWindow::mousePressEvent(QMouseEvent *event)
{
    // 如果現在是 GameOver 狀態，才處理滑鼠點擊
    if (gameState == GameState::GameOver) {
        int mouseX = event->x();
        int mouseY = event->y();

        // 假設 CONTINUE 按鈕在中間偏下
        if (mouseX > 1100 && mouseX < 1800 && mouseY > 480 && mouseY < 630) {
            gameState = GameState::StartMenu;
            kirby.lives = 3;
            kirby.hp = 3;
            kirby.x = 100;
            kirby.y = 400;
            cameraX = 0;
            currentStage = 1;
            loadStage1();
        }
        // 假設 QUIT 按鈕在下面一點
        else if (mouseX > 1100 && mouseX < 1800 && mouseY > 640 && mouseY < 790) {
            close();
        }
    }

    // 點擊畫面顯示座標
    float worldX = event->x() + cameraX;
    float worldY = event->y();
    qDebug() << "x:" << worldX << "y:" << worldY;
}

// =======
// 新增敵人
// =======
void GameWindow::initStageEnemies(int stage) {
    enemies.clear(); // 換關卡前清空上一關的怪
    tomatoes.clear(); // 換關卡清空
    oneUps.clear();
    if (stage == 1) {
        // Stage 1：只能有小紅豆跟刺球
        // frame 1 (x：0-1620)
        enemies.push_back(new WaddleDee(650, 700, 200));
        enemies.push_back(new Gordo(1000, 550, 150));
        enemies.push_back(new WaddleDee(1400, 700, 200));
        // frame 2 (x：1621-3240)
        enemies.push_back(new Gordo(2100, 550, 150));
        enemies.push_back(new WaddleDee(2400, 800, 200));
        enemies.push_back(new WaddleDee(2510, 300, 60));
        enemies.push_back(new WaddleDee(3050, 715, 200));
        // frame 3 (x：3241-4860)
        enemies.push_back(new Gordo(3400, 550, 150));
        enemies.push_back(new Gordo(4000, 550, 150));
        enemies.push_back(new WaddleDee(4300, 715, 100));
        // 加tomato
        Tomato t;
        t.x = 2680;
        t.y = 715;
        t.isActive = true;
        tomatoes.push_back(t);
    }
    else if (stage == 2) {
        // Stage 2：加入噴火怪和電電怪
        //enemies.push_back(new WaddleDee(400, 800, 100));
        enemies.push_back(new HotHead(1080, 800, 200, &platforms));
        enemies.push_back(new WaddleDee(1620, 800, 200));
        enemies.push_back(new Sparky(1500, 800, 150));
        enemies.push_back(new Gordo(2400, 400, 150));
        // 1UP
        OneUp up1;
        up1.x = 500; // 設定 1UP 位置
        up1.y = 800;
        oneUps.push_back(up1);
    }
}

// =======
// 生命與重置系統
// =======
void GameWindow::handleLifeLost() {
    kirby.lives--; // 扣除一條命

    if (kirby.lives <= 0) {
        gameState = GameState::GameOver; // 生命耗盡，進入遊戲結束
    } else {
        // 重置 Kirby 狀態
        kirby.hp = 3;             // 回復滿血
        kirby.startInvincible();  // 重生後獲得 2 秒無敵閃爍保護

        // 重置到關卡起點 (建議依據目前的關卡來決定座標)
        kirby.x = 100;
        kirby.y = 400;
        kirby.vx = 0;
        kirby.vy = 0;
        cameraX = 0;
    }
}


void GameWindow::checkInhale()
{
    if (!kirby.isInhaling()) return;

    QRectF inhaleRect = kirby.getInhaleRect();

    for (auto e : enemies) {
        if (e->getIsDead()) continue;
        if (!e->canBeInhaled) continue;

        QRect eRect = e->getCollisionBox();
        if (inhaleRect.intersects(eRect)) {
            qDebug() << "吸入成功！敵人種類：" << e->capability;
            kirby.inhaleEnemy(e->capability);
            e->takeDamage();
            break;
        }
    }
}

void GameWindow::updateStarBullets(float cameraX)
{
    for (auto &s : starBullets) {
        if (!s.active) continue;

        s.update();


        // 超出螢幕視野就消失
        if (s.getRect().x() < cameraX - 100 || s.getRect().x() > cameraX + 1620 + 100) {
            s.active = false;
            continue;
        }

        // 碰到磚頭消失
        for (auto &p : platforms) {
            //if (p.type != PlatformType::Brick) continue;
            if (!p.visible) continue;
            if (s.getRect().intersects(p.getRect())) {
                s.active = false;
                break;
            }
        }

        // 碰到斜坡消失
        for (auto &s2 : slopes) {
            float starCenterX = s.getRect().x() + s.getRect().width() / 2.0f;
            float starBottom  = s.getRect().y() + s.getRect().height();

            if (s2.containsX(starCenterX)) {
                float slopeY = s2.getYAtX(starCenterX);
                if (starBottom >= slopeY) {
                    s.active = false;
                    break;
                }
            }
        }
        // 碰到敵人消失（Gordo 除外）
        for (auto e : enemies) {
            if (e->getIsDead()) continue;
            //if (e->type == "Gordo") continue;  // Gordo 不受影響
            if (s.getRect().intersects(e->getCollisionBox())) {
                s.active = false;
                e->takeDamage();
                break;
            }
        }
    }

    // 清除已消失的星星彈
    starBullets.erase(
        std::remove_if(starBullets.begin(), starBullets.end(),
                       [](const StarBullet &s) { return !s.active; }),
        starBullets.end()
    );
}
