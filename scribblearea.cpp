/****************************************************************************
**
** Copyright (C) 2013 Digia Plc and/or its subsidiary(-ies).
** Contact: http://www.qt-project.org/legal
**
** This file is part of the examples of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:BSD$
** You may use this file under the terms of the BSD license as follows:
**
** "Redistribution and use in source and binary forms, with or without
** modification, are permitted provided that the following conditions are
** met:
**   * Redistributions of source code must retain the above copyright
**     notice, this list of conditions and the following disclaimer.
**   * Redistributions in binary form must reproduce the above copyright
**     notice, this list of conditions and the following disclaimer in
**     the documentation and/or other materials provided with the
**     distribution.
**   * Neither the name of Digia Plc and its Subsidiary(-ies) nor the names
**     of its contributors may be used to endorse or promote products derived
**     from this software without specific prior written permission.
**
**
** THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
** "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
** LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
** A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
** OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
** SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
** LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
** DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
** THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
** (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
** OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE."
**
** $QT_END_LICENSE$
**
****************************************************************************/

#include <QtWidgets>
#ifndef QT_NO_PRINTER
#include <QPrinter>
#include <QPrintDialog>
#endif

#include "scribblearea.h"
#include "DebugStream.hpp"


//! [0]
ScribbleArea::ScribbleArea(class SettingClass &MySettings, QWidget *parent)
    : QWidget(parent), MyDatas(*this, MySettings), MyCursorManager(this), Interface(this, &MyCursorManager),
      StateMachine(MyDatas, Interface, MySettings), ShowDebugCrosshair(false),
      ShowGestureInfos(false),
      Settings(MySettings)
//      AnimatedCursor(24, 24, 30, ":/images/MousPointers/left_ptr.png", 6, 0)
{
    setAttribute(Qt::WA_StaticContents);
    setFocusPolicy(Qt::StrongFocus);
    setTabletTracking(true);
    QWidget::setAttribute(Qt::WA_AcceptTouchEvents);
    //QWidget::setAttribute(Qt::WA_TouchPadAcceptSingleTouchEvents);
    grabGesture(Qt::PanGesture);

    PointerShape = QImage(":/images/HandWithPen.png");
    SpongeShape = QImage(":/images/HandWithSponge.png");
    EraserShape = QImage(":/images/HandWithEraser.png");
//    connect(&AnimatedPointerTimer, &QTimer::timeout, this, &ScribbleArea::AnimatedPointerTimetick);

    connect(&StateMachine, &ControllingStateMachine::GestureDetected, this, QOverload<>::of(&ScribbleArea::update));

    connect(&AutoSaveTimer, &QTimer::timeout, &MyDatas, &DatabaseClass::AutoSaveDatabase);
    AutoSaveTimer.setSingleShot(false);
    ActualizeAutoSaveIntervall();
    MySettings.RegisterCallback(&MySettings.DefaultAutoSaveIntervall, [this](){ActualizeAutoSaveIntervall();});

    setMouseTracking(true);
}
//! [0]
void ScribbleArea::ActualizeAutoSaveIntervall()
{
   std::cout << "Autosaveintervall changed" << std::endl;
   std::chrono::minutes AutosaveIntervallInMinutes = std::chrono::minutes(Settings.DefaultAutoSaveIntervall);
   if (AutosaveIntervallInMinutes > 0ms) {
      AutoSaveTimer.start(AutosaveIntervallInMinutes);
   } else {
      AutoSaveTimer.stop();
   }
}

//! [3]
bool ScribbleArea::ExportImage(const QString &fileName, const char *fileFormat)
//! [3] //! [4]
{
   return MyDatas.SaveImage(fileName, fileFormat);
}

bool ScribbleArea::SaveImage(const QString &fileName)
{
   return MyDatas.SaveDatabase(fileName);
}
//! [3] //! [4]
bool ScribbleArea::ImportImageToBackgroundLayer(const QString &fileName)
{
   return MyDatas.ImportImageToBackgroundLayer(fileName);
}

