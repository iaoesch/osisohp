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
ScribbleArea::ScribbleArea(QWidget *parent)
    : QWidget(parent), MyDatas(*this), Interface(this), StateMachine(MyDatas, Interface)
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
#ifndef USE_NEW_STATEMACHINE
    ShowPointer = false;
#endif


    setMouseTracking(true);

    connect(&MyTimer, SIGNAL(timeout()), this, SLOT(timeoutSM()));

    MyTimer.setSingleShot(true);

    connect(&PointerTimer, &QTimer::timeout, this, &ScribbleArea::PointerTimeout);
    PointerTimer.setSingleShot(true);

    /*CopyTimeout = 500;
    GestureTimeout = 500;
    SelectTimeout = 500;
    PostItTimeout = 1000;*/
    SelectPostitsDirectly = false;

     // QImage RecentlyMovedObject;
   // BoundingBoxClass RecentlyMovedObjectBoundingBox;


}
//! [0]


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
   emit (NumberOfLayerChanged(Visibilities.size()));
          for (int i = 0; i < Visibilities.size(); i++) {
            emit(SetVisibilityIndicatorOfLayer(i, Visibilities[i]));
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
    } else if (action->iconText() == "ShowOverview") {
       //ToggleShowOverview(action->isChecked());
       StateMachine.HandleOverviewEventSM(action->isChecked());
    }
}

void ScribbleArea::HandleLayerVisibilityAction(QAction *action)
{
   unsigned int SelectedLayer = action->data().value<int>();
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
   DEBUG_LOG << "Mouse: press";
#ifdef USE_NEW_STATEMACHINE
   StateMachine.HandlePressEventSM(event->button(), event->pos(), event->timestamp());
#else
   HandlePressEventSM(event->button(), event->pos(), event->timestamp());
#endif
}

#ifndef USE_NEW_STATEMACHINE
void ScribbleArea::HandlePressEventSM(Qt::MouseButton Button, QPointF Position, ulong Timestamp)
{
    DEBUG_LOG << "Button Down: " << Button  << std::endl;
    if (Button == Qt::LeftButton) {
       setCursor(Qt::ArrowCursor);
        if (State != Idle) {
           DEBUG_LOG << "HandlePressEventSM: unexpected State: " << State  << std::endl;
        }
        State = WaitingToLeaveJitterProtectionForDrawing;

        Tracker.StartTracking(Position, Timestamp);

        MyDatas.setLastPoint(Position);
        MyDatas.setButtonDownPosition(Position);
        MyDatas.RestartCurrentPaintedObjectBoundingBox(Position);
        MyDatas.setSelectedCurrentPosition(Position);

        if ((SelectPostitsDirectly == true) &&
            (MyDatas.PostItSelected(Position))) {
              DownInsideObject = false;
              MyTimer.start(Settings.DirectSelectTimeout);

        } else {
           MyTimer.start(Settings.SelectTimeout);


           if (MyDatas.IsInsideLstPaintedObjectBoundingBox(Position)) {
              DownInsideObject = true;
           } else {
              DownInsideObject = false;
           }
        }
    }
}
#endif

void ScribbleArea::PointerTimeout()
{
#ifndef USE_NEW_STATEMACHINE

   ShowPointer = false;
#endif
   update();
}

void ScribbleArea::mouseMoveEvent(QMouseEvent *event)
{
   DEBUG_LOG << "Mouse: move" << event->pointCount() << std::endl;
#ifdef USE_NEW_STATEMACHINE
   StateMachine.HandleMoveEventSM(event->buttons(), event->pos(), event->timestamp(), false, 0);
#else
   HandleMoveEventSM(event->buttons(), event->pos(), event->timestamp(), false, 0);
#endif
}

