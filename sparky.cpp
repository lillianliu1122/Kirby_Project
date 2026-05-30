#include "sparky.h"

Sparky::Sparky(int startX, int startY)
    : Enemy(startX, startY, "Sparky", "Spark", true),
      isDischarging(false), timer(0), jumpTimer(0), isGrounded(true) {
    vx = 0; vy = 0;
}

void Sparky::updateBehavior(int kirbyX, int kirbyY) {
    Q_UNUSED(kirbyX); Q_UNUSED(kirbyY);
    if (isDead) return;

    timer++; jumpTimer++;
    int totalCycle = 300;
    int moveDuration = 200;

    // 1. 放電狀態
    if (timer % totalCycle >= moveDuration) {
        isDischarging = true;
        vx = 0;
    } else {
        isDischarging = false;

        // 2. 範圍檢測：決定方向 (只在落地時檢查)
        if (isGrounded) {
            if (x > startX + 150) moveDirection = -1;      // 超過右邊界，改向左
            else if (x < startX - 150) moveDirection = 1;  // 超過左邊界，改向右
        }

        // 3. 跳躍邏輯
        if (isGrounded && jumpTimer % 45 == 0) {
            vy = -10;
            isGrounded = false;

            // 跳躍瞬間給予水平力道，根據 moveDirection
            vx = moveDirection * 4.0f;

            // --- 更新面向 ---
            lastFacingLeft = (vx < 0);
        }
    }

    // 4. 物理運算
    if (!isGrounded) {
        x += vx;    // 空中維持水平速度
        vy += 1;    // 重力
    } else {
        vx = 0;     // 落地立即歸零，防止滑動
    }
    y += vy;

    // 5. 地面校正
    if (y >= startY) {
        y = startY; vy = 0; isGrounded = true;
    }
}

void Sparky::draw(QPainter &painter) {
    if (isDead) return;

    QPixmap currentImg;

    if (isDischarging) {
        int radius = 60;
        // 1. 發動攻擊時 (顯示 attack 圖片)
        // 這裡我們畫 attack_1 作為本體，attack_2 作為疊加的電流效果
        currentImg = QPixmap(":/Image/Sparky/Sparky_attack_1.png");
        painter.drawPixmap(x, y, width, height, QPixmap(":/Image/Sparky/Sparky_attack_1.png"));

        QPixmap effect(":/Image/Sparky/Sparky_attack_2.png");
        // 將電流效果疊加在 Sparky 周圍，位置根據需要微調
        painter.drawPixmap(x - radius, y - radius, width + (radius * 2), height + (radius * 2), effect);
    }
    else {
        // 2. 移動狀態
        if (isGrounded) {
            // 踩在地板上 (Sparky_left_1 或 right_1)
            currentImg = lastFacingLeft ? QPixmap(":/Image/Sparky/Sparky_left_1.png")
                                         : QPixmap(":/Image/Sparky/Sparky_ritht_1.png");
        } else {
            // 跳躍中 (Sparky_left_2 或 right_2)
            currentImg = lastFacingLeft ? QPixmap(":/Image/Sparky/Sparky_left_2.png")
                                  : QPixmap(":/Image/Sparky/Sparky_ritht_2.png");
        }
        painter.drawPixmap(x, y, width, height, currentImg);
    }
}
