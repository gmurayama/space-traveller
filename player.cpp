#include "player.h"

Player::Player()
{
}

float Player::getPosXOffsetSum()
{
    return positionOffsetLeft + positionOffsetRight;
}

float Player::getPosYOffsetSum()
{
    return positionOffsetUp + positionOffsetDown;
}

float Player::drawModel(Camera camera)
{
    if (!isDead)
    {
        model->drawModel(camera, position, rotation, angle);
    }
}

// QVector3D rotation = QVector3D(0, -1.0f, 0);
float Player::updatePosition(float offsetMultiplicationFactor)
{
    float x = position.x(), y = position.y();

    position.setX(x + getPosXOffsetSum() * offsetMultiplicationFactor);
    position.setY(y + getPosYOffsetSum() * offsetMultiplicationFactor);

    angle = 90 + getPosXOffsetSum() * (3.0f + offsetMultiplicationFactor);
}
