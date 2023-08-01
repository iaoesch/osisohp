#ifndef SELECTIONCLASS_H
#define SELECTIONCLASS_H

#include <QImage>
#include <QPoint>
#include <QPainterPath>
//#include <list>
//#include <vector>
//#include <deque>

#include "box.hpp"
//#include "Settings.hpp"
//#include "backgroundimagemanagerclass.h"
//#include "postitmanagerclass.h"
//#include "drawingobjectclass.h"


class SelectionClass {
    QColor SelectionHintColor;
    QColor SelectionHintBorderColor;
    QPointF SelectedOffset;
    QPointF SelectedCurrentPosition;
    bool DiscardSelection;
    QImage SelectedImagePart;
    QPainterPath SelectedImagePartPath;
    QImage HintSelectedImagePart;
    BoundingBoxClass SelectedImageBoundingBox;

public:
    SelectionClass() {
        SelectionHintColor = QColor(0, 30, 0, 50);
        SelectionHintBorderColor = QColor(0, 30, 0, 50);

    }
    void SetSelectedOffset(QPointF lastPointDrawn) {
        SelectedOffset = QPoint(SelectedImageBoundingBox.GetLeft(), SelectedImageBoundingBox.GetTop()) - lastPointDrawn;
    }
    void setSelectionHintColor(const QColor &newColor) {SelectionHintColor = newColor;}

    bool getDiscardSelection() const
    {
        return DiscardSelection;
    }
    void setDiscardSelection(bool newDiscardSelection)
    {
        DiscardSelection = newDiscardSelection;
    }
    QPointF getSelectedCurrentPosition() const
    {
        return SelectedCurrentPosition;
    }
    void setSelectedCurrentPosition(QPointF newSelectedCurrentPosition)
    {
        SelectedCurrentPosition = newSelectedCurrentPosition;
    }
    void MoveSelectedCurrentPosition(QPointF delta) {SelectedCurrentPosition += delta;}

    void MakeSelectionFromLastDrawnObject(bool Cutout)
    {
        SelectedImagePart =  image.copy(LastPaintedObject.Box().QRectangle().translated(Origin.toPoint()));
        SelectedImageBoundingBox = LastPaintedObject.Box();

        HintSelectedImagePart = SelectedImagePart;
        HintSelectedImagePart.fill(TransparentColor);
        DiscardSelection = false;

        if (Cutout == true) {
            QPainter painter2(&image);
            CurrentlyDrawnObject.CutOut(painter2, Origin);
        }
        QPainter painter(&HintSelectedImagePart);
        painter.setPen(QPen(SelectionHintBorderColor, HintBorderPenWidth, Qt::SolidLine, Qt::RoundCap,
                            Qt::RoundJoin));
        painter.setBrush(QBrush(SelectionHintColor));
        LastPaintedObject.Points().translate(-SelectedImageBoundingBox.GetLeft(), -SelectedImageBoundingBox.GetTop());
        painter.drawPolygon(LastPaintedObject.Points());
        QPainterPath Path;
        Path.addPolygon(LastPaintedObject.Points());
        QImage MaskedSelectedImagePart = SelectedImagePart;
        MaskedSelectedImagePart.fill(TransparentColor);
        QPainter painter3(&MaskedSelectedImagePart);
        painter3.setClipPath(Path);
        painter3.drawImage(QPoint(0,0), SelectedImagePart);
        SelectedImagePart = MaskedSelectedImagePart;
        SelectedImagePartPath = Path;
        CurrentlyDrawnObject.CancelShape();
        LastPaintedObject.Clear();
        //LastDrawnObject.fill(qRgba(0, 0, 0, 0));
    }
    void Draw(QPainter &painter)
    {
        if (DiscardSelection == false) {
            painter.drawImage(SelectedCurrentPosition+SelectedOffset, HintSelectedImagePart);
            painter.drawImage(SelectedCurrentPosition+SelectedOffset, SelectedImagePart);
        }
    }



};


#endif // SELECTIONCLASS_H
