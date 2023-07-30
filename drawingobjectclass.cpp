#include "drawingobjectclass.h"
#include "DebugStream.hpp"
#include <QPainter>
#include "databaseclass.h"

DrawingObjectClass::DrawingObjectClass(const QColor &Transparent, SettingClass &MySettings)
   : TransparentColor(Transparent),
     Settings(MySettings)
{
    ShapeNeedsTransfer = false;
   EraseLastDrawnObject = false;
   myPenWidth = 2;
   myEraserWidth = Settings.EraserSize;
   SelectedPenWidth = myPenWidth;
   myPenColor = Qt::blue;
   MarkerActive = false;



}

QRect DrawingObjectClass::drawLineTo(const QPointF &endPoint, float Pressure)
{
   DEBUG_LOG << "Drawing Pressure: " << Pressure << std::endl;

    double ModifiedPenWidth = CalculatePenWidthLinear(Pressure, myPenWidth);
//    double ModifiedPenWidth = CalculatePenWidthQuadratic(Pressure, myPenWidth);
//   int ModifiedPenWidth = myPenWidth * qMax(1.0, Pressure*Pressure*4);
    QPainter painter(&CurrentImage);
    painter.setCompositionMode(QPainter::CompositionMode_Source);
    painter.setPen(QPen(myPenColor, ModifiedPenWidth, Qt::SolidLine, Qt::RoundCap,
                        Qt::RoundJoin));
    painter.drawLine(lastPointDrawn, endPoint);
//    SetModified();
    CurrentShape.AddPoint(endPoint);

    lastPointDrawn = endPoint;
    EraseLastDrawnObject = false;

    int rad = (ModifiedPenWidth / 2) + 2;
  //  update(QRect(lastPointDrawn.toPoint(), endPoint.toPoint()).normalized()
  //                                    .adjusted(-rad, -rad, +rad, +rad));
   return QRect(lastPointDrawn.toPoint(), endPoint.toPoint()).normalized()
                                        .adjusted(-rad, -rad, +rad, +rad);
}

QRect DrawingObjectClass::EraseLineTo(const QPointF &endPoint, float Pressure, QColor &BackGroundColor)
{
    DEBUG_LOG << "Erasing ";
    QPainter painter(&CurrentImage);
    int ModifiedPenWidth = static_cast<int>((myPenWidth+myEraserWidth)*3*(1.0 + Pressure*Pressure*10) + 0.5);
    painter.setPen(QPen(BackGroundColor, ModifiedPenWidth, Qt::SolidLine, Qt::RoundCap,
                        Qt::RoundJoin));
   // painter.setCompositionMode(QPainter::CompositionMode_Source);
    //painter.setCompositionMode(QPainter::CompositionMode_Clear);
    painter.drawLine(lastPointDrawn, endPoint);
  //  SetModified();
    CurrentShape.AddPoint(endPoint);

    EraseLastDrawnObject = true;
    lastPointDrawn = endPoint;

    int rad = (ModifiedPenWidth / 2) + 2;
  //  update(QRect(lastPointDrawn.toPoint(), endPoint.toPoint()).normalized()
  //                                   .adjusted(-rad, -rad, +rad, +rad));
    return QRect(lastPointDrawn.toPoint(), endPoint.toPoint()).normalized()
                                     .adjusted(-rad, -rad, +rad, +rad);
}

double DrawingObjectClass::CalculatePenWidthQuadratic(double Pressure, int BaseWidth)
{
   constexpr double MaxPenScaling = 11.0;
   constexpr double MinPenScaling = 1.0;
   constexpr double MaxPenForce = 0.9;
   constexpr double MinPenForce = DatabaseClass::JitterPressureLimit;
   constexpr double dx = MaxPenForce*MaxPenForce - MinPenForce*MinPenForce;
   constexpr double dy = MaxPenScaling - MinPenScaling;

   return (BaseWidth * qMax(MinPenScaling, Pressure*Pressure*(dy/dx)+(MinPenScaling-(MinPenForce*MinPenForce*dy/dx))) + 0.5);
}

