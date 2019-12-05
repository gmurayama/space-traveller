#ifndef LIGHT_H
#define LIGHT_H

#include <QVector4D>

class Light
{
public:
    Light();

    QVector4D position = QVector4D(0.0f, 0.0f, 0.0f, 0);
    QVector4D ambient = QVector4D(0.1f, 0.1f, 0.2f, 1);
    QVector4D diffuse = QVector4D(1.0f, 1.0f, 0.7f, 1);
    QVector4D specular = QVector4D(0.05f, 0.05f, 0.05f, 1);
};

#endif // LIGHT_H