bool ScribbleArea::LoadImage(const QString &fileName)
{
   return MyDatas.LoadDatabase(fileName);
}

//! [1] //! [2]
//! [4]

//! [5]
void ScribbleArea::setPenColor(const QColor &newColor)
{
   return MyDatas.setPenColor(newColor);
}

void ScribbleArea::UpdateGUI(std::vector<bool> const &Visibilities)
{
   emit (NumberOfLayerChanged(static_cast<int>(Visibilities.size())));
          for (size_t i = 0; i < Visibilities.size(); i++) {
            emit(SetVisibilityIndicatorOfLayer(static_cast<int>(i), Visibilities[i]));
          }
   }

void ScribbleArea::HandleToolAction(QAction *action)
{
    //MyDatas.RestorePenWidth();

    if (action->iconText() == "PenColor") {
        MyDatas.setPenColor(action->data().value<QColor>());
        MyDatas.setMarkerActive(false);
    } else if(action->iconText() == "Red") {
        MyDatas.setPenColor(Qt::red);
    } else if (action->iconText() == "Blue") {
        MyDatas.setPenColor(Qt::blue);
    } else if (action->iconText() == "Green") {
        MyDatas.setPenColor(Qt::darkGreen);
    } else if (action->iconText() == "Yellow") {
        MyDatas.setPenColor(QColor(196,196,0));
    } else if (action->iconText() == "Black") {
        MyDatas.setPenColor(Qt::black);
    } else if (action->iconText() == "Orange") {
        MyDatas.setPenColor(QColor(255,128,0));
    } else if (action->iconText() == "Magenta") {
        MyDatas.setPenColor(Qt::magenta);
    } else if (action->iconText() == "MarkerYellow") {
       QColor YellowMarkerColor(Qt::yellow);
       YellowMarkerColor.setAlpha(64+190);
       MyDatas.setPenColor(Qt::yellow);
//       MyDatas.ExtendPenWidthForMarker();
       MyDatas.setMarkerActive(true);
    } else if (action->iconText() == "SmallPen") {
       MyDatas.setPenWidth(1);
    } else if (action->iconText() == "MediumPen") {
       MyDatas.setPenWidth(2);
    } else if (action->iconText() == "LargePen") {
       MyDatas.setPenWidth(4);
    } else if (action->iconText() == "Sponge") {
       MyDatas.UseSpongeAsEraser(action->isChecked());
       StateMachine.UseSpongeAsEraser(action->isChecked());
    } else if (action->iconText() == "NewPlane") {
       MoveImageToBackgroundLayer();
    } else if (action->iconText() == "Merge") {
       CollapseBackgroundLayers();
    } else if (action->iconText() == "ToTop") {
       CollapseAllVisibleLayersToTop();
    } else if (action->iconText() == "Freeze") {
       Freeze(action->isChecked());
    } else if (action->iconText() == "Cut") {
       MyDatas.CutSelection(action->isChecked());
    } else if (action->iconText() == "ShowOverview") {
       //ToggleShowOverview(action->isChecked());
       StateMachine.HandleOverviewEventSM(action->isChecked());
    }
}

void ScribbleArea::HandleLayerVisibilityAction(QAction *action)
{
   unsigned int SelectedLayer = action->data().value<unsigned int>();
   if (MyDatas.SetLayerVisibility(SelectedLayer, action->isChecked())) {
      update();
   }
}


//! [7]
void ScribbleArea::setPenWidth(int newWidth)
//! [7] //! [8]
{
    MyDatas.setPenWidth(newWidth);
}



void ScribbleArea::PasteImage(QImage ImageToPaste)
{
   //MyDatas.PasteImage(ImageToPaste);
   StateMachine.HandlePasteEventSM(ImageToPaste);
}

void ScribbleArea::UpdateShowOverviewChanged(bool OverviewEnabled)
{
   emit(ShowOverviewChanged(OverviewEnabled));
}