#ifndef USE_NEW_STATEMACHINE
void ScribbleArea::HandleMoveEventSM(Qt::MouseButtons Buttons, QPointF Position, ulong Timestamp, bool Erasing, double Pressure)
{

    Tracker.Trackmovement(Position, Timestamp);
    //WaitForPostIt = false;
    Showeraser = Erasing;
    LastPointerPosition = Position;
    if (Settings.PointerHoldon >= 0) {
       if (   (State == Idle)
            ||(State == WaitingToLeaveJitterProtectionForDrawing)
              ||(State == DrawingPaused)
              ||(State == Drawing)
              ||(State == DrawingFillRequested)) {
          ShowPointer = true;
          PointerTimer.start(Settings.PointerHoldon);
          update();
       }
    }

    if ((Buttons & Qt::LeftButton)) {
        if ((Position-MyDatas.getLastPoint()).manhattanLength() < MyDatas.getMyPenWidth()+2) {
            return; // ignore small movements (probably use penwidth*2)
        }
        switch (State) {
           case Idle:
              break;
           case WaitingToLeaveJitterProtectionForDrawing:
              if (((Position-MyDatas.getButtonDownPosition()).manhattanLength() < (MyDatas.getMyPenWidth()*3+2))) {
                  return; // ignore small movements (probably use penwidth*2)
              }
              State = Drawing;
              [[fallthrough]];
           case DrawingPaused:
           case Drawing:
           case DrawingFillRequested:
               if (State == DrawingFillRequested) {
                  if ((Position-FillPolygonStartPosition).manhattanLength() > (MyDatas.getMyPenWidth()*3+2)) {
                     State = Drawing;
                     setCursor(Qt::ArrowCursor);

                  }
               } else if (State == DrawingPaused) {
                  State = DrawingFillRequested;
               }
               MyTimer.stop();
               MyTimer.start(Settings.GestureTimeout);
               MyDatas.FlushLastDrawnPicture();
              //LastDrawnObject.fill(BackgroundColor);
              if (Erasing) {
                 MyDatas.EraseLineTo(Position, Pressure);
              } else {
                 MyDatas.drawLineTo(Position, Pressure);
              }
              MyDatas.ExtendBoundingboxAndShape(Position);
              break;

              break;
              break;
           case WaitingToLeaveJitterProtectionWithSelectedAreaForMoving:
           case MovingSelectionPaused:
              State = MovingSelection;
           case MovingSelection:
              // QPoint Offset = event->pos() - SelectedPoint;
                MyTimer.stop();
                MyTimer.start(Settings.CopyTimeout);

                MyDatas.setSelectedCurrentPosition(Position);
                update();
              // DrawMovedSelection(Offset);
              // BoundingBoxClass MovedRectangle(LastPaintedObjectBoundingBox);
              // MovedRectangle.Move(PositionClass(Offset.x(), Offset.y()));
              // drawrectangle(MovedRectangle);
               //        drawrectangle(BoundingBoxClass(LastPaintedObjectBoundingBox).Move(PositionClass(Offset.x(), Offset.y())));
              break;
              break;
           case WaitingToLeaveJitterProtectionWithCreatedPostitForMoving:
           case WaitingToLeaveJitterProtectionWithSelectedPostitForMoving:
              State = MovingPostit;
           case MovingPostit:
              if (MyDatas.IsAnySelectedPostit()) {
                 DEBUG_LOG << "Moving postit " << std::endl;
                 MyDatas.MoveSelectedPostits(Position);
                 update();
              }
              break;
           case WaitingToLeaveJitterProtectionForScrolling:
              State = ScrollingDrawingArea;
           case ScrollingDrawingArea:
              MyDatas.CompleteImage();
              MyDatas.MoveOrigin(Position- ScrollingLastPosition);


             ScrollingLastPosition = Position;
             update();
              break;

        }
    } else if (Buttons == Qt::NoButton) {
        if (MyDatas.IsInsideAnyPostIt(Position)) {
           setCursor(Qt::PointingHandCursor);
        }  else {
           setCursor(Qt::ArrowCursor);
        }
    } else {
       setCursor(Qt::ArrowCursor);
    }
}
#endif

void ScribbleArea::mouseReleaseEvent(QMouseEvent *event)
{
   DEBUG_LOG << "Mouse: ";
#ifdef USE_NEW_STATEMACHINE
   StateMachine.HandleReleaseEventSM(event->button(), event->pos(), false, 0);
#else
   HandleReleaseEventSM(event->button(), event->pos(), false, 0);
#endif
}



