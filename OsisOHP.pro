QT += widgets
qtHaveModule(printsupport): QT += printsupport

HEADERS       = mainwindow.h \
   Settings.hpp \
   databaseclass.h \
   gesturetracker.hpp \
   interface.hpp \
                scribblearea.h \
    box.hpp \
    position.hpp \
    PaintingArea.h \
   scribbleareaStateMachine.h
SOURCES       = main.cpp \
   databaseclass.cpp \
   gesturetracker.cpp \
                mainwindow.cpp \
                scribblearea.cpp \
    Box.cpp \
    position.cpp \
    PaintingArea.cpp \
   scribbleareaStateMachine.cpp\
   interface.cpp

resources.files = \
    images/HandWithPen.png \
    images/HandWithEraser.png \
    images/HandWithSponge.png



resources.prefix = /

RESOURCES = resources

# install
target.path = $$[QT_INSTALL_EXAMPLES]/widgets/widgets/scribble
INSTALLS += target

DISTFILES += \
   HandWithPen.png
