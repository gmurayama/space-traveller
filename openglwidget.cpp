#include <QFile>
#include <QTextStream>
#include <fstream>
#include <limits>
#include <iostream>

#include "openglwidget.h"
#include "model.h"
 #include <QDebug>

OpenGLWidget::OpenGLWidget(QWidget *parent) : QOpenGLWidget(parent)
{
}

void OpenGLWidget::initializeGL()
{
    initializeOpenGLFunctions();
    glClearColor(0.0f, 0.0, 0.0f, 1);

    qDebug("OpenGL version: %s", glGetString(GL_VERSION));
    qDebug("GLSL %s", glGetString(GL_SHADING_LANGUAGE_VERSION));

    player.model = std::make_shared<Model>(this);
    asteroidModel = std::make_shared<Model>(this);

    light.ambient = QVector4D(0.0f, 0.0f, 0.2f, 1) * QVector4D(1.0f, 1.0f, 1.0f, 1);
    light.diffuse = QVector4D(1.0f, 1.0f, 0.7f, 1) * QVector4D(0.9f, 0.8f, 0.8f, 1);
    light.specular = QVector4D(0.0f, 0.0f, 0.0f, 1) * QVector4D(1.0f, 1.0f, 1.0f, 1);
    light.position = QVector4D(0.0f, 0.0f, 0.0f, 0);

    player.model->readOFFFile(":/models/spaceship.off");
    asteroidModel->readOFFFile(":/models/cube.off");

    for(int i=0; i<NUM_STARS; ++i)
    {
        QVector3D pos = starPos[i];
        pos.setX(((qrand() / (float)RAND_MAX) * 20.0f) - 10.0f);
        pos.setY(((qrand() / (float)RAND_MAX) * 20.0f) - 10.0f);
        pos.setZ(-((qrand() / (float)RAND_MAX) * 100.0f));
        starPos[i] = pos;

        float x = (qrand() / (float)RAND_MAX) * 2.0f - 1.0f;
        float y = (qrand() / (float)RAND_MAX) * 2.0f - 1.0f;
        float z = (qrand() / (float)RAND_MAX) * 2.0f - 1.0f;
        starRot[i] = QVector3D(x,y,z).normalized();
    }

    time1.start();
    time2.start();
    timeSpeed.start();
    timer.start(0);
    connect(&timer, SIGNAL(timeout()), this, SLOT(animate()));

    glEnable(GL_DEPTH_TEST);
}

void OpenGLWidget::resizeGL(int width, int height)
{
    glViewport(0, 0, width, height);
    camera.resizeViewPort(width, height);
    update();
}

void OpenGLWidget::paintGL()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    fpsCounter++;

    if(time1.elapsed() > 1000)
    {
        double fps = fpsCounter * 1000.0 / time1.elapsed();
        QString str;
        str.sprintf("FPS: %.2f", fps);
        emit setLabelText(str);
        fpsCounter = 0;
        time1.restart();
    }

    player.model->setLight(light);
    player.drawModel(camera);

    asteroidModel->setLight(light);

    for(int i=0; i<NUM_STARS; ++i)
    {
        asteroidModel->drawModel(camera, starPos[i], starRot[i], angle);
    }
}

void OpenGLWidget::animate()
{
    float factor = time2.elapsed() / 500.0f;
    player.updatePosition(factor);

    camera.center.setX(camera.center.x() + player.getPosXOffsetSum() * factor);
    camera.center.setY(camera.center.y() + player.getPosYOffsetSum() * factor);
    camera.eye.setX(camera.eye.x() + player.getPosXOffsetSum() * factor);
    camera.eye.setY(camera.eye.y() + player.getPosYOffsetSum() * factor);
    camera.computeViewMatrix();

    for(int i=0; i<NUM_STARS; ++i)
    {
        if (player.position.y() > starPos[i].y() - 0.25f &&
            player.position.y() < starPos[i].y() + 0.25f &&
            player.position.x() > starPos[i].x() - 0.25f &&
            player.position.x() < starPos[i].x() + 0.25f &&
            player.position.z() > starPos[i].z() - 0.25f &&
            player.position.z() < starPos[i].z() + 0.25f)
        {
            player.isDead = true;
        }

        if (player.isAccelerating)
            player.acceleration = player.stoppedAcceleratingAt = timeSpeed.elapsed() / (std::log10(timeSpeed.elapsed()) * std::log10(timeSpeed.elapsed()));
        else
            player.acceleration = player.stoppedAcceleratingAt * std::pow(2, -1 * timeSpeed.elapsed() / 3.0f);

        if (player.acceleration >= 2)
            player.acceleration = 2;
        else if (player.acceleration <= time2.elapsed() / 100.0f)
            player.acceleration = time2.elapsed() / 100.0f;

        QVector3D pos = starPos[i];
        pos.setZ(pos.z() + player.acceleration);

        if (pos.z() > 1.0f)
        {
            pos.setX(camera.center.x() + ((qrand() / (float)RAND_MAX) * 30.0f) - 15.0f);
            pos.setY(camera.center.y() + ((qrand() / (float)RAND_MAX) * 30.0f) - 15.0f);
            pos.setZ(-100.0f);

            float x = (qrand() / (float)RAND_MAX) * 2.0f - 1.0f;
            float y = (qrand() / (float)RAND_MAX) * 2.0f - 1.0f;
            float z = (qrand() / (float)RAND_MAX) * 2.0f - 1.0f;
            starRot[i] = QVector3D(x,y,z).normalized();
        }

        starPos[i] = pos;
    }

    angle += time2.elapsed() / 5.0f;
    time2.restart();

    update();
}

void OpenGLWidget::keyPressEvent(QKeyEvent *event)
{
    if (event->key() == Qt::Key_D)
    {
        player.positionOffsetRight = 1.0f * player.speedDirections;
    }
    if (event->key() == Qt::Key_A)
    {
        player.positionOffsetLeft = -1.0f * player.speedDirections;
    }
    if (event->key() == Qt::Key_W)
    {
        player.positionOffsetUp = 1.0f * player.speedDirections;
    }
    if (event->key() == Qt::Key_S)
    {
        player.positionOffsetDown = -1.0f * player.speedDirections;
    }
    if (event->key() == Qt::Key_Space)
    {
        if (!player.isAccelerating)
        {
            player.isAccelerating = true;
            timeSpeed.restart();
        }
    }
}

void OpenGLWidget::keyReleaseEvent(QKeyEvent *event)
{
    if (event->key() == Qt::Key_D)
    {
        player.positionOffsetRight = 0;
    }
    if (event->key() == Qt::Key_A)
    {
        player.positionOffsetLeft = 0;
    }
    if (event->key() == Qt::Key_W)
    {
        player.positionOffsetUp = 0;
    }
    if (event->key() == Qt::Key_S)
    {
        player.positionOffsetDown = 0;
    }
    if(event->key() == Qt::Key_Space)
    {
        if (player.isAccelerating)
        {
            player.isAccelerating = false;
            timeSpeed.restart();
        }
    }
}
