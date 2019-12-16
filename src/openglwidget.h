#ifndef OPENGLWIDGET_H
#define OPENGLWIDGET_H

#include <QtOpenGL>
#include <QOpenGLExtraFunctions>

#include <memory>
#include <math.h>

#include "model.h"
#include "camera.h"
#include "light.h"
#include "player.h"

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

    void keyPressEvent(QKeyEvent *event);
    void keyReleaseEvent(QKeyEvent *event);

private:
    Player player;
    Camera camera;
    Light light;

    std::shared_ptr<Model> asteroidModel;
    std::shared_ptr<Model> moonModel;

    QVector3D starPos[NUM_STARS], starRot[NUM_STARS];

    QTimer timer;
    QTime time1, time2;
    QTime timeSpeed;


    double fpsCounter = 0;
    float angle = 0;

signals:
    void setLabelText(QString);

public slots:
    void animate();
};

#endif // OPENGLWIDGET_H
