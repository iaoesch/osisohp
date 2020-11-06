#ifndef GESTURETRACKER_HPP
#define GESTURETRACKER_HPP
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
#include <QPoint>
#include <QPointF>


/* Class constant declaration  */

/* Class Type declaration      */

/* Class data declaration      */

/* Class definition            */
class GestureTracker {

   // Data
   private:

   QPoint  GestureTrackeStartPosition;
   ulong   GestureTrackerStartPositionTimeStamp;
   QPoint  GestureTrackerLastPosition;
   ulong   GestureTrackerLastPositionTimeStamp;
   QPointF GestureTrackerAccumulatedSpeed;
   QPointF GestureTrackerAccumulatedSquaredSpeed;

   ulong CurrentDistance;
   ulong LastDistance;
   ulong DeltaTLastDistance;
   ulong DeltaTCurrentDistance;


   // Methods
   public:
   void StartTracking(QPoint Position, ulong Timestamp);
   void Trackmovement(QPoint Position, ulong Timestamp);
   float GetCurrentSpeed();
   bool IsFastShaking();
   GestureTracker();
};


/*****************************************************************************/
/*  End Header  : BoundingBoxClass                                           */
/*****************************************************************************/
#endif // GESTURETRACKER_HPP
