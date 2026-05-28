#include "enemy.h"
#include <cmath>

// 基底類別 Enemy 函式實作
Enemy::Enemy(int startX, int startY, QString type, QString cap, bool inhale)
    : startX(startX), startY(startY),
      isDead(false), isActive(true),
      x(startX), y(startY),
      vx(0), vy(0), width(100), height(100),
      type(type), capability(cap), canBeInhaled(inhale) {}

// 在 enemy.cpp 中，加入基底類別的碰撞檢測邏輯
void Enemy::checkWallCollision(const QVector<Platform>& platforms) {
    QRect enemyRect = getCollisionBox(); // 獲取當前怪物的矩形

    for (const auto &p : platforms) {
        if (!p.visible) continue;
        QRect pRect = p.getRect().toRect();

        if (enemyRect.intersects(pRect)) {
            // 偵測到碰撞，根據 vx 進行反彈
            if (vx > 0) { // 往右撞牆
                x = pRect.left() - width-5;
                vx = -vx; // 回頭
            } else if (vx < 0) { // 往左撞牆
                x = pRect.right()+5;
                vx = -vx; // 回頭
            }
        }
    }
}
Enemy::~Enemy() {}

QRect Enemy::getCollisionBox() const { return QRect(x, y, width, height); }
bool Enemy::getIsDead() const { return isDead; }

void Enemy::takeDamage() {
    if (type != "Gordo") isDead = true; // Gordo受到一般攻擊不會消失 [cite: 13, 46]
}

void Enemy::checkRespawn(int cameraX, int screenWidth) {
    // 7.1 離開有效場景區域後移除，卡比返回時重新生成 [cite: 43]
    if (x + width < cameraX - 200 || x > cameraX + screenWidth + 200) {
        isActive = false;
        if (isDead) { // 如果在螢幕外且卡比往回走，復活並重置 [cite: 43]
            x = startX; y = startY;
            isDead = false;
        }
    } else {
        isActive = true;
    }
}

// ====================
// Waddle Dee 走路怪實作
// ====================
WaddleDee::WaddleDee(int startX, int startY, int range)
    : Enemy(startX, startY, "WaddleDee", "None", true), walkRange(range) {
    vx = -2; // 初始往左走
}

void WaddleDee::updateBehavior(int kirbyX, int kirbyY) {
    Q_UNUSED(kirbyX); Q_UNUSED(kirbyY);
    if (isDead) return;

    // 1. 物理移動 (包含重力)
    //if (!isGrounded) vy += 0.6f;
    x += vx;
    y += vy;

    // 2. 地板檢測 (確保不穿過地板)
    /*if (y >= 800) {
        y = 800; vy = 0; isGrounded = true;
    } else {
        isGrounded = false;
    }*/

    // 3. 【核心邏輯】範圍內來回移動
    // 如果怪物離出生點的距離超過了 walkRange，就反轉速度
    if (x > startX + walkRange) {
        x = startX + walkRange; // 強制校正位置
        vx = -std::abs(vx);     // 強制向左
    } else if (x < startX - walkRange) {
        x = startX - walkRange; // 強制校正位置
        vx = std::abs(vx);      // 強制向右
    }
}

void WaddleDee::draw(QPainter &painter) {
    if (isDead) return;
    QPixmap img(":/Image/Waddle Dee/Waddle_Dee_0.png");

    // 如果向右走，我們要把圖水平翻轉
    if (vx > 0) {
        // 使用 QTransform 進行水平翻轉 (scale -1)
        QTransform trans;
        trans.scale(-1, 1);
        img = img.transformed(trans);
    }

    painter.drawPixmap(x, y, width, height, img);
}

// ==================
// Gordo 刺球障礙物實作
// ==================
Gordo::Gordo(int startX, int startY, int range)
    : Enemy(startX, startY, "Gordo", "None", false), moveRange(range), moveDirection(1) {
    vy = 3;
}

