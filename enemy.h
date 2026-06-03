#ifndef ENEMY_H
#define ENEMY_H

#include <QPainter>
#include <QRect>
#include <QString>
#include "platform.h"

class Enemy {
protected:
    int startX, startY; // 記錄初始位置，用於回頭時重新生成
    bool isDead;        // 是否被消滅
    bool isActive;      // 是否在有效場景區域內

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

    // 子類別自定義
    virtual void updateBehavior(int kirbyX, int kirbyY) = 0;
    virtual void draw(QPainter &painter) = 0;
    virtual void checkWallCollision(const QVector<class Platform>& platforms);

    QRect getCollisionBox() const;
    void takeDamage();
    bool getIsDead() const;
    void checkRespawn(int cameraX, int screenWidth);
};
#endif // ENEMY_H
