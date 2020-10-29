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
    modified = false;
    scribbling = false;
    LastDrawingValid = false;
    Scrolling = false;
    EraseLastDrawnObject = false;
    myPenWidth = 1;
    SelectedPenWidth = myPenWidth;
    myPenColor = Qt::blue;
    connect(&MyTimer, SIGNAL(timeout()), this, SLOT(timeout()));
    MyTimer.setSingleShot(true);

    CopyTimeout = 500;
    GestureTimeout = 500;
    SelectTimeout = 500;
    PostItTimeout = 1000;
    SelectedPostit = nullptr;

    TransparentColor = QColor(255, 255, 255, 0);
    BackGroundColor = QColor(230,230, 200,255);

    CurrentDistance = 0;
    LastDistance = 0;
    DeltaTLastDistance = 0;
    DeltaTCurrentDistance = 0;


    RecentlyPastedObjectValid = false;
    WaitForPostIt = false;
   // QImage RecentlyMovedObject;
   // BoundingBoxClass RecentlyMovedObjectBoundingBox;


}
//! [0]

//! [1]
bool ScribbleArea::openImage(const QString &fileName)
//! [1] //! [2]
{
    QImage loadedImage;
    if (!loadedImage.load(fileName))
        return false;

    QSize newSize = loadedImage.size().expandedTo(size());
    resizeImage(&loadedImage, newSize);
    image = loadedImage;
    modified = false;
    update();
    return true;
}
//! [2]

//! [3]
bool ScribbleArea::saveImage(const QString &fileName, const char *fileFormat)
//! [3] //! [4]
{

    if (image.save(fileName, fileFormat)) {
        modified = false;
        return true;
    } else {
        return false;
    }
}
//! [4]

//! [5]
void ScribbleArea::setPenColor(const QColor &newColor)
//! [5] //! [6]
{
    myPenColor = newColor;
    myPenWidth = SelectedPenWidth;
}
//! [6]

void ScribbleArea::HandleToolAction(QAction *action)
{
    myPenWidth = SelectedPenWidth;
    if (action->iconText() == "Red") {
        myPenColor = Qt::red;
    } else if (action->iconText() == "Blue") {
        myPenColor = Qt::blue;
    } else if (action->iconText() == "Green") {
        myPenColor = Qt::green;
    } else if (action->iconText() == "Yellow") {
        myPenColor = QColor(128,128,0);
    } else if (action->iconText() == "Black") {
        myPenColor = Qt::black;
    } else if (action->iconText() == "Orange") {
        myPenColor = QColor(255,128,0);
    } else if (action->iconText() == "Magenta") {
        myPenColor = Qt::magenta;
    } else if (action->iconText() == "MarkerYellow") {
       myPenColor = Qt::yellow;
       myPenColor.setAlpha(64);
       myPenWidth = SelectedPenWidth * 5 + 2;
   }
}

//! [7]
void ScribbleArea::setPenWidth(int newWidth)
//! [7] //! [8]
{
    myPenWidth = newWidth;
    SelectedPenWidth = myPenWidth;
}
//! [8]

//! [9]
void ScribbleArea::clearImage()
//! [9] //! [10]
{
    image.fill(TransparentColor);
    modified = true;
    update();
}
//! [10]

//! [11]
void ScribbleArea::mousePressEvent(QMouseEvent *event)
{
   std::cout << "Mouse: ";
   HandlePressEvent(event->button(), event->pos(), event->timestamp());
}

void ScribbleArea::HandlePressEvent(Qt::MouseButton Button, QPoint Position, ulong Timestamp)

