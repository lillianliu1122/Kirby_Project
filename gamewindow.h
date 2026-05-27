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
#include <vector>

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

    void mousePressEvent(QMouseEvent *event) override; // 鎖定座標 之後刪掉

private slots:
    void gameLoop();   // 每幀更新

private:
    QTimer *timer;
    QSet<int> keysHeld;  // 記錄目前哪些鍵被按住
    Kirby kirby;
    float cameraX;  // 攝影機水平偏移量
    QVector<Platform> platforms;  // 所有平台
    void loadStage1();             // 載入 Stage 1 地圖
    void initStageEnemies(int stage);  // 用來載入關卡怪物的函式
    void checkCollisions();        // 碰撞判定
    QPixmap bgStage1[3];  // Stage 1 三個 frame 的背景
    QPixmap bgStage2[5];  // Stage 5 有兩張背景
    void loadBackground();
    std::vector<Enemy*> enemies; // 用來放關卡中所有怪物的動態指標陣列

    GameState gameState;
    void drawGame(QPainter &painter);
    void drawStartMenu(QPainter &painter);
    void drawGameOver(QPainter &painter);

    QVector<Portal> portals;
    void checkPortal();
    void loadStage2();
    int currentStage;
};

#endif // GAMEWINDOW_H
