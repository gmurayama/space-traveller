#ifndef PLAYER_H
#define PLAYER_H

#include <memory>

#include "model.h"

class Player
{
public:
    Player();
    std::shared_ptr<Model> model;

    QVector3D position = QVector3D(0, 0, -2);
    QVector3D rotation = QVector3D(0, -1.0f, 0);

    bool isAccelerating = false;
    bool isDead = false;
    float stoppedAcceleratingAt = 0;
    float acceleration = 0;
    float speedDirections = 3;
    float angle = 90;
    float positionOffsetLeft = 0, positionOffsetRight = 0, positionOffsetDown = 0, positionOffsetUp = 0;

    float getPosXOffsetSum();
    float getPosYOffsetSum();
    float drawModel(Camera camera);
    float updatePosition(float offsetMultiplicationFactor);
};

#endif // PLAYER_H
