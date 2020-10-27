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
#include "box.hpp"

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

BoundingBoxClass::BoundingBoxClass(int x1, int y1, int x2, int y2)
{
   /* Method data declaration      */

   /* Method code declaration      */
   if (x1 > x2) {
      Left  = x2;
      Right = x1;
   } else {
      Left  = x1;
      Right = x2;
   }
   
   if (y1 > y2) {
      Top    = y2;
      Bottom = y1;
   } else {
      Top    = y1;
      Bottom = y2;
   }
}

BoundingBoxClass::BoundingBoxClass(const PositionClass &Pos1, const PositionClass &Pos2)
{
   /* Method data declaration      */

   /* Method code declaration      */
   if (Pos1.GetX() > Pos2.GetX()) {
      Left  = Pos2.GetX();
      Right = Pos1.GetX();
   } else {
      Left  = Pos1.GetX();
      Right = Pos2.GetX();
   }
   
   if (Pos1.GetY() > Pos2.GetY()) {
      Top    = Pos2.GetY();
      Bottom = Pos1.GetY();
   } else {
      Top    = Pos1.GetY();
      Bottom = Pos2.GetY();
   }
}

/*****************************************************************************/
/*  End  Method : BoundingBoxClass                                           */
/*****************************************************************************/

/*****************************************************************************/
/*  Method      : AddPoint                                                   */
/*****************************************************************************/
/*                                                                           */
/*  Function    : Resizes the boundingbox if needed to include the given     */
/*                point                                                      */
/*                                                                           */
/*  Type        : Public                                                     */
/*                                                                           */
/*  Input Para  : x, y: Coordinates of point to include                      */
/*                                                                           */
/*  Output Para : None                                                       */
/*                                                                           */
/*  Author      : I. Oesch                                                   */
/*                                                                           */
/*  History     : 05.12.1999  IO  Created                                    */
/*                                                                           */
/*****************************************************************************/
void BoundingBoxClass::AddPoint(const PositionClass &Pos)
{
   /* Method data declaration      */

   /* Method code declaration      */

   // Check if point is outside of box, iff so extend the box
   if (Pos.GetX() > Right) {
      Right = Pos.GetX();
   }
   if (Pos.GetX() < Left) {
      Left = Pos.GetX();
   }
   if (Pos.GetY() > Bottom) {
      Bottom = Pos.GetY();
   }
   if (Pos.GetY() < Top) {
      Top = Pos.GetY();
   }
}

/*****************************************************************************/
/*  End  Method : AddPoint                                                   */
/*****************************************************************************/

/*****************************************************************************/
/*  Method      : AddBox                                                     */
/*****************************************************************************/
/*                                                                           */
/*  Function    : Resizes the boundingbox if needed to include the given     */
/*                boundingbox                                                */
/*                                                                           */
/*  Type        : Public                                                     */
/*                                                                           */
/*  Input Para  : x, y: Coordinates of point to include                      */
/*                                                                           */
/*  Output Para : None                                                       */
/*                                                                           */
/*  Author      : I. Oesch                                                   */
/*                                                                           */
/*  History     : 05.12.1999  IO  Created                                    */
/*                                                                           */
/*****************************************************************************/
void BoundingBoxClass::AddBox(const BoundingBoxClass &Box)
{
   /* Method data declaration      */

   /* Method code declaration      */

   // Check if point is outside of box, iff so extend the box
   if (Box.Right > Right) {
      Right = Box.Right;
   }
   if (Box.Left < Left) {
      Left = Box.Left;
   }
   if (Box.Bottom > Bottom) {
      Bottom = Box.Bottom;
   }
   if (Box.Top < Top) {
      Top = Box.Top;
   }
}

/*****************************************************************************/
/*  End  Method : AddBox                                                     */
/*****************************************************************************/