#ifndef USE_NEW_STATEMACHINE
void ScribbleArea::HandleReleaseEventSM(Qt::MouseButton Button, QPointF Position, bool Erasing, double Pressure)
{
   DEBUG_LOG << "Button Up: " << Button  << std::endl;

   if (Button == Qt::LeftButton) {
      switch(State) {
         case Idle:
//         case WaitingToLeaveJitterProtectionForDrawing:
         case WaitingToLeaveJitterProtectionWithSelectedAreaForMoving:
         case WaitingToLeaveJitterProtectionWithCreatedPostitForMoving:
            State = Idle;
            break;

         case WaitingToLeaveJitterProtectionForDrawing:
         case Drawing:
         case DrawingPaused:
         case DrawingFillRequested:
            MyTimer.stop();
            if (Erasing) {
               MyDatas.EraseLineTo(Position, Pressure);
            } else {
               MyDatas.drawLineTo(Position, Pressure);
            }
            MyDatas.UpdateBoundingboxesForFinishedShape(Position);
            if ((State == DrawingFillRequested)||(State == DrawingPaused)) {
               MyDatas.FilllastDrawnShape();
               setCursor(Qt::ArrowCursor);
               //FillPolygon = false;

               update();
            }
            State = Idle;
            break;
         case MovingSelection:
         case MovingSelectionPaused:
            //WaitForPostIt = false;
             if (Tracker.GetCurrentSpeed() > 0.25f) {
                 DEBUG_LOG << "LeavingSpeed " << Tracker.GetCurrentSpeed() << std::endl;
                 MyDatas.setDiscardSelection(true);
                 update();
             }
             //DEBUG_LOG << "LeavingSpeed " << (LastDistance + CurrentDistance) << " / " << (DeltaTLastDistance + DeltaTCurrentDistance) << " = " << ((LastDistance + CurrentDistance) / (float)(DeltaTLastDistance + DeltaTCurrentDistance)) << std::endl;
                 ;
           // QPoint Offset =  - SelectedPoint;
             if (MyDatas.getDiscardSelection() == false) {
                MyDatas.DrawMovedSelection(Position);
             }
           // BoundingBoxClass MovedRectangle(LastPaintedObjectBoundingBox);
          //  MovedRectangle.Move(PositionClass(Offset.x(), Offset.y()));
          //  drawrectangle(MovedRectangle);
           // MoveSelected = false;
            MyDatas.setLastDrawingValid(false);
            //        drawrectangle(BoundingBoxClass(LastPaintedObjectBoundingBox).Move(PositionClass(Offset.x(), Offset.y())));
            State = Idle;
            break;
         case MovingPostit:
            if (MyDatas.IsAnySelectedPostit()) {
               DEBUG_LOG << "Fixing postit " << std::endl;
               MyDatas.FinishMovingSelectedPostits(Position);
               //  MoveSelected = false;
               MyDatas.setLastDrawingValid(false);
               MyDatas.ClearSelectedPostit();
               setCursor(Qt::ArrowCursor);
               update();
            }
            State = Idle;
            break;

         case WaitingToLeaveJitterProtectionWithSelectedPostitForMoving:
            MyDatas.ClearSelectedPostit();
            setCursor(Qt::ArrowCursor);
            State = Idle;
            break;


         case WaitingToLeaveJitterProtectionForScrolling:
         case ScrollingDrawingArea:
            MyDatas.MoveOrigin(Position - ScrollingLastPosition);
            MyDatas.resizeScrolledImage();
            update();
            State = Idle;
            break;

      }
   }
}

void ScribbleArea::HandleTouchPressEventSM(int NumberOfTouchpoints, QPointF MeanPosition)
{
   if(NumberOfTouchpoints == 2) {
   MyDatas.setSelectedCurrentPosition(MeanPosition);
   MyDatas.MakeSreenMoveHint();
   //Scrolling = true;
   ScrollingLastPosition = MeanPosition;
   ScrollingOldOrigin = MyDatas.GetOrigin();
   //scribbling = false;
   update();
   State = WaitingForTouchScrolling;
   }
}

void ScribbleArea::HandleTouchMoveEventSM(int NumberOfTouchpoints, QPointF MeanPosition)
{
   if(NumberOfTouchpoints == 2) {
   DEBUG_LOG << "TM(" << MeanPosition.x() <<":" << MeanPosition.y() << ")";

   switch (State) {
      case WaitingForTouchScrolling:
         State = TouchScrollingDrawingArea;
         [[fallthrough]];
      case TouchScrollingDrawingArea:

         MyDatas.CompleteImage();
         MyDatas.MoveOrigin(MeanPosition- ScrollingLastPosition);
         ScrollingLastPosition = MeanPosition;
         update();
         break;
      default:
         DEBUG_LOG << "Touch move: unexpected state" << std::endl;
   }
   }
}

