#include "drawingobjectclass.h"
#include "DebugStream.hpp"
#include <QPainter>
#include "databaseclass.h"

DrawingObjectClass::DrawingObjectClass(const QColor &Transparent, SettingClass &MySettings)
   : TransparentColor(Transparent),
     Settings(MySettings)
{
   ShapeNeedsTransfer = false;
   Empty = true;
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
   double MaxPenScaling = Settings.MaxPenForceScaling;
   constexpr double MinPenScaling = 1.0;
   constexpr double MaxPenForce = 0.9;
   constexpr double MinPenForce = DatabaseClass::JitterPressureLimit;
   constexpr double dx = MaxPenForce*MaxPenForce - MinPenForce*MinPenForce;
   double dy = MaxPenScaling - MinPenScaling;

   return (BaseWidth * qMax(MinPenScaling, Pressure*Pressure*(dy/dx)+(MinPenScaling-(MinPenForce*MinPenForce*dy/dx))) + 0.5);
}

double DrawingObjectClass::CalculatePenWidthLinear(double Pressure, int BaseWidth)
{
   constexpr double MinPenScaling = 1.0;
   constexpr double MaxPenForce = 0.9;
   constexpr double MinPenForce = DatabaseClass::JitterPressureLimit;
   constexpr double dx = MaxPenForce - MinPenForce;
   double dy = Settings.MaxPenForceScaling - MinPenScaling;

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

void DrawingObjectClass::CancelShape()
{
    ShapeNeedsTransfer = false;
   EraseLastDrawnObject = false;
    Empty = true;
   CurrentShape.Clear();
   CurrentImage.fill(TransparentColor);
}

void DrawingObjectClass::DrawBackgroundPart(QPainter &painter, const QRect &dirtyRect)
{
   if (MarkerActive & !Empty) {
      painter.drawImage(dirtyRect, CurrentImage, dirtyRect);
   }
}

void DrawingObjectClass::DrawForegroundPartAndModifiedImage(QPainter &painter, const QImage &image, const QPointF &Offset, const QRect &dirtyRect)
{

   if (EraseLastDrawnObject  & !Empty) {
      QImage ModifiedImage(image);
      QPainter MIpainter(&ModifiedImage);
      MIpainter.setCompositionMode(QPainter::CompositionMode_DestinationOut);
      MIpainter.drawImage(Offset, CurrentImage);
      painter.drawImage(dirtyRect, ModifiedImage, dirtyRect.translated(Offset.toPoint()));

   } else {
      painter.drawImage(dirtyRect, image, dirtyRect.translated(Offset.toPoint()));
      if (!MarkerActive & !Empty) {
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

#if 0
bool DrawingObjectClass::DrawLastDrawnShapeAndStartNewShape(QPainter &painter, const QPointF &Offset)
{
   if (TransferLastDrawnShape(painter, Offset)) {
      CurrentShape.LastDrawnObjectPoints.append(lastPointDrawn);
      return true;
   } else {
      return false;
   }
}
#endif

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
      Empty = true;
#if 1
   //   BeginNewShape(lastPointDrawn);
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

void DrawingObjectClass::BeginNewShape(const QPointF &Startpoint)
{
      CurrentImage.fill(TransparentColor);

      ShapeNeedsTransfer = false;
      Empty = false;
      CurrentShape.Clear();
      CurrentShape.AddPoint(Startpoint);
      lastPointDrawn = Startpoint;
      //CurrentShape.LastDrawnObjectPoints.append(Startpoint);
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
 //  CurrentShape.CurrentPaintedObjectBoundingBox.Clear();
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