/*****************************************************************************/
/*  Method      : Move                                                       */
/*****************************************************************************/
/*                                                                           */
/*  Function    : Moves the boundingbox for the given amount                 */
/*                                                                           */
/*  Type        : Public                                                     */
/*                                                                           */
/*  Input Para  : dx, dy: Offset to move                                     */
/*                                                                           */
/*  Output Para : None                                                       */
/*                                                                           */
/*  Author      : I. Oesch                                                   */
/*                                                                           */
/*  History     : 05.12.1999  IO  Created                                    */
/*                                                                           */
/*****************************************************************************/
void BoundingBoxClass::Move(const PositionClass &Delta)
{
   /* Method data declaration      */

   /* Method code declaration      */

   // Move the box
   Right  += Delta.GetX();
   Left   += Delta.GetX();
   Bottom += Delta.GetY();
   Top    += Delta.GetY();
}

/*****************************************************************************/
/*  End  Method : Move                                                       */
/*****************************************************************************/

/*****************************************************************************/
/*  Method      : IsInside                                                   */
/*****************************************************************************/
/*                                                                           */
/*  Function    : Check if given point or box is inside this box             */
/*                                                                           */
/*  Type        : Public                                                     */
/*                                                                           */
/*  Input Para  : x, y: Coordinates of point to test for beeing inside       */
/*                Box:  Box to test for beeing inside this box               */
/*                                                                           */
/*  Output Para : None                                                       */
/*                                                                           */
/*  Author      : I. Oesch                                                   */
/*                                                                           */
/*  History     : 05.12.1999  IO  Created                                    */
/*                                                                           */
/*****************************************************************************/
int BoundingBoxClass::IsInside(const PositionClass &Pos) const
{
   /* Method data declaration      */

   /* Method code declaration      */

   // Check if point is inside the box
   return (   (Pos.GetX() <= Right)
           && (Pos.GetX() >= Left)
           && (Pos.GetY() <= Bottom)
           && (Pos.GetY() >= Top) );
}

int BoundingBoxClass::IsInside(int x, int y) const
{
   /* Method data declaration      */

   /* Method code declaration      */

   // Check if point is inside the box
   return (   (x <= Right)
           && (x >= Left)
           && (y <= Bottom)
           && (y >= Top) );
}


int BoundingBoxClass::IsInside(const BoundingBoxClass &Box) const
{
   /* Method data declaration      */

   /* Method code declaration      */

   // Check if box is inside our box
   return (   (Box.Right <= Right)
           && (Box.Left >= Left)
           && (Box.Bottom <= Bottom)
           && (Box.Top >= Top) ); 
}

/*****************************************************************************/
/*  End  Method : IsInside                                                   */
/*****************************************************************************/

/*****************************************************************************/
/*  Method      : IsOverlapping                                              */
/*****************************************************************************/
/*                                                                           */
/*  Function    : Check if given box and our box are overlapping             */
/*                                                                           */
/*  Type        : Public                                                     */
/*                                                                           */
/*  Input Para  : Box:  Box to test for overlapping with                     */
/*                                                                           */
/*  Output Para : None                                                       */
/*                                                                           */
/*  Author      : I. Oesch                                                   */
/*                                                                           */
/*  History     : 05.12.1999  IO  Created                                    */
/*                                                                           */
/*****************************************************************************/
int BoundingBoxClass::IsOverlapping(const BoundingBoxClass &Box) const
{
   /* Method data declaration      */

   /* Method code declaration      */

   // Check if anypoint of box is in our box
   // or any point of our box is inside box

   // I think this method may be optimized quit a bit...
   return (   IsInside(Box.Right, Box.Top)
           || IsInside(Box.Right, Box.Bottom)
           || IsInside(Box.Left,  Box.Top)
           || IsInside(Box.Left,  Box.Bottom)
           || Box.IsInside(Right, Top)
           || Box.IsInside(Right, Bottom)
           || Box.IsInside(Left,  Top)
           || Box.IsInside(Left,  Bottom));
}

/*****************************************************************************/
/*  End  Method : IsOverlapping                                              */
/*****************************************************************************/

/*****************************************************************************/
/*  End Class   : BoundingBoxClass                                           */
/*****************************************************************************/


