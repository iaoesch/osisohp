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


//! [0]
ScribbleArea::ScribbleArea(QWidget *parent)
    : QWidget(parent)
{
    setAttribute(Qt::WA_StaticContents);
    setTabletTracking(true);
    QWidget::setAttribute(Qt::WA_AcceptTouchEvents);
    //QWidget::setAttribute(Qt::WA_TouchPadAcceptSingleTouchEvents);
    grabGesture(Qt::PanGesture);

    PointerShape = QImage(":/images/HandWithPen.png");
    SpongeShape = QImage(":/images/HandWithSponge.png");
    EraserShape = QImage(":/images/HandWithEraser.png");
    ShowPointer = false;

    ShowOverview = false;

    setMouseTracking(true);
    modified = false;
    LastDrawingValid = false;
    EraseLastDrawnObject = false;
    Frozen = false;
    myPenWidth = 2;
    SelectedPenWidth = myPenWidth;
    myPenColor = Qt::blue;

    connect(&MyTimer, SIGNAL(timeout()), this, SLOT(timeoutSM()));

    MyTimer.setSingleShot(true);

    connect(&PointerTimer, &QTimer::timeout, this, &ScribbleArea::PointerTimeout);
    PointerTimer.setSingleShot(true);

    /*CopyTimeout = 500;
    GestureTimeout = 500;
    SelectTimeout = 500;
    PostItTimeout = 1000;*/
    SelectedPostit.clear();
    SelectPostitsDirectly = false;
    ShowPostitsFrame = false;

    TransparentColor = QColor(255, 255, 255, 0);
    BackGroundColor = QColor(230,230, 200,255);
    DefaultBackGroundColor = BackGroundColor;
    PostItBackgroundColor = QColor(100, 0, 0, 50);


    RecentlyPastedObjectValid = false;
    MarkerActive = false;
   // QImage RecentlyMovedObject;
   // BoundingBoxClass RecentlyMovedObjectBoundingBox;


}
//! [0]

//! [1]

//! [2]

//! [3]
void StateBaseClass::HandlePressEventSM(Qt::MouseButton Button, QPointF Position, ulong Timestamp)
{
   std::cout << "HandlePressEventSM: unexpected State: " << StateId()  << std::endl;
   StateMachine.Idle.HandlePressEventSM(Button, Position, Timestamp);
}

template<>
void StateClass<State::ScribblingState::Idle>::HandlePressEventSM(Qt::MouseButton Button, QPointF Position, ulong Timestamp)
{
    std::cout << "Button Down: " << Button  << std::endl;
    if (Button == Qt::LeftButton) {
       setCursor(Qt::ArrowCursor);

        StateMachine.SetNewState(&StateMachine.WaitingToLeaveJitterProtectionForDrawing);


        StateMachine.Tracker.StartTracking(Position, Timestamp);


        lastPoint = Position;
        ButtonDownPosition = Position;
        CurrentPaintedObjectBoundingBox.Clear();
        CurrentPaintedObjectBoundingBox.AddPoint(PositionClass(lastPoint.x(), lastPoint.y()));
        SelectedCurrentPosition = Position;

        if ((SelectPostitsDirectly == true) &&
            (PostItSelected(SelectedCurrentPosition))) {
              DownInsideObject = false;
              MyTimer.start(Settings.DirectSelectTimeout);

        } else {
           MyTimer.start(Settings.SelectTimeout);


           if (LastPaintedObjectBoundingBox.IsInside(PositionClass(Position.x(), Position.y()))) {
              DownInsideObject = true;
           } else {
              DownInsideObject = false;
           }
        }
    }
}

void ScribbleArea::PointerTimeout()
{
   ShowPointer = false;
   update();
}

void ScribbleArea::mouseMoveEvent(QMouseEvent *event)
{
   std::cout << "Mouse: move" << event->pointCount() << std::endl;

   HandleMoveEventSM(event->buttons(), event->pos(), event->timestamp(), false, 0);
}


void ScribbleArea::CompleteImage()
{
   if (LastDrawingValid) {
      DrawLastDrawnPicture();

      LastDrawingValid = false;
      LastDrawnObjectPoints.clear();


   }
}

void StateBaseClass::HandleMoveEventSM(Qt::MouseButtons Buttons, QPointF Position, ulong Timestamp, bool Erasing, double Pressure)
{

   StateMachine.Tracker.Trackmovement(Position, Timestamp);
   //WaitForPostIt = false;
   Showeraser = Erasing;
   LastPointerPosition = Position;
}

void ControllingStateMachine::ShowBigPointer()
{
   if (Settings.PointerHoldon >= 0) {
   ShowPointer = true;
   PointerTimer.start(Settings.PointerHoldon);
   update();
   }
}

void StateBaseClass::HandleMoveNoLeftButtonEvent()
{
   if (Buttons == Qt::NoButton) {
      if (IsInsideAnyPostIt(Position)) {
         setCursor(Qt::PointingHandCursor);
      }  else {
         setCursor(Qt::ArrowCursor);
      }
   } else {
     setCursor(Qt::ArrowCursor);
   }
}

