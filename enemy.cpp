#include "enemy.h"
#include <cmath>

// Enemy 建構子
Enemy::Enemy(int startX, int startY, QString type, QString cap, bool inhale)
    : startX(startX), startY(startY),
      isDead(false), isActive(true),
      x(startX), y(startY),
      vx(0), vy(0), width(100), height(100),
      type(type), capability(cap), canBeInhaled(inhale) {}

// 碰撞地形檢測
void Enemy::checkWallCollision(const QVector<Platform>& platforms) {
    QRect enemyRect = getCollisionBox(); // 獲取當前矩形

    for (const auto &p : platforms) {
        if (!p.visible) continue;
        QRect pRect = p.getRect().toRect();

        // 只有在碰撞時才進行處理
        if (enemyRect.intersects(pRect)) {
            // 計算兩個矩形重疊的區域
            QRect overlap = enemyRect.intersected(pRect);

            // 透過重疊區域判斷是水平碰撞還是垂直碰撞
            // 若重疊寬度 < 高度，視為水平碰撞
            if (overlap.width() < overlap.height()) {
                if (vx > 0) { // 原本向右，撞牆後修正位置並向左反彈
                    x -= overlap.width();
                    vx = -vx;
                } else if (vx < 0) { // 原本向左，撞牆後修正位置並向右反彈
                    x += overlap.width();
                    vx = -vx;
                }
            }
        }
    }
}
Enemy::~Enemy() {}

QRect Enemy::getCollisionBox() const { return QRect(x, y, width, height); }
bool Enemy::getIsDead() const { return isDead; }

// 傷害處理
void Enemy::takeDamage() {
    if (type != "Gordo") isDead = true; // Gordo 不會因攻擊而死亡
}

// 場景管理
void Enemy::checkRespawn(int cameraX, int screenWidth) {
    // 離開有效場景區域後移除，卡比返回時重新生成
    if (x + width < cameraX - 200 || x > cameraX + screenWidth + 200) {
        isActive = false;
        // 若敵人已死亡，當 kirby 離去再回來時，自動復活至初始點
        if (isDead) {
            x = startX; y = startY;
            isDead = false;
        }
    } else {
        isActive = true;
    }
}
