#ifndef GAMEWINDOW_H
#define GAMEWINDOW_H

#include <QMainWindow>
#include <QKeyEvent>
#include <QTimer>
#include <QPainter>
#include <QSet>
#include <QVector>
#include "kirby.h"
#include "platform.h"
#include "portal.h"
#include "enemy.h"
#include "item.h"
#include <vector>
#include "slopeplatform.h"
#include "starbullet.h"

enum class GameState {
    StartMenu,   // 開始畫面
    Playing,     // 遊戲進行中
    GameOver,    // 遊戲結束
    StageClear   // 通關
};

class GameWindow : public QMainWindow
{
    Q_OBJECT

public:
    GameWindow(QWidget *parent = nullptr);
    ~GameWindow();

protected:
    void paintEvent(QPaintEvent *event) override;      // 負責畫面繪製
    void keyPressEvent(QKeyEvent *event) override;     // 按下按鍵
    void keyReleaseEvent(QKeyEvent *event) override;   // 放開按鍵

    void mousePressEvent(QMouseEvent *event) override; // 鎖定座標

private slots:
    void gameLoop();   // 每幀更新
    void handleLifeLost();

private:
    QTimer *timer;
    QSet<int> keysHeld;  // 記錄目前哪些鍵被按住
    QSet<int> keysJustPressed;
    Kirby kirby;

    float cameraX;  // 攝影機水平偏移量
    int currentStage;
    GameState gameState;
    int gameOverOption = 0; // 0 表示 Continue, 1 表示 Quit

    QVector<Platform> platforms;  // 所有平台
    QVector<SlopePlatform> slopes; // 放置斜坡
    QVector<Portal> portals;
    QVector<StarBullet> starBullets;
    QVector<Tomato> tomatoes;
    QVector<OneUp> oneUps;
    QPixmap bgStage1[3];  // Stage 1 三個 frame 的背景
    QPixmap bgStage2[5];  // Stage 5 有兩張背景
    QPixmap bgSky1;  // Stage 1 天空
    QPixmap bgSky2;  // Stage 2 天空
    std::vector<Enemy*> enemies; // 用來放關卡中所有敵人的動態指標陣列

    // Fire 攻擊
    bool fireAttackActive = false;
    int fireAttackTimer = 0;
    QRectF fireAttackRect;
    int fireAnimFrame = 0;
    int fireAnimCounter = 0;
    QPixmap *fireImgs[3][2] = {}; // [frame][L/R]

    // Spark 攻擊
    bool sparkAttackActive = false;
    int sparkAttackTimer = 0;
    QRectF sparkAttackRect;

    bool tomatoCollected = false;  // Stage1 番茄
    bool oneUpCollected = false;   // Stage2 1UP

    void loadStage1();             // 載入 Stage 1 地圖
    void loadStage2();
    void loadBackground();
    void initStageEnemies(int stage);  // 用來載入關卡怪物的函式

    void checkCollisions();        // 碰撞判定
    void checkSlopeCollisions();
    void checkPortal();
    void checkInhale();
    void checkPuffAttack();
    void updateStarBullets(float cameraX);

    void drawGame(QPainter &painter);
    void drawStartMenu(QPainter &painter);
    void drawGameOver(QPainter &painter);
    void drawStageClear(QPainter &painter);
};

#endif // GAMEWINDOW_H
