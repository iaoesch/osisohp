#include "selectionclass.h"

#include <QPainter>
#include "drawingobjectclass.h"
#include "databaseclass.h"











void SelectionClass::MakeSelectionFromLastDrawnObject(QImage &image, QPointF Origin, DrawingObjectClass &DrawnObject,
                                                      bool Cutout)
{
    SelectedImagePart =  image.copy(DrawnObject.Shape().Box().QRectangle().translated(Origin.toPoint()));
    SelectedImageBoundingBox = DrawnObject.Shape().Box();

    HintSelectedImagePart = SelectedImagePart;
    HintSelectedImagePart.fill(DatabaseClass::getTransparentColor());
    DiscardSelection = false;

    if (Cutout == true) {
        QPainter painter2(&image);
        DrawnObject.CutOut(painter2, Origin);
    }
    QPainter painter(&HintSelectedImagePart);
    painter.setPen(QPen(SelectionHintBorderColor, DatabaseClass::getHintBorderPenWidth(), Qt::SolidLine, Qt::RoundCap,
                        Qt::RoundJoin));
    painter.setBrush(QBrush(SelectionHintColor));
    auto TranslatedPoints = DrawnObject.Shape().Points().translated(-SelectedImageBoundingBox.GetLeft(), -SelectedImageBoundingBox.GetTop());
    painter.drawPolygon(TranslatedPoints);
    QPainterPath Path;
    Path.addPolygon(TranslatedPoints);
    QImage MaskedSelectedImagePart = SelectedImagePart;
    MaskedSelectedImagePart.fill(DatabaseClass::getTransparentColor());
    QPainter painter3(&MaskedSelectedImagePart);
    painter3.setClipPath(Path);
    painter3.drawImage(QPoint(0,0), SelectedImagePart);
    SelectedImagePart = MaskedSelectedImagePart;
    SelectedImagePartPath = Path;
    //CurrentlyDrawnObject.CancelShape();
    //LastPaintedObject.Clear();
    ////LastDrawnObject.fill(qRgba(0, 0, 0, 0));
}

void SelectionClass::Draw(QPainter &painter)
{
    if (DiscardSelection == false) {
        painter.drawImage(SelectedCurrentPosition+SelectedOffset, HintSelectedImagePart);
        painter.drawImage(SelectedCurrentPosition+SelectedOffset, SelectedImagePart);
    }
}

bool SelectionClass::DrawMovedSelection(QPainter &painter, const QPointF Offset)
{
    if (DiscardSelection == false) {
        painter.drawImage(SelectedOffset+Offset, SelectedImagePart);
        return true;
    } else {
        return false;
    }
};


void SelectionClass::MakeSreenMoveHint(QImage &image)
{
    SelectedImagePart =  image.copy();
    HintSelectedImagePart = SelectedImagePart;
    //  HintSelectedImagePart.fill(qRgba(200, 200, 200, 140));
    HintSelectedImagePart.fill(SelectHintColor);
}


