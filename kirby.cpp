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
const int Kirby::FIRE_CH = 90;

Kirby::Kirby()
    : x(100), y(600), vx(0), vy(0),
      onGround(false), facingRight(true),
      wantsToSpitStar(false), isFlying(false), state(KirbyState::Idle), animFrame(0),
      animCounter(0), flyCount(0), isMouthful(false), inhaledType("")
{
    loadImages();
    ability = KirbyAbility::None;
    isUsingAbility = false;
    wantsFireAttack = false;
    wantsSparkAttack = false;
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

    // Fire
    imgFire_stop_R << QPixmap(":/Image/Kirby_fire/kirbyfire_stop_R.png");
    imgFire_stop_L << QPixmap(":/Image/Kirby_fire/kirbyfire_stop_L.png");
    imgFire_run_R  << QPixmap(":/Image/Kirby_fire/kirbyfire_run(1)_R.png")
                   << QPixmap(":/Image/Kirby_fire/kirbyfire_run(2)_R.png")
                   << QPixmap(":/Image/Kirby_fire/kirbyfire_run(3)_R.png");
    imgFire_run_L  << QPixmap(":/Image/Kirby_fire/kirbyfire_run(1)_L.png")
                   << QPixmap(":/Image/Kirby_fire/kirbyfire_run(2)_L.png")
                   << QPixmap(":/Image/Kirby_fire/kirbyfire_run(3)_L.png");
    imgFire_fly_R  << QPixmap(":/Image/Kirby_fire/kirbyfire_fly(1)_R.png")
                   << QPixmap(":/Image/Kirby_fire/kirbyfire_fly(2)_R.png");
    imgFire_fly_L  << QPixmap(":/Image/Kirby_fire/kirbyfire_fly(1)_L.png")
                   << QPixmap(":/Image/Kirby_fire/kirbyfire_fly(2)_L.png");
    imgFire_atk_R << QPixmap(":/Image/Kirby_fire/kirbyfire_attack_R.png");
    imgFire_atk_L << QPixmap(":/Image/Kirby_fire/kirbyfire_attack_L.png");
    imgFire_down_R << QPixmap(":/Image/Kirby_fire/kirbyfire_down_R.png");
    imgFire_down_L << QPixmap(":/Image/Kirby_fire/kirbyfire_down_L.png");

    // Spark
    imgSpark_stop_R << QPixmap(":/Image/Kirby_spark/Kirby_spark_stop_R.png");
    imgSpark_stop_L << QPixmap(":/Image/Kirby_spark/Kirby_spark_stop_L.png");
    imgSpark_run_R  << QPixmap(":/Image/Kirby_spark/Kirby_spark_run(1)_R.png")
                    << QPixmap(":/Image/Kirby_spark/Kirby_spark_run(2)_R.png");
    imgSpark_run_L  << QPixmap(":/Image/Kirby_spark/Kirby_spark_run(1)_L.png")
                    << QPixmap(":/Image/Kirby_spark/Kirby_spark_run(2)_L.png");
    imgSpark_fly_R  << QPixmap(":/Image/Kirby_spark/Kirby_spark_fly(1)_R.png")
                    << QPixmap(":/Image/Kirby_spark/Kirby_spark_fly(2)_R.png");
    imgSpark_fly_L  << QPixmap(":/Image/Kirby_spark/Kirby_spark_fly(1)_L.png")
                    << QPixmap(":/Image/Kirby_spark/Kirby_spark_fly(2)_L.png");
    imgSpark_atk    << QPixmap(":/Image/Kirby_spark/Kirby_spark_attack(1).png")
                    << QPixmap(":/Image/Kirby_spark/Kirby_spark_attack(2).png")
                    << QPixmap(":/Image/Kirby_spark/Kirby_spark_attack(3).png");
    imgSpark_down_R << QPixmap(":/Image/Kirby_spark/Kirby_spark_down_R.png");
    imgSpark_down_L << QPixmap(":/Image/Kirby_spark/Kirby_spark_down_L.png");

}