//! [11] //! [12]
{
    std::cout << "Button Down: " << Button  << std::endl;
    if (Button == Qt::LeftButton) {
        WaitForPostIt = false;

        GestureTrackerLastPosition =  Position;
        GestureTrackerLastPositionTimeStamp = Timestamp;
        GestureTrackeStartPosition =  Position;
        GestureTrackerStartPositionTimeStamp = Timestamp;
        GestureTrackerAccumulatedSpeed = QPoint(0,0);
        GestureTrackerAccumulatedSquaredSpeed =  QPoint(0,0);

        CurrentDistance = 0;
        LastDistance = 0;
        DeltaTLastDistance = 0;
        DeltaTCurrentDistance = 0;

        lastPoint = Position;
        ScribblingStartPosition = Position;
        scribbling = true;
        FillPolygon = false;
        ScribblingStarted = false;
        NewDrawingStarted = true;
        MoveSelected = false;
        CurrentPaintedObjectBoundingBox.Clear();
        CurrentPaintedObjectBoundingBox.AddPoint(PositionClass(lastPoint.x(), lastPoint.y()));
        SelectedCurrentPosition = Position;
        MyTimer.start(SelectTimeout);
        if (LastPaintedObjectBoundingBox.IsInside(PositionClass(Position.x(), Position.y()))) {
           DownInsideObject = true;
        } else {
           DownInsideObject = false;
        }
    }
}

void ScribbleArea::mouseMoveEvent(QMouseEvent *event)
{
   std::cout << "Mouse: ";
   HandleMoveEvent(event->buttons(), event->pos(), event->timestamp(), false, 0);
}

void ScribbleArea::HandleMoveEvent(Qt::MouseButtons Buttons, QPoint Position, ulong Timestamp, bool Erasing, double Pressure)
{

    QPoint GestureMovement = Position - GestureTrackerLastPosition;
    ulong  GestureTime = Timestamp - GestureTrackerLastPositionTimeStamp;

    LastDistance = CurrentDistance;
    DeltaTLastDistance = DeltaTCurrentDistance;
    DeltaTCurrentDistance = GestureTime;
    CurrentDistance = GestureMovement.manhattanLength();
    WaitForPostIt = false;

    if (GestureTime > 0) {
       QPointF GestureSpeed = QPointF(GestureMovement) / GestureTime;
       GestureTrackerLastPosition =  Position;
       GestureTrackerLastPositionTimeStamp = Timestamp;

       GestureTrackerAccumulatedSpeed += GestureSpeed;
       GestureTrackerAccumulatedSquaredSpeed += QPointF(GestureSpeed.x() * GestureSpeed.x(), GestureSpeed.y()*GestureSpeed.y() );
    }

    if ((Buttons & Qt::LeftButton)) {
        if ((Position-lastPoint).manhattanLength() < myPenWidth) {
            return; // ignore small movements (probably use penwidth*2)
        }
        if (scribbling) {
           if ((ScribblingStarted == false) && ((Position-ScribblingStartPosition).manhattanLength() < (myPenWidth*3+2))) {
               return; // ignore small movements (probably use penwidth*2)
           }
           ScribblingStarted = true;
            NewDrawingStarted = false;
            //FillPolygon = false;
           MyTimer.stop();
            MyTimer.start(GestureTimeout);
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
        }
        if (MoveSelected) {
          // QPoint Offset = event->pos() - SelectedPoint;
            MyTimer.stop();
            MyTimer.start(CopyTimeout);

            SelectedCurrentPosition = Position;
            update();
          // DrawMovedSelection(Offset);
          // BoundingBoxClass MovedRectangle(LastPaintedObjectBoundingBox);
          // MovedRectangle.Move(PositionClass(Offset.x(), Offset.y()));
          // drawrectangle(MovedRectangle);
           //        drawrectangle(BoundingBoxClass(LastPaintedObjectBoundingBox).Move(PositionClass(Offset.x(), Offset.y())));
        }
        if (SelectedPostit != nullptr) {
           std::cout << "Moving postit " << std::endl;
           SelectedPostit->Position = Position;
           update();
        }
        if (Scrolling) {
            if (LastDrawingValid) {
               DrawLastDrawnPicture();

               LastDrawingValid = false;
               LastDrawnObjectPoints.clear();


            }
           Origin -= Position- ScrollingLastPosition;
           ScrollingLastPosition = Position;
           update();
        }
    }
}

void ScribbleArea::mouseReleaseEvent(QMouseEvent *event)
{
   std::cout << "Mouse: ";
   HandleReleaseEvent(event->button(), event->pos(), false, 0);
}

