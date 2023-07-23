#include "drawingobjectclass.h"
#include "DebugStream.hpp"
#include <QPainter>

DrawingObjectClass::DrawingObjectClass()
{
   LastDrawingValid = false;
   EraseLastDrawnObject = false;
   myPenWidth = 2;
   myEraserWidth = Settings.EraserSize;
   SelectedPenWidth = myPenWidth;
   myPenColor = Qt::blue;
   MarkerActive = false;



}

QRect DrawingObjectClass::drawLineTo(const QPointF &endPoint, double Pressure)
{
   DEBUG_LOG << "Drawing Pressure: " << Pressure << std::endl;

    double ModifiedPenWidth = CalculatePenWidthLinear(Pressure, myPenWidth);
//    double ModifiedPenWidth = CalculatePenWidthQuadratic(Pressure, myPenWidth);
//   int ModifiedPenWidth = myPenWidth * qMax(1.0, Pressure*Pressure*4);
    QPainter painter(&LastDrawnObject);
    painter.setCompositionMode(QPainter::CompositionMode_Source);
    painter.setPen(QPen(myPenColor, ModifiedPenWidth, Qt::SolidLine, Qt::RoundCap,
                        Qt::RoundJoin));
    painter.drawLine(lastPointDrawn, endPoint);
//    SetModified();
    lastPointDrawn = endPoint;
    EraseLastDrawnObject = false;

    int rad = (ModifiedPenWidth / 2) + 2;
  //  update(QRect(lastPointDrawn.toPoint(), endPoint.toPoint()).normalized()
  //                                    .adjusted(-rad, -rad, +rad, +rad));
   return QRect(lastPointDrawn.toPoint(), endPoint.toPoint()).normalized()
                                        .adjusted(-rad, -rad, +rad, +rad);
}

QRect DrawingObjectClass::EraseLineTo(const QPointF &endPoint, double Pressure)
{
    DEBUG_LOG << "Erasing ";
    QPainter painter(&LastDrawnObject);
    int ModifiedPenWidth = static_cast<int>((myPenWidth+myEraserWidth)*3*(1.0 + Pressure*Pressure*10) + 0.5);
    painter.setPen(QPen(BackGroundColor, ModifiedPenWidth, Qt::SolidLine, Qt::RoundCap,
                        Qt::RoundJoin));
   // painter.setCompositionMode(QPainter::CompositionMode_Source);
    //painter.setCompositionMode(QPainter::CompositionMode_Clear);
    painter.drawLine(lastPointDrawn, endPoint);
  //  SetModified();
    EraseLastDrawnObject = true;
    lastPointDrawn = endPoint;

    int rad = (ModifiedPenWidth / 2) + 2;
  //  update(QRect(lastPointDrawn.toPoint(), endPoint.toPoint()).normalized()
  //                                   .adjusted(-rad, -rad, +rad, +rad));
    return QRect(lastPointDrawn.toPoint(), endPoint.toPoint()).normalized()
                                     .adjusted(-rad, -rad, +rad, +rad);
}

void DrawingObjectClass::DrawLastDrawnPicture(QPainter &painter, const QPointF &Offset)
{
    //painter.setCompositionMode(QPainter::CompositionMode_Source);
    if (EraseLastDrawnObject) {
       painter.setCompositionMode(QPainter::CompositionMode_DestinationOut);
    }
    if (MarkerActive) {
       painter.setCompositionMode(QPainter::CompositionMode_DestinationOver);

    }
    painter.drawImage(Offset, LastDrawnObject);
    LastDrawnObject.fill(TransparentColor);
    //SetModified();
    //update();
}

void DrawingObjectClass::setPenColor(const QColor &newColor)
{
    auto alpha = myPenColor.alpha();
    myPenColor = newColor;
    myPenColor.setAlpha(alpha);
    //myPenWidth = SelectedPenWidth;
}
void DrawingObjectClass::setPenWidth(int newWidth)
{
    myPenWidth = newWidth;
    SelectedPenWidth = myPenWidth;
    if (MarkerActive) {
       ExtendPenWidthForMarker();
    }
}

void DrawingObjectClass::Clear()
{
   LastDrawingValid = false;
   EraseLastDrawnObject = false;
   CurrentShape.LastDrawnObjectPoints.clear();
   LastDrawnObject.fill(TransparentColor);
}

