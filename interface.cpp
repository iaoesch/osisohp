/*****************************************************************************/
/*  Class      : BoundingBoxClass                               Version 1.0  */
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
/*  File       : box.cpp                                                     */
/*                                                                           */
/*****************************************************************************/
/* MagicSoft                                                                 */
/*****************************************************************************/

/* imports */
#include "interface.hpp"
#include "scribblearea.h"

/* Class constant declaration  */

/* Class Type declaration      */

/* Class data declaration      */

/* Class procedure declaration */

/*****************************************************************************/
/*  Method      : BoundingBoxClass                                           */
/*****************************************************************************/
/*                                                                           */
/*  Function    : Constructor for the boundingbox Class. initializes a       */
/*                boundingbox                                                */
/*                                                                           */
/*  Type        : Constructor                                                */
/*                                                                           */
/*  Input Para  : x1, y1, x2, y2: Shape of box                               */
/*                                                                           */
/*  Output Para : None                                                       */
/*                                                                           */
/*  Author      : I. Oesch                                                   */
/*                                                                           */
/*  History     : 05.12.1999  IO  Created                                    */
/*                                                                           */
/*****************************************************************************/

/*****************************************************************************/
/*  End  Method : IsOverlapping                                              */
/*****************************************************************************/

/*****************************************************************************/
/*  End Class   : BoundingBoxClass                                           */
/*****************************************************************************/



void GuiInterface::UpdateRequest()
{
   Scribler->update();
}

#if 0
void GuiInterface::modified()
{
   //Scribler->modified = true;
}

void GuiInterface::drawLineTo(const QPointF &endPoint, double Pressure)
{

}

void GuiInterface::drawrectangle(const BoundingBoxClass &Region)
{

}

void GuiInterface::DrawMovedSelection(const QPointF Offset)
{

}

void GuiInterface::DrawLastDrawnPicture()
{

}

void GuiInterface::resizeImage(QImage *image, const QSize &newSize, QPoint Offset)
{

}

void GuiInterface::resizeScrolledImage()
{

}

bool GuiInterface::PostItSelected(QPointF Position)
{

}

void GuiInterface::EraseLineTo(const QPointF &endPoint, double Pressure)
{

}

bool GuiInterface::IsInsideAnyPostIt(QPointF Position)
{

}

void GuiInterface::MoveSelectedPostits(QPointF Position)
{

}

void GuiInterface::FinishMovingSelectedPostits(QPointF Position)
{

}

bool GuiInterface::AreAnyPostitsSelected()
{

}

void GuiInterface::ClearSelectedPostits()
{

}

void GuiInterface::CompleteImage()
{

}

void GuiInterface::FilllastDrawnShape()
{

}

void GuiInterface::MakeSreenMoveHint()
{

}

void GuiInterface::MakeSelectionFromLastDrawnObject()
{

}

void GuiInterface::CreeatePostitFromSelection()
{

}
#endif


void GuiInterface::setSpeciallCursor()
{
   Scribler->SetSpecialCursor();
}

GuiInterface::GuiInterface(ScribbleArea *Scribler, CursorManager *TheCursorManager) : Scribler(Scribler), MyCursorManager(TheCursorManager)
{}