void ScribbleArea::CopyImageToClipboard()
{
   MyDatas.CopyImageToClipboard();
}

//! [11]
void ScribbleArea::mousePressEvent(QMouseEvent *event)
{
   std::cout << "<" << event->device()->name().toStdString() << "> : " << event->device()->systemId() << std::endl;
   DEBUG_LOG << "Mouse: press";
   StateMachine.HandlePressEventSM(event->button(), event->pos(), ControllingStateMachine::Milliseconds(event->timestamp()));
}

#if 0
void ScribbleArea::PointerTimeout()
{
#ifndef USE_NEW_STATEMACHINE

   ShowPointer = false;
#endif
   update();
}
#endif

void ScribbleArea::mouseMoveEvent(QMouseEvent *event)
{
   DEBUG_LOG << "Mouse: move" << event->pointCount() << std::endl;
   StateMachine.HandleMoveEventSM(event->buttons(), event->pos(), ControllingStateMachine::Milliseconds(event->timestamp()), PenInfoClass(event->modifiers() == Qt::ShiftModifier));
}


void ScribbleArea::mouseReleaseEvent(QMouseEvent *event)
{
   DEBUG_LOG << "Mouse: ";
   StateMachine.HandleReleaseEventSM(event->button(), event->pos(), PenInfoClass(event->modifiers() == Qt::ShiftModifier));
}



void ScribbleArea::tabletEvent(QTabletEvent * event)
{
  // DEBUG_LOG << "Tablett Pen Type " << event->pointerType() << std::endl;
   std::cout << "<" << event->device()->name().toStdString() << "> : " << event->device()->systemId() << std::endl;
    switch(event->type()){
       case QEvent::TabletRelease:
          DEBUG_LOG << "Tablett up " << event->type() << "/"<< event->button() << std::endl;
          StateMachine.HandleReleaseEventSM(event->button(), event->position(), PenInfoClass(event));
          event->accept();
          break;

       case QEvent::TabletPress:
        DEBUG_LOG << "Tablett down " << event->type() << "/"<< event->button() << std::endl;
        StateMachine.HandlePressEventSM(event->button(), event->position(), ControllingStateMachine::Milliseconds(event->timestamp()));
        switch (event->button()) {
           case Qt::NoButton:
              break;
           case Qt::LeftButton:
              break;
           case Qt::RightButton:
              break;
           case Qt::MiddleButton:
              break;
           case Qt::BackButton:
              break;
             case Qt::ForwardButton:
              break;
            case Qt::TaskButton:
              break;
            case Qt::ExtraButton4:
           default:
              break;

        }
        event->accept();
        break;
       case QEvent::TabletMove:
          // Tablett move also called on pressure or tilt changes
          // ToDo Probably bettter not to filterr out pressure or tilt changes
          if (LastTablettMovePosition != event->position()) {
             DEBUG_LOG << "Tablett move " << event->type() << "/"<< event->buttons() << " <" << event->position().x() << ";" << event->position().y() << ">:" << event->pressure() << std::endl;
             StateMachine.HandleMoveEventSM(event->buttons(), event->position(), ControllingStateMachine::Milliseconds(event->timestamp()), PenInfoClass(event));
             LastTablettMovePosition = event->position();
          }
          event->accept();
        break;
       default: event->ignore();
    }
}

