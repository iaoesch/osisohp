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
#include "animatedcursorclass.h"


/* Class constant declaration  */

/* Class Type declaration      */

/* Class data declaration      */

/* Class definition            */
class CursorManager;
class QImage;
class ScribbleArea;

class GuiInterface
{
   ScribbleArea *Scribler;
   CursorManager *MyCursorManager;
public:
   GuiInterface(ScribbleArea *Scribler, CursorManager *TheCursorManager);
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
   void SetCursor(CursorManager::CursorType Cursor)
   {
       MyCursorManager->SetCursor(Cursor, 0ms, 0ms);
   }
   void SetCursor(CursorManager::CursorType Cursor, double Duration)
   {
      SetCursor(Cursor, std::chrono::milliseconds(static_cast<int>(Duration)));
   }
   void SetCursor(CursorManager::CursorType Cursor, std::chrono::milliseconds Duration)
   {
      MyCursorManager->SetCursor(Cursor, 3*Duration/4, Duration/4);
   }
   void SetCursor(CursorManager::CursorType Cursor, std::chrono::milliseconds Duration, std::chrono::milliseconds StartupDelay)
   {
      MyCursorManager->SetCursor(Cursor, Duration, StartupDelay);
   }
   void RestartAnimatedCursor() {
      MyCursorManager->RestartAnimatedCursor();
   }


};

/*****************************************************************************/
/*  End Header  : BoundingBoxClass                                           */
/*****************************************************************************/
#endif

