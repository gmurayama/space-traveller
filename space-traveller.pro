QT += core gui widgets opengl

TARGET = space-traveller
TEMPLATE = app

CONFIG += c++14

SOURCES += main.cpp\
        mainwindow.cpp \
    openglwidget.cpp

HEADERS  += mainwindow.h \
    openglwidget.h

FORMS    += mainwindow.ui

OTHER_FILES += \
    vshader.glsl \
    fshader.glsl

RESOURCES += \
    resources.qrc