template<>
void StateClass<State::ScribblingState::Idle>::HandleMoveEventSM(Qt::MouseButtons Buttons, QPointF Position, ulong Timestamp, bool Erasing, double Pressure)
{
   StateBaseClass::HandleMoveEventSM(Buttons, Position, Timestamp, Erasing, Pressure);
   StateMachine.ShowBigPointer();
   if ((Buttons & Qt::LeftButton)) {
       if ((Position-lastPoint).manhattanLength() < myPenWidth+2) {
           return; // ignore small movements (probably use penwidth*2)
       }
   }  else {
      HandleMoveNoLeftButtonEvent();
   }

}
template<>
void StateClass<State::ScribblingState::WaitingToLeaveJitterProtectionForDrawing>::HandleMoveEventSM(Qt::MouseButtons Buttons, QPointF Position, ulong Timestamp, bool Erasing, double Pressure)
{
   StateBaseClass::HandleMoveEventSM(Buttons, Position, Timestamp, Erasing, Pressure);
   StateMachine.ShowBigPointer();

   if ((Buttons & Qt::LeftButton)) {
       if ((Position-lastPoint).manhattanLength() < myPenWidth+2) {
           return; // ignore small movements (probably use penwidth*2)
       }
       if (((Position-ButtonDownPosition).manhattanLength() < (myPenWidth*3+2))) {
           return; // ignore small movements (probably use penwidth*2)
       }
       StateMachine.SetNewState(&StateMachine.Drawing);
       StateMachine.Drawing.HandleMoveEventSM(Buttons, Position, Timestamp, Erasing, Pressure);

   }  else {
      HandleMoveNoLeftButtonEvent();
   }
}

template<>
void StateClass<State::ScribblingState::Drawing>::HandleMoveEventSM(Qt::MouseButtons Buttons, QPointF Position, ulong Timestamp, bool Erasing, double Pressure)
{
   StateBaseClass::HandleMoveEventSM(Buttons, Position, Timestamp, Erasing, Pressure);
   StateMachine.ShowBigPointer();

   if ((Buttons & Qt::LeftButton)) {
       if ((Position-lastPoint).manhattanLength() < myPenWidth+2) {
           return; // ignore small movements (probably use penwidth*2)
       }
       if (((Position-ButtonDownPosition).manhattanLength() < (myPenWidth*3+2))) {
           return; // ignore small movements (probably use penwidth*2)
       }
       MyTimer.stop();
       MyTimer.start(Settings.GestureTimeout);
       if (LastDrawingValid) {
         DrawLastDrawnPicture();

         LastDrawingValid = false;
         LastDrawnObjectPoints.clear();
         LastDrawnObjectPoints.append(lastPoint);

      }
      //LastDrawnObject.fill(BackgroundColor);
      if (Erasing) {
         EraseLineTo(Position, Pressure);
      } else {
         drawLineTo(Position, Pressure);
      }
      LastDrawnObjectPoints.append(Position);
      CurrentPaintedObjectBoundingBox.AddPoint(PositionClass(Position.x(), Position.y()));

   }  else {
      HandleMoveNoLeftButtonEvent();
   }

}

template<>
void StateClass<State::ScribblingState::DrawingPaused>::HandleMoveEventSM(Qt::MouseButtons Buttons, QPointF Position, ulong Timestamp, bool Erasing, double Pressure)
{
   StateBaseClass::HandleMoveEventSM(Buttons, Position, Timestamp, Erasing, Pressure);
   StateMachine.ShowBigPointer();

   if ((Buttons & Qt::LeftButton)) {
       if ((Position-lastPoint).manhattanLength() < myPenWidth+2) {
           return; // ignore small movements (probably use penwidth*2)
       }
       if (((Position-ButtonDownPosition).manhattanLength() < (myPenWidth*3+2))) {
           return; // ignore small movements (probably use penwidth*2)
       }
       StateMachine.SetNewState(&StateMachine.DrawingFillRequested);
       StateMachine.Drawing.HandleMoveEventSM(Buttons, Position, Timestamp, Erasing, Pressure);

   }  else {
      HandleMoveNoLeftButtonEvent();
   }

}

template<>
void StateClass<State::ScribblingState::DrawingFillRequested>::HandleMoveEventSM(Qt::MouseButtons Buttons, QPointF Position, ulong Timestamp, bool Erasing, double Pressure)
{
   StateBaseClass::HandleMoveEventSM(Buttons, Position, Timestamp, Erasing, Pressure);
   StateMachine.ShowBigPointer();

   if ((Buttons & Qt::LeftButton)) {
       if ((Position-lastPoint).manhattanLength() < myPenWidth+2) {
           return; // ignore small movements (probably use penwidth*2)
       }
       if (((Position-ButtonDownPosition).manhattanLength() < (myPenWidth*3+2))) {
           return; // ignore small movements (probably use penwidth*2)
       }
       if ((Position-FillPolygonStartPosition).manhattanLength() > (myPenWidth*3+2)) {
          StateMachine.SetNewState(&StateMachine.Drawing);
          setCursor(Qt::ArrowCursor);

       }
       StateMachine.Drawing.HandleMoveEventSM(Buttons, Position, Timestamp, Erasing, Pressure);

   }  else {
      HandleMoveNoLeftButtonEvent();
   }

}

template<>
void StateClass<State::ScribblingState::MovingSelection>::HandleMoveEventSM(Qt::MouseButtons Buttons, QPointF Position, ulong Timestamp, bool Erasing, double Pressure)
{
   StateBaseClass::HandleMoveEventSM(Buttons, Position, Timestamp, Erasing, Pressure);

   if ((Buttons & Qt::LeftButton)) {
       if ((Position-lastPoint).manhattanLength() < myPenWidth+2) {
           return; // ignore small movements (probably use penwidth*2)
       }
       if (((Position-ButtonDownPosition).manhattanLength() < (myPenWidth*3+2))) {
           return; // ignore small movements (probably use penwidth*2)
       }
       // QPoint Offset = event->pos() - SelectedPoint;
         MyTimer.stop();
         MyTimer.start(Settings.CopyTimeout);

         SelectedCurrentPosition = Position;
         update();
       // DrawMovedSelection(Offset);
       // BoundingBoxClass MovedRectangle(LastPaintedObjectBoundingBox);
       // MovedRectangle.Move(PositionClass(Offset.x(), Offset.y()));
       // drawrectangle(MovedRectangle);
        //        drawrectangle(BoundingBoxClass(LastPaintedObjectBoundingBox).Move(PositionClass(Offset.x(), Offset.y())));


   }  else {
      HandleMoveNoLeftButtonEvent();
   }

}

