QT += widgets
qtHaveModule(printsupport): QT += printsupport

HEADERS       = mainwindow.h \
   DebugStream.hpp \
   Settings.hpp \
   SettingsDialog.h \
   ValueGroupDescriptor.h \
   animatedcursorclass.h \
   backgroundimagemanagerclass.h \
   databaseclass.h \
   drawingobjectclass.h \
   gesturetracker.hpp \
   global.h \
   interface.hpp \
   pastingobjectclass.h \
   postitmanagerclass.h \
                scribblearea.h \
    box.hpp \
    position.hpp \
   scribbleareaStateMachine.h \
   selectionclass.h \
   tabletapplication.h
SOURCES       = main.cpp \
   Settings.cpp \
   SettingsDialog.cpp \
   animatedcursorclass.cpp \
   backgroundimagemanagerclass.cpp \
   databaseclass.cpp \
   drawingobjectclass.cpp \
   gesturetracker.cpp \
                mainwindow.cpp \
   pastingobjectclass.cpp \
   postitmanagerclass.cpp \
                scribblearea.cpp \
    Box.cpp \
    position.cpp \
   scribbleareaStateMachine.cpp\
   interface.cpp \
   selectionclass.cpp \
   tabletapplication.cpp

resources.files = \
    images/HandWithPen.png \
    images/HandWithEraser.png \
    images/HandWithSponge.png \
    images/Restore.png \
    images/spongeicon.png\
    images/MousPointers/pencil.png\
    images/MousPointers/all-scroll.png \
    images/MousPointers/hand1.png \
    images/MousPointers/hand2.png \
    images/MousPointers/hand1plus.png \
    images/MousPointers/handMoveplus.png \
    images/MousPointers/move.png \
    images/MousPointers/bucketfill16.png \
    images/MousPointers/bucketfill200.png \
    images/MousPointers/bucketfillstop200.png \
    images/MousPointers/handMoveMultiple.png \
    images/MousPointers/scissorscopy24.png \
    images/MousPointers/scissors24.png \
    images/MousPointers/left_ptr_edit_plus.png \
    images/MousPointers/left_ptr_edit.png \
    images/MousPointers/X_cursor.png \
    images/MousPointers/left_ptr.png


ICON = ohp.icns
# QMAKE_INFO_PLIST = myapp.plist

resources.prefix = /

RESOURCES = resources

# install
target.path = $$[QT_INSTALL_EXAMPLES]/widgets/widgets/scribble
INSTALLS += target

DISTFILES += \
   HandWithPen.png
