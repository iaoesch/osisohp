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
bool ScribbleArea::ImportImage(const QString &fileName)
//! [1] //! [2]
{
    QImage loadedImage;
    if (!loadedImage.load(fileName))
        return false;

    QSize newSize = loadedImage.size().expandedTo(size());
    resizeImage(&loadedImage, newSize);
    image = loadedImage;
    modified = false;
    Origin = {0,0};
    BackgroundImagesOrigin = {0,0};
    modified = false;
    LastDrawingValid = false;
    EraseLastDrawnObject = false;
    BackgroundImages.clear();
    update();
    return true;
}
//! [2]

//! [3]
bool ScribbleArea::ExportImage(const QString &fileName, const char *fileFormat)
//! [3] //! [4]
{
   QImage ImageToSave(image.size(), QImage::Format_ARGB32);
   QPainter painter(&ImageToSave);
   PaintVisibleDrawing(painter, image.rect(), {0,0}, BackgroundImagesOrigin-Origin);

    if (ImageToSave.save(fileName, fileFormat)) {
        return true;
    } else {
        return false;
    }
}

bool ScribbleArea::SaveImage(const QString &fileName)
//! [3] //! [4]
{
   // todo: read and save colors pen and background and backgroundimages

   QFile file(fileName);
   file.open(QIODevice::WriteOnly);
   QDataStream out(&file);

   // Write a header with a "magic number" and a version
   out << (quint32)0x139A1A7F;
   out << (qint32)100;

   out.setVersion(QDataStream::Qt_5_0);
   CompleteImage();

   // Write the data
   out << image;
   out << Origin;
   out << BackgroundImagesOrigin;
   // Now save all postits
   out << (qint32)(PostIts.size());
   for (auto &&Picture: PostIts) {
      out << Picture.Position;
      out << Picture.Image;
      out << Picture.Box;
   }
   out << (qint32)(BackgroundImages.size());
   for (auto &Picture: BackgroundImages) {
      out << Picture.IsVisible();
      out << *Picture;
   }
   return true;


}
bool ScribbleArea::LoadImage(const QString &fileName)
//! [1] //! [2]
{
#if 1
   // todo: read and save colors pen and background and backgroundimages
   //Then read it in with:

   QFile file(fileName);
   file.open(QIODevice::ReadOnly);
   QDataStream in(&file);

   // Read and check the header
   quint32 magic = 0;
   in >> magic;
   if (magic != 0x139A1A7F)
       return ImportImage(fileName);

   // Read the version
   qint32 version = 0;
   in >> version;
   if (version < 90)
       return false; // too old
   if (version > 100)
       return false; // too new

   if (version <= 100) {
       in.setVersion(QDataStream::Qt_5_0);
   }
   else {
      // in.setVersion(QDataStream::Qt_6_0);
   }

   // Write the data
   in >> image;
   in >> Origin;
   in >> BackgroundImagesOrigin;

   // Now read all postits
   qint32 NumberOfSavedPostits = 0;
   in >> NumberOfSavedPostits;
   QImage NewImage;
   QPoint Position;
   BoundingBoxClass NewBox;
   PostIts.clear();
   for (int i = 0; i < NumberOfSavedPostits; i++) {
      in >> Position;
      in >> NewImage;
      in >> NewBox;
      PostIts.push_back(PostIt(NewImage, Position, NewBox));
   }
   qint32 NumberOfBackgroundLayers = 0;
   in >> NumberOfBackgroundLayers;
   BackgroundImages.clear();
   bool Visible;
   emit(NumberOfLayerChanged(NumberOfBackgroundLayers));
   for (int i = 0; i < NumberOfBackgroundLayers; i++) {
      in >> Visible;
      in >> NewImage;
      BackgroundImages.push_back(ImageDescriptor(std::make_unique<QImage>(NewImage), Visible));
      emit(SetVisibilityIndicatorOfLayer(i, Visible));
   }


   modified = false;
   LastDrawingValid = false;
   EraseLastDrawnObject = false;

   update();
   return true;
#endif
}
//! [4]