template<>
void StateClass<State::ScribblingState::WaitingToLeaveJitterProtectionWithSelectedAreaForMoving>::HandleMoveEventSM(Qt::MouseButtons Buttons, QPointF Position, ulong Timestamp, bool Erasing, double Pressure)
{
   StateBaseClass::HandleMoveEventSM(Buttons, Position, Timestamp, Erasing, Pressure);

   if ((Buttons & Qt::LeftButton)) {
       if ((Position-lastPoint).manhattanLength() < myPenWidth+2) {
           return; // ignore small movements (probably use penwidth*2)
       }
       if (((Position-ButtonDownPosition).manhattanLength() < (myPenWidth*3+2))) {
           return; // ignore small movements (probably use penwidth*2)
       }
       StateMachine.SetNewState(&StateMachine.MovingSelection);
       StateMachine.MovingSelection.HandleMoveEventSM(Buttons, Position, Timestamp, Erasing, Pressure);


   }  else {
      HandleMoveNoLeftButtonEvent();
   }

}


template<>
void StateClass<State::ScribblingState::MovingSelectionPaused>::HandleMoveEventSM(Qt::MouseButtons Buttons, QPointF Position, ulong Timestamp, bool Erasing, double Pressure)
{
   StateBaseClass::HandleMoveEventSM(Buttons, Position, Timestamp, Erasing, Pressure);

   if ((Buttons & Qt::LeftButton)) {
       if ((Position-lastPoint).manhattanLength() < myPenWidth+2) {
           return; // ignore small movements (probably use penwidth*2)
       }
       if (((Position-ButtonDownPosition).manhattanLength() < (myPenWidth*3+2))) {
           return; // ignore small movements (probably use penwidth*2)
       }
       StateMachine.SetNewState(&StateMachine.MovingSelection);
       StateMachine.MovingSelection.HandleMoveEventSM(Buttons, Position, Timestamp, Erasing, Pressure);


   }  else {
      HandleMoveNoLeftButtonEvent();
   }

}

template<>
void StateClass<State::ScribblingState::MovingPostit>::HandleMoveEventSM(Qt::MouseButtons Buttons, QPointF Position, ulong Timestamp, bool Erasing, double Pressure)
{
   StateBaseClass::HandleMoveEventSM(Buttons, Position, Timestamp, Erasing, Pressure);

   if ((Buttons & Qt::LeftButton)) {
       if ((Position-lastPoint).manhattanLength() < myPenWidth+2) {
           return; // ignore small movements (probably use penwidth*2)
       }
       if (((Position-ButtonDownPosition).manhattanLength() < (myPenWidth*3+2))) {
           return; // ignore small movements (probably use penwidth*2)
       }
       if (!SelectedPostit.empty()) {
          std::cout << "Moving postit " << std::endl;
          for(auto &r: SelectedPostit) {
             //SelectedPostit->Position = Position;
             QPointF LastPosition = r.postit->Position;
             r.postit->Position = r.StartPosition + (Position - ButtonDownPosition);

             r.postit->Box.Move(PositionClass(r.postit->Position.x()-LastPosition.x(), r.postit->Position.y()-LastPosition.y()));
         //  SelectedPostit->Position = SelectedPostit->Position  Origin + Position;
          }
          update();
       }

   }  else {
      HandleMoveNoLeftButtonEvent();
   }

}

template<>
void StateClass<State::ScribblingState::WaitingToLeaveJitterProtectionWithCreatedPostitForMoving>::HandleMoveEventSM(Qt::MouseButtons Buttons, QPointF Position, ulong Timestamp, bool Erasing, double Pressure)
{
   StateBaseClass::HandleMoveEventSM(Buttons, Position, Timestamp, Erasing, Pressure);

   if ((Buttons & Qt::LeftButton)) {
       if ((Position-lastPoint).manhattanLength() < myPenWidth+2) {
           return; // ignore small movements (probably use penwidth*2)
       }
       if (((Position-ButtonDownPosition).manhattanLength() < (myPenWidth*3+2))) {
           return; // ignore small movements (probably use penwidth*2)
       }
       StateMachine.SetNewState(&StateMachine.MovingPostit);
       StateMachine.MovingPostit.HandleMoveEventSM(Buttons, Position, Timestamp, Erasing, Pressure);


   }  else {
      HandleMoveNoLeftButtonEvent();
   }

}

template<>
void StateClass<State::ScribblingState::WaitingToLeaveJitterProtectionWithSelectedPostitForMoving>::HandleMoveEventSM(Qt::MouseButtons Buttons, QPointF Position, ulong Timestamp, bool Erasing, double Pressure)
{
   StateBaseClass::HandleMoveEventSM(Buttons, Position, Timestamp, Erasing, Pressure);

   if ((Buttons & Qt::LeftButton)) {
       if ((Position-lastPoint).manhattanLength() < myPenWidth+2) {
           return; // ignore small movements (probably use penwidth*2)
       }
       if (((Position-ButtonDownPosition).manhattanLength() < (myPenWidth*3+2))) {
           return; // ignore small movements (probably use penwidth*2)
       }
       StateMachine.SetNewState(&StateMachine.MovingPostit);
       StateMachine.MovingPostit.HandleMoveEventSM(Buttons, Position, Timestamp, Erasing, Pressure);


   }  else {
      HandleMoveNoLeftButtonEvent();
   }

}

