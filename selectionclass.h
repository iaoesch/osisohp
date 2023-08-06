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

class DrawingObjectClass;


class SelectionClass {
    QColor SelectHintColor;
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
        SelectHintColor = qRgba(0, 0, 0, 40);


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

    void MakeSelectionFromLastDrawnObject(QImage &image, QPointF Origin, DrawingObjectClass &DrawnObject, bool Cutout);

    void Draw(QPainter &painter);



    bool DrawMovedSelection(QPainter &painter, const QPointF Offset);
    void MakeSreenMoveHint(QImage &image);
    const BoundingBoxClass &getBoundingBox() const
    {
        return SelectedImageBoundingBox;
    }
    const QImage &getHintSelectedImagePart() const
    {
        return HintSelectedImagePart;
    }
    const QImage &getSelectedImagePart() const
    {
        return SelectedImagePart;
    }
    QPointF getSelectedOffset() const
    {
        return SelectedOffset;
    }
    const QPainterPath &getSelectedImagePartPath() const
    {
        return SelectedImagePartPath;
    }
};


#endif // SELECTIONCLASS_H
