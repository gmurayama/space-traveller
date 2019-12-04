QT += core gui widgets opengl

TARGET = space-traveller
TEMPLATE = app

CONFIG += c++14

SOURCES += main.cpp\
    camera.cpp \
    light.cpp \
        mainwindow.cpp \
    model.cpp \
    openglwidget.cpp \
    player.cpp

HEADERS  += mainwindow.h \
    camera.h \
    light.h \
    model.h \
    openglwidget.h \
    player.h

FORMS    += mainwindow.ui

OTHER_FILES += \
    vshader.glsl \
    fshader.glsl

RESOURCES += \
    resources.qrc
