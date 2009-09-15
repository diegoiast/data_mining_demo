TEMPLATE = app
QT = gui core
CONFIG += qt warn_on console release
OBJECTS_DIR = build
MOC_DIR = build
UI_DIR = build
FORMS = ../ui/mainwindow.ui
HEADERS = ../src/mainwindowimpl.h ../src/nodeplotcurve.h ../src/dataset.h ../src/datasetview.h
SOURCES = ../src/mainwindowimpl.cpp \
 ../src/main.cpp \
 ../src/nodeplotcurve.cpp \
 ../src/dataset.cpp \
 ../src/datasetview.cpp
INCLUDEPATH = ../qwt-5.0.2/src
LIBS = ../qwt-5.0.2/lib/libqwt.a