void ScribbleArea::HandleReleaseEvent(Qt::MouseButton Button, QPoint Position, bool Erasing, double Pressure)
{
   std::cout << "Button Up: " << Button  << std::endl;

    if (Button == Qt::LeftButton && scribbling) {
        MyTimer.stop();
        if (Erasing) {
           EraseLineTo(Position, Pressure);
        } else {
           drawLineTo(Position, Pressure);
        }
        LastDrawnObjectPoints.append(Position);
        scribbling = false;
        LastDrawingValid = true;
        CurrentPaintedObjectBoundingBox.AddPoint(PositionClass(Position.x(), Position.y()));
        LastPaintedObjectBoundingBox = CurrentPaintedObjectBoundingBox;
        CurrentPaintedObjectBoundingBox.Clear();
        if (FillPolygon) {
        QPainter painter2(&image);
        painter2.setPen(QPen(QColor(0, 0, 0, 0), myPenWidth, Qt::SolidLine, Qt::RoundCap,
                             Qt::RoundJoin));
        painter2.setBrush(QBrush(myPenColor));
        painter2.setCompositionMode(QPainter::CompositionMode_Source);
        // LastDrawnObjectPoints.translate(-LastPaintedObjectBoundingBox.GetTop(), -LastPaintedObjectBoundingBox.GetLeft());
        painter2.drawPolygon(LastDrawnObjectPoints.translated(Origin));
        update();
        }
    }
    if (Button == Qt::LeftButton && MoveSelected) {
       WaitForPostIt = false;
        if ( ((DeltaTLastDistance + DeltaTCurrentDistance) > 0) &&
             ((LastDistance + CurrentDistance) / (float)(DeltaTLastDistance + DeltaTCurrentDistance)) > 0.25f) {
            std::cout << "LeavingSpeed " << ((LastDistance + CurrentDistance) / (float)(DeltaTLastDistance + DeltaTCurrentDistance)) << std::endl;
            DiscardSelection = true;
        }
        std::cout << "LeavingSpeed " << (LastDistance + CurrentDistance) << " / " << (DeltaTLastDistance + DeltaTCurrentDistance) << " = " << ((LastDistance + CurrentDistance) / (float)(DeltaTLastDistance + DeltaTCurrentDistance)) << std::endl;
            ;
      // QPoint Offset =  - SelectedPoint;
        if (DiscardSelection == false) {
           DrawMovedSelection(Position);
        }
      // BoundingBoxClass MovedRectangle(LastPaintedObjectBoundingBox);
     //  MovedRectangle.Move(PositionClass(Offset.x(), Offset.y()));
     //  drawrectangle(MovedRectangle);
       MoveSelected = false;
       LastDrawingValid = false;
       //        drawrectangle(BoundingBoxClass(LastPaintedObjectBoundingBox).Move(PositionClass(Offset.x(), Offset.y())));
    }
    if ((Button == Qt::LeftButton) && (SelectedPostit != nullptr)) {
       std::cout << "Fixing postit " << std::endl;
       SelectedPostit->Position = Position;
       SelectedPostit->Box.Move(PositionClass(Position.x()-StartPositionSelectedPostIt.x(), Position.y()-StartPositionSelectedPostIt.y()));
       MoveSelected = false;
       LastDrawingValid = false;
       SelectedPostit = nullptr;
       setCursor(Qt::ArrowCursor);
       update();
    }
    if (Button == Qt::LeftButton && Scrolling) {
       Scrolling = false;
       Origin -= Position - ScrollingLastPosition;
       resizeScrolledImage();
       update();
    }
}

void ScribbleArea::tabletEvent(QTabletEvent * event)
{
   std::cout << "Tablett Pen Type " << event->pointerType() << std::endl;
    switch(event->type()){
       case QEvent::TabletRelease:
          std::cout << "Tablett up " << event->type() << "/"<< event->button() << std::endl;
          HandleReleaseEvent(event->button(), event->pos(), event->pointerType() == QTabletEvent::Eraser, event->pressure());
          break;

       case QEvent::TabletPress:
        std::cout << "Tablett down " << event->type() << "/"<< event->button() << std::endl;
        HandlePressEvent(event->button(), event->pos(), event->timestamp());
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
          if (LastTablettMovePosition != event->pos()) {
             std::cout << "Tablett move " << event->type() << "/"<< event->buttons() << " <" << event->pos().x() << ";" << event->pos().y() << ">:" << event->pressure() << std::endl;
             HandleMoveEvent(event->buttons(), event->pos(), event->timestamp(), event->pointerType() == QTabletEvent::Eraser, event->pressure());
             LastTablettMovePosition = event->pos();
          }
        break;
    }
}



