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
#if 0

#include <QtWidgets>
#ifndef QT_NO_PRINTER
#include <QPrinter>
#include <QPrintDialog>
#endif

#include "PaintingArea.h"


//! [0]
PaintingArea::PaintingArea(QWidget *parent)
    : QWidget(parent), BackGroundColor(230,230, 200,255), SelectedBackGroundColor(90, 0, 0, 50)
{
    modified = false;
    LastDrawingValid = false;
    Scrolling = false;
    myPenWidth = 3;
    myPenColor = Qt::blue;

}
//! [0]

//! [1]
bool PaintingArea::openImage(const QString &fileName)
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
bool PaintingArea::saveImage(const QString &fileName, const char *fileFormat)
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
void PaintingArea::setPenColor(const QColor &newColor)
//! [5] //! [6]
{
    myPenColor = newColor;
}
//! [6]

//! [7]
void PaintingArea::setPenWidth(int newWidth)
//! [7] //! [8]
{
    myPenWidth = newWidth;
}
//! [8]

//! [9]
void PaintingArea::clearImage()
//! [9] //! [10]
{
    image.fill(qRgba(255, 255, 255, 0));
    modified = true;
    update();
}
//! [10]

//! [12] //! [13]
void PaintingArea::paintEvent(QPaintEvent *event)
//! [13] //! [14]
{
    QPainter painter(this);
    QRect dirtyRect = event->rect();
    painter.setPen(QPen(BackGroundColor, myPenWidth, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin));
    painter.setBrush(QBrush(BackGroundColor));
    painter.drawRect(dirtyRect);
    painter.drawImage(dirtyRect, image, dirtyRect.translated(Origin));
    painter.drawImage(dirtyRect, LastDrawnObject, dirtyRect);
    if ((ShowSelection == true)) {
       painter.drawImage(SelectedCurrentPosition+SelectedOffset, HintSelectedImagePart);
       painter.drawImage(SelectedCurrentPosition+SelectedOffset, SelectedImagePart);
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
void PaintingArea::resizeEvent(QResizeEvent *event)
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


void PaintingArea::StartNewObject(const QPoint &StartPoint)
{
  //  LastDrawingValid = false;
    LastDrawnObjectPoints.clear();
    LastDrawnObjectPoints.append(StartPoint);

    LastDrawnObjectBoundingBox.Clear();
    LastDrawnObjectBoundingBox.AddPoint(PositionClass(StartPoint.x(), StartPoint.y()));

    LastDrawnObjectLastPoint = StartPoint;

    LastDrawnObject.fill(qRgba(255, 255, 255, 0));
//    modified = true;

}

void PaintingArea::FinishNewObject()
{
    // Currently nothing to do...
}




//! [17]
void PaintingArea::drawLineTo(const QPoint &endPoint)
//! [17] //! [18]
{
    QPainter painter(&LastDrawnObject);
    painter.setPen(QPen(myPenColor, myPenWidth, Qt::SolidLine, Qt::RoundCap,
                        Qt::RoundJoin));
    painter.drawLine(LastDrawnObjectLastPoint, endPoint);
    LastDrawnObjectLastPoint = endPoint;
    LastDrawnObjectPoints.append(endPoint);
    LastDrawnObjectBoundingBox.AddPoint(PositionClass(endPoint.x(), endPoint.y()));
    modified = true;

    // int rad = (myPenWidth / 2) + 2;
    // update(QRect(lastPoint, endPoint).normalized()
    //                                 .adjusted(-rad, -rad, +rad, +rad));
    // lastPoint = endPoint;
}

void PaintingArea::DrawLastDrawnPicture()
{
    QPainter painter(&image);

    painter.drawImage(Origin, LastDrawnObject);
    LastDrawnObject.fill(qRgba(255, 255, 255, 0));
    modified = true;

    update();

}

void PaintingArea::DrawMovedSelection(const QPoint Offset)
{
    QPainter painter(&image);


    painter.drawImage(SelectedOffset+Offset+Origin, SelectedImagePart);
    modified = true;

    update();

};

//! [18]

//! [19]
void PaintingArea::resizeImage(QImage *image, const QSize &newSize)
//! [19] //! [20]
{
    if (image->size() == newSize)
        return;

    QImage newImage(newSize, QImage::Format_ARGB32);
    newImage.fill(qRgba(255, 255, 255, 0));
    QPainter painter(&newImage);
    painter.drawImage(QPoint(0, 0), *image);
    *image = newImage;
}

void PaintingArea::resizeScrolledImage()
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
    newImage.fill(qRgba(255, 255, 255, 0));
    QPainter painter(&newImage);
    painter.drawImage(QPoint(OffsetX, OffsetY), image);
    image = newImage;
}

//! [20]

//! [21]
void PaintingArea::print()
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
#endif