template<>
void StateClass<State::ScribblingState::ScrollingDrawingArea>::HandleMoveEventSM(Qt::MouseButtons Buttons, QPointF Position, ulong Timestamp, bool Erasing, double Pressure)
{
   StateBaseClass::HandleMoveEventSM(Buttons, Position, Timestamp, Erasing, Pressure);

   if ((Buttons & Qt::LeftButton)) {
       if ((Position-lastPoint).manhattanLength() < myPenWidth+2) {
           return; // ignore small movements (probably use penwidth*2)
       }
       if (((Position-ButtonDownPosition).manhattanLength() < (myPenWidth*3+2))) {
           return; // ignore small movements (probably use penwidth*2)
       }
       CompleteImage();
      Origin -= Position- ScrollingLastPosition;
      if (!Frozen) {
         BackgroundImagesOrigin -= Position- ScrollingLastPosition;
      }

      ScrollingLastPosition = Position;
      update();

   }  else {
      HandleMoveNoLeftButtonEvent();
   }

}

template<>
void StateClass<State::ScribblingState::WaitingToLeaveJitterProtectionForScrolling>::HandleMoveEventSM(Qt::MouseButtons Buttons, QPointF Position, ulong Timestamp, bool Erasing, double Pressure)
{
   StateBaseClass::HandleMoveEventSM(Buttons, Position, Timestamp, Erasing, Pressure);

   if ((Buttons & Qt::LeftButton)) {
       if ((Position-lastPoint).manhattanLength() < myPenWidth+2) {
           return; // ignore small movements (probably use penwidth*2)
       }
       if (((Position-ButtonDownPosition).manhattanLength() < (myPenWidth*3+2))) {
           return; // ignore small movements (probably use penwidth*2)
       }
       StateMachine.SetNewState(&StateMachine.ScrollingDrawingArea);
       StateMachine.ScrollingDrawingArea.HandleMoveEventSM(Buttons, Position, Timestamp, Erasing, Pressure);


   }  else {
      HandleMoveNoLeftButtonEvent();
   }

}




template<State::ScribblingState State>
State::ScribblingState StateClass<State>::StateId() {return TheState;}

void ScribbleArea::mouseReleaseEvent(QMouseEvent *event)
{
   std::cout << "Mouse: ";
   HandleReleaseEventSM(event->button(), event->pos(), false, 0);
}


void ControllingStateMachine::HandleReleaseEventSM(Qt::MouseButton Button, QPointF Position, bool Erasing, double Pressure)
{
   std::cout << "Button Up: " << Button  << std::endl;

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
               EraseLineTo(Position, Pressure);
            } else {
               drawLineTo(Position, Pressure);
            }
            LastDrawnObjectPoints.append(Position);
            LastDrawingValid = true;
            CurrentPaintedObjectBoundingBox.AddPoint(PositionClass(Position.x(), Position.y()));
            LastPaintedObjectBoundingBox = CurrentPaintedObjectBoundingBox;
            CurrentPaintedObjectBoundingBox.Clear();
            if ((State == DrawingFillRequested)||(State == DrawingPaused)) {
               QPainter painter2(&image);
               painter2.setPen(QPen(QColor(0, 0, 0, 0), myPenWidth, Qt::SolidLine, Qt::RoundCap,
                                    Qt::RoundJoin));
               painter2.setBrush(QBrush(myPenColor));
               painter2.setCompositionMode(QPainter::CompositionMode_Source);
               // LastDrawnObjectPoints.translate(-LastPaintedObjectBoundingBox.GetTop(), -LastPaintedObjectBoundingBox.GetLeft());
               painter2.drawPolygon(LastDrawnObjectPoints.translated(Origin));
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
                 std::cout << "LeavingSpeed " << Tracker.GetCurrentSpeed() << std::endl;
                 DiscardSelection = true;
                 update();
             }
             //std::cout << "LeavingSpeed " << (LastDistance + CurrentDistance) << " / " << (DeltaTLastDistance + DeltaTCurrentDistance) << " = " << ((LastDistance + CurrentDistance) / (float)(DeltaTLastDistance + DeltaTCurrentDistance)) << std::endl;
                 ;
           // QPoint Offset =  - SelectedPoint;
             if (DiscardSelection == false) {
                DrawMovedSelection(Position);
             }
           // BoundingBoxClass MovedRectangle(LastPaintedObjectBoundingBox);
          //  MovedRectangle.Move(PositionClass(Offset.x(), Offset.y()));
          //  drawrectangle(MovedRectangle);
           // MoveSelected = false;
            LastDrawingValid = false;
            //        drawrectangle(BoundingBoxClass(LastPaintedObjectBoundingBox).Move(PositionClass(Offset.x(), Offset.y())));
            State = Idle;
            break;
         case MovingPostit:
            if ((!SelectedPostit.empty())) {
               std::cout << "Fixing postit " << std::endl;
               for (auto &r: SelectedPostit) {
                  QPointF LastPosition = r.postit->Position;
                  r.postit->Position = r.StartPosition + (Position - ButtonDownPosition);

                  r.postit->Box.Move(PositionClass(r.postit->Position.x()-LastPosition.x(), r.postit->Position.y()-LastPosition.y()));
                  // Place moved postits on top of each other
                  PostIts.splice( PostIts.end(), PostIts, r.postit);
               }
               //  MoveSelected = false;
               LastDrawingValid = false;
               SelectedPostit.clear();
               setCursor(Qt::ArrowCursor);
               update();
            }
            State = Idle;
            break;

         case WaitingToLeaveJitterProtectionWithSelectedPostitForMoving:
            SelectedPostit.clear();
            setCursor(Qt::ArrowCursor);
            State = Idle;
            break;


         case WaitingToLeaveJitterProtectionForScrolling:
         case ScrollingDrawingArea:
            Origin -= Position - ScrollingLastPosition;
            if (!Frozen) {
               BackgroundImagesOrigin -= Position- ScrollingLastPosition;
            }
            resizeScrolledImage();
            update();
            State = Idle;
            break;

      }
   }
}

