QT += widgets
qtHaveModule(printsupport): QT += printsupport

HEADERS       = mainwindow.h \
   gesturetracker.hpp \
                scribblearea.h \
    box.hpp \
    position.hpp \
    PaintingArea.h
SOURCES       = main.cpp \
   gesturetracker.cpp \
                mainwindow.cpp \
                scribblearea.cpp \
    Box.cpp \
    position.cpp \
    PaintingArea.cpp

resources.files = \
    images/HandWithPen \
resources.prefix = /

RESOURCES = resources

# install
target.path = $$[QT_INSTALL_EXAMPLES]/widgets/widgets/scribble
INSTALLS += target

DISTFILES += \
   HandWithPen.png