void ScribbleArea::timeout()
{
    /* Copying on long move pauses */
   std::cout << "timeout " << std::endl;

    if (MoveSelected) {
       if (WaitForPostIt == true) {
          std::cout << "Creating postit " << std::endl;

          WaitForPostIt = false;
          QImage NewPostit(HintSelectedImagePart);
          QPainter painter(&NewPostit);
          painter.drawImage(0,0,SelectedImagePart);
          PostIts.push_back(PostIt(NewPostit, SelectedCurrentPosition+SelectedOffset, LastPaintedObjectBoundingBox));
       } else {
          if (DiscardSelection == false) {
             if (GestureTrackerAccumulatedSpeed.manhattanLength()*10 > GestureTrackerAccumulatedSquaredSpeed.manhattanLength()) {
                QPoint CopyPos(SelectedCurrentPosition);
                SelectedCurrentPosition += QPoint(3,3);
                DrawMovedSelection(CopyPos);
             } else {
                DiscardSelection = true;
                update();
             }
          }
       }
    }
    if (NewDrawingStarted) {
       //drawrectangle(LastPaintedObjectBoundingBox);
       if (DownInsideObject) {

          SelectedImagePart =  image.copy(LastPaintedObjectBoundingBox.QRectangle().translated(Origin));
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


          MoveSelected = true;
          NewDrawingStarted = false;
          SelectedPoint = lastPoint;
          SelectedOffset = QPoint(LastPaintedObjectBoundingBox.GetLeft(), LastPaintedObjectBoundingBox.GetTop()) - lastPoint;
          scribbling = false;
          update();
          WaitForPostIt = true;
          MyTimer.start(PostItTimeout);

       } else {

          if (PostItSelected(SelectedCurrentPosition)) {
             scribbling = false;
             NewDrawingStarted = false;
             setCursor(Qt::ClosedHandCursor);
             std::cout << "Selected postit " << std::endl;

          } else {
          SelectedImagePart =  image.copy();
          HintSelectedImagePart = SelectedImagePart;
          HintSelectedImagePart.fill(qRgba(0, 0, 0, 40));
          Scrolling = true;
          ScrollingLastPosition = SelectedCurrentPosition;
          ScrollingOldOrigin = Origin;
          scribbling = false;
          update();
          }

       }
    }
    if (scribbling) {
       //   QTextStream(stdout) << "<" << GestureTrackerAccumulatedSpeed.x() << "; " << GestureTrackerAccumulatedSpeed.y() << "> <"  << GestureTrackerAccumulatedSquaredSpeed.x() << ";" << GestureTrackerAccumulatedSquaredSpeed.y() << endl;

       FillPolygon = true;
       update();

    }

}

bool ScribbleArea::PostItSelected(QPoint Position)
{
   for (auto &&PostIt: PostIts) {
     if (PostIt.Box.IsInside(PositionClass(Position.x(), Position.y()))) {
        // first found, first served...
        SelectedPostit = &PostIt;
        StartPositionSelectedPostIt = PostIt.Position;
        return true;
     }
   }
   SelectedPostit = nullptr;
   return false;
}

