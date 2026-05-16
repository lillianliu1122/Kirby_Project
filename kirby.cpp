#include "kirby.h"
#include <QSet>
#include <QPainter>
#include <Qt>

const float Kirby::SPEED      = 5.0f;
const float Kirby::JUMP_FORCE = -15.0f;
const float Kirby::GRAVITY    = 0.6f;

const int Kirby::KIRBY_W = 80;
const int Kirby::KIRBY_H = 80;
const int Kirby::KIRBY_CH = 76;   // 蹲下高度（down 比例）

Kirby::Kirby()
    : x(100), y(800), vx(0), vy(0),
      onGround(false), facingRight(true),
      state(KirbyState::Idle), animFrame(0), animCounter(0)
{
    loadImages();
}

void Kirby::loadImages()
{
    // 靜止
    imgIdle_R << QPixmap(":/Image/Kirby_normal/kirby_stop_R.png");
    imgIdle_L << QPixmap(":/Image/Kirby_normal/kirby_stop_L.png");

    // 跑步
    imgRun_R << QPixmap(":/Image/Kirby_normal/kirby_run_1_R.png")
             << QPixmap(":/Image/Kirby_normal/kirby_run_2_R.png")
             << QPixmap(":/Image/Kirby_normal/kirby_run_3_R.png")
             << QPixmap(":/Image/Kirby_normal/kirby_run_4_R.png");
    imgRun_L << QPixmap(":/Image/Kirby_normal/kirby_run_1_L.png")
             << QPixmap(":/Image/Kirby_normal/kirby_run_2_L.png")
             << QPixmap(":/Image/Kirby_normal/kirby_run_3_L.png")
             << QPixmap(":/Image/Kirby_normal/kirby_run_4_L.png");

    // 跳躍
    imgJump << QPixmap(":/Image/Kirby_normal/kirby_jump(1).png")
            << QPixmap(":/Image/Kirby_normal/kirby_jump(2).png")
            << QPixmap(":/Image/Kirby_normal/kirby_jump(3).png");

    // 蹲下
    imgCrouch_R << QPixmap(":/Image/Kirby_normal/kirby_down_R.png");
    imgCrouch_L << QPixmap(":/Image/Kirby_normal/kirby_down_L.png");

    // 飛行
    imgFly_R << QPixmap(":/Image/Kirby_normal/kirby_fly_1_R.png")
             << QPixmap(":/Image/Kirby_normal/kirby_fly_2_R.png");
    imgFly_L << QPixmap(":/Image/Kirby_normal/kirby_fly_1_L.png")
             << QPixmap(":/Image/Kirby_normal/kirby_fly_2_L.png");
}

void Kirby::update(const QSet<int> &keys)
{
    // 左右移動
    if (keys.contains(Qt::Key_Left)) {
        vx = -SPEED;
        facingRight = false;
    } else if (keys.contains(Qt::Key_Right)) {
        vx = SPEED;
        facingRight = true;
    } else {
        vx = 0;
    }

    // 蹲下
    if (keys.contains(Qt::Key_Down) && onGround) {
        state = KirbyState::Crouch;
        vx = 0;
    }
    // 跳躍
    else if (keys.contains(Qt::Key_Up) && onGround) {
        vy = JUMP_FORCE;
        onGround = false;
    }

    // 重力
    vy += GRAVITY;

    // 更新位置
    x += vx;
    y += vy;

    // 暫時地板
    if (y >= 850) {
        y = 850;
        vy = 0;
        onGround = true;
    }

    // 更新動作狀態
    if (!onGround) {
        state = KirbyState::Jump;
    } else if (keys.contains(Qt::Key_Down)) {
        state = KirbyState::Crouch;
    } else if (vx != 0) {
        state = KirbyState::Run;
    } else {
        state = KirbyState::Idle;
    }

    updateAnimation();
}

void Kirby::updateAnimation()
{
    animCounter++;

    // 每 8 幀換一張圖（可調整速度）
    if (animCounter >= 8) {
        animCounter = 0;
        animFrame++;
    }

    // 防止 animFrame 超出當前動作的圖片數量
    int maxFrames = 1;
    switch (state) {
        case KirbyState::Run:    maxFrames = 4; break;
        case KirbyState::Jump:   maxFrames = 3; break;
        case KirbyState::Fly:    maxFrames = 2; break;
        default:                 maxFrames = 1; break;
    }
    if (animFrame >= maxFrames) animFrame = 0;
}

QPixmap Kirby::currentFrame() const
{
    switch (state) {
        case KirbyState::Run:
            return facingRight ? imgRun_R[animFrame] : imgRun_L[animFrame];
        case KirbyState::Jump:
            return imgJump[animFrame];
        case KirbyState::Crouch:
            return facingRight ? imgCrouch_R[0] : imgCrouch_L[0];
        case KirbyState::Fly:
            return facingRight ? imgFly_R[animFrame] : imgFly_L[animFrame];
        default: // Idle
            return facingRight ? imgIdle_R[0] : imgIdle_L[0];
    }
}

void Kirby::draw(QPainter &painter)
{
    QPixmap frame = currentFrame();
    if (!frame.isNull()) {
        // 蹲下時用原始圖片比例，其他動作維持 50x50 ()
        //kirby_down 這張圖比較矮
        if (state == KirbyState::Crouch) {
            // 蹲下：貼齊地板底部
            painter.drawPixmap((int)x, (int)y + 4*(KIRBY_H - KIRBY_CH), KIRBY_W, KIRBY_CH, frame);
        } else {
            painter.drawPixmap((int)x, (int)y, KIRBY_W, KIRBY_H, frame);
        }
    } else {
        // 圖片載入失敗時顯示粉紅色方塊（debug 用）
        painter.setBrush(QColor(255, 105, 180));
        painter.drawRect((int)x, (int)y, KIRBY_W, KIRBY_H);
    }
}

QRectF Kirby::getRect() const
{
    return QRectF(x, y, KIRBY_W, KIRBY_H);
}
