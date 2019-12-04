#ifndef MODEL_H
#define MODEL_H

#include <QtOpenGL>
#include <QOpenGLWidget>
#include <QOpenGLExtraFunctions>
#include <QTextStream>
#include <QFile>

#include <fstream>
#include <limits>
#include <iostream>
#include <memory>

#include "camera.h"
#include "light.h"

class Model : public QOpenGLExtraFunctions
{
public:
    Model(QOpenGLWidget *_glWidget);
    ~Model();

    void readOFFFile(const QString &fileName);
    void drawModel(Camera camera, QVector3D position, QVector3D rotation, float angle);
    void setLight(Light light);

    GLuint shaderProgram;
    GLuint vao = 0;

private:
    void normalizeModel();
    void createVBOs();
    void createShaders();
    void createNormals();
    void destroyVBOs();
    void destroyShaders();

    QOpenGLWidget *glWidget;

    GLuint vboVertices = 0;
    GLuint vboNormals = 0;
    GLuint vboIndices = 0;

    unsigned int numVertices = 0, numFaces = 0;

    std::unique_ptr<QVector4D []> vertices = nullptr;
    std::unique_ptr<QVector3D []> normals = nullptr;
    std::unique_ptr<unsigned int[]> indices = nullptr;
};

#endif // MODEL_H
