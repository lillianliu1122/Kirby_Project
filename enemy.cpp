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