//! [5]
void ScribbleArea::setPenColor(const QColor &newColor)
//! [5] //! [6]
{
    auto alpha = myPenColor.alpha();
    myPenColor = newColor;
    myPenColor.setAlpha(alpha);
    //myPenWidth = SelectedPenWidth;
}
//! [6]

void ScribbleArea::HandleToolAction(QAction *action)
{
    myPenWidth = SelectedPenWidth;

    if (action->iconText() == "PenColor") {
        myPenColor = action->data().value<QColor>();
        MarkerActive = false;
    } else if(action->iconText() == "Red") {
        myPenColor = Qt::red;
    } else if (action->iconText() == "Blue") {
        myPenColor = Qt::blue;
    } else if (action->iconText() == "Green") {
        myPenColor = Qt::darkGreen;
    } else if (action->iconText() == "Yellow") {
        myPenColor = QColor(196,196,0);
    } else if (action->iconText() == "Black") {
        myPenColor = Qt::black;
    } else if (action->iconText() == "Orange") {
        myPenColor = QColor(255,128,0);
    } else if (action->iconText() == "Magenta") {
        myPenColor = Qt::magenta;
    } else if (action->iconText() == "MarkerYellow") {
       myPenColor = Qt::yellow;
       myPenColor.setAlpha(64+190);
       myPenWidth = SelectedPenWidth * 5 + 2;
       MarkerActive = true;
    } else if (action->iconText() == "SmallPen") {
       SelectedPenWidth = 1;
       myPenWidth = SelectedPenWidth;
    } else if (action->iconText() == "MediumPen") {
       SelectedPenWidth = 2;
       myPenWidth = SelectedPenWidth;
    } else if (action->iconText() == "LargePen") {
       SelectedPenWidth = 4;
       myPenWidth = SelectedPenWidth;
    } else if (action->iconText() == "NewPlane") {
       int NumberOfLayers = MoveImageToBackgroundLayer();
       emit(NumberOfLayerChanged(NumberOfLayers));
       for (int i = 0; i < NumberOfLayers; i++) {
         emit(SetVisibilityIndicatorOfLayer(i, BackgroundImages[i].IsVisible()));
       }
    } else if (action->iconText() == "Freeze") {
       Freeze(action->isChecked());
    }
}

void ScribbleArea::HandleLayerVisibilityAction(QAction *action)
{
   unsigned int SelectedLayer = action->data().value<int>();
   if (SelectedLayer < BackgroundImages.size()) {
      BackgroundImages[SelectedLayer].SetVisible(!action->isChecked());
      update();
   }
}

//! [7]
void ScribbleArea::setPenWidth(int newWidth)
//! [7] //! [8]
{
    myPenWidth = newWidth;
    SelectedPenWidth = myPenWidth;
}

int ScribbleArea::MoveImageToBackgroundLayer()
{
   CompleteImage();
   BackgroundImages.push_back(std::make_unique<QImage>(image));
   clearImage();
   return BackgroundImages.size();
}

int ScribbleArea::MoveTopBackgroundLayerToImage()
{
   if (!BackgroundImages.empty()) {
      // create empty image
      QImage newImage(image.size(), QImage::Format_ARGB32);
      newImage.fill(TransparentColor);
      QPainter painter(&newImage);

      // draw top bg layer onto it
      painter.drawImage((BackgroundImagesOrigin-Origin).toPoint(), *BackgroundImages.back());
      BackgroundImages.pop_back();

      // Then draw our current image (Without the currently drawn object)
      painter.drawImage(QPoint(0,0), image);

      //Set combined image as new image
      image = newImage;
   }
   return BackgroundImages.size();
}

int ScribbleArea::CollapseBackgroundLayers()
{
   if (!BackgroundImages.empty()) {
      QImage newImage;
      QPainter painter;
      auto it = BackgroundImages.begin();
      while (it != BackgroundImages.end()) {
         if (it->IsVisible()) {
            if (newImage.isNull()) {
               newImage = **it;
               painter.begin(&newImage);
            } else {
               painter.drawImage(QPoint(0,0), **it);
            }
            it = BackgroundImages.erase(it);
         } else {
            it++;
         }
      }
      BackgroundImages.push_back(std::make_unique<QImage>(newImage));
   }
   return BackgroundImages.size();
}
//! [8]