double DrawingObjectClass::CalculatePenWidthLinear(double Pressure, int BaseWidth)
{
   constexpr double MaxPenScaling = 11.0;
   constexpr double MinPenScaling = 1.0;
   constexpr double MaxPenForce = 0.9;
   constexpr double MinPenForce = DatabaseClass::JitterPressureLimit;
   constexpr double dx = MaxPenForce - MinPenForce;
   constexpr double dy = MaxPenScaling - MinPenScaling;

   return (BaseWidth * qMax(MinPenScaling, Pressure*(dy/dx)+(MinPenScaling-(MinPenForce*dy/dx))) + 0.5);
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
    ShapeNeedsTransfer = false;
   EraseLastDrawnObject = false;
   CurrentShape.LastDrawnObjectPoints.clear();
   CurrentImage.fill(TransparentColor);
}

void DrawingObjectClass::DrawBackgroundPart(QPainter &painter, const QRect &dirtyRect)
{
   if (MarkerActive) {
      painter.drawImage(dirtyRect, CurrentImage, dirtyRect);
   }
}

void DrawingObjectClass::DrawForegroundPart(QPainter &painter, const QImage &image, const QPointF &Offset, const QRect &dirtyRect)
{
   if (EraseLastDrawnObject) {
      QImage ModifiedImage(image);
      QPainter MIpainter(&ModifiedImage);
      MIpainter.setCompositionMode(QPainter::CompositionMode_DestinationOut);
      MIpainter.drawImage(Offset, CurrentImage);
      painter.drawImage(dirtyRect, ModifiedImage, dirtyRect.translated(Offset.toPoint()));

   } else {
      painter.drawImage(dirtyRect, image, dirtyRect.translated(Offset.toPoint()));
      if (!MarkerActive) {
          painter.drawImage(dirtyRect, CurrentImage, dirtyRect);
      }
   }
}




void DrawingObjectClass::FillLastDrawnShape(QPainter &&painter2, const QPointF &Offset)
{
   painter2.setPen(QPen(QColor(0, 0, 0, 0), myPenWidth, Qt::SolidLine, Qt::RoundCap,
                        Qt::RoundJoin));
   painter2.setBrush(QBrush(myPenColor));
   painter2.setCompositionMode(QPainter::CompositionMode_Source);
   // LastDrawnObjectPoints.translate(-LastPaintedObjectBoundingBox.GetTop(), -LastPaintedObjectBoundingBox.GetLeft());
   painter2.drawPolygon(CurrentShape.Points().translated(Offset));
}


bool DrawingObjectClass::DrawLastDrawnShapeAndStartNewShape(QPainter &painter, const QPointF &Offset)
{
   if (TransferLastDrawnShape(painter, Offset)) {
      CurrentShape.LastDrawnObjectPoints.append(lastPointDrawn);
      return true;
   } else {
      return false;
   }
}

bool DrawingObjectClass::TransferLastDrawnShape(QPainter &painter, const QPointF &Offset)
{
   if (ShapeNeedsTransfer) {
      if (EraseLastDrawnObject) {
         painter.setCompositionMode(QPainter::CompositionMode_DestinationOut);
      }
      if (MarkerActive) {
         painter.setCompositionMode(QPainter::CompositionMode_DestinationOver);

      }
      painter.drawImage(Offset, CurrentImage);
      ShapeNeedsTransfer = false;
#if 1
      BeginNewShape();
#else
      CurrentImage.fill(TransparentColor);

      //LastDrawingValid = false;
      CurrentShape.LastDrawnObjectPoints.clear();
#endif
      return true;
   } else {
      return false;
   }
}

void DrawingObjectClass::BeginNewShape()
{
      CurrentImage.fill(TransparentColor);

   ShapeNeedsTransfer = false;
      CurrentShape.Clear();
      CurrentShape.LastDrawnObjectPoints.append(lastPointDrawn);
}


#if 0
void DrawingObjectClass::ExtendBoundingboxAndShape(QPointF Position)
{
   CurrentShape.AddPoint(Position);
}
#endif

DrawingObjectClass::ShapeClass DrawingObjectClass::EndShape(/*QPointF Position*/)
{
//   CurrentShape.AddPoint(Position);
      ShapeNeedsTransfer = true;
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
   painter2.drawPolygon(CurrentShape.Points().translated(Offset));
}