void Kirby::update(const QSet<int> &keys, const QSet<int> &justPressed)
{
    updateInvincible(); // 處理受傷後的無敵時間

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

    // fire ability
    if (ability == KirbyAbility::Fire && isUsingAbility) {
        vx = 0;
        vy += GRAVITY;
        x += vx;
        y += vy;
        if (justPressed.contains(Qt::Key_V)) dropAbility();
        state = KirbyState::Idle;
        updateAnimation();
        return;
    }


    // spark ability
    if (justPressed.contains(Qt::Key_X) && ability == KirbyAbility::Spark) {
        isUsingAbility = true;
        wantsSparkAttack = true;
    }
    // Spark：長按時持續維持攻擊狀態
    if (keys.contains(Qt::Key_X) && ability == KirbyAbility::Spark && isUsingAbility) {
        wantsSparkAttack = true;
    }

    // Spark 使用中：禁止移動
    if (ability == KirbyAbility::Spark && isUsingAbility) {
        vx = 0;
        vy += GRAVITY;
        x += vx;
        y += vy;
        if (justPressed.contains(Qt::Key_V)) dropAbility();
        state = KirbyState::Idle;
        updateAnimation();
        return;
    }

    // === 以下是正常狀態邏輯 ===
    {
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

    // 蹲下
    if (keys.contains(Qt::Key_Down) && onGround) {
        state = KirbyState::Crouch;
        vx = 0;
    }
    // 吐氣
    else if (justPressed.contains(Qt::Key_X) && isFlying) {
        isFlying = false;
        wantsPuffAttack = true;
        vy += GRAVITY;
        x += vx;
        y += vy;
        state = KirbyState::Idle;
        updateAnimation();
        return;
    }
    // 跳躍與飛行
    else if (keys.contains(Qt::Key_Up)) {
        if (onGround) {
            vy = JUMP_FORCE;
            onGround = false;
            isFlying = false;
            flyCount = 0;
            upHoldTimer = 0;
        } else if (!isFlying) {
            upHoldTimer++;
            if (upHoldTimer >= 5) { // 按住 5 幀後進入飛行
                isFlying = true;
                vy = -8.0f;
            }
        } else {
            vy = -4.0f;
        }
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
    }

    // 狀態更新
    if (keys.contains(Qt::Key_X)) {
        state = KirbyState::Inhaling;
    } else if (isFlying) {
        state = KirbyState::Fly;
    } else if (!onGround) {
        state = isFlying ? KirbyState::Fly : KirbyState::Jump;
    } else if (keys.contains(Qt::Key_Down)) {
        state = KirbyState::Crouch;
    } else if (vx != 0) {
        state = KirbyState::Run;
    } else {
        state = KirbyState::Idle;
    }

    // 棄置能力（按 V）
    if (justPressed.contains(Qt::Key_V)) {
        dropAbility();
    }

    updateAnimation();


}

void Kirby::updateAnimation()
{
    // 透過累加器達到每 8 個遊戲週期切換一次圖片
    animCounter++;
    if (animCounter >= 8) {
        animCounter = 0;
        animFrame++;
    }

    int maxFrames = 1;

    if (ability == KirbyAbility::Fire) {
        switch (state) {
            case KirbyState::Run:  maxFrames = 3; break;
            case KirbyState::Fly:
            case KirbyState::Jump: maxFrames = 2; break;
            default:               maxFrames = 1; break;
        }
    } else if (ability == KirbyAbility::Spark) {
        if (isUsingAbility) {
            maxFrames = 2;  // 攻擊時在 index 1, 2 之間切換，所以需要 2 幀
        } else {
            switch (state) {
                case KirbyState::Run:  maxFrames = 2; break;
                case KirbyState::Fly:
                case KirbyState::Jump: maxFrames = 2; break;
                case KirbyState::Inhaling: maxFrames = 3; break;
                default:               maxFrames = 1; break;
            }
        }
    } else {
        switch (state) {
            case KirbyState::Run:  maxFrames = 4; break;
            case KirbyState::Jump: maxFrames = 3; break;
            case KirbyState::Fly:  maxFrames = 2; break;
            default:               maxFrames = 1; break;
        }
    }

    if (animFrame >= maxFrames) animFrame = 0;
}

QPixmap Kirby::currentFrame() const
{
    // Fire 能力
    if (ability == KirbyAbility::Fire) {
        if (isUsingAbility)
            return facingRight ? imgFire_atk_R[0] : imgFire_atk_L[0];
        switch (state) {
            case KirbyState::Run:
                return facingRight ? imgFire_run_R[animFrame % imgFire_run_R.size()]
                                   : imgFire_run_L[animFrame % imgFire_run_L.size()];
            case KirbyState::Fly: case KirbyState::Jump:
                return facingRight ? imgFire_fly_R[animFrame % imgFire_fly_R.size()]
                                   : imgFire_fly_L[animFrame % imgFire_fly_L.size()];
            case KirbyState::Crouch:
                return facingRight ? imgFire_down_R[0] : imgFire_down_L[0];
            default:
                return facingRight ? imgFire_stop_R[0] : imgFire_stop_L[0];
        }
    }

    // Spark 能力
    if (ability == KirbyAbility::Spark) {
        if (isUsingAbility){
            int frame = 1 + (animFrame % 2);
            return imgSpark_atk[frame];
            //return imgSpark_atk[animFrame % imgSpark_atk.size()];
        }
        switch (state) {
            case KirbyState::Run:
                return facingRight ? imgSpark_run_R[animFrame % imgSpark_run_R.size()]
                                   : imgSpark_run_L[animFrame % imgSpark_run_L.size()];
            case KirbyState::Fly: case KirbyState::Jump:
                return facingRight ? imgSpark_fly_R[animFrame % imgSpark_fly_R.size()]
                                   : imgSpark_fly_L[animFrame % imgSpark_fly_L.size()];
            case KirbyState::Crouch:
                return facingRight ? imgSpark_down_R[0] : imgSpark_down_L[0];
            case KirbyState::Inhaling:
                return imgSpark_atk[animFrame % imgSpark_atk.size()];
            default:
                return facingRight ? imgSpark_stop_R[0] : imgSpark_stop_L[0];
        }
    }

    // 原本的 Normal 狀態（不變）
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
        default:
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

    // Fire 能力時高度加大，讓火焰不被裁切
    int drawW = KIRBY_W;
    int drawH = KIRBY_H;
    int drawY = (int)y;

    // Fire 攻擊時：分開畫 kirby 本體 + 火焰
    if (ability != KirbyAbility::None) {
        drawH = KIRBY_H * 1.5f;
        drawW = KIRBY_W * 1.0f;
        drawY = (int)y - (drawH - KIRBY_H);
    }

    if (!frame.isNull()) {
        // spark ability
        if (ability == KirbyAbility::Spark && isUsingAbility) {
            // 電流圖片大小與判定範圍一致
            int sparkRadius = 80;
            int sparkW = KIRBY_W + sparkRadius * 2;  // 260
            int sparkH = KIRBY_H + sparkRadius * 2;  // 260
            int sparkX = drawX - sparkRadius;         // 向左偏移讓圖片置中
            int sparkY = (int)y - sparkRadius;        // 向上偏移讓圖片置中
            painter.drawPixmap(sparkX, sparkY, sparkW, sparkH, frame);
        }
        // 蹲下
        else if (state == KirbyState::Crouch) {
            if(ability != KirbyAbility::None){
                int crouchH = FIRE_CH;
                int crouchY = (int)y + (KIRBY_H - crouchH);
                painter.drawPixmap(drawX, crouchY, drawW, crouchH, frame);
            }
            else{
                painter.drawPixmap(drawX, drawY + 4*(KIRBY_H - KIRBY_CH), drawW, KIRBY_CH, frame);
            }
        }
        else {
            painter.drawPixmap(drawX, drawY, drawW, drawH, frame);
        }
    } else {
        // 圖片載入失敗時顯示粉紅色方塊（debug 用）
        painter.setBrush(QColor(255, 105, 180));
        painter.drawRect(drawX, (int)y, KIRBY_W, KIRBY_H);
    }
}

// 地形碰撞判定
QRectF Kirby::getRect() const
{
    return QRectF(x, y, KIRBY_W, KIRBY_H);
}

// 受傷
void Kirby::takeDamage() {
    if (isInvincible) return; // 若處於無敵狀態，忽略傷害

    hp--;
    startInvincible(); // 觸發無敵狀態
    if (hp < 0) hp = 0;
}

// 無敵狀態啟動
void Kirby::startInvincible() {
    isInvincible = true;
    invincibleTimer = 120; // 持續 120 幀 (約 2 秒)
}

// 無敵倒數
void Kirby::updateInvincible() {
    if (isInvincible) {
        invincibleTimer--;
        if (invincibleTimer <= 0) {
            isInvincible = false; // 無敵時間結束
        }
    }
}

bool Kirby::isInhaling() const
{
    return state == KirbyState::Inhaling;
}

// 吸入判定
QRectF Kirby::getInhaleRect() const
{
    // 吸力範圍：在 Kirby 前方約一個身長
    if (facingRight)
        return QRectF(x + KIRBY_W, y, KIRBY_W * 2, KIRBY_H);
    else
        return QRectF(x - KIRBY_W * 2, y, KIRBY_W * 2, KIRBY_H);
}

// 吸入敵人狀態轉換
void Kirby::inhaleEnemy(QString enemyType)
{
    isMouthful = true;
    inhaledType = enemyType;
    state = KirbyState::Mouthful;
}

// 吞下敵人
void Kirby::swallow()
{
    if (!isMouthful) return;
    if (inhaledType == "Fire") {
        ability = KirbyAbility::Fire;
    } else if (inhaledType == "Spark") {
        ability = KirbyAbility::Spark;
    } else {
        ability = KirbyAbility::None;
    }
    isMouthful = false;
    inhaledType = "";
    state = KirbyState::Idle;
}

// 吐出星星
void Kirby::spitStar()
{
    if (!isMouthful) return;
    isMouthful = false;
    inhaledType = "";
    state = KirbyState::Idle;
    wantsToSpitStar = true;
}

// 使用 fire / spark ability
void Kirby::useAbility()
{
    if (isUsingAbility) return;
    if (ability == KirbyAbility::Fire) {
        isUsingAbility = true;
        wantsFireAttack = true;
    } else if (ability == KirbyAbility::Spark) {
        isUsingAbility = true;
        wantsSparkAttack = true;
    }
}

// 棄置能力
void Kirby::dropAbility()
{
    ability = KirbyAbility::None;
    isUsingAbility = false;
}
