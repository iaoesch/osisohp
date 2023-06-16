#ifndef SETTINGS_CLASS_HPP
#define SETTINGS_CLASS_HPP
/*****************************************************************************/
/*  Header     : Settings                               Version 1.0  */
/*****************************************************************************/
/*                                                                           */
/*  Function   : This class holds the settings      */
/*                     */
/*                                                                           */
/*  Methodes   : BoundingBoxClass()                                          */
/*              ~BoundingBoxClass()                                          */
/*                                                                           */
/*  Author     : I. Oesch                                                    */
/*                                                                           */
/*  History    : 05.12.1999  IO Created                                      */
/*                                                                           */
/*  File       : Settiings.hpp                                                     */
/*                                                                           */
/*****************************************************************************/
/* MagicSoft                                                                 */
/*****************************************************************************/

/* imports */

/* Class constant declaration  */

/* Class Type declaration      */

/* Class data declaration      */

/* Class definition            */
class Settings {
public:
   double Touchscaling = 4.0;
   double DirectSelectTimeout = 10.0;
   double CopyTimeout = 500;
   double GestureTimeout = 800;
   double SelectTimeout = 500;
   double PostItTimeout = 1000;
   double PointerHoldon = 250;
   int    EraserSize = 2;
   int    SpongeSize = 15;

};
/*****************************************************************************/
/*  End Header  : Settings                                           */
/*****************************************************************************/
#endif

