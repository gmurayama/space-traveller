#include <QFile>
#include <QTextStream>
#include <fstream>
#include <limits>
#include <iostream>

#include "openglwidget.h"
#include "model.h"

OpenGLWidget::OpenGLWidget(QWidget *parent) : QOpenGLWidget(parent)
{
}

void OpenGLWidget::initializeGL()
{
    initializeOpenGLFunctions();
    glClearColor(0.0f, 0.0, 0.0f, 1);

    qDebug("OpenGL version: %s", glGetString(GL_VERSION));
    qDebug("GLSL %s", glGetString(GL_SHADING_LANGUAGE_VERSION));

    model = std::make_shared<Model>(this);

    model->readOFFFile(":/models/cube.off");

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

    GLint locShininess = glGetUniformLocation(model->shaderProgram, "shininess");
    GLint locAmbient = glGetUniformLocation(model->shaderProgram, "Ia");
    GLint locDiffuse = glGetUniformLocation(model->shaderProgram, "Id");
    GLint locSpecular = glGetUniformLocation(model->shaderProgram, "Is");
    GLint locLightPos = glGetUniformLocation(model->shaderProgram, "lightPos");

    glBindVertexArray(model->vao);
    glUseProgram(model->shaderProgram);

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

    //QVector3D ambient = QVector3D(0.0f, 0.0f, 0.2f) * QVector3D(1.0f, 1.0f, 1.0f);
    //QVector3D diffuse = QVector3D(1.0f, 1.0f, 0.7f) * QVector3D(0.9f, 0.8f, 0.8f);
    //QVector3D specular = QVector3D(0.0f, 0.0f, 0.0f) * QVector3D(1.0f, 1.0f, 1.0f);
    //QVector3D lightPos = QVector3D(0.0f, 0.0f, 0.0f);

    float shininess = 128.0f;
    QVector3D ambient = QVector3D(0.0f, 0.0f, 0.2f) * QVector3D(1.0f, 1.0f, 1.0f);
    QVector3D diffuse = QVector3D(1.0f, 1.0f, 0.7f) * QVector3D(0.9f, 0.8f, 0.8f);
    QVector3D specular = QVector3D(0.0f, 0.0f, 0.0f) * QVector3D(1.0f, 1.0f, 1.0f);
    QVector3D lightPos = QVector3D(0.0f, 0.0f, 0.0f);

    glUniform3fv(locAmbient, 1, &(ambient[0]));
    glUniform3fv(locDiffuse, 1, &(diffuse[0]));
    glUniform3fv(locSpecular, 1, &(specular[0]));
    glUniform1f(locShininess, shininess);
    glUniform3fv(locLightPos, 1, &(lightPos[0]));

    for(int i=0; i<NUM_STARS; ++i)
    {
        model->drawModel(camera, starPos[i], starRot[i], angle);
    }
}

void OpenGLWidget::animate()
{   
    for(int i=0; i<NUM_STARS; ++i)
    {
        QVector3D pos = starPos[i];
        pos.setZ(pos.z() + time2.elapsed() / 100.0f);
        if (pos.z() > 1.0f)
        {
            pos.setX(((qrand() / (float)RAND_MAX) * 20.0f) - 10.0f);
            pos.setY(((qrand() / (float)RAND_MAX) * 20.0f) - 10.0f);
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