//! [12] //! [13]
void ScribbleArea::paintEvent(QPaintEvent *event)
//! [13] //! [14]
{
    QPainter painter(this);
    QRect dirtyRect = event->rect();
    painter.setPen(QPen(BackGroundColor, myPenWidth, Qt::SolidLine, Qt::RoundCap,
                        Qt::RoundJoin));
    painter.setBrush(QBrush(BackGroundColor));
    painter.drawRect(dirtyRect);
    painter.drawImage(dirtyRect, image, dirtyRect.translated(Origin));
    //painter.setCompositionMode(QPainter::CompositionMode_Source);
#if 0
    // Probably nonsense, as widget cannot be transparent ???
    if (EraseLastDrawnObject) {

       painter.setCompositionMode(QPainter::CompositionMode_DestinationOut);
    }
#endif
    painter.drawImage(dirtyRect, LastDrawnObject, dirtyRect);
    if (EraseLastDrawnObject) {
       painter.setCompositionMode(QPainter::CompositionMode_SourceOut);
    }

    for (auto &&Picture: PostIts) {
       painter.drawImage(Picture.Position, Picture.Image);
    }
    if ((MoveSelected) && (DiscardSelection == false)) {
       painter.drawImage(SelectedCurrentPosition+SelectedOffset, HintSelectedImagePart);
       painter.drawImage(SelectedCurrentPosition+SelectedOffset, SelectedImagePart);
    }
    if (RecentlyPastedObjectValid == true) {
        painter.drawImage(RecentlyPastedObjectPosition, RecentlyPastedObject);
    }
    if (Scrolling) {
        painter.setPen(QPen(QColor(90, 0, 0, 50), myPenWidth, Qt::SolidLine, Qt::RoundCap,
                            Qt::RoundJoin));
        painter.setBrush(QBrush(QColor(0, 30, 0, 50)));

        painter.drawRect(0,0,this->width(), this->height());

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
        update();
    }
    QWidget::resizeEvent(event);
}
//! [16]

//! [17]
void ScribbleArea::drawLineTo(const QPoint &endPoint, double Pressure)
//! [17] //! [18]
{
   std::cout << "Drawing ";
   int ModifiedPenWidth = myPenWidth * (1.0 + Pressure*Pressure*4);
    QPainter painter(&LastDrawnObject);
    painter.setPen(QPen(myPenColor, ModifiedPenWidth, Qt::SolidLine, Qt::RoundCap,
                        Qt::RoundJoin));
    painter.drawLine(lastPoint, endPoint);
    modified = true;

    int rad = (ModifiedPenWidth / 2) + 2;
    update(QRect(lastPoint, endPoint).normalized()
                                     .adjusted(-rad, -rad, +rad, +rad));
    lastPoint = endPoint;
    EraseLastDrawnObject = false;

}

void ScribbleArea::EraseLineTo(const QPoint &endPoint, double Pressure)
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
    update(QRect(lastPoint, endPoint).normalized()
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
    painter.drawImage(Origin, LastDrawnObject);
    LastDrawnObject.fill(TransparentColor);
    modified = true;

    update();

}

void ScribbleArea::DrawMovedSelection(const QPoint Offset)
{
    QPainter painter(&image);


    painter.drawImage(SelectedOffset+Offset+Origin, SelectedImagePart);
    modified = true;

    update();

};

//! [18]

//! [19]
void ScribbleArea::resizeImage(QImage *image, const QSize &newSize)
//! [19] //! [20]
{
    if (image->size() == newSize)
        return;

    QImage newImage(newSize, QImage::Format_ARGB32);
    newImage.fill(TransparentColor);
    QPainter painter(&newImage);
    //painter.setCompositionMode(QPainter::CompositionMode_Source);
    painter.drawImage(QPoint(0, 0), *image);
    *image = newImage;
}

void ScribbleArea::resizeScrolledImage()
//! [19] //! [20]
{
    int NewWidth;
    int NewHeight;
    int OffsetX = 0;
    int OffsetY = 0;

    if (Origin.x() < 0) {
        NewWidth = image.size().width() - Origin.x();
        OffsetX = - Origin.x();
        Origin.setX(0);
    } else if (Origin.x()+this->width() > image.size().width()){
        NewWidth = Origin.x()+this->width();
    } else {
        NewWidth = image.size().width();
    }
    if (Origin.y() < 0) {
        NewHeight = image.size().height() - Origin.y();
        OffsetY = - Origin.y();
        Origin.setY(0);
    } else if (Origin.y()+this->height() > image.size().height()){
        NewHeight = Origin.y()+this->height();
    } else {
        NewHeight = image.size().height();
    }

    QImage newImage(QSize(NewWidth ,NewHeight), QImage::Format_ARGB32);
    newImage.fill(TransparentColor);
    QPainter painter(&newImage);
    painter.drawImage(QPoint(OffsetX, OffsetY), image);
    image = newImage;
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
