QT += widgets
qtHaveModule(printsupport): QT += printsupport

HEADERS       = mainwindow.h \
   DebugStream.hpp \
   Settings.hpp \
   SettingsDialog.h \
   databaseclass.h \
   gesturetracker.hpp \
   interface.hpp \
                scribblearea.h \
    box.hpp \
    position.hpp \
   scribbleareaStateMachine.h
SOURCES       = main.cpp \
   SettingsDialog.cpp \
   databaseclass.cpp \
   gesturetracker.cpp \
                mainwindow.cpp \
                scribblearea.cpp \
    Box.cpp \
    position.cpp \
   scribbleareaStateMachine.cpp\
   interface.cpp

resources.files = \
    images/HandWithPen.png \
    images/HandWithEraser.png \
    images/HandWithSponge.png \
    images/spongeicon.png



resources.prefix = /

RESOURCES = resources

# install
target.path = $$[QT_INSTALL_EXAMPLES]/widgets/widgets/scribble
INSTALLS += target

DISTFILES += \
   HandWithPen.png
