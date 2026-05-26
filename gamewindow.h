#ifndef GAMEWINDOW_H
#define GAMEWINDOW_H

#include <QMainWindow>
#include <QKeyEvent>
#include <QTimer>
#include <QPainter>
#include <QSet>
#include "kirby.h"
#include "platform.h"
#include <QVector>

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
    void checkCollisions();        // 碰撞判定
    QPixmap bgStage1[3];  // Stage 1 三個 frame 的背景
    void loadBackground();
};

#endif // GAMEWINDOW_H
