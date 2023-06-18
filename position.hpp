#ifndef POSITION_CLASS_HPP
#define POSITION_CLASS_HPP
/*****************************************************************************/
/*  Header     : PositionClass                                  Version 1.0  */
/*****************************************************************************/
/*                                                                           */
/*  Function   : This object holds a position                                */
/*                                                                           */
/*  Methodes   : PositionClass()                                             */
/*              ~PositionClass()                                             */
/*               operator =  ()                                              */
/*               operator == ()                                              */
/*               operator != ()                                              */
/*               operator += ()                                              */
/*               operator -= ()                                              */
/*               operator + ()                                               */
/*               operator - ()                                               */
/*               GetX ()                                                     */
/*               GetY ()                                                     */
/*               SetX ()                                                     */
/*               SetY ()                                                     */
/*               SetXY ()                                                    */
/*               friend operator << ()                                       */
/*               friend operator >> ()                                       */
/*                                                                           */
/*  Author     : I. Oesch                                                    */
/*                                                                           */
/*  History    : 29.10.1999  IO Created                                      */
/*                                                                           */
/*  File       : position.hpp                                                   */
/*                                                                           */
/*****************************************************************************/
/* MagicSoft                                                                 */
/*****************************************************************************/

/* imports */
#include <iostream>

/* Class constant declaration  */

/* Class Type declaration      */
using std::ostream;
using std::istream;

/* Class data declaration      */

/* Class definition            */
class PositionClass
{
   // Data
   int X;
   int Y;

   friend ostream& operator << (ostream &Stream, const PositionClass& Pos);
   friend istream& operator >> (istream &Stream, PositionClass& Pos);
   // Methods
   public:
            PositionClass(int newX, int newY) : X(newX), Y(newY) {}
            PositionClass(double newX, double newY) : X(static_cast<int>(newX+0.5)), Y(static_cast<int>(newY+0.5)) {}
            PositionClass(const PositionClass &Pos) : X(Pos.X), Y(Pos.Y)  {}
           ~PositionClass(void){}

   PositionClass& operator =  (const PositionClass &Pos)  { Y  = Pos.Y; X  = Pos.X; return *this;}
   PositionClass& operator += (const PositionClass &Pos)  { Y += Pos.Y; X += Pos.X; return *this;}
   PositionClass& operator -= (const PositionClass &Pos)  { Y -= Pos.Y; X -= Pos.X; return *this;}
   int            operator == (const PositionClass &Pos)  const { return ((Y == Pos.Y) && (X == Pos.X));}
   int            operator != (const PositionClass &Pos)  const { return !(*this == Pos);}
   PositionClass  operator +  (const PositionClass &Pos2) const { return PositionClass(X + Pos2.X, Y + Pos2.Y);}
   PositionClass  operator -  (const PositionClass &Pos2) const { return PositionClass(X - Pos2.X, Y - Pos2.Y);}
   PositionClass  operator -  () const { return PositionClass(-X, -Y);}

   int      GetX(void) const {return X;}
   int      GetY(void) const {return Y;}
   void     SetX(int _nX) {X = _nX;}
   void     SetY(int _nY) {Y = _nY;}
   void     SetXY(int _nX, int _nY) {X = _nX; Y = _nY;}
};

//ostream &operator << (ostream &Stream, const PositionClass& Pos);
//istream &operator >> (istream &Stream, PositionClass& Pos);

/*****************************************************************************/
/*  End Header  : PositionClass                                              */
/*****************************************************************************/
#endif

