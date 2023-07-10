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
#include <QObject>
#include <QPoint>
#include <QPointF>
#include <QTimer>


/* Class constant declaration  */

/* Class Type declaration      */
class SettingClass;

/* Class data declaration      */

/* Class definition            */
class GestureTrackerClass {

   Q_OBJECT

   // Data
   private:
   QTimer   MyTimer;

   SettingClass &Settings;

   bool GestureFinished;

   QPointF StartPosition;
   ulong   StartPositionTimeStamp;
   QPointF LastPosition;
   QPointF LastSpeed;
   ulong   LastPositionTimeStamp;
   QPointF AccumulatedSpeed;
   QPointF AccumulatedSquaredSpeed;

   QPointF AccumulatedAcceleration;
   QPointF AccumulatedAbsolutesOfAcceleration;

   ulong CurrentDistance;
   ulong LastDistance;
   ulong DeltaTimeLastDistance;
   ulong DeltaTimeCurrentDistance;

   // Methods
   void StartNewGesture(QPointF Position, ulong Timestamp);
public:
   void StartTracking(QPointF Position, ulong Timestamp);
   void Trackmovement(QPointF Position, ulong Timestamp);
   float GetCurrentSpeed();
   bool IsFastShaking();
   GestureTrackerClass(SettingClass &TheSettings);

private slots:
   void Timeout();
};

/*****************************************************************************/
/*  End Header  : GestureTrackerClass                                        */
/*****************************************************************************/
#endif // GESTURETRACKER_HPP
