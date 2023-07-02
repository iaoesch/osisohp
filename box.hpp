#ifndef BOUNDING_BOX_CLASS_HPP
#define BOUNDING_BOX_CLASS_HPP
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
#include <QDataStream>


#include"position.hpp"

/* Class constant declaration  */

/* Class Type declaration      */

/* Class data declaration      */

/* Class definition            */
class BoundingBoxClass
{
   // Data
   private:
   int Top, Bottom;
   int Left, Right;

   // Methods
   public:
            BoundingBoxClass(void): Top(32767), Bottom(-32767), Left(32767), Right(-32767) {}
            BoundingBoxClass(const PositionClass &Pos): Top(Pos.GetY()), Bottom(Pos.GetY()), Left(Pos.GetX()), Right(Pos.GetX()) {}
            BoundingBoxClass(const PositionClass &Pos1, const PositionClass &Pos2);
            BoundingBoxClass(int x1, int y1, int x2, int y2);
            BoundingBoxClass(const BoundingBoxClass& Src) : Top(Src.Top), Bottom(Src.Bottom), Left(Src.Left), Right(Src.Right) {}
      const BoundingBoxClass &operator = (const BoundingBoxClass& Src) {Top = Src.Top; Bottom = Src.Bottom; Left = Src.Left; Right = Src.Right; return *this;}
      void  Clear() {Top = 32767; Bottom = -32767; Left = 32767; Right = -32767;}
      void  AddPoint(const PositionClass &Pos);
      void  AddBox(const BoundingBoxClass &Box);
      int   IsInside(const PositionClass &Pos) const;
      int   IsInside(int x, int y) const;
      int   IsInside(const BoundingBoxClass &Box) const;
      int   IsOverlapping(const BoundingBoxClass &Box) const;
      void  Move(const PositionClass &Delta);
      int   GetTop(void)    const {return Top;}
      int   GetBottom(void) const {return Bottom;}
      int   GetLeft(void)   const {return Left;}
      int   GetRight(void)  const {return Right;}
      int   IsValid(void)   const {return (Left <= Right);} 
      QRect QRectangle() const { return QRect(Left, Top, Right-Left, Bottom - Top);}
      friend QDataStream& operator << (QDataStream &Stream, const BoundingBoxClass &Box);
      friend QDataStream& operator >> (QDataStream &Stream, BoundingBoxClass &Box);
};

inline QDataStream& operator << (QDataStream &Stream, const BoundingBoxClass &Box)
{
   Stream << Box.Top << Box.Left << Box.Right << Box.Bottom;
   return Stream;
}

inline QDataStream& operator >> (QDataStream &Stream, BoundingBoxClass &Box)
{
   Stream >> Box.Top >> Box.Left >> Box.Right >> Box.Bottom;
   return Stream;
}

/*****************************************************************************/
/*  End Header  : BoundingBoxClass                                           */
/*****************************************************************************/
#endif

