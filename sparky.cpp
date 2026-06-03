#include "sparky.h"

Sparky::Sparky(int startX, int startY, int range)
    : Enemy(startX, startY, "Sparky", "Spark", true),
      isDischarging(false), timer(0), jumpTimer(0), moveRange(range), isGrounded(true) {
    vx = 0; vy = 0;
}

void Sparky::updateBehavior(int kirbyX, int kirbyY) {
    Q_UNUSED(kirbyX); Q_UNUSED(kirbyY);
    if (isDead) return;

    timer++; jumpTimer++;
    int totalCycle = 300;
    int moveDuration = 200;

    // 放電狀態
    if (timer % totalCycle >= moveDuration) {
        isDischarging = true;
        vx = 0;
    } else {
        isDischarging = false;

        // 邊界檢測
        if (isGrounded) {
            if (x > startX + moveRange) moveDirection = -1;      // 超過右邊界，改向左
            else if (x < startX - moveRange) moveDirection = 1;  // 超過左邊界，改向右
        }

        // 跳躍
        if (isGrounded && jumpTimer % 45 == 0) {
            vy = -10; // 向上初速
            isGrounded = false;
            vx = moveDirection * 4.0f; // 跳躍時水平方向力
            lastFacingLeft = (vx < 0);
        }
    }

    // 跳躍狀態
    if (!isGrounded) {
        x += vx;    // 空中維持水平速度
        vy += 1;    // 重力
    }
    y += vy;
    if (vx != 0) moveDirection = (vx > 0) ? 1 : -1;

    // 當觸及地面時恢復狀態
    if (y >= startY) {
        y = startY; vy = 0; isGrounded = true;
    }
    moveDirection = (vx >= 0) ? 1 : -1;
    lastFacingLeft = (vx < 0);
}

void Sparky::draw(QPainter &painter) {
    if (isDead) return;
    QPixmap currentImg;

    // 放電時，繪製 sparky 並疊加電流特效
    if (isDischarging) {
        int radius = 60;
        QPixmap effect(":/Image/Sparky/Sparky_attack_2.png");
        painter.drawPixmap(x - radius, y - radius, width + (radius * 2), height + (radius * 2), effect);
        currentImg = QPixmap(":/Image/Sparky/Sparky_attack_1.png");
        painter.drawPixmap(x, y, width, height, QPixmap(":/Image/Sparky/Sparky_attack_1.png"));
    }

    // 非放電時，根據落地與跳躍狀態切換行走/跳躍圖片
    else {
        if (isGrounded) {
            currentImg = lastFacingLeft ? QPixmap(":/Image/Sparky/Sparky_left_1.png")
                                         : QPixmap(":/Image/Sparky/Sparky_ritht_1.png");
        } else {
            currentImg = lastFacingLeft ? QPixmap(":/Image/Sparky/Sparky_left_2.png")
                                  : QPixmap(":/Image/Sparky/Sparky_ritht_2.png");
        }
        painter.drawPixmap(x, y, width, height, currentImg);
    }
}
