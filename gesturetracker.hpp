#ifndef GESTURETRACKER_HPP
#define GESTURETRACKER_HPP
/*****************************************************************************/
/*  Header     : GestureTrackerClass                            Version 1.0  */
/*****************************************************************************/
/*                                                                           */
/*  Function   : This class keeps track of gestures                          */
/*                                                                           */
/*  Methodes   : GestureTrackerClass()                                       */
/*               StartTracking()                                             */
/*               Trackmovement()                                             */
/*               GetCurrentSpeed()                                           */
/*               IsFastShaking()                                             */
/*                                                                           */
/*  Author     : I. Oesch                                                    */
/*                                                                           */
/*  History    : 05.11.2020  IO Created                                      */
/*                                                                           */
/*  File       : gesturetracker.hpp                                          */
/*                                                                           */
/*****************************************************************************/
/* MagicSoft                                                                 */
/*****************************************************************************/

/* imports */
#include <QPoint>
#include <QPointF>


/* Class constant declaration  */

/* Class Type declaration      */

/* Class data declaration      */

/* Class definition            */
class GestureTrackerClass {

   // Data
   private:

   QPointF  StartPosition;
   ulong   StartPositionTimeStamp;
   QPointF  LastPosition;
   ulong   LastPositionTimeStamp;
   QPointF AccumulatedSpeed;
   QPointF AccumulatedSquaredSpeed;

   ulong CurrentDistance;
   ulong LastDistance;
   ulong DeltaTimeLastDistance;
   ulong DeltaTimeCurrentDistance;

   // Methods
   public:
   void StartTracking(QPointF Position, ulong Timestamp);
   void Trackmovement(QPointF Position, ulong Timestamp);
   float GetCurrentSpeed();
   bool IsFastShaking();
   GestureTrackerClass();
};

/*****************************************************************************/
/*  End Header  : GestureTrackerClass                                        */
/*****************************************************************************/
#endif // GESTURETRACKER_HPP
