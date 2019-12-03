#include "model.h"

Model::Model(QOpenGLWidget *_glWidget)
{
    glWidget = _glWidget;
    glWidget->makeCurrent();

    initializeOpenGLFunctions();
}

Model::~Model()
{
    destroyVBOs();
    destroyShaders();
}

void Model::readOFFFile(const QString &fileName)
{
    QFile file(fileName);
    file.open(QFile::ReadOnly | QFile::Text);

    QTextStream stream(&file);

    QString line;
    stream >> line;
    stream >> numVertices >> numFaces >> line;

    if (numVertices == 0)
        return;

    vertices = std::make_unique<QVector4D []>(numVertices);
    indices = std::make_unique<unsigned int[]>(numFaces * 3);

    float minLim = std::numeric_limits<float>::lowest();
    float maxLim = std::numeric_limits<float>::max();
    QVector4D max(minLim, minLim, minLim, 1.0);
    QVector4D min(maxLim, maxLim, maxLim, 1.0);

    // Read the list of vertices
    for (unsigned int i = 0; i < numVertices; ++i)
    {
        float x, y, z;
        stream >> x >> y >> z;
        vertices[i] = QVector4D(x, y, z, 1.0);

        // Also keep the min and max XYZ coordinates
        max.setX(std::max(max.x(), x));
        max.setY(std::max(max.y(), y));
        max.setZ(std::max(max.z(), z));
        min.setX(std::min(min.x(), x));
        min.setY(std::min(min.y(), y));
        min.setZ(std::min(min.z(), z));
    }

    // Computes the centroid of the model
    QVector3D midPoint = QVector3D((min + max) * 0.5);

    // Computes a scaling factor (sx, sy, sz) to normalize
    // the size of the model
    float invDiag = std::sqrt(3.0) / (max - min).length();

    // Center and normalize the model
    for (unsigned int i = 0; i < numVertices; ++i)
    {
        QVector4D v = vertices[i];
        v = (v - midPoint) * invDiag;
        v.setW(1);
        vertices[i] = v;
    }

    // Read the indices
    for (unsigned int i = 0; i < numFaces; ++i)
    {
        unsigned int a, b, c;
        stream >> line >> a >> b >> c;
        indices[i * 3 + 0] = a;
        indices[i * 3 + 1] = b;
        indices[i * 3 + 2] = c;
    }

    createNormals();
    createShaders();
    createVBOs();

    file.close();
}

void Model::normalizeModel()
{
    // Compute minimum and maximum values
    float fmax = std::numeric_limits<float>::max();
    float minx = fmax, miny = fmax, minz = fmax;
    float maxx = -fmax, maxy = -fmax, maxz = -fmax;
    for(unsigned int i=0; i<numVertices; ++i)
    {
        if(vertices[i].x() < minx) minx = vertices[i].x();
        if(vertices[i].y() < miny) miny = vertices[i].y();
        if(vertices[i].z() < minz) minz = vertices[i].z();

        if(vertices[i].x() > maxx) maxx = vertices[i].x();
        if(vertices[i].y() > maxy) maxy = vertices[i].y();
        if(vertices[i].z() > maxz) maxz = vertices[i].z();
    }

    float w = maxx-minx;
    float h = maxy-miny;
    float d = maxz-minz;
    float maxSize = qMax(w, qMax(h, d));

    // Translate minimum coordinate point to origin
    for(unsigned int i=0; i<numVertices; ++i)
    {
        vertices[i].setX(vertices[i].x()-minx);
        vertices[i].setY(vertices[i].y()-miny);
        vertices[i].setZ(vertices[i].z()-minz);
    }

    // Normalize each side
    for(unsigned int i=0; i<numVertices; ++i)
    {
        vertices[i].setX(vertices[i].x()/maxSize);
        vertices[i].setY(vertices[i].y()/maxSize);
        vertices[i].setZ(vertices[i].z()/maxSize);
    }

    // Center around origin
    for(unsigned int i=0; i<numVertices; ++i)
    {
        vertices[i].setX(vertices[i].x()-(w/maxSize / 2.0f));
        vertices[i].setY(vertices[i].y()-(h/maxSize / 2.0f));
        vertices[i].setZ(vertices[i].z()-(d/maxSize / 2.0f));
    }
}

