QT += widgets
qtHaveModule(printsupport): QT += printsupport

HEADERS       = mainwindow.h \
                scribblearea.h \
    box.hpp \
    position.hpp
SOURCES       = main.cpp \
                mainwindow.cpp \
                scribblearea.cpp \
    Box.cpp \
    position.cpp

# install
target.path = $$[QT_INSTALL_EXAMPLES]/widgets/widgets/scribble
INSTALLS += target
