#ifndef OPENGLWIDGET_H
#define OPENGLWIDGET_H

#include <QtOpenGL>
#include <QOpenGLExtraFunctions>

#include <memory>

#include "model.h"
#include "camera.h"

#define NUM_STARS 500

class OpenGLWidget : public QOpenGLWidget, public QOpenGLExtraFunctions
{
    Q_OBJECT

public:
    explicit OpenGLWidget(QWidget *parent = nullptr);

protected:
    void initializeGL();
    void resizeGL(int width, int height);
    void paintGL();

private:
    std::shared_ptr<Model> model;
    Camera camera;

    QVector3D starPos[NUM_STARS], starRot[NUM_STARS];

    QTimer timer;
    QTime time1, time2;

    double fpsCounter = 0;
    float angle = 0;

signals:
    void setLabelText(QString);

public slots:
    void animate();
};

#endif // OPENGLWIDGET_H