void ControllingStateMachine::HandleTouchPressEventSM(int NumberOfTouchpoints, QPointF MeanPosition)
{
   if(NumberOfTouchpoints == 2) {
   SelectedCurrentPosition = MeanPosition;
   SelectedImagePart =  image.copy();
   HintSelectedImagePart = SelectedImagePart;
   HintSelectedImagePart.fill(qRgba(0, 0, 0, 40));
   //Scrolling = true;
   ScrollingLastPosition = SelectedCurrentPosition;
   ScrollingOldOrigin = Origin;
   //scribbling = false;
   update();
   State = WaitingForTouchScrolling;
   }
}

void ControllingStateMachine::HandleTouchMoveEventSM(int NumberOfTouchpoints, QPointF MeanPosition)
{
   if(NumberOfTouchpoints == 2) {
   std::cout << "TM(" << MeanPosition.x() <<":" << MeanPosition.y() << ")";

   switch (State) {
      case WaitingForTouchScrolling:
         State = TouchScrollingDrawingArea;
         [[fallthrough]];
      case TouchScrollingDrawingArea:

         CompleteImage();
         Origin -= MeanPosition- ScrollingLastPosition;
         if (!Frozen) {
            BackgroundImagesOrigin -= MeanPosition- ScrollingLastPosition;
         }
         ScrollingLastPosition = MeanPosition;
         update();
         break;
      default:
         std::cout << "Touch move: unexpected state" << std::endl;
   }
   }
}

void ControllingStateMachine::HandleTouchReleaseEventSM(int NumberOfTouchpoints, QPointF MeanPosition)
{
   if(NumberOfTouchpoints == 2) {

   switch (State) {
   case WaitingForTouchScrolling:
   case TouchScrollingDrawingArea:
      Origin -= MeanPosition - ScrollingLastPosition;
      if (!Frozen) {
         BackgroundImagesOrigin -= MeanPosition- ScrollingLastPosition;
      }
      resizeScrolledImage();
      update();
      State = Idle;
      break;
   }

}

}