bool ScribbleArea::TouchEvent(QTouchEvent *event)
{
   double TouchScaling = Settings.Touchscaling;
   switch (event->type()) {
      case QEvent::TouchBegin:
         DEBUG_LOG << "Got touch begin" << std::endl;
         std::cout << "<" << event->device()->name().toStdString() << "> : " << event->device()->systemId() << std::endl;
         {
         QPointF MeanPosition(0,0);
         for (auto &p: event->points()) {
            MeanPosition += p.position();
         }
         MeanPosition *= TouchScaling/static_cast<double>(event->pointCount());
         StateMachine.HandleTouchPressEventSM(static_cast<int>(event->pointCount()), MeanPosition);
         }
         event->accept();
         return true;

      case QEvent::TouchCancel:
         DEBUG_LOG << "Got touch Cancel" << std::endl;
         event->accept();
         return true;

      case QEvent::TouchEnd:
         DEBUG_LOG << "Got touch End" << std::endl;
         {
            QPointF MeanPosition(0,0);
            for (auto &p: event->points()) {
               MeanPosition += p.position();
            }
            MeanPosition *= TouchScaling/static_cast<double>(event->pointCount());
            StateMachine.HandleTouchReleaseEventSM(static_cast<int>(event->pointCount()), MeanPosition);
         }
         event->accept();
         return true;

      case QEvent::TouchUpdate:
         DEBUG_LOG << "Got touch Update" << std::endl;
         {
         QPointF MeanPosition(0,0);
         for (auto &p: event->points()) {
            MeanPosition += p.position();
         }
         MeanPosition *= TouchScaling/static_cast<double>(event->pointCount());
         if (event->isBeginEvent()) {
            StateMachine.HandleTouchPressEventSM(static_cast<int>(event->pointCount()), MeanPosition);
         } else if (event->isEndEvent()) {
            StateMachine.HandleTouchReleaseEventSM(static_cast<int>(event->pointCount()), MeanPosition);
         } else {
            StateMachine.HandleTouchMoveEventSM(static_cast<int>(event->pointCount()), MeanPosition);
         }
#if 0
         {

            for (auto &p: event->points()) {
               DEBUG_LOG << "(" << p.position().x() <<":" << p.position().y() << ")";
            }
            DEBUG_LOG << event->isBeginEvent() << event->isEndEvent() << std::endl;
         }
#endif
         }
         event->accept();
         return true;

      default:
         return QWidget::event(event);

   }

}


bool ScribbleArea::event(QEvent *event)
{

   switch (event->type()) {
      case QEvent::TouchBegin:
      case QEvent::TouchCancel:
      case QEvent::TouchEnd:
      case QEvent::TouchUpdate:
         {
         QTouchEvent *te = dynamic_cast<QTouchEvent *>(event);
         if (te) {
            return TouchEvent(te);
         } else {
            return QWidget::event(event);
         }
         }

      case QEvent::Gesture:
         DEBUG_LOG << "Gesrture" << std::endl;
         return QWidget::event(event);
      case QEvent::Scroll:
         DEBUG_LOG << "Scroll" << std::endl;
         return QWidget::event(event);
      case QEvent::Wheel:
         DEBUG_LOG << "Wheel" << static_cast<QWheelEvent*>(event)->angleDelta().x() << ":" << static_cast<QWheelEvent*>(event)->angleDelta().y() << std::endl;
         {
            QPointF Delta (static_cast<QWheelEvent*>(event)->angleDelta());

            MyDatas.CompleteImage();
            MyDatas.MoveOrigin(Delta/20.0);
            MyDatas.resizeScrolledImage();

            update();

         }
         event->accept();
         return true;
       default:
         //DEBUG_LOG << "<" << event->type() << ">";
         return QWidget::event(event);

   }

}

void ScribbleArea::keyPressEvent(QKeyEvent *event)
{
   std::cout << "GotKeyEvent" << std::endl;
   switch(event->key()) {
      case Qt::Key_Return:
         StateMachine.HandleKeyEventSM(DatabaseClass::PasteEvent::PasteDrawing);
         break;

      case Qt::Key_T:
         StateMachine.HandleKeyEventSM(DatabaseClass::PasteEvent::PasteTopLayer);
         break;

      case Qt::Key_B:
         StateMachine.HandleKeyEventSM(DatabaseClass::PasteEvent::PasteBottomLayer);
         break;

      case Qt::Key_Escape:
         StateMachine.HandleKeyEventSM(DatabaseClass::PasteEvent::CancelPasting);
         break;

      case Qt::Key_Backspace:
      case Qt::Key_O:
         StateMachine.HandleKeyEventSM(DatabaseClass::PasteEvent::MakeOriginalSize);
         break;

      case Qt::Key_Plus:
      case Qt::Key_1:
         StateMachine.HandleKeyEventSM(DatabaseClass::PasteEvent::MakeBigger);
         break;


      case Qt::Key_Minus:
      case Qt::Key_Underscore:
      case Qt::Key_Space:
         StateMachine.HandleKeyEventSM(DatabaseClass::PasteEvent::MakeSmaller);
         break;

      case Qt::Key_C:
         emit (SwitchToNextPenColor());
         break;

   }
}


