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
class GestureTrackerClass : public QObject {

   Q_OBJECT

   // Data
   public:

   struct GestureInfo {
      double  AccumulatedLength;
      std::chrono::milliseconds  AccumulatedTime;

      QPointF AccumulatedSpeed;
      QPointF AccumulatedSquaredSpeed;

      QPointF AccumulatedAcceleration;
      QPointF AccumulatedAbsolutesOfAcceleration;

      QPointF AccumulatedRuck;
      QPointF AccumulatedAbsolutesOfRuck;

      QPointF StartPosition;
      std::chrono::milliseconds   StartPositionTimeStamp;
      QPointF EndPosition;
      std::chrono::milliseconds   EndPositionTimeStamp;

      void Clear();
   };

   private:
   QTimer   MyTimer;

   SettingClass &Settings;

   bool GestureFinished;
   GestureInfo CurrentGesture;
   GestureInfo LastGesture;

   QPointF LastPosition;
   QPointF LastSpeed;
   QPointF LastAcceleration;
   std::chrono::milliseconds   LastPositionTimeStamp;

   ulong CurrentDistance;
   ulong LastDistance;
   std::chrono::milliseconds DeltaTimeLastDistance;
   std::chrono::milliseconds DeltaTimeCurrentDistance;

   // Methods
   void StartNewGesture(QPointF Position, std::chrono::milliseconds Timestamp);
public:
   void StartTracking(QPointF Position, std::chrono::milliseconds Timestamp);
   void Trackmovement(QPointF Position, std::chrono::milliseconds Timestamp);
   float GetCurrentSpeed();
   bool IsFastShaking();
   GestureTrackerClass(SettingClass &TheSettings);

   bool IsThrowing();
private slots:
   void Timeout();
};

/*****************************************************************************/
/*  End Header  : GestureTrackerClass                                        */
/*****************************************************************************/
#endif // GESTURETRACKER_HPP