void ScribbleArea::tabletEvent(QTabletEvent * event)
{
  // std::cout << "Tablett Pen Type " << event->pointerType() << std::endl;
    switch(event->type()){
       case QEvent::TabletRelease:
          std::cout << "Tablett up " << event->type() << "/"<< event->button() << std::endl;
          HandleReleaseEventSM(event->button(), event->position(), event->pointerType() == QPointingDevice::PointerType::Eraser, event->pressure());
          event->accept();
          break;

       case QEvent::TabletPress:
        std::cout << "Tablett down " << event->type() << "/"<< event->button() << std::endl;
        HandlePressEventSM(event->button(), event->position(), event->timestamp());
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
             std::cout << "Tablett move " << event->type() << "/"<< event->buttons() << " <" << event->pos().x() << ";" << event->pos().y() << ">:" << event->pressure() << std::endl;
             HandleMoveEventSM(event->buttons(), event->position(), event->timestamp(), event->pointerType() == QPointingDevice::PointerType::Eraser, event->pressure());
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
         std::cout << "Got touch begin" << std::endl;
         {
         QPointF MeanPosition(0,0);
         for (auto &p: event->points()) {
            MeanPosition += p.position();
         }
         MeanPosition *= TouchScaling/event->pointCount();
            HandleTouchPressEventSM(event->pointCount(), MeanPosition);
         }
         event->accept();
         return true;
         break;
      case QEvent::TouchCancel:
         std::cout << "Got touch Cancel" << std::endl;
         event->accept();
         return true;
         break;
      case QEvent::TouchEnd:
         std::cout << "Got touch End" << std::endl;
         {
         QPointF MeanPosition(0,0);
         for (auto &p: event->points()) {
            MeanPosition += p.position();
         }
         MeanPosition *= TouchScaling/event->pointCount();
            HandleTouchReleaseEventSM(event->pointCount(), MeanPosition);
         }
         event->accept();
         return true;
         break;
      case QEvent::TouchUpdate:
         std::cout << "Got touch Update" << std::endl;
         {
         QPointF MeanPosition(0,0);
         for (auto &p: event->points()) {
            MeanPosition += p.position();
         }
         MeanPosition *= TouchScaling/event->pointCount();
         if (event->isBeginEvent()) {
            HandleTouchPressEventSM(event->pointCount(), MeanPosition);
         } else if (event->isEndEvent()) {
            HandleTouchReleaseEventSM(event->pointCount(), MeanPosition);
         } else {
            HandleTouchMoveEventSM(event->pointCount(), MeanPosition);
         }
#if 0
         {

            for (auto &p: event->points()) {
               std::cout << "(" << p.position().x() <<":" << p.position().y() << ")";
            }
            std::cout << event->isBeginEvent() << event->isEndEvent() << std::endl;
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
         std::cout << "Gesrture" << std::endl;
         return QWidget::event(event);
      case QEvent::Scroll:
         std::cout << "Scroll" << std::endl;
         return QWidget::event(event);
      case QEvent::Wheel:
         std::cout << "Wheel" << static_cast<QWheelEvent*>(event)->angleDelta().x() << ":" << static_cast<QWheelEvent*>(event)->angleDelta().y() << std::endl;
         {
            QPointF Delta (static_cast<QWheelEvent*>(event)->angleDelta());

            CompleteImage();
            Origin += Delta/20.0;
            if (!Frozen) {
               BackgroundImagesOrigin += Delta/20.0;
            }
            resizeScrolledImage();

            update();

         }
      default:
         //std::cout << "<" << event->type() << ">";
         return QWidget::event(event);

   }

}


void ControllingStateMachine::timeoutSM()
{
    /* Copying on long move pauses */
   std::cout << "timeout " << std::endl;
   switch(State) {
      case Idle:
         break;
      case WaitingToLeaveJitterProtectionForDrawing:
         if (DownInsideObject) {

            SelectedImagePart =  image.copy(LastPaintedObjectBoundingBox.QRectangle().translated(Origin.toPoint()));
            HintSelectedImagePart = SelectedImagePart;
            HintSelectedImagePart.fill(qRgba(0, 0, 0, 0));
            DiscardSelection = false;

            QPainter painter2(&image);
            painter2.setPen(QPen(QColor(0, 0, 0, 0), myPenWidth, Qt::SolidLine, Qt::RoundCap,
                                 Qt::RoundJoin));
            painter2.setBrush(QBrush(QColor(0, 0, 0, 0)));
            painter2.setCompositionMode(QPainter::CompositionMode_Source);
            // LastDrawnObjectPoints.translate(-LastPaintedObjectBoundingBox.GetTop(), -LastPaintedObjectBoundingBox.GetLeft());
            painter2.drawPolygon(LastDrawnObjectPoints.translated(Origin));

            QPainter painter(&HintSelectedImagePart);
            painter.setPen(QPen(QColor(0, 30, 0, 50), myPenWidth, Qt::SolidLine, Qt::RoundCap,
                                Qt::RoundJoin));
            painter.setBrush(QBrush(QColor(0, 30, 0, 50)));
            LastDrawnObjectPoints.translate(-LastPaintedObjectBoundingBox.GetLeft(), -LastPaintedObjectBoundingBox.GetTop());
            painter.drawPolygon(LastDrawnObjectPoints);
            QPainterPath Path;
            Path.addPolygon(LastDrawnObjectPoints);
            QImage MaskedSelectedImagePart = SelectedImagePart;
            MaskedSelectedImagePart.fill(qRgba(0, 0, 0, 0));
            QPainter painter3(&MaskedSelectedImagePart);
            painter3.setClipPath(Path);
            painter3.drawImage(QPoint(0,0), SelectedImagePart);
            SelectedImagePart = MaskedSelectedImagePart;

            LastDrawnObjectPoints.clear();
            modified = true;
            LastDrawnObject.fill(qRgba(0, 0, 0, 0));


            //MoveSelected = true;
            //NewDrawingStarted = false;
            SelectedPoint = lastPoint;
            SelectedOffset = QPoint(LastPaintedObjectBoundingBox.GetLeft(), LastPaintedObjectBoundingBox.GetTop()) - lastPoint;
            //scribbling = false;
            update();
            //WaitForPostIt = true;
            MyTimer.start(Settings.PostItTimeout);
            State = WaitingToLeaveJitterProtectionWithSelectedAreaForMoving;

         } else {

            if (PostItSelected(SelectedCurrentPosition)) {
               //scribbling = false;
               //NewDrawingStarted = false;
               setCursor(Qt::ClosedHandCursor);
               std::cout << "Selected postit " << std::endl;
               State = WaitingToLeaveJitterProtectionWithSelectedPostitForMoving;


            } else {
               SelectedImagePart =  image.copy();
               HintSelectedImagePart = SelectedImagePart;
               HintSelectedImagePart.fill(qRgba(0, 0, 0, 40));
               //Scrolling = true;
               ScrollingLastPosition = SelectedCurrentPosition;
               ScrollingOldOrigin = Origin;
               //scribbling = false;
               update();
               State = WaitingToLeaveJitterProtectionForScrolling;
            }

         }
         break;
      case WaitingToLeaveJitterProtectionWithSelectedAreaForMoving:
         {
            std::cout << "Creating postit " << std::endl;

            //WaitForPostIt = false;
            QImage NewPostit(HintSelectedImagePart);
            // Here we could add a different background for postits
            QPainter painter(&NewPostit);
            painter.setCompositionMode(QPainter::CompositionMode_SourceIn);
            painter.setBrush(QBrush(PostItBackgroundColor));
            painter.drawRect(NewPostit.rect());
            painter.setCompositionMode(QPainter::CompositionMode_SourceOver);
            painter.drawImage(0,0,SelectedImagePart);
            BoundingBoxClass TranslatedBoundingBox (LastPaintedObjectBoundingBox);
            TranslatedBoundingBox.Move(PositionClass(Origin.x(), Origin.y()));
            PostIts.push_back(PostIt(NewPostit, Origin + SelectedCurrentPosition+SelectedOffset, TranslatedBoundingBox));
            SelectedPostit.push_back({std::prev(PostIts.end()), PostIts.back().Position});
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
         FillPolygonStartPosition = lastPoint;
         setCursor(Qt::BusyCursor);
         update();
         State = DrawingPaused;
         break;
      case DrawingPaused:
         break;
      case DrawingFillRequested:
         break;
      case MovingSelection:
         if (DiscardSelection == false) {

            // Fast shaking followed by a pause means throw away selection
            if (Tracker.IsFastShaking()) {
               DiscardSelection = true;
               update();
            } else {
               QPointF CopyPos(SelectedCurrentPosition);
               SelectedCurrentPosition += QPointF(3,3);
               DrawMovedSelection(CopyPos);
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

bool ScribbleArea::PostItSelected(QPointF Position)
{
   bool Found = false;
   SelectedPostit.clear();
   Position += Origin;
   for (auto PostIt = PostIts.begin(); PostIt != PostIts.end(); PostIt++) {
     if (PostIt->Box.IsInside(PositionClass(Position.x(), Position.y()))) {
        SelectedPostit.push_back({PostIt, PostIt->Position});
        Found = true;
     }
   }
   return Found;
}


bool ScribbleArea::IsInsideAnyPostIt(QPointF Position)
{
   Position += Origin;
   for (auto &&PostIt: PostIts) {
     if (PostIt.Box.IsInside(PositionClass(Position.x(), Position.y()))) {
        return true;
     }
   }
   return false;
}
//! [12] //! [13]
void ScribbleArea::paintEvent(QPaintEvent *event)
//! [13] //! [14]
{
    if (ShowOverview) {
       QImage Overview(image.size(), QImage::Format_ARGB32);
       QPainter painter(&Overview);
       PaintVisibleDrawing(painter, Overview.rect(), {0,0}, BackgroundImagesOrigin-Origin);
       painter.setPen(QPen(QColor(90, 0, 0, 50), myPenWidth, Qt::SolidLine, Qt::RoundCap,
                           Qt::RoundJoin));
       painter.setBrush(QBrush(QColor(0, 30, 0, 50)));

       painter.drawRect(Origin.x(), Origin.y(), this->width(), this->height());
       QPainter p(this);
       p.drawImage(QPointF(0,0), Overview.scaled(QSize(this->width(), this->height()), Qt::KeepAspectRatio));
    } else {
    QPainter painter(this);
    PaintVisibleDrawing(painter, event->rect(), Origin, BackgroundImagesOrigin);

    // If we are scrolling, draw a 'shadow' nover everting as feedback
    if ((State == ScrollingDrawingArea)||(State == WaitingToLeaveJitterProtectionForScrolling)
        ||(State == WaitingForTouchScrolling) ||(State == TouchScrollingDrawingArea)) {
        painter.setPen(QPen(QColor(90, 0, 0, 50), myPenWidth, Qt::SolidLine, Qt::RoundCap,
                            Qt::RoundJoin));
        painter.setBrush(QBrush(QColor(0, 30, 0, 50)));

        painter.drawRect(0,0,this->width(), this->height());

    }
    if (ShowPointer) {
       if (Showeraser) {
          painter.drawImage(LastPointerPosition - QPointF(0, 36), EraserShape);
         // painter.drawImage(LastPointerPosition - QPointF(0, 36), SpongeShape);

       } else {
          painter.drawImage(LastPointerPosition - QPointF(0, 36), PointerShape);
       }
    }
    }

}



void ScribbleArea::PaintVisibleDrawing(QPainter &painter, QRect const &dirtyRect, QPointF const &Origin, QPointF const &BackgroundImagesOrigin)
//! [13] //! [14]
{
    //QPainter painter(this);

    // First draw the background
    //QRect dirtyRect = event->rect();
    painter.setPen(QPen(QColor(50,0,0,0), myPenWidth, Qt::SolidLine, Qt::RoundCap,
                        Qt::RoundJoin));
    painter.setBrush(QBrush(BackGroundColor));
    //painter.setBrush(QBrush(QColor(50,0,0,100)));

    painter.drawRect(dirtyRect);

    for (auto &p: BackgroundImages) {
       if (p.IsVisible()) {
          painter.drawImage(dirtyRect, *p, dirtyRect.translated(BackgroundImagesOrigin.toPoint()));
       }
    }

    // In marker mode, last drawn objec belongs into background
    if (MarkerActive) {
        painter.drawImage(dirtyRect, LastDrawnObject, dirtyRect);
    }

    // Then draw our current image (Without the currently drawn object)
    if (EraseLastDrawnObject) {
       QImage ModifiedImage(image);
       QPainter MIpainter(&ModifiedImage);
       MIpainter.setCompositionMode(QPainter::CompositionMode_DestinationOut);
       MIpainter.drawImage(Origin, LastDrawnObject);
       painter.drawImage(dirtyRect, ModifiedImage, dirtyRect.translated(Origin.toPoint()));

    } else {
      painter.drawImage(dirtyRect, image, dirtyRect.translated(Origin.toPoint()));
    //painter.setCompositionMode(QPainter::CompositionMode_Source);
    }
#if 0
    // Probably nonsense, as widget cannot be transparent ???
    if (EraseLastDrawnObject) {

       painter.setCompositionMode(QPainter::CompositionMode_DestinationOut);
    }
#endif
    // Now draw the currently drawn object, in marker mode it was already drawn earlier in background
    if (!MarkerActive && !EraseLastDrawnObject) {
       painter.drawImage(dirtyRect, LastDrawnObject, dirtyRect);
    }
#if 0
    if (EraseLastDrawnObject) {
       painter.setCompositionMode(QPainter::CompositionMode_SourceOut);
    }
#endif

    // Now draw all postits
    for (auto &&Picture: PostIts) {
       painter.drawImage(Picture.Position-Origin, Picture.Image);
       if (ShowPostitsFrame == true) {
          painter.setBrush(QBrush(Qt::NoBrush));
          painter.setPen(Qt::black);
          painter.drawRect(Picture.Box.QRectangle().translated(-Origin.toPoint()));
       }
    }

    // If we have something selected, draw it
    if (((State == MovingSelection)||(State == WaitingToLeaveJitterProtectionWithSelectedAreaForMoving)||(State == MovingSelectionPaused)) && (DiscardSelection == false)) {
       painter.drawImage(SelectedCurrentPosition+SelectedOffset, HintSelectedImagePart);
       painter.drawImage(SelectedCurrentPosition+SelectedOffset, SelectedImagePart);
    }

    if (RecentlyPastedObjectValid == true) {
        painter.drawImage(RecentlyPastedObjectPosition, RecentlyPastedObject);
    }

}
//! [14]

//! [15]
void ScribbleArea::resizeEvent(QResizeEvent *event)
//! [15] //! [16]
{
    if (width() > LastDrawnObject.width() || height() > LastDrawnObject.height()) {
        int newWidth = qMax(width() + 128, image.width());
        int newHeight = qMax(height() + 128, image.height());
        resizeImage(&image, QSize(newWidth+Origin.x(), newHeight+Origin.y()));
        resizeImage(&LastDrawnObject, QSize(newWidth, newHeight));
        if (!Frozen) {
           for (auto &p: BackgroundImages) {
              resizeImage(&*p, QSize(newWidth+BackgroundImagesOrigin.x(), newHeight+BackgroundImagesOrigin.y()));
           }
        }
        update();
    }
    QWidget::resizeEvent(event);
}
//! [16]

//! [17]
void ScribbleArea::drawLineTo(const QPointF &endPoint, double Pressure)
//! [17] //! [18]
{
   std::cout << "Drawing ";
   int ModifiedPenWidth = myPenWidth * (1.0 + Pressure*Pressure*4);
    QPainter painter(&LastDrawnObject);
    painter.setCompositionMode(QPainter::CompositionMode_Source);
    painter.setPen(QPen(myPenColor, ModifiedPenWidth, Qt::SolidLine, Qt::RoundCap,
                        Qt::RoundJoin));
    painter.drawLine(lastPoint, endPoint);
    modified = true;

    int rad = (ModifiedPenWidth / 2) + 2;
    update(QRect(lastPoint.toPoint(), endPoint.toPoint()).normalized()
                                     .adjusted(-rad, -rad, +rad, +rad));
    lastPoint = endPoint;
    EraseLastDrawnObject = false;

}

void ScribbleArea::EraseLineTo(const QPointF &endPoint, double Pressure)
//! [17] //! [18]
{
    std::cout << "Erasing ";
    QPainter painter(&LastDrawnObject);
    int ModifiedPenWidth = (myPenWidth+2)*3*(1.0 + Pressure*Pressure*10);
    painter.setPen(QPen(BackGroundColor, ModifiedPenWidth, Qt::SolidLine, Qt::RoundCap,
                        Qt::RoundJoin));
   // painter.setCompositionMode(QPainter::CompositionMode_Source);
    //painter.setCompositionMode(QPainter::CompositionMode_Clear);
    painter.drawLine(lastPoint, endPoint);
    modified = true;
    EraseLastDrawnObject = true;

    int rad = (ModifiedPenWidth / 2) + 2;
    update(QRect(lastPoint.toPoint(), endPoint.toPoint()).normalized()
                                     .adjusted(-rad, -rad, +rad, +rad));
    lastPoint = endPoint;
}

/*
void ScribbleArea::drawrectangle(const BoundingBoxClass &Region)
//! [17] //! [18]
{
    QPainter painter(&image);
    painter.setPen(QPen(myPenColor, myPenWidth, Qt::SolidLine, Qt::RoundCap,
                        Qt::RoundJoin));
    QRect Rectangle(Region.QRectangle());
    painter.drawRect(Rectangle);
    modified = true;

    int rad = (myPenWidth / 2) + 2;
    update(Rectangle.normalized()
                                     .adjusted(-rad, -rad, +rad, +rad));

}
*/
void ScribbleArea::DrawLastDrawnPicture()
{
    QPainter painter(&image);
    //painter.setCompositionMode(QPainter::CompositionMode_Source);
    if (EraseLastDrawnObject) {
       painter.setCompositionMode(QPainter::CompositionMode_DestinationOut);
    }
    if (MarkerActive) {
       painter.setCompositionMode(QPainter::CompositionMode_DestinationOver);

    }
    painter.drawImage(Origin, LastDrawnObject);
    LastDrawnObject.fill(TransparentColor);
    modified = true;

    update();

}

void ScribbleArea::DrawMovedSelection(const QPointF Offset)
{
    QPainter painter(&image);


    painter.drawImage(SelectedOffset+Offset+Origin, SelectedImagePart);
    modified = true;

    update();

};

//! [18]

//! [19]
void ScribbleArea::resizeImage(QImage *image, const QSize &newSize, QPoint Offset)
//! [19] //! [20]
{
    if (image->size() == newSize && Offset == QPoint(0,0))
        return;

    QImage newImage(newSize, QImage::Format_ARGB32);
    newImage.fill(TransparentColor);
    QPainter painter(&newImage);
    //painter.setCompositionMode(QPainter::CompositionMode_Source);
    painter.drawImage(Offset, *image);
    *image = newImage;
}

void ScribbleArea::GetOffsetAndAdjustOrigin(QImage &Image, QPointF &Origin, QPoint &Offset, QSize &Size)
{
   if (Origin.x() < 0) {
       Size.setWidth(image.size().width() - Origin.x());
       Offset.setX(- Origin.x());
       Origin.setX(0);
   } else if (Origin.x()+this->width() > image.size().width()){
       Size.setWidth(Origin.x()+this->width());
   } else {
       Size.setWidth(image.size().width());
   }
   if (Origin.y() < 0) {
       Size.setHeight(image.size().height() - Origin.y());
       Offset.setY(- Origin.y());
       Origin.setY(0);
   } else if (Origin.y()+this->height() > image.size().height()){
       Size.setHeight(Origin.y()+this->height());
   } else {
       Size.setHeight(image.size().height());
   }
}

void ScribbleArea::resizeScrolledImage()
//! [19] //! [20]
{
    int NewWidth;
    int NewHeight;
    int OffsetX = 0;
    int OffsetY = 0;
    QPoint Offset;
    QSize Size;

    GetOffsetAndAdjustOrigin(image, Origin, Offset, Size);
#if 1
    resizeImage(&image, Size, Offset);
    if (!Frozen && !BackgroundImages.empty()) {
       GetOffsetAndAdjustOrigin(*BackgroundImages[0], BackgroundImagesOrigin, Offset, Size);

       for (auto &p: BackgroundImages) {
          resizeImage(&*p, Size, Offset);
       }
    }

#else
    QImage newImage(QSize(NewWidth ,NewHeight), QImage::Format_ARGB32);
    newImage.fill(TransparentColor);
    QPainter painter(&newImage);
    painter.drawImage(QPoint(OffsetX, OffsetY), image);
    image = newImage;
#endif
    // Now adjust all postits
    for (auto &&Picture: PostIts) {
       Picture.Position += QPoint(OffsetX, OffsetY);
       Picture.Box.Move(PositionClass(OffsetX, OffsetY));

    }

}





//! [20]

//! [21]
void ScribbleArea::print()
{
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
}
//! [22]
