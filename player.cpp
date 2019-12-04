#include "player.h"

Player::Player()
{
}

float Player::getPosXOffsetSum()
{
    return positionOffsetLeft + positionOffsetRight;
}

float Player::drawModel(Camera camera)
{
    model->drawModel(camera, position, rotation, angle);
}

float Player::updatePosition(float offsetMultiplicationFactor)
{
    float x = position.x(), y = position.y(), z = position.z();

    position.setX(x + getPosXOffsetSum() * offsetMultiplicationFactor);
}