void Model::createShaders()
{
    destroyShaders();

    QFile vs(":/shaders/vshader.glsl");
    QFile fs(":/shaders/fshader.glsl");

    vs.open(QFile::ReadOnly | QFile::Text);
    fs.open(QFile::ReadOnly | QFile::Text);

    QTextStream streamVs(&vs), streamFs(&fs);

    QString qtStringVs = streamVs.readAll();
    QString qtStringFs = streamFs.readAll();

    std::string stdStringVs = qtStringVs.toStdString();
    std::string stdStringFs = qtStringFs.toStdString();
    GLuint vertexShader = 0;
    vertexShader = glCreateShader(GL_VERTEX_SHADER);

    // Send the vertex shader source code to GL
    const GLchar *source = stdStringVs.c_str();

    glShaderSource(vertexShader, 1, &source, nullptr);

    // Compile the vertex shader
    glCompileShader(vertexShader);

    GLint isCompiled = 0;
    glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &isCompiled);
    if (isCompiled == GL_FALSE)
    {
        GLint maxLength = 0;
        glGetShaderiv(vertexShader, GL_INFO_LOG_LENGTH, &maxLength);
        // The maxLength includes the NULL character
        std::vector<GLchar> infoLog(maxLength);
        glGetShaderInfoLog(vertexShader, maxLength, &maxLength, &infoLog[0]);
        qDebug("%s", &infoLog[0]);

        glDeleteShader(vertexShader);
        return;
    }
    GLuint fragmentShader = 0;
    fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);

    // Send the fragment shader source code to GL
    source = stdStringFs.c_str();
    glShaderSource(fragmentShader, 1, &source, nullptr);

    // Compile the fragment shader
    glCompileShader(fragmentShader);

    glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &isCompiled);
    if (isCompiled == GL_FALSE)
    {
        GLint maxLength = 0;
        glGetShaderiv(fragmentShader, GL_INFO_LOG_LENGTH, &maxLength);

        std::vector<GLchar> infoLog(maxLength);
        glGetShaderInfoLog(fragmentShader, maxLength, &maxLength, &infoLog[0]);
        qDebug("%s", &infoLog[0]);

        glDeleteShader(fragmentShader);
        glDeleteShader(vertexShader);
        return;
    }

    shaderProgram = glCreateProgram();

    // Attach our shaders to our program
    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);

    // Link our program
    glLinkProgram(shaderProgram);

    // Note the different functions here: glGetProgram* instead of glGetShader*.
    GLint isLinked = 0;
    glGetProgramiv(shaderProgram, GL_LINK_STATUS, (int *)&isLinked);
    if (isLinked == GL_FALSE)
    {
        GLint maxLength = 0;
        glGetProgramiv(shaderProgram, GL_INFO_LOG_LENGTH, &maxLength);

        // The maxLength includes the NULL character
        std::vector<GLchar> infoLog(maxLength);
        glGetProgramInfoLog(shaderProgram, maxLength, &maxLength, &infoLog[0]);
        qDebug("%s", &infoLog[0]);

        glDeleteProgram(shaderProgram);
        glDeleteShader(vertexShader);
        glDeleteShader(fragmentShader);
        return;
    }
    glDetachShader(shaderProgram, vertexShader);
    glDetachShader(shaderProgram, fragmentShader);

    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

    vs.close();
    fs.close();
}

void Model::destroyShaders()
{
    glWidget->makeCurrent();
    glDeleteProgram(shaderProgram);
}

void Model::createVBOs()
{
    glWidget->makeCurrent();
    destroyVBOs();

    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);

    glGenBuffers(1, &vboVertices);
    glBindBuffer(GL_ARRAY_BUFFER, vboVertices);
    glBufferData(GL_ARRAY_BUFFER, numVertices * sizeof(QVector4D),
                 vertices.get(), GL_STATIC_DRAW);
    glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 0, nullptr);
    glEnableVertexAttribArray(0);

    glGenBuffers(1, &vboNormals);
    glBindBuffer(GL_ARRAY_BUFFER, vboNormals);
    glBufferData(GL_ARRAY_BUFFER, numVertices * sizeof(QVector3D),
                 normals.get(), GL_STATIC_DRAW);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, nullptr);
    glEnableVertexAttribArray(1);

    glGenBuffers(1, &vboIndices);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vboIndices);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, numFaces * 3 * sizeof(unsigned int),
                 indices.get(), GL_STATIC_DRAW);
}

void Model::destroyVBOs()
{
    glDeleteBuffers(1, &vboVertices);
    glDeleteBuffers(1, &vboIndices);
    glDeleteBuffers(1, &vboNormals);

    glDeleteVertexArrays(1, &vao);

    vboVertices = 0;
    vboIndices = 0;
    vboNormals = 0;
    vao = 0;
}

void Model::createNormals()
{
    normals = std::make_unique<QVector3D[]>(numVertices);

    for (unsigned int i = 0; i < numFaces; ++i)
    {
        QVector3D a = QVector3D(vertices[indices[i * 3 + 0]]);
        QVector3D b = QVector3D(vertices[indices[i * 3 + 1]]);
        QVector3D c = QVector3D(vertices[indices[i * 3 + 2]]);
        QVector3D faceNormal = QVector3D::crossProduct((b - a), (c - b));

        // Accumulates face normals on the vertices
        normals[indices[i * 3 + 0]] += faceNormal;
        normals[indices[i * 3 + 1]] += faceNormal;
        normals[indices[i * 3 + 2]] += faceNormal;
    }

    for (unsigned int i = 0; i < numVertices; ++i)
    {
        normals[i].normalize();
    }
}
