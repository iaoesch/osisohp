/*****************************************************************************/
/*  Class      : PositionClass                                  Version 1.0  */
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
/*  History    : 02.11.1999  IO Created                                      */
/*                                                                           */
/*  File       : position.cpp                                                */
/*                                                                           */
/*****************************************************************************/
/* MagicSoft                                                                 */
/*****************************************************************************/

/* imports */
#include "position.hpp"
#include <iostream>

/* Class constant declaration  */

/* Class Type declaration      */

/* Class data declaration      */

/* Class procedure declaration */

/*****************************************************************************/
/*  Function    : operator <<                                                */
/*****************************************************************************/
/*                                                                           */
/*  Function    : Writes a Positionobject to a stream                        */
/*                                                                           */
/*  Type        : Friend to PositionClass                                    */
/*                                                                           */
/*  Input Para  : Stream Stream to write to                                  */
/*                Pos    Positionobject to write out                         */
/*                                                                           */
/*  Output Para : Stremaobject (for chaining of <<)                          */
/*                                                                           */
/*  Author      : I. Oesch                                                   */
/*                                                                           */
/*  History     : 02.11.1999  IO  Created                                    */
/*                                                                           */
/*****************************************************************************/
ostream& operator << (ostream &Stream, const PositionClass& Pos)
{
   /* Method data declaration      */

   /* Method code declaration      */
   Stream << '(' << Pos.X << ';' << Pos.Y << ')';
   return Stream;
}
/*****************************************************************************/
/*  End  Function : operator <<                                              */
/*****************************************************************************/

/*****************************************************************************/
/*  Function    : operator >>                                                */
/*****************************************************************************/
/*                                                                           */
/*  Function    : Reads a Positionobject from a stream                       */
/*                                                                           */
/*  Type        : Friend to PositionClass                                    */
/*                                                                           */
/*  Input Para  : Stream Stream to read from                                 */
/*                Pos    Positionobject to read into                         */
/*                                                                           */
/*  Output Para : Streamobject (for chaining of >>)                          */
/*                                                                           */
/*  Author      : I. Oesch                                                   */
/*                                                                           */
/*  History     : 02.11.1999  IO  Created                                    */
/*                                                                           */
/*****************************************************************************/
istream& operator >> (istream &Stream, PositionClass& Pos)
{
   /* Method data declaration      */
   int Code;

   /* Method code declaration      */

   // Read our datas from a stream
   // Data must be of format (xxxx;yyyy)

   // Check for opening '('
   Code = Stream.get();
   if (Code == '(') {

      // Read X position and check for separator
      Stream >> Pos.X;
      Code = Stream.get();
      if (Code == ';') {

         // Read Y position and check for ')'
         Stream >> Pos.Y;
         Code = Stream.get();
         if (Code != ')') {
            std::cerr << "Fileerror PositionClass, missing ')' " << std::endl;
         }
      } else {
         std::cerr << "Fileerror PositionClass, missing ';' " << std::endl;
      }
   } else {
      std::cerr << "Fileerror PositionClass, missing '(' " << std::endl;
   }
   return Stream;
}
/*****************************************************************************/
/*  End  Function : operator >>                                              */
/*****************************************************************************/

/*****************************************************************************/
/*  End Class   : PositionClass                                              */
/*****************************************************************************/


