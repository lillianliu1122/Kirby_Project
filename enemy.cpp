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

// Waddle Dee 走路怪實作
WaddleDee::WaddleDee(int startX, int startY, int range)
    : Enemy(startX, startY, "WaddleDee", "None", true), walkRange(range) {
    vx = -2; // 初始往左走
}

void WaddleDee::updateBehavior(int kirbyX, int kirbyY) {
    Q_UNUSED(kirbyX); Q_UNUSED(kirbyY);
    if (isDead) return;

    // 1. 簡易重力模擬
    if (!isGrounded) {
        vy += 0.6f; // 重力加速度
    }

    // 2. 移動座標
    x += vx;
    y += vy;

    // 3. 地板偵測（假設地板高度為 800）
    // 你可以根據該關卡的實際地形高度設定，或者之後改為動態偵測平台
    if (y >= 800) {
        y = 800;
        vy = 0;
        isGrounded = true;
    } else {
        isGrounded = false;
    }
}

void WaddleDee::draw(QPainter &painter) {
    if (isDead) return;
    QPixmap img(":/Image/enemies/waddledee.png");
    painter.drawPixmap(x, y, width, height, img);
}

// Gordo 刺球障礙物實作
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
    QPixmap img(":/Image/enemies/gordo.png");
    painter.drawPixmap(x, y, width, height, img);
}

// Hot Head 噴火怪實作
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
    QPixmap img(":/Image/enemies/hothead.png");
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

// Sparky 電火花怪實作
Sparky::Sparky(int startX, int startY)
    : Enemy(startX, startY, "Sparky", "Spark", true),
      timer(0), jumpTimer(0), isGrounded(true), isDischarging(false) {
    vx = -2; vy = 0;
}

void Sparky::updateBehavior(int kirbyX, int kirbyY) {
    Q_UNUSED(kirbyX);
    Q_UNUSED(kirbyY);
    if (isDead) return;
    timer++; jumpTimer++;

    // 7.4 週期性在自身周圍產生電流攻擊範圍 [cite: 14, 44]
    if (timer % 180 < 60) {
        isDischarging = true; // 放電時站在原地
    } else {
        isDischarging = false;
        // 7.4 以小幅跳躍（蹦蹦跳）方式移動 [cite: 14, 44]
        if (isGrounded && jumpTimer % 45 == 0) {
            vy = -10;
            isGrounded = false;
        }
    }

    if (!isGrounded) {
        vy += 1; // 模擬重力
        x += vx;
    }
    y += vy;

    if (y >= startY) { // 回到地表
        y = startY; vy = 0; isGrounded = true;
        if (x < startX - 150 || x > startX + 150) vx = -vx;
    }
}

void Sparky::draw(QPainter &painter) {
    if (isDead) return;
    QPixmap img(":/Image/enemies/sparky.png");
    painter.drawPixmap(x, y, width, height, img);

    if (isDischarging) { // 畫圓形電流範圍
        painter.setBrush(QBrush(QColor(0, 191, 255, 100)));
        painter.setPen(QPen(Qt::white, 2, Qt::DashLine));
        painter.drawEllipse(x - 20, y - 20, width + 40, height + 40);
        painter.setPen(Qt::NoPen);
    }
}
