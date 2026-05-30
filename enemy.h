#ifndef ENEMY_H
#define ENEMY_H

#include <QPainter>
#include <QRect>
#include <QString>
#include "platform.h"

// ==========================================
// 1. 所有敵人的基底類別 (父類別) [cite: 6, 45]
// ==========================================
class Enemy {
protected:
    int startX, startY; // 記錄初始位置，用於回頭時重新生成 [cite: 46]
    bool isDead;        // 是否被消滅 [cite: 46]
    bool isActive;      // 是否在有效場景區域內 [cite: 46]

public:
    int x, y;
    int vx, vy;
    int width, height;
    bool isGrounded;
    QString type;       // "WaddleDee", "Gordo", "HotHead", "Sparky"
    QString capability; // "None", "Fire", "Spark"
    bool canBeInhaled;  // 是否可被吸入

    Enemy(int startX, int startY, QString type, QString cap, bool inhale);
    virtual ~Enemy();

    // 純虛擬函式：強迫子類別各自實作獨特 AI 行為 [cite: 46]
    virtual void updateBehavior(int kirbyX, int kirbyY) = 0;
    virtual void draw(QPainter &painter) = 0;
    virtual void checkWallCollision(const QVector<class Platform>& platforms);
    QRect getCollisionBox() const;
    void takeDamage();
    bool getIsDead() const;
    void checkRespawn(int cameraX, int screenWidth);
};
#endif // ENEMY_H
