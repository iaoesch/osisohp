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

#include "scribbleareaStateMachine.h"
#ifdef USE_NEW_STATEMACHINE


//! [0]
ControllingStateMachine::ControllingStateMachine()

{
    Context.ShowPointer = false;

    Context.ShowOverview = false;

    Context.LastDrawingValid = false;
    Context.EraseLastDrawnObject = false;
    Context.Frozen = false;
    Context.StateMachine.Context.myPenWidth = 2;
    Context.SelectedPenWidth = StateMachine.Context.myPenWidth;

    connect(&MyTimer, SIGNAL(timeout()), this, SLOT(timeoutSM()));
    MyTimer.setSingleShot(true);

    connect(&PointerTimer, &QTimer::timeout, this, &ScribbleArea::PointerTimeout);
    PointerTimer.setSingleShot(true);

    Context.SelectedPostit.clear();
    Context.SelectPostitsDirectly = false;
    Context.ShowPostitsFrame = false;



    Context.RecentlyPastedObjectValid = false;
    Context.MarkerActive = false;
   // QImage RecentlyMovedObject;
   // BoundingBoxClass RecentlyMovedObjectBoundingBox;


}

//! [0]
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


        StateMachine.Context.lastPoint = Position;
        StateMachine.Context.ButtonDownPosition = Position;
        StateMachine.Context.CurrentPaintedObjectBoundingBox.Clear();
        StateMachine.Context.CurrentPaintedObjectBoundingBox.AddPoint(PositionClass(StateMachine.Context.lastPoint.x(), StateMachine.Context.lastPoint.y()));
        StateMachine.Context.SelectedCurrentPosition = Position;

        if ((StateMachine.Context.SelectPostitsDirectly == true) &&
            (StateMachine.PostItSelected(StateMachine.Context.SelectedCurrentPosition))) {
              StateMachine.Context.DownInsideObject = false;
              StateMachine.MyTimer.start(StateMachine.Settings.DirectSelectTimeout);

        } else {
           StateMachine.MyTimer.start(StateMachine.Settings.SelectTimeout);


           if (StateMachine.Context.LastPaintedObjectBoundingBox.IsInside(PositionClass(Position.x(), Position.y()))) {
              StateMachine.Context.DownInsideObject = true;
           } else {
              StateMachine.Context.DownInsideObject = false;
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




void StateBaseClass::HandleMoveEventSM(Qt::MouseButtons Buttons, QPointF Position, ulong Timestamp, bool Erasing, double Pressure)
{

   StateMachine.Tracker.Trackmovement(Position, Timestamp);
   //WaitForPostIt = false;
   StateMachine.Context.Showeraser = Erasing;
   StateMachine.Context.LastPointerPosition = Position;
}

void ControllingStateMachine::ShowBigPointer()
{
   if (Settings.PointerHoldon >= 0) {
   Context.ShowPointer = true;
   PointerTimer.start(Settings.PointerHoldon);
   update();
   }
}

void StateBaseClass::HandleMoveNoLeftButtonEvent(Qt::MouseButtons Buttons)
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
       if ((Position-StateMachine.Context.lastPoint).manhattanLength() < StateMachine.Context.myPenWidth+2) {
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
       if ((Position-StateMachine.Context.lastPoint).manhattanLength() < StateMachine.Context.myPenWidth+2) {
           return; // ignore small movements (probably use penwidth*2)
       }
       if (((Position-StateMachine.Context.ButtonDownPosition).manhattanLength() < (StateMachine.Context.myPenWidth*3+2))) {
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
       if ((Position-StateMachine.Context.lastPoint).manhattanLength() < StateMachine.Context.myPenWidth+2) {
           return; // ignore small movements (probably use penwidth*2)
       }
       if (((Position-StateMachine.Context.ButtonDownPosition).manhattanLength() < (StateMachine.Context.myPenWidth*3+2))) {
           return; // ignore small movements (probably use penwidth*2)
       }
       StateMachine.MyTimer.stop();
       StateMachine.MyTimer.start(StateMachine.Settings.GestureTimeout);
       if (StateMachine.Context.LastDrawingValid) {
         DrawLastDrawnPicture();

         StateMachine.Context.LastDrawingValid = false;
         StateMachine.Context.LastDrawnObjectPoints.clear();
         StateMachine.Context.LastDrawnObjectPoints.append(StateMachine.Context.lastPoint);

      }
      //LastDrawnObject.fill(BackgroundColor);
      if (Erasing) {
         EraseLineTo(Position, Pressure);
      } else {
         drawLineTo(Position, Pressure);
      }
      StateMachine.Context.LastDrawnObjectPoints.append(Position);
      StateMachine.Context.CurrentPaintedObjectBoundingBox.AddPoint(PositionClass(Position.x(), Position.y()));

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
       if ((Position-StateMachine.Context.lastPoint).manhattanLength() < StateMachine.Context.myPenWidth+2) {
           return; // ignore small movements (probably use penwidth*2)
       }
       if (((Position-StateMachine.Context.ButtonDownPosition).manhattanLength() < (StateMachine.Context.myPenWidth*3+2))) {
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
       if ((Position-StateMachine.Context.lastPoint).manhattanLength() < StateMachine.Context.myPenWidth+2) {
           return; // ignore small movements (probably use penwidth*2)
       }
       if (((Position-StateMachine.Context.ButtonDownPosition).manhattanLength() < (StateMachine.Context.myPenWidth*3+2))) {
           return; // ignore small movements (probably use penwidth*2)
       }
       if ((Position-StateMachine.Context.FillPolygonStartPosition).manhattanLength() > (StateMachine.Context.myPenWidth*3+2)) {
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
       if ((Position-StateMachine.Context.lastPoint).manhattanLength() < StateMachine.Context.myPenWidth+2) {
           return; // ignore small movements (probably use penwidth*2)
       }
       if (((Position-StateMachine.Context.ButtonDownPosition).manhattanLength() < (StateMachine.Context.myPenWidth*3+2))) {
           return; // ignore small movements (probably use penwidth*2)
       }
       // QPoint Offset = event->pos() - SelectedPoint;
         StateMachine.MyTimer.stop();
         StateMachine.MyTimer.start(StateMachine.Settings.CopyTimeout);

         StateMachine.Context.SelectedCurrentPosition = Position;
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
       if ((Position-StateMachine.Context.lastPoint).manhattanLength() < StateMachine.Context.myPenWidth+2) {
           return; // ignore small movements (probably use penwidth*2)
       }
       if (((Position-StateMachine.Context.ButtonDownPosition).manhattanLength() < (StateMachine.Context.myPenWidth*3+2))) {
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
       if ((Position-StateMachine.Context.lastPoint).manhattanLength() < StateMachine.Context.myPenWidth+2) {
           return; // ignore small movements (probably use penwidth*2)
       }
       if (((Position-StateMachine.Context.ButtonDownPosition).manhattanLength() < (StateMachine.Context.myPenWidth*3+2))) {
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
       if ((Position-StateMachine.Context.lastPoint).manhattanLength() < StateMachine.Context.myPenWidth+2) {
           return; // ignore small movements (probably use penwidth*2)
       }
       if (((Position-StateMachine.Context.ButtonDownPosition).manhattanLength() < (StateMachine.Context.myPenWidth*3+2))) {
           return; // ignore small movements (probably use penwidth*2)
       }
       if (!SelectedPostit.empty()) {
          std::cout << "Moving postit " << std::endl;
          for(auto &r: SelectedPostit) {
             //SelectedPostit->Position = Position;
             QPointF LastPosition = r.postit->Position;
             r.postit->Position = r.StartPosition + (Position - StateMachine.Context.ButtonDownPosition);

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
       if ((Position-StateMachine.Context.lastPoint).manhattanLength() < StateMachine.Context.myPenWidth+2) {
           return; // ignore small movements (probably use penwidth*2)
       }
       if (((Position-StateMachine.Context.ButtonDownPosition).manhattanLength() < (StateMachine.Context.myPenWidth*3+2))) {
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
       if ((Position-StateMachine.Context.lastPoint).manhattanLength() < StateMachine.Context.myPenWidth+2) {
           return; // ignore small movements (probably use penwidth*2)
       }
       if (((Position-StateMachine.Context.ButtonDownPosition).manhattanLength() < (StateMachine.Context.myPenWidth*3+2))) {
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
       if ((Position-StateMachine.Context.lastPoint).manhattanLength() < StateMachine.Context.myPenWidth+2) {
           return; // ignore small movements (probably use penwidth*2)
       }
       if (((Position-StateMachine.Context.ButtonDownPosition).manhattanLength() < (StateMachine.Context.myPenWidth*3+2))) {
           return; // ignore small movements (probably use penwidth*2)
       }
       CompleteImage();
      StateMachine.Context.Origin -= Position- StateMachine.Context.ScrollingLastPosition;
      if (!StateMachine.Context.Frozen) {
         StateMachine.Context.BackgroundImagesOrigin -= Position- StateMachine.Context.ScrollingLastPosition;
      }

      StateMachine.Context.ScrollingLastPosition = Position;
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
       if ((Position-StateMachine.Context.lastPoint).manhattanLength() < StateMachine.Context.myPenWidth+2) {
           return; // ignore small movements (probably use penwidth*2)
       }
       if (((Position-StateMachine.Context.ButtonDownPosition).manhattanLength() < (StateMachine.Context.myPenWidth*3+2))) {
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

template<>
void StateClass<State::ScribblingState::Idle>::HandleReleaseEventSM(Qt::MouseButton Button, QPointF Position, bool Erasing, double Pressure)
{
   if (Button == Qt::LeftButton) {
      // Do Nothing
   }
}

template<>
void StateClass<State::ScribblingState::WaitingToLeaveJitterProtectionWithSelectedAreaForMoving>::HandleReleaseEventSM(Qt::MouseButton Button, QPointF Position, bool Erasing, double Pressure)
{
   if (Button == Qt::LeftButton) {
      StateMachine.SetNewState(&StateMachine.Idle);
   }
}

template<>
void StateClass<State::ScribblingState::WaitingToLeaveJitterProtectionWithCreatedPostitForMoving>::HandleReleaseEventSM(Qt::MouseButton Button, QPointF Position, bool Erasing, double Pressure)
{
   if (Button == Qt::LeftButton) {
      StateMachine.SetNewState(&StateMachine.Idle);
   }
}

template<>
void StateClass<State::ScribblingState::Drawing>::HandleReleaseEventSM(Qt::MouseButton Button, QPointF Position, bool Erasing, double Pressure)
{
   if (Button == Qt::LeftButton) {
      StateMachine.MyTimer.stop();
      if (Erasing) {
         EraseLineTo(Position, Pressure);
      } else {
         drawLineTo(Position, Pressure);
      }
      StateMachine.Context.LastDrawnObjectPoints.append(Position);
      StateMachine.Context.LastDrawingValid = true;
      StateMachine.Context.CurrentPaintedObjectBoundingBox.AddPoint(PositionClass(Position.x(), Position.y()));
      StateMachine.Context.LastPaintedObjectBoundingBox = StateMachine.Context.CurrentPaintedObjectBoundingBox;
      StateMachine.Context.CurrentPaintedObjectBoundingBox.Clear();

      StateMachine.SetNewState(&StateMachine.Idle);
   }
}

template<>
void StateClass<State::ScribblingState::WaitingToLeaveJitterProtectionForDrawing>::HandleReleaseEventSM(Qt::MouseButton Button, QPointF Position, bool Erasing, double Pressure)
{
   if (Button == Qt::LeftButton) {
      StateMachine.Drawing.HandleReleaseEventSM(Button, Position, Erasing, Pressure);
   }
}

template<>
void StateClass<State::ScribblingState::DrawingFillRequested>::HandleReleaseEventSM(Qt::MouseButton Button, QPointF Position, bool Erasing, double Pressure)
{
   if (Button == Qt::LeftButton) {
      StateMachine.Drawing.HandleReleaseEventSM(Button, Position, Erasing, Pressure);
      QPainter painter2(&image);
      painter2.setPen(QPen(QColor(0, 0, 0, 0), StateMachine.Context.myPenWidth, Qt::SolidLine, Qt::RoundCap,
                           Qt::RoundJoin));
      painter2.setBrush(QBrush(myPenColor));
      painter2.setCompositionMode(QPainter::CompositionMode_Source);
      // StateMachine.Context.LastDrawnObjectPoints.translate(-LastPaintedObjectBoundingBox.GetTop(), -LastPaintedObjectBoundingBox.GetLeft());
      painter2.drawPolygon(StateMachine.Context.LastDrawnObjectPoints.translated(StateMachine.Context.Origin));
      setCursor(Qt::ArrowCursor);
      //FillPolygon = false;

      update();
   }
}

template<>
void StateClass<State::ScribblingState::DrawingPaused>::HandleReleaseEventSM(Qt::MouseButton Button, QPointF Position, bool Erasing, double Pressure)
{
   if (Button == Qt::LeftButton) {
      StateMachine.DrawingFillRequested.HandleReleaseEventSM(Button, Position, Erasing, Pressure);

   }
}

template<>
void StateClass<State::ScribblingState::MovingSelection>::HandleReleaseEventSM(Qt::MouseButton Button, QPointF Position, bool Erasing, double Pressure)
{
   if (Button == Qt::LeftButton) {
      //WaitForPostIt = false;
       if (StateMachine.Tracker.GetCurrentSpeed() > 0.25f) {
           std::cout << "LeavingSpeed " << StateMachine.Tracker.GetCurrentSpeed() << std::endl;
           StateMachine.Context.DiscardSelection = true;
           update();
       }
       //std::cout << "LeavingSpeed " << (LastDistance + CurrentDistance) << " / " << (DeltaTLastDistance + DeltaTCurrentDistance) << " = " << ((LastDistance + CurrentDistance) / (float)(DeltaTLastDistance + DeltaTCurrentDistance)) << std::endl;
           ;
     // QPoint Offset =  - SelectedPoint;
       if (StateMachine.Context.DiscardSelection == false) {
          DrawMovedSelection(Position);
       }
     // BoundingBoxClass MovedRectangle(LastPaintedObjectBoundingBox);
    //  MovedRectangle.Move(PositionClass(Offset.x(), Offset.y()));
    //  drawrectangle(MovedRectangle);
     // MoveSelected = false;
      StateMachine.Context.LastDrawingValid = false;
      //        drawrectangle(BoundingBoxClass(LastPaintedObjectBoundingBox).Move(PositionClass(Offset.x(), Offset.y())));
      StateMachine.SetNewState(&StateMachine.Idle);
   }
}
template<>
void StateClass<State::ScribblingState::MovingSelectionPaused>::HandleReleaseEventSM(Qt::MouseButton Button, QPointF Position, bool Erasing, double Pressure)
{
   if (Button == Qt::LeftButton) {
      StateMachine.MovingSelection.HandleReleaseEventSM(Button, Position, Erasing, Pressure);

   }
}
template<>
void StateClass<State::ScribblingState::MovingPostit>::HandleReleaseEventSM(Qt::MouseButton Button, QPointF Position, bool Erasing, double Pressure)
{
   if (Button == Qt::LeftButton) {
      if ((!SelectedPostit.empty())) {
         std::cout << "Fixing postit " << std::endl;
         for (auto &r: SelectedPostit) {
            QPointF LastPosition = r.postit->Position;
            r.postit->Position = r.StartPosition + (Position - StateMachine.Context.ButtonDownPosition);

            r.postit->Box.Move(PositionClass(r.postit->Position.x()-LastPosition.x(), r.postit->Position.y()-LastPosition.y()));
            // Place moved postits on top of each other
            PostIts.splice( PostIts.end(), PostIts, r.postit);
         }
         //  MoveSelected = false;
         StateMachine.Context.LastDrawingValid = false;
         SelectedPostit.clear();
         setCursor(Qt::ArrowCursor);
         update();
      }
      StateMachine.SetNewState(&StateMachine.Idle);
   }
}

template<>
void StateClass<State::ScribblingState::WaitingToLeaveJitterProtectionWithSelectedPostitForMoving>::HandleReleaseEventSM(Qt::MouseButton Button, QPointF Position, bool Erasing, double Pressure)
{
   if (Button == Qt::LeftButton) {
      SelectedPostit.clear();
      setCursor(Qt::ArrowCursor);

      StateMachine.SetNewState(&StateMachine.Idle);

   }
}

template<>
void StateClass<State::ScribblingState::ScrollingDrawingArea>::HandleReleaseEventSM(Qt::MouseButton Button, QPointF Position, bool Erasing, double Pressure)
{
   if (Button == Qt::LeftButton) {
      StateMachine.Context.Origin -= Position - StateMachine.Context.ScrollingLastPosition;
      if (!StateMachine.Context.Frozen) {
         StateMachine.Context.BackgroundImagesOrigin -= Position- StateMachine.Context.ScrollingLastPosition;
      }
      resizeScrolledImage();
      update();

      StateMachine.SetNewState(&StateMachine.Idle);

   }
}
template<>
void StateClass<State::ScribblingState::WaitingToLeaveJitterProtectionForScrolling>::HandleReleaseEventSM(Qt::MouseButton Button, QPointF Position, bool Erasing, double Pressure)
{
   if (Button == Qt::LeftButton) {
      StateMachine.ScrollingDrawingArea.HandleReleaseEventSM(Button, Position, Erasing, Pressure);

   }
}


void ControllingStateMachine::HandleReleaseEventSM(Qt::MouseButton Button, QPointF Position, bool Erasing, double Pressure)
{
   std::cout << "Button Up: " << Button  << std::endl;

}


void StateBaseClass::HandleTouchPressEventSM(int NumberOfTouchpoints, QPointF MeanPosition)
{
   if(NumberOfTouchpoints == 2) {
   StateMachine.Context.SelectedCurrentPosition = MeanPosition;
   SelectedImagePart =  image.copy();
   HintSelectedImagePart = SelectedImagePart;
   HintSelectedImagePart.fill(qRgba(0, 0, 0, 40));
   //Scrolling = true;
   StateMachine.Context.ScrollingLastPosition = StateMachine.Context.SelectedCurrentPosition;
   StateMachine.Context.ScrollingOldOrigin = StateMachine.Context.Origin;
   //scribbling = false;
   update();
   StateMachine.SetNewState(&StateMachine.WaitingForTouchScrolling);
   }

}
void ControllingStateMachine::HandleTouchPressEventSM(int NumberOfTouchpoints, QPointF MeanPosition)
{
}

template<>
void StateClass<State::ScribblingState::TouchScrollingDrawingArea>::HandleTouchMoveEventSM(int NumberOfTouchpoints, QPointF MeanPosition)
{
   if(NumberOfTouchpoints == 2) {
   std::cout << "TM(" << MeanPosition.x() <<":" << MeanPosition.y() << ")";

         CompleteImage();
         StateMachine.Context.Origin -= MeanPosition- StateMachine.Context.ScrollingLastPosition;
         if (!Frozen) {
            StateMachine.Context.BackgroundImagesOrigin -= MeanPosition- StateMachine.Context.ScrollingLastPosition;
         }
         StateMachine.Context.ScrollingLastPosition = MeanPosition;
         update();
   }

}
template<>
void StateClass<State::ScribblingState::WaitingForTouchScrolling>::HandleTouchMoveEventSM(int NumberOfTouchpoints, QPointF MeanPosition)
{
   if(NumberOfTouchpoints == 2) {
   std::cout << "TM(" << MeanPosition.x() <<":" << MeanPosition.y() << ")";

   StateMachine.SetNewState(&StateMachine.TouchScrollingDrawingArea);
   StateMachine.TouchScrollingDrawingArea.HandleTouchMoveEventSM(NumberOfTouchpoints, MeanPosition);
   }
}


void ControllingStateMachine::HandleTouchMoveEventSM(int NumberOfTouchpoints, QPointF MeanPosition)
{
}

template<>
void StateClass<State::ScribblingState::TouchScrollingDrawingArea>::HandleTouchReleaseEventSM(int NumberOfTouchpoints, QPointF MeanPosition)
{
   if(NumberOfTouchpoints == 2) {
      StateMachine.Context.Origin -= MeanPosition - StateMachine.Context.ScrollingLastPosition;
      if (!Frozen) {
         StateMachine.Context.BackgroundImagesOrigin -= MeanPosition- StateMachine.Context.ScrollingLastPosition;
      }
      resizeScrolledImage();
      update();

      StateMachine.SetNewState(&StateMachine.Idle);
   }
}

template<>
void StateClass<State::ScribblingState::WaitingForTouchScrolling>::HandleTouchReleaseEventSM(int NumberOfTouchpoints, QPointF MeanPosition)
{
   if(NumberOfTouchpoints == 2) {

      StateMachine.TouchScrollingDrawingArea.HandleTouchMoveEventSM(NumberOfTouchpoints, MeanPosition);
   }
}

void ControllingStateMachine::HandleTouchReleaseEventSM(int NumberOfTouchpoints, QPointF MeanPosition)
{

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
            StateMachine.Context.Origin += Delta/20.0;
            if (!Frozen) {
               StateMachine.Context.BackgroundImagesOrigin += Delta/20.0;
            }
            resizeScrolledImage();

            update();

         }
      default:
         //std::cout << "<" << event->type() << ">";
         return QWidget::event(event);

   }

}

void StateBaseClass::timeoutSM()
{
   // Default does nothing
}


template<>
void StateClass<State::ScribblingState::WaitingToLeaveJitterProtectionForDrawing>::timeoutSM()
{
   if (StateMachine.Context.DownInsideObject) {

      SelectedImagePart =  image.copy(LastPaintedObjectBoundingBox.QRectangle().translated(StateMachine.Context.Origin.toPoint()));
      HintSelectedImagePart = SelectedImagePart;
      HintSelectedImagePart.fill(qRgba(0, 0, 0, 0));
      StateMachine.Context.DiscardSelection = false;

      QPainter painter2(&image);
      painter2.setPen(QPen(QColor(0, 0, 0, 0), StateMachine.Context.myPenWidth, Qt::SolidLine, Qt::RoundCap,
                           Qt::RoundJoin));
      painter2.setBrush(QBrush(QColor(0, 0, 0, 0)));
      painter2.setCompositionMode(QPainter::CompositionMode_Source);
      // StateMachine.Context.LastDrawnObjectPoints.translate(-LastPaintedObjectBoundingBox.GetTop(), -LastPaintedObjectBoundingBox.GetLeft());
      painter2.drawPolygon(StateMachine.Context.LastDrawnObjectPoints.translated(StateMachine.Context.Origin));

      QPainter painter(&HintSelectedImagePart);
      painter.setPen(QPen(QColor(0, 30, 0, 50), StateMachine.Context.myPenWidth, Qt::SolidLine, Qt::RoundCap,
                          Qt::RoundJoin));
      painter.setBrush(QBrush(QColor(0, 30, 0, 50)));
      StateMachine.Context.LastDrawnObjectPoints.translate(-LastPaintedObjectBoundingBox.GetLeft(), -LastPaintedObjectBoundingBox.GetTop());
      painter.drawPolygon(StateMachine.Context.LastDrawnObjectPoints);
      QPainterPath Path;
      Path.addPolygon(StateMachine.Context.LastDrawnObjectPoints);
      QImage MaskedSelectedImagePart = SelectedImagePart;
      MaskedSelectedImagePart.fill(qRgba(0, 0, 0, 0));
      QPainter painter3(&MaskedSelectedImagePart);
      painter3.setClipPath(Path);
      painter3.drawImage(QPoint(0,0), SelectedImagePart);
      SelectedImagePart = MaskedSelectedImagePart;

      StateMachine.Context.LastDrawnObjectPoints.clear();
      modified = true;
      LastDrawnObject.fill(qRgba(0, 0, 0, 0));


      //MoveSelected = true;
      //NewDrawingStarted = false;
      StateMachine.Context.SelectedPoint = StateMachine.Context.lastPoint;
      StateMachine.Context.SelectedOffset = QPoint(StateMachine.Context.LastPaintedObjectBoundingBox.GetLeft(), StateMachine.Context.LastPaintedObjectBoundingBox.GetTop()) - StateMachine.Context.lastPoint;
      //scribbling = false;
      update();
      //WaitForPostIt = true;
      StateMachine.MyTimer.start(StateMachine.Settings.PostItTimeout);
      StateMachine.SetNewState(&StateMachine.WaitingToLeaveJitterProtectionWithSelectedAreaForMoving);

   } else {

      if (PostItSelected(StateMachine.Context.SelectedCurrentPosition)) {
         //scribbling = false;
         //NewDrawingStarted = false;
         setCursor(Qt::ClosedHandCursor);
         std::cout << "Selected postit " << std::endl;
         StateMachine.SetNewState(&StateMachine.WaitingToLeaveJitterProtectionWithSelectedPostitForMoving);


      } else {
         SelectedImagePart =  image.copy();
         HintSelectedImagePart = SelectedImagePart;
         HintSelectedImagePart.fill(qRgba(0, 0, 0, 40));
         //Scrolling = true;
         StateMachine.Context.ScrollingLastPosition = StateMachine.Context.SelectedCurrentPosition;
         StateMachine.Context.ScrollingOldOrigin = StateMachine.Context.Origin;
         //scribbling = false;
         update();
         StateMachine.SetNewState(&StateMachine.WaitingToLeaveJitterProtectionForScrolling);
      }
   }
}


template<>
void StateClass<State::ScribblingState::WaitingToLeaveJitterProtectionWithSelectedAreaForMoving>::timeoutSM()
{
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
      TranslatedBoundingBox.Move(PositionClass(StateMachine.Context.Origin.x(), StateMachine.Context.Origin.y()));
      PostIts.push_back(PostIt(NewPostit, StateMachine.Context.Origin + StateMachine.Context.SelectedCurrentPosition+SelectedOffset, TranslatedBoundingBox));
      SelectedPostit.push_back({std::prev(PostIts.end()), PostIts.back().Position});
      StateMachine.SetNewState(&StateMachine.WaitingToLeaveJitterProtectionWithCreatedPostitForMoving);
      update();
   }

}

template<>
void StateClass<State::ScribblingState::Drawing>::timeoutSM()
{
   //   QTextStream(stdout) << "<" << GestureTrackerAccumulatedSpeed.x() << "; " << GestureTrackerAccumulatedSpeed.y() << "> <"  << GestureTrackerAccumulatedSquaredSpeed.x() << ";" << GestureTrackerAccumulatedSquaredSpeed.y() << endl;

   //FillPolygon = true;
   StateMachine.Context.FillPolygonStartPosition = StateMachine.Context.lastPoint;
   setCursor(Qt::BusyCursor);
   update();
   StateMachine.SetNewState(&StateMachine.DrawingPaused);

}
template<>
void StateClass<State::ScribblingState::MovingSelection>::timeoutSM()
{
   if (StateMachine.Context.DiscardSelection == false) {

      // Fast shaking followed by a pause means throw away selection
      if (StateMachine.Tracker.IsFastShaking()) {
         StateMachine.Context.DiscardSelection = true;
         update();
      } else {
         QPointF CopyPos(StateMachine.Context.SelectedCurrentPosition);
         StateMachine.Context.SelectedCurrentPosition += QPointF(3,3);
         DrawMovedSelection(CopyPos);
      }
      StateMachine.SetNewState(&StateMachine.MovingSelectionPaused);
   }

}

#endif

