#ifndef OPENGLWIDGET_H
#define OPENGLWIDGET_H

#include <QtOpenGL>
#include <QOpenGLExtraFunctions>

#include <memory>

#define NUM_STARS 500

class OpenGLWidget : public QOpenGLWidget, public QOpenGLExtraFunctions
{
    Q_OBJECT

public:
    explicit OpenGLWidget(QWidget *parent = nullptr);

    void createVBOs();
    void createShaders();

    void destroyVBOs();
    void destroyShaders();

    void readOFFFile(const QString &fileName);
    void normalizeModel();
    void createNormals();

protected:
    void initializeGL();
    void resizeGL(int width, int height);
    void paintGL();
    void keyPressEvent(QKeyEvent *event);
    void keyReleaseEvent(QKeyEvent *event);

private:
    GLuint vboVertices = 0;
    GLuint vboNormals = 0;
    GLuint vboIndices = 0;

    GLuint vao = 0;
    GLuint shaderProgram;

    unsigned int numVertices = 0, numFaces = 0;
    std::unique_ptr<QVector4D []> vertices = nullptr;
    std::unique_ptr<QVector3D []> normals = nullptr;
    std::unique_ptr<unsigned int[]> indices = nullptr;

    QMatrix4x4 mProjection;

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
