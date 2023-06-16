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
#include "databaseclass.h"
#include "DebugStream.hpp"

template<>
void StateClass<State::ScribblingState::Idle>::HandlePressEventSM(Qt::MouseButton Button, QPointF Position, ulong Timestamp);

void StateBaseClass::HandlePressEventSM(Qt::MouseButton Button, QPointF Position, ulong Timestamp)
{
   DEBUG_LOG << "HandlePressEventSM: unexpected State: (Baseclass)" << StateId()  << std::endl;
   StateMachine.Idle.HandlePressEventSM(Button, Position, Timestamp);
}





void StateBaseClass::HandleMoveEventSM(Qt::MouseButtons Buttons, QPointF Position, ulong Timestamp, bool Erasing, double Pressure)
{

   StateMachine.Tracker.Trackmovement(Position, Timestamp);
   //WaitForPostIt = false;
   StateMachine.Context.Showeraser = Erasing;
   StateMachine.Context.LastPointerPosition = Position;
}

void StateBaseClass::HandleReleaseEventSM(Qt::MouseButton Button, QPointF Position, bool Erasing, double Pressure)
{

}

void StateBaseClass::HandleMoveNoLeftButtonEvent(Qt::MouseButtons Buttons, QPointF Position)
{
   if (Buttons == Qt::NoButton) {
      if (StateMachine.Context.MyDatas.IsInsideAnyPostIt(Position)) {
         StateMachine.Interface.setCursor(Qt::PointingHandCursor);
      }  else {
         StateMachine.Interface.setCursor(Qt::ArrowCursor);
      }
   } else {
     StateMachine.Interface.setCursor(Qt::ArrowCursor);
   }
}


void StateBaseClass::timeoutSM()
{
   // Default does nothing
}



void StateBaseClass::HandleTouchPressEventSM(int NumberOfTouchpoints, QPointF MeanPosition)
{
   if(NumberOfTouchpoints == 2) {
      StateMachine.Context.MyDatas.setSelectedCurrentPosition(MeanPosition);
      StateMachine.Context.MyDatas.MakeSreenMoveHint();
   //Scrolling = true;
   StateMachine.Context.ScrollingLastPosition = StateMachine.Context.MyDatas.getSelectedCurrentPosition();
   StateMachine.Context.ScrollingOldOrigin = StateMachine.Context.MyDatas.GetOrigin();
   //scribbling = false;
   StateMachine.Interface.UpdateRequest();
   StateMachine.SetNewState(&StateMachine.WaitingForTouchScrolling);
   }

}

void StateBaseClass::HandleTouchMoveEventSM(int NumberOfTouchpoints, QPointF MeanPosition)
{

}

void StateBaseClass::HandleTouchReleaseEventSM(int NumberOfTouchpoints, QPointF MeanPosition)
{
}

void StateBaseClass::HandleOverviewEventSM(bool Enabled)
{

}

void StateBaseClass::HandlePasteEventSM(QImage Image)
{

}

void StateBaseClass::HandleKeyEventSM(PasteEvent Event)
{

}

template<State::ScribblingState State>
State::ScribblingState StateClass<State>::StateId() {return TheState;}



template<State::ScribblingState State>
void StateClass<State>::HandlePressEventSM(Qt::MouseButton Button, QPointF Position, ulong Timestamp)
{
   DEBUG_LOG << "Unexpected event HandlePressEventSM() in default<> State " << StateId() << std::endl;
}

template<State::ScribblingState State>
void StateClass<State>::HandleMoveEventSM(Qt::MouseButtons Buttons, QPointF Position, ulong Timestamp, bool Erasing, double Pressure)
{
   DEBUG_LOG << "Unexpected event HandleMoveEventSM() in default<> State " << StateId() << std::endl;
}

template<State::ScribblingState State>
void StateClass<State>::HandleReleaseEventSM(Qt::MouseButton Button, QPointF Position, bool Erasing, double Pressure)

{
   DEBUG_LOG << "Unexpected event HandleReleaseEventSM() in default<>  State " << StateId() << std::endl;
}


template<State::ScribblingState State>
void StateClass<State>::HandleTouchPressEventSM(int NumberOfTouchpoints, QPointF MeanPosition)
{
   //DEBUG_LOG << "Unexpected event HandlePressEventSM() in State " << StateId() << std::endl;
   StateBaseClass::HandleTouchPressEventSM(NumberOfTouchpoints, MeanPosition);
}

   template<State::ScribblingState State>
   void StateClass<State>::HandleTouchMoveEventSM(int NumberOfTouchpoints, QPointF MeanPosition)
   {
      DEBUG_LOG << "Unexpected event HandleTouchMoveEventSM() in default<> State " << StateId() << std::endl;
   }

   template<State::ScribblingState State>
   void StateClass<State>::HandleTouchReleaseEventSM(int NumberOfTouchpoints, QPointF MeanPosition)
   {
      DEBUG_LOG << "Unexpected event HandleTouchRelaseEventSM() in default<> State " << StateId() << std::endl;
   }

   template<State::ScribblingState State>
   void StateClass<State>::HandleOverviewEventSM(bool Enabled)
   {
      DEBUG_LOG << "Unexpected event HandleOverviewEventSM() in default<> State " << StateId() << std::endl;
   }

   template<State::ScribblingState State>
   void StateClass<State>::HandlePasteEventSM(QImage Image)
   {
      DEBUG_LOG << "Unexpected event HandlePasteEventSM() in default<> State " << StateId() << std::endl;
   }

   template<State::ScribblingState State>
   void StateClass<State>::HandleKeyEventSM(PasteEvent Event)
   {
      DEBUG_LOG << "Unexpected event HandleKeyEventSM() in default<> State " << StateId() << std::endl;
   }


   template<State::ScribblingState State>
   void StateClass<State>::timeoutSM()
   {
      DEBUG_LOG << "Unexpected event timeoutSM() in default<> State " << StateId() << std::endl;
   }



