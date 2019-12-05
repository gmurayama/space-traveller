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

    void readOFFFile(const QString &fileName, QString shaderName);
    void drawModel(Camera camera, QVector3D position, float scale, QVector3D rotation, float angle);
    void loadTexture(const QImage &image);
    void loadTextureLayer(const QImage &image);
    void setLight(Light light);

    GLuint textureID = 0;
    GLuint textureLayerID = 0;

private:
    void normalizeModel();
    void createVBOs();
    void createShaders(QString shaderName);
    void createNormals();
    void destroyVBOs();
    void destroyShaders();
    void createTangents();
    void createTexCoords();

    QOpenGLWidget *glWidget;

    QVector3D midPoint;

    GLuint shaderProgram;
    GLuint vao = 0;

    GLuint vboVertices = 0;
    GLuint vboNormals = 0;
    GLuint vboIndices = 0;
    GLuint vboTexCoords = 0;
    GLuint vboTangents = 0;

    unsigned int numVertices = 0, numFaces = 0;

    std::unique_ptr<QVector4D []> vertices = nullptr;
    std::unique_ptr<QVector3D []> normals = nullptr;
    std::unique_ptr<unsigned int[]> indices = nullptr;
    std:: unique_ptr<QVector4D []> tangents = nullptr;
    std::unique_ptr<QVector2D []> texCoords = nullptr;
};

#endif // MODEL_H
