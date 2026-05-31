#include "kirby.h"
#include <QSet>
#include <QPainter>
#include <Qt>
#include <QDebug>

const float Kirby::SPEED      = 5.0f;
const float Kirby::JUMP_FORCE = -15.0f;
const float Kirby::GRAVITY    = 0.6f;

const int Kirby::KIRBY_W = 100;
const int Kirby::KIRBY_H = 100;
const int Kirby::KIRBY_CH = 100*76/80;   // 蹲下高度（down 比例）

Kirby::Kirby()
    : x(100), y(600), vx(0), vy(0),
      onGround(false), facingRight(true),
      state(KirbyState::Idle), animFrame(0), animCounter(0),
      isFlying(false), flyCount(0), isMouthful(false), inhaledType(""), wantsToSpitStar(false)
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

    // 吸入動作
    imgInhale_R << QPixmap(":/Image/Kirby_normal/kirby_attack_R.png");
    imgInhale_L << QPixmap(":/Image/Kirby_normal/kirby_attack_L.png");

    // 吸滿（Mouthful）
    imgMouthful_R << QPixmap(":/Image/Kirby_normal/kirby_mouthful_stop_R.png");
    imgMouthful_L << QPixmap(":/Image/Kirby_normal/kirby_mouthful_stop_L.png");

}
void Kirby::update(const QSet<int> &keys, const QSet<int> &justPressed)
{
    updateInvincible();

    // === 最優先：Mouthful 狀態 ===
    if (isMouthful) {
        state = KirbyState::Mouthful;

        // 可以左右移動
        if (keys.contains(Qt::Key_Left)) {
            vx = -SPEED;
            facingRight = false;
        } else if (keys.contains(Qt::Key_Right)) {
            vx = SPEED;
            facingRight = true;
        } else {
            vx = 0;
        }

        // 不能跳躍飛行，只套用重力
        vy += GRAVITY;
        x += vx;
        y += vy;

        if (justPressed.contains(Qt::Key_Down)) {
            swallow();
        } else if (justPressed.contains(Qt::Key_X)) {
            spitStar();
        }

        updateAnimation();
        return;  // 直接結束，跳過所有跳躍飛行邏輯
    }

    // === 以下是正常狀態邏輯 ===
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

    // 蹲下 / 跳躍 / 飛行
    if (keys.contains(Qt::Key_Down) && onGround) {
        state = KirbyState::Crouch;
        vx = 0;
    } else if (keys.contains(Qt::Key_Up)) {
        if (onGround) {
            vy = JUMP_FORCE;
            onGround = false;
            isFlying = false;
            flyCount = 0;
        } else if (!isFlying) {
            isFlying = true;
            vy = -8.0f;
        } else {
            vy = -4.0f;
        }
    } else {
        if (isFlying) isFlying = false;
    }

    // 重力
    if (isFlying) {
        vy += GRAVITY * 0.5f;
    } else {
        vy += GRAVITY;
    }
    if (isFlying && vy > 3.0f) vy = 3.0f;

    // 更新位置
    x += vx;
    y += vy;

    // 狀態更新
    if (keys.contains(Qt::Key_X)) {
        state = KirbyState::Inhaling;
    } else if (!onGround) {
        state = isFlying ? KirbyState::Fly : KirbyState::Jump;
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
        case KirbyState::Inhaling:
            return facingRight ? imgInhale_R[0] : imgInhale_L[0];
        case KirbyState::Mouthful:
            return facingRight ? imgMouthful_R[0] : imgMouthful_L[0];
        default: // Idle
            return facingRight ? imgIdle_R[0] : imgIdle_L[0];
    }
}

void Kirby::draw(QPainter &painter, float cameraX)
{
    // 閃爍效果：無敵時，每 10 幀隱藏一次
    if (isInvincible && (invincibleTimer % 20 < 10)) {
            return; // 直接跳過繪製，達到閃爍效果
    }

    QPixmap frame = currentFrame();
    int drawX = (int)(x - cameraX);  // 套用攝影機偏移

    if (!frame.isNull()) {
        // 蹲下時用原始圖片比例，其他動作維持 50x50 ()
        //kirby_down 這張圖比較矮
        if (state == KirbyState::Crouch) {
            // 蹲下：貼齊地板底部
            painter.drawPixmap(drawX, (int)y + 4*(KIRBY_H - KIRBY_CH), KIRBY_W, KIRBY_CH, frame);
        } else {
            painter.drawPixmap(drawX, (int)y, KIRBY_W, KIRBY_H, frame);
        }
    } else {
        // 圖片載入失敗時顯示粉紅色方塊（debug 用）
        painter.setBrush(QColor(255, 105, 180));
        painter.drawRect(drawX, (int)y, KIRBY_W, KIRBY_H);
    }
}

QRectF Kirby::getRect() const
{
    return QRectF(x, y, KIRBY_W, KIRBY_H);
}

void Kirby::takeDamage() {
    // 如果已經無敵中，就不要重複扣血
    if (isInvincible) return;

    hp--;
    startInvincible(); // 扣血後進入無敵閃爍狀態
    if (hp < 0) {
        // HP 歸零的邏輯交給 GameWindow 處理 (handleLifeLost)
        // 這裡只需要確保 HP 不要變成負數
        hp = 0;
    }
}

void Kirby::startInvincible() {
    isInvincible = true;
    invincibleTimer = 120; // 持續 120 幀 (約 2 秒)
}

void Kirby::updateInvincible() {
    if (isInvincible) {
        invincibleTimer--;
        if (invincibleTimer <= 0) {
            isInvincible = false;
        }
    }
}

bool Kirby::isInhaling() const
{
    return state == KirbyState::Inhaling;
}

QRectF Kirby::getInhaleRect() const
{
    // 吸力範圍：在 Kirby 前方約一個身長
    if (facingRight)
        return QRectF(x + KIRBY_W, y, KIRBY_W * 2, KIRBY_H);
    else
        return QRectF(x - KIRBY_W * 2, y, KIRBY_W * 2, KIRBY_H);
}

void Kirby::inhaleEnemy(QString enemyType)
{
    isMouthful = true;
    inhaledType = enemyType;
    state = KirbyState::Mouthful;
}

void Kirby::swallow()
{
    if (!isMouthful) return;
    // 依敵人種類決定能力（之後接能力系統）
    isMouthful = false;
    inhaledType = "";
    state = KirbyState::Idle;
}

void Kirby::spitStar()
{
    if (!isMouthful) return;
    isMouthful = false;
    inhaledType = "";
    state = KirbyState::Idle;
    wantsToSpitStar = true;  // 通知 GameWindow
    // 星星彈的生成交給 GameWindow 處理
}