/***************  IDLE  *****************/
//! [0]
template<>
void StateClass<State::ScribblingState::Idle>::HandlePressEventSM(Qt::MouseButton Button, QPointF Position, ulong Timestamp)
{
    DEBUG_LOG << "Button Down: " << Button  << std::endl;
    if (Button == Qt::LeftButton) {
       StateMachine.Interface.setCursor(Qt::ArrowCursor);

        StateMachine.SetNewState(&StateMachine.WaitingToLeaveJitterProtectionForDrawing);


        StateMachine.Tracker.StartTracking(Position, Timestamp);

        StateMachine.Context.MyDatas.setLastPoint(Position);
        StateMachine.Context.MyDatas.setButtonDownPosition(Position);
        StateMachine.Context.MyDatas.RestartCurrentPaintedObjectBoundingBox(Position);
        StateMachine.Context.MyDatas.setSelectedCurrentPosition(Position);

        if ((StateMachine.Context.SelectPostitsDirectly == true) &&
            (StateMachine.Context.MyDatas.PostItSelected(Position))) {
              StateMachine.Context.DownInsideObject = false;
              StateMachine.MyTimer.start(StateMachine.Settings.DirectSelectTimeout);

        } else {
           StateMachine.MyTimer.start(StateMachine.Settings.SelectTimeout);


           if (StateMachine.Context.MyDatas.IsInsideLstPaintedObjectBoundingBox(Position)) {
              StateMachine.Context.DownInsideObject = true;
           } else {
              StateMachine.Context.DownInsideObject = false;
           }
        }
    }
}


template<>
void StateClass<State::ScribblingState::Idle>::HandleMoveEventSM(Qt::MouseButtons Buttons, QPointF Position, ulong Timestamp, bool Erasing, double Pressure)
{
   StateBaseClass::HandleMoveEventSM(Buttons, Position, Timestamp, Erasing, Pressure);
   StateMachine.ShowBigPointer();
   if ((Buttons & Qt::LeftButton)) {
       if (StateMachine.Context.MyDatas.IsSelectionJitter(Position, StateMachine.Context.MyDatas.getLastPoint(), Pressure)) {
           return; // ignore small movements (probably use penwidth*2)
       }
   }  else {
      HandleMoveNoLeftButtonEvent(Buttons, Position);
   }

}



template<>
void StateClass<State::ScribblingState::Idle>::HandleReleaseEventSM(Qt::MouseButton Button, QPointF Position, bool Erasing, double Pressure)
{
   if (Button == Qt::LeftButton) {
      // Do Nothing
   }
}

template<>
void StateClass<State::ScribblingState::Idle>::HandleOverviewEventSM(bool Enabled)
{
   if (Enabled) {
      StateMachine.Context.MyDatas.CompleteImage();
      StateMachine.SetNewState(&StateMachine.WaitingToSelectRegionFromOverview);
      StateMachine.Context.MyDatas.ToggleShowOverview(true);
   }
}

template<>
void StateClass<State::ScribblingState::Idle>::HandlePasteEventSM(QImage Image)
{
   StateMachine.Context.MyDatas.SetImageToPaste(Image);
   StateMachine.SetNewState(&StateMachine.WaitingToPasteClippboardImage);
}




/***************  WaitingToLeaveJitterProtectionForDrawing  *****************/
template <>
void StateClass<State::ScribblingState::Drawing>::HandleMoveEventSM(Qt::MouseButtons Buttons, QPointF Position, ulong Timestamp, bool Erasing, double Pressure);
template<>
void StateClass<State::ScribblingState::Drawing>::HandleReleaseEventSM(Qt::MouseButton Button, QPointF Position, bool Erasing, double Pressure);

