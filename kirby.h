#ifndef KIRBY_CLASS_H
#define KIRBY_CLASS_H

#include <QRectF>
#include <QPixmap>
#include <QVector>
#include <QSet>

// Kirby 的動作狀態
enum class KirbyState {
    Idle,    // 靜止
    Run,     // 跑步
    Jump,    // 跳躍
    Crouch,  // 蹲下
    Fly,      // 飛行
    Inhaling,   // 吸入中
    Mouthful    // 已吸入敵人
};

enum class KirbyAbility {
    None,
    Fire,
    Spark
};

class Kirby
{
public:
    Kirby();

    //void update(const class QSet<int> &keys);  // 每幀更新位置
    void update(const QSet<int> &keys, const QSet<int> &justPressed);
    void draw(class QPainter &painter, float cameraX);
    QRectF getRect() const;                    // 取得碰撞框

    // 血量與生命系統
    int hp = 3;
    int lives = 3;
    bool isInvincible = false;
    int invincibleTimer = 0;

    void takeDamage();          // 受到傷害
    void startInvincible();     // 開啟無敵狀態
    void updateInvincible();    // 每幀更新無敵計時

    float x, y;         // 位置
    float vx, vy;       // 速度
    bool onGround;      // 是否站在地上
    bool facingRight;   // 面向右邊？
    static const int KIRBY_W;  // 顯示寬度
    static const int KIRBY_H;   // 一般高度（run 比例）
    static const int KIRBY_CH;   // 蹲下高度（down 比例）
    bool isInhaling() const;
    QRectF getInhaleRect() const;  // 吸力範圍
    void inhaleEnemy(QString enemyType);  // 吸入敵人
    void spitStar();   // 吐出星星
    void swallow();    // 吞下
    bool wantsToSpitStar;  // 通知 GameWindow 要生成星星彈
    KirbyAbility ability;
    bool isUsingAbility;    // 正在使用能力中
    void useAbility();      // 按 X 使用能力
    void dropAbility();     // 按 V 棄置能力
    bool wantsFireAttack;   // 通知 GameWindow 產生火焰
    bool wantsSparkAttack;  // 通知 GameWindow 產生電流

private:
    void loadImages();
    void updateAnimation();
    QPixmap currentFrame() const;

    KirbyState state;

    // 各動作的圖片序列
    QVector<QPixmap> imgIdle_L, imgIdle_R;
    QVector<QPixmap> imgRun_L,  imgRun_R;
    QVector<QPixmap> imgJump;
    QVector<QPixmap> imgCrouch_L, imgCrouch_R;
    QVector<QPixmap> imgFly_L, imgFly_R;
    QVector<QPixmap> imgInhale_L, imgInhale_R;
    QVector<QPixmap> imgMouthful_L, imgMouthful_R;

    // Fire 能力圖片
    QVector<QPixmap> imgFire_stop_L, imgFire_stop_R;
    QVector<QPixmap> imgFire_run_L,  imgFire_run_R;
    QVector<QPixmap> imgFire_fly_L,  imgFire_fly_R;
    QVector<QPixmap> imgFire_atk_L,  imgFire_atk_R;
    QVector<QPixmap> imgFire_down_L,  imgFire_down_R;

    // Spark 能力圖片
    QVector<QPixmap> imgSpark_stop_L, imgSpark_stop_R;
    QVector<QPixmap> imgSpark_run_L,  imgSpark_run_R;
    QVector<QPixmap> imgSpark_fly_L,  imgSpark_fly_R;
    QVector<QPixmap> imgSpark_atk;
    QVector<QPixmap> imgSpark_down_L, imgSpark_down_R;

    int animFrame;    // 目前播到第幾張
    int animCounter;  // 計數器，控制換圖速度
    bool isFlying;      // 是否在飛行中
    int flyCount;       // 已飛幾次（限制連續飛行次數）
    bool isMouthful;      // 是否吞著敵人
    QString inhaledType;  // 吸入的敵人種類（決定給什麼能力）

    static const float SPEED;
    static const float JUMP_FORCE;
    static const float GRAVITY;
};

#endif // KIRBY_H