//! [12] //! [13]
void ScribbleArea::paintEvent(QPaintEvent *event)
//! [13] //! [14]
{
   if (MyDatas.GetShowOverview()) {
      QPainter painter(this);
      MyDatas.PaintOverview(painter, this->size());
   } else {
      QPainter painter(this);
      MyDatas.PaintVisibleDrawing(painter, event->rect());

      // If we are scrolling, draw a 'shadow' nover everting as feedback
      if (StateMachine.IsScrollingState()){
         painter.setPen(QPen(QColor(0, 0, 0, 0), 1, Qt::SolidLine, Qt::RoundCap,
                             Qt::RoundJoin));
         painter.setBrush(QBrush(MyDatas.getScrollHintColor()));
         //painter.setBrush(QBrush(QColor(200, 230, 200, 50)));

         painter.drawRect(0,0,this->width(), this->height());
       }
         switch(StateMachine.PointerTypeToShow()) {
            case ControllingStateMachine::NONE:
               DEBUG_LOG << "No Pointer to show" << std::endl;
               break;
            case ControllingStateMachine::DRAWER:
               DEBUG_LOG << "Draw Pointer to show" << std::endl;
               painter.drawImage(StateMachine.getLastPointerPosition() - QPointF(0, 36), PointerShape);
               break;
            case ControllingStateMachine::ERASER:
               painter.drawImage(StateMachine.getLastPointerPosition() - QPointF(0, 36), EraserShape);
               break;
            case ControllingStateMachine::WIPER:
               painter.drawImage(StateMachine.getLastPointerPosition() - QPointF(100, 60), SpongeShape);
               break;

       }
       if (ShowDebugCrosshair) {
         painter.setPen(QPen(QColor(Qt::black), 1, Qt::SolidLine, Qt::RoundCap,
                             Qt::RoundJoin));
         painter.setBrush(QBrush(Qt::black));
         painter.drawLine(StateMachine.getLastPointerPosition()-QPointF(50,0), StateMachine.getLastPointerPosition()+QPointF(50,0));
         painter.drawLine(StateMachine.getLastPointerPosition()-QPointF(0,50), StateMachine.getLastPointerPosition()+QPointF(0,50));
       }
       if (ShowGestureInfos) {
         StateMachine.DrawGestureTrackerDebugInfo(painter, QPoint(20,20));
       }
   }
}



//! [15]
void ScribbleArea::resizeEvent(QResizeEvent *event)
//! [15] //! [16]
{
   MyDatas.ResizeAll(width(), height());
    QWidget::resizeEvent(event);
}
//! [16]



//! [18]


//! [20]

//! [21]
void ScribbleArea::print()
{
#if 0
#if !defined(QT_NO_PRINTER) && !defined(QT_NO_PRINTDIALOG)
    QPrinter printer(QPrinter::HighResolution);

    QPrintDialog printDialog(&printer, this);
//! [21] //! [22]
    if (printDialog.exec() == QDialog::Accepted) {
        QPainter painter(&printer);
        QRect rect = painter.viewport();
        QSize size = image.size();
        size.scale(rect.size(), Qt::KeepAspectRatio);
        painter.setViewport(rect.x(), rect.y(), size.width(), size.height());
        painter.setWindow(image.rect());
        painter.drawImage(0, 0, image);
    }
#endif // QT_NO_PRINTER
#endif
}
//! [22]
