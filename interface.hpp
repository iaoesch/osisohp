#ifndef INTERFACE_CLASS_HPP
#define INTERFACE_CLASS_HPP
/*****************************************************************************/
/*  Header     : BoundingBoxClass                               Version 1.0  */
/*****************************************************************************/
/*                                                                           */
/*  Function   : This class holds a bounding box. This is the rectangle      */
/*               enclosing all elements belonging to a graphical object      */
/*                                                                           */
/*  Methodes   : BoundingBoxClass()                                          */
/*              ~BoundingBoxClass()                                          */
/*               AddPoint()                                                  */
/*               AddBox()                                                    */
/*               IsInside()                                                  */
/*               IsOverlapping()                                             */
/*               GetTop()                                                    */
/*               GetBottom()                                                 */
/*               GetLeft()                                                   */
/*               GetRigth()                                                  */
/*               IsValid()                                                   */
/*                                                                           */
/*  Author     : I. Oesch                                                    */
/*                                                                           */
/*  History    : 05.12.1999  IO Created                                      */
/*                                                                           */
/*  File       : box.hpp                                                     */
/*                                                                           */
/*****************************************************************************/
/* MagicSoft                                                                 */
/*****************************************************************************/

/* imports */

#include <QRect>
#include "box.hpp"


/* Class constant declaration  */

/* Class Type declaration      */

/* Class data declaration      */

/* Class definition            */
class QCursor;
class QImage;
class ScribbleArea;

class GuiInterface
{
   ScribbleArea *Scribler;

public:
   GuiInterface(ScribbleArea *Scribler);
   void UpdateRequest();
#if 0
   void modified(); //modified = True;
   void drawLineTo(const QPointF &endPoint, double Pressure);
   void drawrectangle(const BoundingBoxClass &Region);
   void DrawMovedSelection(const QPointF Offset);

   void DrawLastDrawnPicture();
   void resizeImage(QImage *image, const QSize &newSize, QPoint Offset = {0,0});
   void resizeScrolledImage();
   bool PostItSelected(QPointF Position);
   void EraseLineTo(const QPointF &endPoint, double Pressure);
   bool IsInsideAnyPostIt(QPointF Position);
   void MoveSelectedPostits(QPointF Position);
   void FinishMovingSelectedPostits(QPointF Position);
   bool AreAnyPostitsSelected(); // !SelectedPostit.empty()
   void ClearSelectedPostits(); // SelectedPostit.clear();

   void CompleteImage();
   void FilllastDrawnShape();
   void MakeSreenMoveHint();
   void MakeSelectionFromLastDrawnObject();
   void CreeatePostitFromSelection();
#endif
   void setCursor(QCursor const &Cursor);
   void setSpeciallCursor();


};

/*****************************************************************************/
/*  End Header  : BoundingBoxClass                                           */
/*****************************************************************************/
#endif

