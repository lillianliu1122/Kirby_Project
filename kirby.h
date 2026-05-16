#ifndef KIRBY_CLASS_H
#define KIRBY_CLASS_H

#include <QRectF>
#include <QPixmap>
#include <QVector>

// Kirby 的動作狀態
enum class KirbyState {
    Idle,    // 靜止
    Run,     // 跑步
    Jump,    // 跳躍
    Crouch,  // 蹲下
    Fly      // 飛行
};


class Kirby
{
public:
    Kirby();

    void update(const class QSet<int> &keys);  // 每幀更新位置
    void draw(class QPainter &painter, float cameraX);
    QRectF getRect() const;                    // 取得碰撞框

    float x, y;         // 位置
    float vx, vy;       // 速度
    bool onGround;      // 是否站在地上
    bool facingRight;   // 面向右邊？

private:
    void loadImages();
    void updateAnimation();
    QPixmap currentFrame() const;

    KirbyState state;

    // 各動作的圖片序列
    QVector<QPixmap> imgIdle_L, imgIdle_R;
    QVector<QPixmap> imgRun_L,  imgRun_R;
    QVector<QPixmap> imgJump;
    QVector<QPixmap> imgCrouch_L, imgCrouch_R;
    QVector<QPixmap> imgFly_L, imgFly_R;

    int animFrame;    // 目前播到第幾張
    int animCounter;  // 計數器，控制換圖速度
    bool isFlying;      // 是否在飛行中
    int flyCount;       // 已飛幾次（限制連續飛行次數）

    static const float SPEED;
    static const float JUMP_FORCE;
    static const float GRAVITY;

    static const int KIRBY_W;  // 顯示寬度
    static const int KIRBY_H;   // 一般高度（run 比例）
    static const int KIRBY_CH;   // 蹲下高度（down 比例）
};

#endif // KIRBY_H
