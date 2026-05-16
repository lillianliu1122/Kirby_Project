#ifndef GAMEWINDOW_H
#define GAMEWINDOW_H

#include <QMainWindow>
#include <QKeyEvent>
#include <QTimer>
#include <QPainter>
#include <QSet>
#include "kirby.h"

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

private slots:
    void gameLoop();   // 每幀更新

private:
    QTimer *timer;
    QSet<int> keysHeld;  // 記錄目前哪些鍵被按住
    Kirby kirby;
};

#endif // GAMEWINDOW_H