void ScribbleArea::HandleTouchReleaseEventSM(int NumberOfTouchpoints, QPointF MeanPosition)
{
   if(NumberOfTouchpoints == 2) {

   switch (State) {
   case WaitingForTouchScrolling:
   case TouchScrollingDrawingArea:
      MyDatas.MoveOrigin(MeanPosition - ScrollingLastPosition);
      MyDatas.resizeScrolledImage();
      update();
      State = Idle;
      break;
   }

}

}
#endif
void ScribbleArea::tabletEvent(QTabletEvent * event)
{
  // DEBUG_LOG << "Tablett Pen Type " << event->pointerType() << std::endl;
    switch(event->type()){
       case QEvent::TabletRelease:
          DEBUG_LOG << "Tablett up " << event->type() << "/"<< event->button() << std::endl;
#ifdef USE_NEW_STATEMACHINE
          StateMachine.HandleReleaseEventSM(event->button(), event->position(), event->pointerType() == QPointingDevice::PointerType::Eraser, event->pressure());
#else
          HandleReleaseEventSM(event->button(), event->position(), event->pointerType() == QPointingDevice::PointerType::Eraser, event->pressure());
#endif
          event->accept();
          break;

       case QEvent::TabletPress:
        DEBUG_LOG << "Tablett down " << event->type() << "/"<< event->button() << std::endl;
#ifdef USE_NEW_STATEMACHINE
        StateMachine.HandlePressEventSM(event->button(), event->position(), event->timestamp());
#else
        HandlePressEventSM(event->button(), event->position(), event->timestamp());
#endif
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
              break;

        }
        event->accept();
        break;
       case QEvent::TabletMove:
          // Tablett move also called on pressure or tilt changes
          if (LastTablettMovePosition != event->position()) {
             DEBUG_LOG << "Tablett move " << event->type() << "/"<< event->buttons() << " <" << event->pos().x() << ";" << event->pos().y() << ">:" << event->pressure() << std::endl;
#ifdef USE_NEW_STATEMACHINE
             StateMachine.HandleMoveEventSM(event->buttons(), event->position(), event->timestamp(), event->pointerType() == QPointingDevice::PointerType::Eraser, event->pressure());
#else
             HandleMoveEventSM(event->buttons(), event->position(), event->timestamp(), event->pointerType() == QPointingDevice::PointerType::Eraser, event->pressure());
#endif
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
         {
         QPointF MeanPosition(0,0);
         for (auto &p: event->points()) {
            MeanPosition += p.position();
         }
         MeanPosition *= TouchScaling/event->pointCount();
#ifdef USE_NEW_STATEMACHINE
         StateMachine.HandleTouchPressEventSM(event->pointCount(), MeanPosition);
#else
         HandleTouchPressEventSM(event->pointCount(), MeanPosition);
#endif
         }
         event->accept();
         return true;
         break;
      case QEvent::TouchCancel:
         DEBUG_LOG << "Got touch Cancel" << std::endl;
         event->accept();
         return true;
         break;
      case QEvent::TouchEnd:
         DEBUG_LOG << "Got touch End" << std::endl;
         {
            QPointF MeanPosition(0,0);
            for (auto &p: event->points()) {
               MeanPosition += p.position();
            }
            MeanPosition *= TouchScaling/event->pointCount();
#ifdef USE_NEW_STATEMACHINE
            StateMachine.HandleTouchReleaseEventSM(event->pointCount(), MeanPosition);
#else
            HandleTouchReleaseEventSM(event->pointCount(), MeanPosition);
#endif
         }
         event->accept();
         return true;
         break;
      case QEvent::TouchUpdate:
         DEBUG_LOG << "Got touch Update" << std::endl;
         {
         QPointF MeanPosition(0,0);
         for (auto &p: event->points()) {
            MeanPosition += p.position();
         }
         MeanPosition *= TouchScaling/event->pointCount();
         if (event->isBeginEvent()) {
#ifdef USE_NEW_STATEMACHINE
            StateMachine.HandleTouchPressEventSM(event->pointCount(), MeanPosition);
#else
            HandleTouchPressEventSM(event->pointCount(), MeanPosition);
#endif
         } else if (event->isEndEvent()) {
#ifdef USE_NEW_STATEMACHINE
            StateMachine.HandleTouchReleaseEventSM(event->pointCount(), MeanPosition);
#else
            HandleTouchReleaseEventSM(event->pointCount(), MeanPosition);
#endif
         } else {
#ifdef USE_NEW_STATEMACHINE
            StateMachine.HandleTouchMoveEventSM(event->pointCount(), MeanPosition);
#else
            HandleTouchMoveEventSM(event->pointCount(), MeanPosition);
#endif
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

         break;
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
         break;
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

      case Qt::Key_Plus:
         StateMachine.HandleKeyEventSM(DatabaseClass::PasteEvent::MakeBigger);
         break;


      case Qt::Key_Minus:
         StateMachine.HandleKeyEventSM(DatabaseClass::PasteEvent::MakeSmaller);
         break;

   }
}

#ifndef USE_NEW_STATEMACHINE

void ScribbleArea::timeoutSM()
{
    /* Copying on long move pauses */
   DEBUG_LOG << "timeout " << std::endl;
   switch(State) {
      case Idle:
         break;
      case WaitingToLeaveJitterProtectionForDrawing:
         if (DownInsideObject) {

            MyDatas.MakeSelectionFromLastDrawnObject();

            MyDatas.ClearLastDrawnObjectPoints();
            MyDatas.SetModified();



            //MoveSelected = true;
            //NewDrawingStarted = false;
            //SelectedPoint = MyDatas.getLastPoint();
            MyDatas.SetSelectedOffset();
            //scribbling = false;
            update();
            //WaitForPostIt = true;
            MyTimer.start(Settings.PostItTimeout);
            State = WaitingToLeaveJitterProtectionWithSelectedAreaForMoving;

         } else {

            if (MyDatas.PostItSelected(MyDatas.getSelectedCurrentPosition())) {
               //scribbling = false;
               //NewDrawingStarted = false;
               setCursor(Qt::ClosedHandCursor);
               DEBUG_LOG << "Selected postit " << std::endl;
               State = WaitingToLeaveJitterProtectionWithSelectedPostitForMoving;


            } else {
               MyDatas.MakeSreenMoveHint();
               //Scrolling = true;
               ScrollingLastPosition = MyDatas.getSelectedCurrentPosition();
               ScrollingOldOrigin = MyDatas.GetOrigin();
               //scribbling = false;
               update();
               State = WaitingToLeaveJitterProtectionForScrolling;
            }

         }
         break;
      case WaitingToLeaveJitterProtectionWithSelectedAreaForMoving:
         {
            DEBUG_LOG << "Creating postit " << std::endl;

            //WaitForPostIt = false;
            MyDatas.CreeatePostitFromSelection();
            State = WaitingToLeaveJitterProtectionWithCreatedPostitForMoving;
            update();
         }
         break;

      case WaitingToLeaveJitterProtectionForScrolling:
         break;
      case WaitingToLeaveJitterProtectionWithCreatedPostitForMoving:
         break;
      case WaitingToLeaveJitterProtectionWithSelectedPostitForMoving:
         break;
      case Drawing:
         //   QTextStream(stdout) << "<" << GestureTrackerAccumulatedSpeed.x() << "; " << GestureTrackerAccumulatedSpeed.y() << "> <"  << GestureTrackerAccumulatedSquaredSpeed.x() << ";" << GestureTrackerAccumulatedSquaredSpeed.y() << endl;

         //FillPolygon = true;
         FillPolygonStartPosition = MyDatas.getLastPoint();
         setCursor(Qt::BusyCursor);
         update();
         State = DrawingPaused;
         break;
      case DrawingPaused:
         break;
      case DrawingFillRequested:
         break;
      case MovingSelection:
         if (MyDatas.getDiscardSelection() == false) {

            // Fast shaking followed by a pause means throw away selection
            if (Tracker.IsFastShaking()) {
               MyDatas.setDiscardSelection(true);
               update();
            } else {
               QPointF CopyPos(MyDatas.getSelectedCurrentPosition());
               MyDatas.MoveSelectedCurrentPosition ( QPointF(3,3));
               MyDatas.DrawMovedSelection(CopyPos);
            }
            State = MovingSelectionPaused;
         }
         break;
      case MovingSelectionPaused:
         break;
      case MovingPostit:
         break;
      case ScrollingDrawingArea:
         break;
   }


}
#endif

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
