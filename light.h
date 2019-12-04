#ifndef LIGHT_H
#define LIGHT_H

#include <QVector3D>

class Light
{
public:
    Light();

    QVector3D position = QVector3D(0.0f, 0.0f, 0.0f);
    QVector3D ambient = QVector3D(0.0f, 0.0f, 0.2f);
    QVector3D diffuse = QVector3D(1.0f, 1.0f, 0.7f);
    QVector3D specular = QVector3D(0.0f, 0.0f, 0.0f);
};

#endif // LIGHT_H