void DrawingObjectClass::DrawIfMarking(QPainter &painter, const QRect &dirtyRect)
{
   if (MarkerActive) {
      painter.drawImage(dirtyRect, LastDrawnObject, dirtyRect);
   }
}

void DrawingObjectClass::DrawNormal(QPainter &painter, const QRect &dirtyRect)
{
   if (!MarkerActive && !EraseLastDrawnObject) {
      painter.drawImage(dirtyRect, LastDrawnObject, dirtyRect);
   }
}

void DrawingObjectClass::DrawIfErasing(QPainter &painter, const QImage &image, const QPointF &Offset, const QRect &dirtyRect)
{
   if (EraseLastDrawnObject) {
      QImage ModifiedImage(image);
      QPainter MIpainter(&ModifiedImage);
      MIpainter.setCompositionMode(QPainter::CompositionMode_DestinationOut);
      MIpainter.drawImage(Offset, LastDrawnObject);
      painter.drawImage(dirtyRect, ModifiedImage, dirtyRect.translated(Offset.toPoint()));

   } else {
      painter.drawImage(dirtyRect, image, dirtyRect.translated(Offset.toPoint()));
      //painter.setCompositionMode(QPainter::CompositionMode_Source);
   }
}

void DrawingObjectClass::FillLastDrawnShape(QPainter &&painter2, const QPointF &Offset)
{
   painter2.setPen(QPen(QColor(0, 0, 0, 0), myPenWidth, Qt::SolidLine, Qt::RoundCap,
                        Qt::RoundJoin));
   painter2.setBrush(QBrush(myPenColor));
   painter2.setCompositionMode(QPainter::CompositionMode_Source);
   // LastDrawnObjectPoints.translate(-LastPaintedObjectBoundingBox.GetTop(), -LastPaintedObjectBoundingBox.GetLeft());
   painter2.drawPolygon(CurrentShape.LastDrawnObjectPoints.translated(Offset));
}

bool DrawingObjectClass::CompleteImage(QPainter &painter, const QPointF &Offset)
{
   if (LastDrawingValid) {
      DrawLastDrawnPicture(painter, Offset);
      LastDrawingValid = false;
      CurrentShape.LastDrawnObjectPoints.clear();
      return true;
   } else {
      return false;
   }
}

bool DrawingObjectClass::FlushLastDrawnPicture(QPainter &painter, const QPointF &Offset)
{
   if (LastDrawingValid) {
      DrawLastDrawnPicture(painter, Offset);

      LastDrawingValid = false;
      CurrentShape.LastDrawnObjectPoints.clear();
      CurrentShape.LastDrawnObjectPoints.append(lastPointDrawn);
   }
}

void DrawingObjectClass::ExtendBoundingboxAndShape(QPointF Position)
{
   CurrentShape.LastDrawnObjectPoints.append(Position);
   CurrentShape.CurrentPaintedObjectBoundingBox.AddPoint(PositionClass(Position.x(), Position.y()));
}

DrawingObjectClass::ShapeClass DrawingObjectClass::UpdateBoundingboxesForFinishedShape(QPointF Position)
{
   CurrentShape.LastDrawnObjectPoints.append(Position);
   LastDrawingValid = true;
   CurrentShape.CurrentPaintedObjectBoundingBox.AddPoint(PositionClass(Position.x(), Position.y()));
   ShapeClass LastPaintedObject = CurrentShape;
   CurrentShape.CurrentPaintedObjectBoundingBox.Clear();
   return LastPaintedObject;
}

void DrawingObjectClass::CutOut(QPainter &painter2, QPointF Offset)
{
   painter2.setPen(QPen(QColor(0, 0, 0, 0), 1, Qt::SolidLine, Qt::RoundCap,
                        Qt::RoundJoin));
   painter2.setBrush(QBrush(QColor(0, 0, 0, 0)));
   painter2.setCompositionMode(QPainter::CompositionMode_Source);
   // LastDrawnObjectPoints.translate(-LastPaintedObjectBoundingBox.GetTop(), -LastPaintedObjectBoundingBox.GetLeft());
   painter2.drawPolygon(CurrentShape.LastDrawnObjectPoints.translated(Offset));
}