void Gordo::updateBehavior(int kirbyX, int kirbyY) {
    Q_UNUSED(kirbyX);
    Q_UNUSED(kirbyY);
    y += vy * moveDirection; // 7.4 沿預先設定的固定軌跡上下移動 [cite: 13, 43]
    if (y < startY - moveRange || y > startY + moveRange) {
        moveDirection = -moveDirection;
    }
}

void Gordo::draw(QPainter &painter) {
    QPixmap img(":/Image/Gordo/Gordo(0).png");
    painter.drawPixmap(x, y, width, height, img);
}

// ==================
// Hot Head 噴火怪實作
// ==================
HotHead::HotHead(int startX, int startY)
    : Enemy(startX, startY, "HotHead", "Fire", true),
      state(0), actionTimer(0), isFacingRight(false), isBreathingFire(false) {
    vx = -1;
}

void HotHead::updateBehavior(int kirbyX, int kirbyY) {
    if (isDead) return;
    actionTimer++;
    isFacingRight = (kirbyX > x);

    if (fireBall.active) {
        fireBall.fx += fireBall.fvx;
        if (std::abs(fireBall.fx - x) > 600) fireBall.active = false;
    }

    int distanceX = std::abs(kirbyX - x);
    int distanceY = std::abs(kirbyY - y);

    // 7.4 攻擊模式切換：太近就噴火，500px內水平前方就吐火球 [cite: 14, 44]
    if (distanceX < 120 && distanceY < 50) {
        state = 2; // Flame Breath 近距離噴火
    } else if (distanceX < 500 && distanceY < 50 && ((isFacingRight && kirbyX > x) || (!isFacingRight && kirbyX < x))) {
        state = 1; // Fire Ball 遠距離火球
    } else {
        state = 0; // 平時巡邏
    }

    if (state == 0) {
        isBreathingFire = false;
        x += vx;
        if (std::abs(x - startX) > 100) vx = -vx;
    }
    else if (state == 1) {
        isBreathingFire = false;
        if (actionTimer % 90 == 0 && !fireBall.active) {
            fireBall.active = true;
            fireBall.fx = isFacingRight ? (x + width) : (x - 20);
            fireBall.fy = y + height / 3;
            fireBall.fvx = isFacingRight ? 6 : -6;
        }
    }
    else if (state == 2) {
        isBreathingFire = true; // 7.4 停止移動並朝前方噴火 [cite: 14, 44]
        if (actionTimer % 120 == 0) state = 0; // 結束後恢復原本移動
    }
}

void HotHead::draw(QPainter &painter) {
    if (isDead) return;
    QPixmap img(":/Image/Hot Head/Hot_head_stop_L.png");
    painter.drawPixmap(x, y, width, height, img);

    if (state == 2) { // 畫近距離噴火範圍
        painter.setBrush(QBrush(QColor(255, 69, 0, 180)));
        if (isFacingRight) painter.drawRect(x + width, y, 80, height);
        else painter.drawRect(x - 80, y, 80, height);
    }
    if (fireBall.active) { // 畫飛行火球
        painter.setBrush(QBrush(QColor(255, 165, 0)));
        painter.drawEllipse(fireBall.fx, fireBall.fy, 24, 24);
    }
}

// ==================
// Sparky 電火花怪實作
// ==================
Sparky::Sparky(int startX, int startY)
    : Enemy(startX, startY, "Sparky", "Spark", true),
      timer(0), jumpTimer(0), isGrounded(true), isDischarging(false) {
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
            currentImg = (vx < 0) ? QPixmap(":/Image/Sparky/Sparky_left_1.png")
                                  : QPixmap(":/Image/Sparky/Sparky_ritht_1.png");
        } else {
            // 跳躍中 (Sparky_left_2 或 right_2)
            currentImg = (vx < 0) ? QPixmap(":/Image/Sparky/Sparky_left_2.png")
                                  : QPixmap(":/Image/Sparky/Sparky_ritht_2.png");
        }
        painter.drawPixmap(x, y, width, height, currentImg);
    }
}
