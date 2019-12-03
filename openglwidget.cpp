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
    model->normalizeModel();
    model->createNormals();
    model->createShaders();
    model->createVBOs();

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
    float aspectRatio = (float)width / (float)height;

    mProjection.setToIdentity();
    mProjection.perspective(20.0f, aspectRatio, 0.01f, 100.0f);
}

void OpenGLWidget::paintGL()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    GLint locProjectionMatrix = glGetUniformLocation(model->shaderProgram, "mProjection");
    GLint locModelViewMatrix = glGetUniformLocation(model->shaderProgram, "mModelView");
    GLint locNormalMatrix = glGetUniformLocation(model->shaderProgram, "mNormal");
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

    QMatrix4x4 mView;
    mView.setToIdentity();
    mView.lookAt(QVector3D(0.0f, 0.0f, 0.0f),
                 QVector3D(0.0f, 0.0f, -1.0f),
                 QVector3D(0.0f, 1.0f, 0.0f));

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
        QMatrix4x4 mModel;
        mModel.translate(starPos[i]);

        QMatrix4x4 mModelView;
        mModel.scale(0.2f);
        mModel.rotate(angle, starRot[i]);
        mModelView = mView * mModel;

        glUniformMatrix4fv(locProjectionMatrix, 1, GL_FALSE, mProjection.data());
        glUniformMatrix4fv(locModelViewMatrix, 1, GL_FALSE, mModelView.data());
        glUniformMatrix3fv(locNormalMatrix, 1, GL_FALSE, mModelView.normalMatrix().data());

        glDrawElements(GL_TRIANGLES, model->numFaces*3, GL_UNSIGNED_INT, nullptr);
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