template<>
void StateClass<State::ScribblingState::WaitingToLeaveJitterProtectionForDrawing>
::HandleMoveEventSM(Qt::MouseButtons Buttons, QPointF Position, ulong Timestamp, bool Erasing, double Pressure)
{
   StateBaseClass::HandleMoveEventSM(Buttons, Position, Timestamp, Erasing, Pressure);
   StateMachine.ShowBigPointer();

   if ((Buttons & Qt::LeftButton)) {

       if (StateMachine.Context.MyDatas.IsJitter(Position, StateMachine.Context.MyDatas.getButtonDownPosition(), Pressure)) {
           return; // ignore small movements (probably use penwidth*2)
       }
       StateMachine.SetNewState(&StateMachine.Drawing);

       StateMachine.MyTimer.stop();
       StateMachine.MyTimer.start(StateMachine.Settings.GestureTimeout);
       StateMachine.Context.MyDatas.FlushLastDrawnPicture();
      //LastDrawnObject.fill(BackgroundColor);
      if (Erasing) {
         StateMachine.Context.MyDatas.EraseLineTo(Position, Pressure);
      } else {
         StateMachine.Context.MyDatas.drawLineTo(Position, Pressure);
      }
      StateMachine.Context.MyDatas.ExtendBoundingboxAndShape(Position);

       //StateMachine.Drawing.HandleMoveEventSM(Buttons, Position, Timestamp, Erasing, Pressure);

   }  else {
      HandleMoveNoLeftButtonEvent(Buttons, Position);
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
void StateClass<State::ScribblingState::WaitingToLeaveJitterProtectionForDrawing>
::timeoutSM()
{
   if (StateMachine.Context.DownInsideObject) {

      StateMachine.Context.MyDatas.MakeSelectionFromLastDrawnObject();

      StateMachine.Context.MyDatas.ClearLastDrawnObjectPoints();
      StateMachine.Context.MyDatas.SetModified();

      //MoveSelected = true;
      //NewDrawingStarted = false;
     // StateMachine.Context.MyDatas.SelectedPoint = StateMachine.Context.MyDatas.getLastPoint();
      StateMachine.Context.MyDatas.SetSelectedOffset();
      //scribbling = false;
      StateMachine.Interface.UpdateRequest();
      //WaitForPostIt = true;
      StateMachine.Interface.setCursor(Qt::DragMoveCursor);

      StateMachine.MyTimer.start(StateMachine.Settings.PostItTimeout);
      StateMachine.SetNewState(&StateMachine.WaitingToLeaveJitterProtectionWithSelectedAreaForMoving);

   } else {

      if (StateMachine.Context.MyDatas.PostItSelected(StateMachine.Context.MyDatas.getSelectedCurrentPosition())) {
         //scribbling = false;
         //NewDrawingStarted = false;
         StateMachine.Interface.setCursor(Qt::ClosedHandCursor);
         DEBUG_LOG << "Selected postit " << std::endl;
         StateMachine.SetNewState(&StateMachine.WaitingToLeaveJitterProtectionWithSelectedPostitForMoving);


      } else {
         StateMachine.Context.MyDatas.MakeSreenMoveHint();
         //Scrolling = true;
         StateMachine.Context.ScrollingLastPosition = StateMachine.Context.MyDatas.getSelectedCurrentPosition();
         StateMachine.Context.ScrollingOldOrigin = StateMachine.Context.MyDatas.GetOrigin();
         StateMachine.Interface.setCursor(Qt::SizeAllCursor);
         //scribbling = false;
         StateMachine.Interface.UpdateRequest();
         StateMachine.SetNewState(&StateMachine.WaitingToLeaveJitterProtectionForScrolling);
      }
   }
}


/***************  Drawing  *****************/

template<>
void StateClass<State::ScribblingState::Drawing>::HandleMoveEventSM(Qt::MouseButtons Buttons, QPointF Position, ulong Timestamp, bool Erasing, double Pressure)
{
   StateBaseClass::HandleMoveEventSM(Buttons, Position, Timestamp, Erasing, Pressure);
   StateMachine.ShowBigPointer();

   if ((Buttons & Qt::LeftButton)) {
       if ((StateMachine.Context.MyDatas.IsSelectionJitter(Position, StateMachine.Context.MyDatas.getLastPoint(), Pressure))) {
          // return; // ignore small movements (probably use penwidth*2)
          // Dont restart timer on small movements
       } else {
          StateMachine.MyTimer.stop();
          StateMachine.MyTimer.start(StateMachine.Settings.GestureTimeout);
       }
       StateMachine.Context.MyDatas.FlushLastDrawnPicture();
      //LastDrawnObject.fill(BackgroundColor);
      if (Erasing) {
         StateMachine.Context.MyDatas.EraseLineTo(Position, Pressure);
      } else {
         StateMachine.Context.MyDatas.drawLineTo(Position, Pressure);
      }
      StateMachine.Context.MyDatas.ExtendBoundingboxAndShape(Position);

   }  else {
      HandleMoveNoLeftButtonEvent(Buttons, Position);
   }

}

template<>
void StateClass<State::ScribblingState::Drawing>::HandleReleaseEventSM(Qt::MouseButton Button, QPointF Position, bool Erasing, double Pressure)
{
   if (Button == Qt::LeftButton) {
      StateMachine.MyTimer.stop();
      if (Erasing) {
         StateMachine.Context.MyDatas.EraseLineTo(Position, Pressure);
      } else {
         StateMachine.Context.MyDatas.drawLineTo(Position, Pressure);
      }
      StateMachine.Context.MyDatas.UpdateBoundingboxesForFinishedShape(Position);

      StateMachine.SetNewState(&StateMachine.Idle);
   }
}


template<>
void StateClass<State::ScribblingState::Drawing>::timeoutSM()
{
   //   QTextStream(stdout) << "<" << GestureTrackerAccumulatedSpeed.x() << "; " << GestureTrackerAccumulatedSpeed.y() << "> <"  << GestureTrackerAccumulatedSquaredSpeed.x() << ";" << GestureTrackerAccumulatedSquaredSpeed.y() << endl;

   //FillPolygon = true;
   StateMachine.Context.FillPolygonStartPosition = StateMachine.Context.MyDatas.getLastPoint();
   StateMachine.Interface.setCursor(Qt::BusyCursor);
   StateMachine.Interface.UpdateRequest();
   StateMachine.SetNewState(&StateMachine.DrawingPaused);

}

/***************  DrawingPaused  *****************/


template<>
void StateClass<State::ScribblingState::DrawingPaused>::HandleMoveEventSM(Qt::MouseButtons Buttons, QPointF Position, ulong Timestamp, bool Erasing, double Pressure)
{
   StateBaseClass::HandleMoveEventSM(Buttons, Position, Timestamp, Erasing, Pressure);
   StateMachine.ShowBigPointer();

   if ((Buttons & Qt::LeftButton)) {
       if (StateMachine.Context.MyDatas.IsSelectionJitter(Position, StateMachine.Context.MyDatas.getLastPoint(), Pressure)) {
           return; // ignore small movements (probably use penwidth*2)
       }

       StateMachine.SetNewState(&StateMachine.DrawingFillRequested);
       StateMachine.Drawing.HandleMoveEventSM(Buttons, Position, Timestamp, Erasing, Pressure);

   }  else {
      HandleMoveNoLeftButtonEvent(Buttons, Position);
   }

}

template<>
void StateClass<State::ScribblingState::DrawingFillRequested>::HandleReleaseEventSM(Qt::MouseButton Button, QPointF Position, bool Erasing, double Pressure);

template<>
void StateClass<State::ScribblingState::DrawingPaused>::HandleReleaseEventSM(Qt::MouseButton Button, QPointF Position, bool Erasing, double Pressure)
{
   if (Button == Qt::LeftButton) {
      StateMachine.DrawingFillRequested.HandleReleaseEventSM(Button, Position, Erasing, Pressure);

   }
}

/***************  DrawingFillRequested  *****************/

template<>
void StateClass<State::ScribblingState::DrawingFillRequested>::HandleMoveEventSM(Qt::MouseButtons Buttons, QPointF Position, ulong Timestamp, bool Erasing, double Pressure)
{
   StateBaseClass::HandleMoveEventSM(Buttons, Position, Timestamp, Erasing, Pressure);
   StateMachine.ShowBigPointer();

   if ((Buttons & Qt::LeftButton)) {
       if (StateMachine.Context.MyDatas.IsSelectionJitter(Position, StateMachine.Context.MyDatas.getLastPoint(), Pressure)) {
           return; // ignore small movements (probably use penwidth*2)
       }

       if (StateMachine.Context.MyDatas.IsJitter(Position, StateMachine.Context.FillPolygonStartPosition, Pressure)) {
          StateMachine.SetNewState(&StateMachine.Drawing);
          StateMachine.Interface.setCursor(Qt::ArrowCursor);

       }
       StateMachine.Drawing.HandleMoveEventSM(Buttons, Position, Timestamp, Erasing, Pressure);

   }  else {
      HandleMoveNoLeftButtonEvent(Buttons, Position);
   }

}

template<>
void StateClass<State::ScribblingState::DrawingFillRequested>::HandleReleaseEventSM(Qt::MouseButton Button, QPointF Position, bool Erasing, double Pressure)
{
   if (Button == Qt::LeftButton) {
      StateMachine.Drawing.HandleReleaseEventSM(Button, Position, Erasing, Pressure);
      StateMachine.Context.MyDatas.FilllastDrawnShape();
      StateMachine.Interface.setCursor(Qt::ArrowCursor);
      //FillPolygon = false;

      StateMachine.Interface.UpdateRequest();
   }
}


/***************  WaitingToLeaveJitterProtectionWithSelectedAreaForMoving  *****************/

template<>
void StateClass<State::ScribblingState::MovingSelection>::HandleMoveEventSM(Qt::MouseButtons Buttons, QPointF Position, ulong Timestamp, bool Erasing, double Pressure);

template<>
void StateClass<State::ScribblingState::WaitingToLeaveJitterProtectionWithSelectedAreaForMoving>
::HandleMoveEventSM(Qt::MouseButtons Buttons, QPointF Position, ulong Timestamp, bool Erasing, double Pressure)
{
   StateBaseClass::HandleMoveEventSM(Buttons, Position, Timestamp, Erasing, Pressure);

   if ((Buttons & Qt::LeftButton)) {
       if (StateMachine.Context.MyDatas.IsSelectionJitter(Position, StateMachine.Context.MyDatas.getLastPoint(), Pressure)) {
           return; // ignore small movements (probably use penwidth*2)
       }
       StateMachine.Interface.setCursor(Qt::DragCopyCursor);

       StateMachine.SetNewState(&StateMachine.MovingSelection);
       StateMachine.MovingSelection.HandleMoveEventSM(Buttons, Position, Timestamp, Erasing, Pressure);


   }  else {
      HandleMoveNoLeftButtonEvent(Buttons, Position);
   }

}


template<>
void StateClass<State::ScribblingState::WaitingToLeaveJitterProtectionWithSelectedAreaForMoving>
::HandleReleaseEventSM(Qt::MouseButton Button, QPointF Position, bool Erasing, double Pressure)
{
   if (Button == Qt::LeftButton) {
      StateMachine.SetNewState(&StateMachine.Idle);
   }
}

template<>
void StateClass<State::ScribblingState::WaitingToLeaveJitterProtectionWithSelectedAreaForMoving>
::timeoutSM()
{
   {
      DEBUG_LOG << "Creating postit " << std::endl;

      //WaitForPostIt = false;
      StateMachine.Context.MyDatas.CreeatePostitFromSelection();
      StateMachine.SetNewState(&StateMachine.WaitingToLeaveJitterProtectionWithCreatedPostitForMoving);
      StateMachine.Interface.UpdateRequest();
   }

}

/***************  MovingSelection  *****************/

template<>
void StateClass<State::ScribblingState::MovingSelection>::HandleMoveEventSM(Qt::MouseButtons Buttons, QPointF Position, ulong Timestamp, bool Erasing, double Pressure)
{
   StateBaseClass::HandleMoveEventSM(Buttons, Position, Timestamp, Erasing, Pressure);

   if ((Buttons & Qt::LeftButton)) {
       if (StateMachine.Context.MyDatas.IsSelectionJitter(Position, StateMachine.Context.MyDatas.getLastPoint(), Pressure)) {
           return; // ignore small movements (probably use penwidth*2)
       }
       // QPoint Offset = event->pos() - SelectedPoint;
         StateMachine.MyTimer.stop();
         StateMachine.MyTimer.start(StateMachine.Settings.CopyTimeout);

         StateMachine.Context.MyDatas.setSelectedCurrentPosition(Position);
         StateMachine.Interface.UpdateRequest();
       // DrawMovedSelection(Offset);
       // BoundingBoxClass MovedRectangle(LastPaintedObjectBoundingBox);
       // MovedRectangle.Move(PositionClass(Offset.x(), Offset.y()));
       // drawrectangle(MovedRectangle);
        //        drawrectangle(BoundingBoxClass(LastPaintedObjectBoundingBox).Move(PositionClass(Offset.x(), Offset.y())));


   }  else {
      HandleMoveNoLeftButtonEvent(Buttons, Position);
   }

}

template<>
void StateClass<State::ScribblingState::MovingSelection>::HandleReleaseEventSM(Qt::MouseButton Button, QPointF Position, bool Erasing, double Pressure)
{
   if (Button == Qt::LeftButton) {
      //WaitForPostIt = false;
       if (StateMachine.Tracker.GetCurrentSpeed() > 0.25f) {
           DEBUG_LOG << "LeavingSpeed " << StateMachine.Tracker.GetCurrentSpeed() << std::endl;
           StateMachine.Context.MyDatas.setDiscardSelection(true);
           StateMachine.Interface.UpdateRequest();
       }
       //DEBUG_LOG << "LeavingSpeed " << (LastDistance + CurrentDistance) << " / " << (DeltaTLastDistance + DeltaTCurrentDistance) << " = " << ((LastDistance + CurrentDistance) / (float)(DeltaTLastDistance + DeltaTCurrentDistance)) << std::endl;
           ;
     // QPoint Offset =  - SelectedPoint;
       if (StateMachine.Context.MyDatas.getDiscardSelection() == false) {
          StateMachine.Context.MyDatas.DrawMovedSelection(Position);
       }
     // BoundingBoxClass MovedRectangle(LastPaintedObjectBoundingBox);
    //  MovedRectangle.Move(PositionClass(Offset.x(), Offset.y()));
    //  drawrectangle(MovedRectangle);
     // MoveSelected = false;
      StateMachine.Context.MyDatas.setLastDrawingValid(false);
      //        drawrectangle(BoundingBoxClass(LastPaintedObjectBoundingBox).Move(PositionClass(Offset.x(), Offset.y())));
      StateMachine.SetNewState(&StateMachine.Idle);
   }
}

template<>
void StateClass<State::ScribblingState::MovingSelection>::timeoutSM()
{
   if (StateMachine.Context.MyDatas.getDiscardSelection() == false) {

      // Fast shaking followed by a pause means throw away selection
      if (StateMachine.Tracker.IsFastShaking()) {
         StateMachine.Context.MyDatas.setDiscardSelection(true);
         StateMachine.Interface.UpdateRequest();
      } else {
         QPointF CopyPos(StateMachine.Context.MyDatas.getSelectedCurrentPosition());
         StateMachine.Context.MyDatas.MoveSelectedCurrentPosition ( QPointF(3,3));
         StateMachine.Context.MyDatas.DrawMovedSelection(CopyPos);

      }
      StateMachine.SetNewState(&StateMachine.MovingSelectionPaused);
   }

}


/***************  MovingSelectionPaused  *****************/

template<>
void StateClass<State::ScribblingState::MovingSelectionPaused>::HandleMoveEventSM(Qt::MouseButtons Buttons, QPointF Position, ulong Timestamp, bool Erasing, double Pressure)
{
   StateBaseClass::HandleMoveEventSM(Buttons, Position, Timestamp, Erasing, Pressure);

   if ((Buttons & Qt::LeftButton)) {
       if (StateMachine.Context.MyDatas.IsSelectionJitter(Position, StateMachine.Context.MyDatas.getLastPoint(), Pressure)) {
           return; // ignore small movements (probably use penwidth*2)
       }

       StateMachine.SetNewState(&StateMachine.MovingSelection);
       StateMachine.MovingSelection.HandleMoveEventSM(Buttons, Position, Timestamp, Erasing, Pressure);


   }  else {
      HandleMoveNoLeftButtonEvent(Buttons, Position);
   }

}

template<>
void StateClass<State::ScribblingState::MovingSelectionPaused>::HandleReleaseEventSM(Qt::MouseButton Button, QPointF Position, bool Erasing, double Pressure)
{
   if (Button == Qt::LeftButton) {
      StateMachine.MovingSelection.HandleReleaseEventSM(Button, Position, Erasing, Pressure);

   }
}

/***************  WaitingToLeaveJitterProtectionWithCreatedPostitForMoving  *****************/
template<>
void StateClass<State::ScribblingState::MovingPostit>::HandleMoveEventSM(Qt::MouseButtons Buttons, QPointF Position, ulong Timestamp, bool Erasing, double Pressure);

template<>
void StateClass<State::ScribblingState::WaitingToLeaveJitterProtectionWithCreatedPostitForMoving>::HandleMoveEventSM(Qt::MouseButtons Buttons, QPointF Position, ulong Timestamp, bool Erasing, double Pressure)
{
   StateBaseClass::HandleMoveEventSM(Buttons, Position, Timestamp, Erasing, Pressure);

   if ((Buttons & Qt::LeftButton)) {
       if (StateMachine.Context.MyDatas.IsSelectionJitter(Position, StateMachine.Context.MyDatas.getLastPoint(), Pressure)) {
           return; // ignore small movements (probably use penwidth*2)
       }
       StateMachine.SetNewState(&StateMachine.MovingPostit);
       StateMachine.MovingPostit.HandleMoveEventSM(Buttons, Position, Timestamp, Erasing, Pressure);


   }  else {
      HandleMoveNoLeftButtonEvent(Buttons, Position);
   }

}

template<>
void StateClass<State::ScribblingState::WaitingToLeaveJitterProtectionWithCreatedPostitForMoving>::HandleReleaseEventSM(Qt::MouseButton Button, QPointF Position, bool Erasing, double Pressure)
{
   if (Button == Qt::LeftButton) {
      StateMachine.SetNewState(&StateMachine.Idle);
   }
}


/***************  WaitingToLeaveJitterProtectionWithSelectedPostitForMoving  *****************/
template<>
void StateClass<State::ScribblingState::WaitingToLeaveJitterProtectionWithSelectedPostitForMoving>::HandleMoveEventSM(Qt::MouseButtons Buttons, QPointF Position, ulong Timestamp, bool Erasing, double Pressure)
{
   StateBaseClass::HandleMoveEventSM(Buttons, Position, Timestamp, Erasing, Pressure);

   if ((Buttons & Qt::LeftButton)) {
       if (StateMachine.Context.MyDatas.IsSelectionJitter(Position, StateMachine.Context.MyDatas.getLastPoint(), Pressure)) {
           return; // ignore small movements (probably use penwidth*2)
       }

       StateMachine.SetNewState(&StateMachine.MovingPostit);
       StateMachine.MovingPostit.HandleMoveEventSM(Buttons, Position, Timestamp, Erasing, Pressure);


   }  else {
      HandleMoveNoLeftButtonEvent(Buttons, Position);
   }

}

template<>
void StateClass<State::ScribblingState::WaitingToLeaveJitterProtectionWithSelectedPostitForMoving>::HandleReleaseEventSM(Qt::MouseButton Button, QPointF Position, bool Erasing, double Pressure)
{
   if (Button == Qt::LeftButton) {
      StateMachine.Context.MyDatas.ClearSelectedPostit();
      StateMachine.Interface.setCursor(Qt::ArrowCursor);

      StateMachine.SetNewState(&StateMachine.Idle);

   }
}


/***************  MovingPostit  *****************/

template<>
void StateClass<State::ScribblingState::MovingPostit>::HandleMoveEventSM(Qt::MouseButtons Buttons, QPointF Position, ulong Timestamp, bool Erasing, double Pressure)
{
   StateBaseClass::HandleMoveEventSM(Buttons, Position, Timestamp, Erasing, Pressure);

   if ((Buttons & Qt::LeftButton)) {
       if (StateMachine.Context.MyDatas.IsSelectionJitter(Position, StateMachine.Context.MyDatas.getLastPoint(), Pressure)) {
           return; // ignore small movements (probably use penwidth*2)
       }

       if (StateMachine.Context.MyDatas.IsAnySelectedPostit()) {
          DEBUG_LOG << "Moving postit " << std::endl;
          StateMachine.Context.MyDatas.MoveSelectedPostits(Position);
          StateMachine.Interface.UpdateRequest();
       }

   }  else {
      HandleMoveNoLeftButtonEvent(Buttons, Position);
   }

}

template<>
void StateClass<State::ScribblingState::MovingPostit>::HandleReleaseEventSM(Qt::MouseButton Button, QPointF Position, bool Erasing, double Pressure)
{
   if (Button == Qt::LeftButton) {
      if ((StateMachine.Context.MyDatas.IsAnySelectedPostit())) {
         DEBUG_LOG << "Fixing postit " << std::endl;
         StateMachine.Context.MyDatas.FinishMovingSelectedPostits(Position);

         //  MoveSelected = false;
         StateMachine.Context.MyDatas.setLastDrawingValid(false);
         StateMachine.Context.MyDatas.ClearSelectedPostit();
         StateMachine.Interface.setCursor(Qt::ArrowCursor);
         StateMachine.Interface.UpdateRequest();
      }
      StateMachine.SetNewState(&StateMachine.Idle);
   }
}

/***************  WaitingToLeaveJitterProtectionForScrolling  *****************/
template<>
void StateClass<State::ScribblingState::ScrollingDrawingArea>::HandleMoveEventSM(Qt::MouseButtons Buttons, QPointF Position, ulong Timestamp, bool Erasing, double Pressure);

template<>
void StateClass<State::ScribblingState::WaitingToLeaveJitterProtectionForScrolling>::HandleMoveEventSM(Qt::MouseButtons Buttons, QPointF Position, ulong Timestamp, bool Erasing, double Pressure)
{
   StateBaseClass::HandleMoveEventSM(Buttons, Position, Timestamp, Erasing, Pressure);

   if ((Buttons & Qt::LeftButton)) {
       if (StateMachine.Context.MyDatas.IsSelectionJitter(Position, StateMachine.Context.MyDatas.getLastPoint(), Pressure)) {
           return; // ignore small movements (probably use penwidth*2)
       }
       StateMachine.SetNewState(&StateMachine.ScrollingDrawingArea);
       StateMachine.ScrollingDrawingArea.HandleMoveEventSM(Buttons, Position, Timestamp, Erasing, Pressure);


   }  else {
      HandleMoveNoLeftButtonEvent(Buttons, Position);
   }

}

template<>
void StateClass<State::ScribblingState::ScrollingDrawingArea>::HandleReleaseEventSM(Qt::MouseButton Button, QPointF Position, bool Erasing, double Pressure);

template<>
void StateClass<State::ScribblingState::WaitingToLeaveJitterProtectionForScrolling>::HandleReleaseEventSM(Qt::MouseButton Button, QPointF Position, bool Erasing, double Pressure)
{
   if (Button == Qt::LeftButton) {
      StateMachine.ScrollingDrawingArea.HandleReleaseEventSM(Button, Position, Erasing, Pressure);

   }
}


/***************  ScrollingDrawingArea  *****************/

template<>
void StateClass<State::ScribblingState::ScrollingDrawingArea>::HandleMoveEventSM(Qt::MouseButtons Buttons, QPointF Position, ulong Timestamp, bool Erasing, double Pressure)
{
   StateBaseClass::HandleMoveEventSM(Buttons, Position, Timestamp, Erasing, Pressure);

   if ((Buttons & Qt::LeftButton)) {
       if (StateMachine.Context.MyDatas.IsSelectionJitter(Position, StateMachine.Context.MyDatas.getLastPoint(), Pressure)) {
           return; // ignore small movements (probably use penwidth*2)
       }
       StateMachine.Context.MyDatas.CompleteImage();
      StateMachine.Context.MyDatas.MoveOrigin(Position- StateMachine.Context.ScrollingLastPosition);


      StateMachine.Context.ScrollingLastPosition = Position;
      StateMachine.Interface.UpdateRequest();

   }  else {
      HandleMoveNoLeftButtonEvent(Buttons, Position);
   }

}

template<>
void StateClass<State::ScribblingState::ScrollingDrawingArea>::HandleReleaseEventSM(Qt::MouseButton Button, QPointF Position, bool Erasing, double Pressure)
{
   if (Button == Qt::LeftButton) {
      StateMachine.Context.MyDatas.MoveOrigin(Position - StateMachine.Context.ScrollingLastPosition);

      StateMachine.Context.MyDatas.resizeScrolledImage();
      StateMachine.Interface.UpdateRequest();

      StateMachine.SetNewState(&StateMachine.Idle);

   }
}






template<>
void StateClass<State::ScribblingState::TouchScrollingDrawingArea>::HandleTouchMoveEventSM(int NumberOfTouchpoints, QPointF MeanPosition)
{
   if(NumberOfTouchpoints == 2) {
   DEBUG_LOG << "TM(" << MeanPosition.x() <<":" << MeanPosition.y() << ")";

         StateMachine.Context.MyDatas.CompleteImage();
         StateMachine.Context.MyDatas.MoveOrigin(MeanPosition- StateMachine.Context.ScrollingLastPosition);

         StateMachine.Context.ScrollingLastPosition = MeanPosition;
         StateMachine.Interface.UpdateRequest();
   }

}
template<>
void StateClass<State::ScribblingState::WaitingForTouchScrolling>::HandleTouchMoveEventSM(int NumberOfTouchpoints, QPointF MeanPosition)
{
   if(NumberOfTouchpoints == 2) {
   DEBUG_LOG << "TM(" << MeanPosition.x() <<":" << MeanPosition.y() << ")";

   StateMachine.SetNewState(&StateMachine.TouchScrollingDrawingArea);
   StateMachine.TouchScrollingDrawingArea.HandleTouchMoveEventSM(NumberOfTouchpoints, MeanPosition);
   }
}


template<>
void StateClass<State::ScribblingState::TouchScrollingDrawingArea>::HandleTouchReleaseEventSM(int NumberOfTouchpoints, QPointF MeanPosition)
{
   if(NumberOfTouchpoints >= 2) {
      StateMachine.Context.MyDatas.MoveOrigin(MeanPosition - StateMachine.Context.ScrollingLastPosition);

      StateMachine.Context.MyDatas.resizeScrolledImage();
      StateMachine.Interface.UpdateRequest();

      StateMachine.SetNewState(&StateMachine.Idle);
   }
}

template<>
void StateClass<State::ScribblingState::TouchScrollingDrawingArea>::HandleTouchPressEventSM(int NumberOfTouchpoints, QPointF MeanPosition)
{
   if(NumberOfTouchpoints > 2) {

      HandleTouchReleaseEventSM(NumberOfTouchpoints, MeanPosition);
   }
}

template<>
void StateClass<State::ScribblingState::WaitingForTouchScrolling>::HandleTouchReleaseEventSM(int NumberOfTouchpoints, QPointF MeanPosition)
{
   if(NumberOfTouchpoints >= 2) {

      StateMachine.TouchScrollingDrawingArea.HandleTouchReleaseEventSM(NumberOfTouchpoints, MeanPosition);
   }
}


template<>
void StateClass<State::ScribblingState::WaitingForTouchScrolling>::HandleTouchPressEventSM(int NumberOfTouchpoints, QPointF MeanPosition)
{
   if(NumberOfTouchpoints >= 2) {

      StateMachine.TouchScrollingDrawingArea.HandleTouchReleaseEventSM(NumberOfTouchpoints, MeanPosition);
   }
}
/*********** Overviev *******************************/

template<>
void StateClass<State::ScribblingState::WaitingToSelectRegionFromOverview>::HandlePressEventSM(Qt::MouseButton Button, QPointF Position, ulong Timestamp)
{
   if (Button == Qt::LeftButton) {
      StateMachine.Context.MyDatas.MoveOrigin(-(StateMachine.Context.MyDatas.TranslateCoordinateOffsetFromOverview(Position)-StateMachine.Context.MyDatas.GetOrigin()));
      StateMachine.SetNewState(&StateMachine.Idle);
      StateMachine.Context.MyDatas.ToggleShowOverview(false);
   }
}

template<>
void StateClass<State::ScribblingState::WaitingToSelectRegionFromOverview>::HandleOverviewEventSM(bool Enabled)
{
   if (!Enabled) {
      StateMachine.SetNewState(&StateMachine.Idle);
      StateMachine.Context.MyDatas.ToggleShowOverview(false);
   }
}

template<>
void StateClass<State::ScribblingState::WaitingToSelectRegionFromOverview>::HandleTouchMoveEventSM(int NumberOfTouchpoints, QPointF MeanPosition)
{
   // Ignore in overview
}

template<>
void StateClass<State::ScribblingState::WaitingToSelectRegionFromOverview>::HandleTouchPressEventSM(int NumberOfTouchpoints, QPointF MeanPosition)
{
   // Ignore in overview
}

/*********** Paste Clipboard *******************************/

template<>
void StateClass<State::ScribblingState::WaitingToPasteClippboardImage>::HandleKeyEventSM(PasteEvent Event)
{
   switch(Event) {
      case DatabaseClass::PasteTopLayer:
      case DatabaseClass::PasteBottomLayer:
      case DatabaseClass::PasteDrawing:
         StateMachine.Context.MyDatas.DoPasteImage(Event);
         StateMachine.SetNewState(&StateMachine.Idle);
         break;
      case DatabaseClass::CancelPasting:
         StateMachine.Context.MyDatas.CancelPasteImage();
         StateMachine.SetNewState(&StateMachine.Idle);
         break;
      case DatabaseClass::MakeBigger:
         StateMachine.Context.MyDatas.ScaleImageToPaste(1.1);
         break;
      case DatabaseClass::MakeSmaller:
         StateMachine.Context.MyDatas.ScaleImageToPaste(0.9);
         break;

      case DatabaseClass::MakeOriginalSize:
         StateMachine.Context.MyDatas.ScaleImageToPaste(-1.0);
         break;
   }
}



template<>
void StateClass<State::ScribblingState::WaitingToPasteClippboardImage>::HandlePressEventSM(Qt::MouseButton Button, QPointF Position, ulong Timestamp)
{
   if (Button == Qt::LeftButton) {
      StateMachine.Context.MyDatas.DoPasteImage(DatabaseClass::PasteDrawing);
      StateMachine.SetNewState(&StateMachine.Idle);
   }
}


template<>
void StateClass<State::ScribblingState::WaitingToPasteClippboardImage>::HandleTouchMoveEventSM(int NumberOfTouchpoints, QPointF MeanPosition)
{
   // Ignore in overview
}

template<>
void StateClass<State::ScribblingState::WaitingToPasteClippboardImage>::HandleTouchPressEventSM(int NumberOfTouchpoints, QPointF MeanPosition)
{
   // Ignore in overview
}

/*********** End of Statemachine *******************************/


ControllingStateMachine::PointerType ControllingStateMachine::PointerTypeToShow()
{
   if (Context.ShowPointer) {
      if (Context.Showeraser) {
         if (Context.SpongeAsEraser) {
            return WIPER;
         } else {
            return ERASER;
         }
      } else {
         return DRAWER;
      }
   }
   return NONE;
}
void ControllingStateMachine::ShowBigPointer()
{
   if (Settings.PointerHoldon >= 0) {
   Context.ShowPointer = true;
   PointerTimer.start(Settings.PointerHoldon);
   Interface.UpdateRequest();
   }
}

void ControllingStateMachine::PointerTimeout()
{
   Context.ShowPointer = false;
   Interface.UpdateRequest();
}

void ControllingStateMachine::SetNewState(StateBaseClass *NewState)
{
   CurrentState = NewState;
}

void ControllingStateMachine::HandlePressEventSM(Qt::MouseButton Button, QPointF Position, ulong Timestamp)
{
    CurrentState->HandlePressEventSM(Button, Position, Timestamp);
}

void ControllingStateMachine::HandleMoveEventSM(Qt::MouseButtons Buttons, QPointF Position, ulong Timestamp, bool Erasing, double Pressure)
{
    CurrentState->HandleMoveEventSM(Buttons, Position, Timestamp, Erasing, Pressure);
}

void ControllingStateMachine::HandleReleaseEventSM(Qt::MouseButton Button, QPointF Position, bool Erasing, double Pressure)
{
   DEBUG_LOG << "Button Up: " << Button  << std::endl;
   CurrentState->HandleReleaseEventSM(Button, Position, Erasing, Pressure);
}

void ControllingStateMachine::HandleTouchPressEventSM(int NumberOfTouchpoints, QPointF MeanPosition)
{
   CurrentState->HandleTouchPressEventSM(NumberOfTouchpoints, MeanPosition);
}

void ControllingStateMachine::HandleTouchMoveEventSM(int NumberOfTouchpoints, QPointF MeanPosition)
{
   CurrentState->HandleTouchMoveEventSM(NumberOfTouchpoints, MeanPosition);
}

void ControllingStateMachine::HandleTouchReleaseEventSM(int NumberOfTouchpoints, QPointF MeanPosition)
{
   CurrentState->HandleTouchReleaseEventSM(NumberOfTouchpoints, MeanPosition);
}

void ControllingStateMachine::HandleOverviewEventSM(bool Enabled)
{
   CurrentState->HandleOverviewEventSM(Enabled);
}

void ControllingStateMachine::HandlePasteEventSM(QImage ImageToPaste)
{
   CurrentState->HandlePasteEventSM(ImageToPaste);
}

void ControllingStateMachine::HandleKeyEventSM(DatabaseClass::PasteEvent Event)
{
   CurrentState->HandleKeyEventSM(Event);
}

void ControllingStateMachine::Timeout()
{
   CurrentState->timeoutSM();

}

#define InitStateObject(s) s(*this, ##s)


ControllingStateMachine::ControllingStateMachine(DatabaseClass &Database, GuiInterface &NewInterface)
   :   Context(Database),
       Interface(NewInterface),
       Idle(*this),
       WaitingToLeaveJitterProtectionForDrawing(*this),
       WaitingToLeaveJitterProtectionWithSelectedAreaForMoving(*this),
       WaitingToLeaveJitterProtectionForScrolling(*this),
       WaitingToLeaveJitterProtectionWithCreatedPostitForMoving(*this),
       WaitingToLeaveJitterProtectionWithSelectedPostitForMoving(*this),
       Drawing(*this),
       DrawingPaused(*this),
       DrawingFillRequested(*this),
       MovingSelection(*this),
       MovingSelectionPaused(*this),
       MovingPostit(*this),
       ScrollingDrawingArea(*this),
       WaitingForTouchScrolling(*this),
       TouchScrollingDrawingArea(*this),
       WaitingToSelectRegionFromOverview(*this),
       WaitingToPasteClippboardImage(*this)
{
    CurrentState = &Idle;
    Context.ShowPointer = false;
    Context.SpongeAsEraser = false;

    //Context.ShowOverview = false;

  //  Context.LastDrawingValid = false;
    //Context.EraseLastDrawnObject = false;
 //   Context.Frozen = false;
 //   Context.myPenWidth = 2;
    //Context.SelectedPenWidth = StateMachine.Context.myPenWidth;

    QObject::connect(&MyTimer, &QTimer::timeout, this, &ControllingStateMachine::Timeout);
    MyTimer.setSingleShot(true);

    QObject::connect(&PointerTimer, &QTimer::timeout, this, &ControllingStateMachine::PointerTimeout);
    PointerTimer.setSingleShot(true);

    //Context.SelectedPostit.clear();
    Context.SelectPostitsDirectly = false;
    //Context.ShowPostitsFrame = false;



    //Context.RecentlyPastedObjectValid = false;
    //Context.MarkerActive = false;
   // QImage RecentlyMovedObject;
   // BoundingBoxClass RecentlyMovedObjectBoundingBox;


}

void ControllingStateMachine::timeoutSM()
{
   CurrentState->timeoutSM();
}

//! [0]
//! [0]

//! [1]

//! [2]

//! [3]


#endif