//! [9]
void ScribbleArea::clearImage()
//! [9] //! [10]
{
    image.fill(TransparentColor);
    modified = true;
    LastDrawingValid = false;
    LastDrawnObjectPoints.clear();
    LastDrawnObject.fill(TransparentColor);

    update();
}
//! [10]

//! [11]
void ScribbleArea::mousePressEvent(QMouseEvent *event)
{
   std::cout << "Mouse: press";
   HandlePressEventSM(event->button(), event->pos(), event->timestamp());

}

void ScribbleArea::HandlePressEventSM(Qt::MouseButton Button, QPointF Position, ulong Timestamp)
{
    std::cout << "Button Down: " << Button  << std::endl;
    if (Button == Qt::LeftButton) {
       setCursor(Qt::ArrowCursor);
        if (State != Idle) {
           std::cout << "HandlePressEventSM: unexpected State: " << State  << std::endl;
        }
        State = WaitingToLeaveJitterProtectionForDrawing;

        Tracker.StartTracking(Position, Timestamp);


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

void ScribbleArea::HandleMoveEventSM(Qt::MouseButtons Buttons, QPointF Position, ulong Timestamp, bool Erasing, double Pressure)
{

    Tracker.Trackmovement(Position, Timestamp);
    //WaitForPostIt = false;


    if ((Buttons & Qt::LeftButton)) {
        if ((Position-lastPoint).manhattanLength() < myPenWidth+2) {
            return; // ignore small movements (probably use penwidth*2)
        }
        switch (State) {
           case ScribbleArea::Idle:
              break;
           case ScribbleArea::WaitingToLeaveJitterProtectionForDrawing:
              if (((Position-ButtonDownPosition).manhattanLength() < (myPenWidth*3+2))) {
                  return; // ignore small movements (probably use penwidth*2)
              }
              State = Drawing;
              [[fallthrough]];
           case ScribbleArea::DrawingPaused:
           case ScribbleArea::Drawing:
           case ScribbleArea::DrawingFillRequested:
               if (State == DrawingFillRequested) {
                  if ((Position-FillPolygonStartPosition).manhattanLength() > (myPenWidth*3+2)) {
                     State = Drawing;
                     setCursor(Qt::ArrowCursor);

                  }
               } else if (State == DrawingPaused) {
                  State = DrawingFillRequested;
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
              break;

              break;
              break;
           case ScribbleArea::WaitingToLeaveJitterProtectionWithSelectedAreaForMoving:
           case ScribbleArea::MovingSelectionPaused:
              State = MovingSelection;
           case ScribbleArea::MovingSelection:
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
              break;
              break;
           case ScribbleArea::WaitingToLeaveJitterProtectionWithCreatedPostitForMoving:
           case ScribbleArea::WaitingToLeaveJitterProtectionWithSelectedPostitForMoving:
              State = MovingPostit;
           case ScribbleArea::MovingPostit:
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
              break;
           case ScribbleArea::WaitingToLeaveJitterProtectionForScrolling:
              State = ScrollingDrawingArea;
           case ScribbleArea::ScrollingDrawingArea:
              CompleteImage();
             Origin -= Position- ScrollingLastPosition;
             if (!Frozen) {
                BackgroundImagesOrigin -= Position- ScrollingLastPosition;
             }

             ScrollingLastPosition = Position;
             update();
              break;

        }
    } else if (Buttons == Qt::NoButton) {
        if (IsInsideAnyPostIt(Position)) {
           setCursor(Qt::PointingHandCursor);
        }  else {
           setCursor(Qt::ArrowCursor);
        }
    } else {
       setCursor(Qt::ArrowCursor);
    }
}

void ScribbleArea::mouseReleaseEvent(QMouseEvent *event)
{
   std::cout << "Mouse: ";
   HandleReleaseEventSM(event->button(), event->pos(), false, 0);
}


void ScribbleArea::HandleReleaseEventSM(Qt::MouseButton Button, QPointF Position, bool Erasing, double Pressure)
{
   std::cout << "Button Up: " << Button  << std::endl;

   if (Button == Qt::LeftButton) {
      switch(State) {
         case ScribbleArea::Idle:
//         case ScribbleArea::WaitingToLeaveJitterProtectionForDrawing:
         case ScribbleArea::WaitingToLeaveJitterProtectionWithSelectedAreaForMoving:
         case ScribbleArea::WaitingToLeaveJitterProtectionWithCreatedPostitForMoving:
            State = Idle;
            break;

         case ScribbleArea::WaitingToLeaveJitterProtectionForDrawing:
         case ScribbleArea::Drawing:
         case ScribbleArea::DrawingPaused:
         case ScribbleArea::DrawingFillRequested:
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
         case ScribbleArea::MovingSelection:
         case ScribbleArea::MovingSelectionPaused:
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
         case ScribbleArea::MovingPostit:
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

         case ScribbleArea::WaitingToLeaveJitterProtectionWithSelectedPostitForMoving:
            SelectedPostit.clear();
            setCursor(Qt::ArrowCursor);
            State = Idle;
            break;


         case ScribbleArea::WaitingToLeaveJitterProtectionForScrolling:
         case ScribbleArea::ScrollingDrawingArea:
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

void ScribbleArea::HandleTouchPressEventSM(int NumberOfTouchpoints, QPointF MeanPosition)
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

void ScribbleArea::HandleTouchMoveEventSM(int NumberOfTouchpoints, QPointF MeanPosition)
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

void ScribbleArea::HandleTouchReleaseEventSM(int NumberOfTouchpoints, QPointF MeanPosition)
{
   if(NumberOfTouchpoints == 2) {

   switch (State) {
   case ScribbleArea::WaitingForTouchScrolling:
   case ScribbleArea::TouchScrollingDrawingArea:
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
        break;
       case QEvent::TabletMove:
          // Tablett move also called on pressure or tilt changes
          if (LastTablettMovePosition != event->position()) {
             std::cout << "Tablett move " << event->type() << "/"<< event->buttons() << " <" << event->pos().x() << ";" << event->pos().y() << ">:" << event->pressure() << std::endl;
             HandleMoveEventSM(event->buttons(), event->position(), event->timestamp(), event->pointerType() == QPointingDevice::PointerType::Eraser, event->pressure());
             LastTablettMovePosition = event->position();
          }
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


void ScribbleArea::timeoutSM()
{
    /* Copying on long move pauses */
   std::cout << "timeout " << std::endl;
   switch(State) {
      case ScribbleArea::Idle:
         break;
      case ScribbleArea::WaitingToLeaveJitterProtectionForDrawing:
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
      case ScribbleArea::WaitingToLeaveJitterProtectionWithSelectedAreaForMoving:
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

      case ScribbleArea::WaitingToLeaveJitterProtectionForScrolling:
         break;
      case ScribbleArea::WaitingToLeaveJitterProtectionWithCreatedPostitForMoving:
         break;
      case ScribbleArea::WaitingToLeaveJitterProtectionWithSelectedPostitForMoving:
         break;
      case ScribbleArea::Drawing:
         //   QTextStream(stdout) << "<" << GestureTrackerAccumulatedSpeed.x() << "; " << GestureTrackerAccumulatedSpeed.y() << "> <"  << GestureTrackerAccumulatedSquaredSpeed.x() << ";" << GestureTrackerAccumulatedSquaredSpeed.y() << endl;

         //FillPolygon = true;
         FillPolygonStartPosition = lastPoint;
         setCursor(Qt::BusyCursor);
         update();
         State = DrawingPaused;
         break;
      case ScribbleArea::DrawingPaused:
         break;
      case ScribbleArea::DrawingFillRequested:
         break;
      case ScribbleArea::MovingSelection:
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
      case ScribbleArea::MovingSelectionPaused:
         break;
      case ScribbleArea::MovingPostit:
         break;
      case ScribbleArea::ScrollingDrawingArea:
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
