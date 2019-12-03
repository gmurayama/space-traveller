QT += core gui widgets opengl

TARGET = space-traveller
TEMPLATE = app

CONFIG += c++14

SOURCES += main.cpp\
        mainwindow.cpp \
    model.cpp \
    openglwidget.cpp

HEADERS  += mainwindow.h \
    model.h \
    openglwidget.h

FORMS    += mainwindow.ui

OTHER_FILES += \
    vshader.glsl \
    fshader.glsl

RESOURCES += \
    resources.qrc
