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

class Model : public QOpenGLExtraFunctions
{
public:
    Model(QOpenGLWidget *_glWidget);
    ~Model();

    QOpenGLWidget *glWidget;
    void drawModel(Camera camera, QVector3D position, QVector3D rotation, float angle);

    void createVBOs();
    void createShaders();

    void destroyVBOs();
    void destroyShaders();

    void readOFFFile(const QString &fileName);
    void normalizeModel();
    void createNormals();

    unsigned int numVertices = 0, numFaces = 0;
    GLuint shaderProgram;
    GLuint vao = 0;

private:
    GLuint vboVertices = 0;
    GLuint vboNormals = 0;
    GLuint vboIndices = 0;

    std::unique_ptr<QVector4D []> vertices = nullptr;
    std::unique_ptr<QVector3D []> normals = nullptr;
    std::unique_ptr<unsigned int[]> indices = nullptr;
};

#endif // MODEL_H
