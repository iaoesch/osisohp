/*****************************************************************************/
/*  Class      : GestureTrackerClass                            Version 1.0  */
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
/*  File       : gesturetracker.cpp                                          */
/*                                                                           */
/*****************************************************************************/
/* MagicSoft                                                                 */
/*****************************************************************************/

/* imports */
#include "gesturetracker.hpp"
#include "Settings.hpp"
#include <iostream>

/* Class constant declaration  */

/* Class Type declaration      */
using namespace std::chrono_literals;

/* Class data declaration      */

/* Class procedure declaration */

/*****************************************************************************/
/*  Method      : GestureTrackerClass                                        */
/*****************************************************************************/
/*                                                                           */
/*  Function    : Constructor for the GestureTracker Class. initializes a    */
/*                tracker                                                    */
/*                                                                           */
/*  Type        : Constructor                                                */
/*                                                                           */
/*  Input Para  : None                                                       */
/*                                                                           */
/*  Output Para : None                                                       */
/*                                                                           */
/*  Author      : I. Oesch                                                   */
/*                                                                           */
/*  History     : 05.11.2020  IO  Created                                    */
/*                                                                           */
/*****************************************************************************/
GestureTrackerClass::GestureTrackerClass(SettingClass &TheSettings)
   : Settings(TheSettings)
{
   /* Method data declaration      */

   /* Method code declaration      */
   CurrentDistance = 0;
   LastDistance = 0;
   DeltaTimeLastDistance = 0ms;
   DeltaTimeCurrentDistance = 0ms;

   MyTimer.callOnTimeout(this, &GestureTrackerClass::Timeout);
   MyTimer.setSingleShot(true);
   GestureFinished = false;

}

/*****************************************************************************/
/*  End  Method : GestureTrackerClass                                        */
/*****************************************************************************/

/*****************************************************************************/
/*  Method      : StartTracking                                              */
/*****************************************************************************/
/*                                                                           */
/*  Function    : Starts tracking of movement for detecting gestures         */
/*                                                                           */
/*  Type        : Public                                                     */
/*                                                                           */
/*  Input Para  : Position: Starting position of possible gesture            */
/*                Timestamp: Starting time of possible gesture               */
/*                                                                           */
/*  Output Para : None                                                       */
/*                                                                           */
/*  Author      : I. Oesch                                                   */
/*                                                                           */
/*  History     : 05.11.2020  IO  Created                                    */
/*                                                                           */
/*****************************************************************************/
void GestureTrackerClass::StartTracking(QPointF Position, std::chrono::milliseconds Timestamp)
{
   StartNewGesture(Position, Timestamp);
}

void GestureTrackerClass::StartNewGesture(QPointF Position, std::chrono::milliseconds Timestamp)
{
   /* Method data declaration      */

   /* Method code declaration      */

   /* Prepare for gesture detection */
   CurrentGesture.Clear();
   LastPosition =  Position;
   LastSpeed = QPointF(0, 0);
   LastAcceleration = QPointF(0, 0);
   LastPositionTimeStamp = Timestamp;
   CurrentGesture.StartPosition =  Position;
   CurrentGesture.StartPositionTimeStamp = Timestamp;
   CurrentDistance = 0;
   LastDistance = 0;
   DeltaTimeLastDistance = 0ms;
   DeltaTimeCurrentDistance = 0ms;
   GestureFinished = false;

   MyTimer.start(std::chrono::milliseconds(static_cast<int>(Settings.GestureTrackerTimeout)));

}
/*****************************************************************************/
/*  End  Method : StartTracking                                              */
/*****************************************************************************/

/*****************************************************************************/
/*  Method      : Trackmovement                                              */
/*****************************************************************************/
/*                                                                           */
/*  Function    : Keeps track of movement to detect gestures                 */
/*                                                                           */
/*  Type        : Public                                                     */
/*                                                                           */
/*  Input Para  : Position: Current position of possible gesture             */
/*                Timestamp: Current time of possible gesture                */
/*                                                                           */
/*  Output Para : None                                                       */
/*                                                                           */
/*  Author      : I. Oesch                                                   */
/*                                                                           */
/*  History     : 05.11.2020  IO  Created                                    */
/*                                                                           */
/*****************************************************************************/
void GestureTrackerClass::Trackmovement(QPointF Position, std::chrono::milliseconds Timestamp)
{

   if (GestureFinished) {
      StartNewGesture(Position,  Timestamp);
      return;
   }

   /* Method data declaration      */
   QPointF MovementSinceLastTracking = Position - LastPosition;
   std::chrono::milliseconds  TimeSinceLastTracking = Timestamp - LastPositionTimeStamp;

   /* Method code declaration      */

   /* Keep track of old Move */
   LastDistance = CurrentDistance;
   DeltaTimeLastDistance = DeltaTimeCurrentDistance;

   /* Store current move */
   DeltaTimeCurrentDistance = TimeSinceLastTracking;
   CurrentDistance = static_cast<ulong>(MovementSinceLastTracking.manhattanLength() + 0.5);

   CurrentGesture.AccumulatedLength += CurrentDistance;
   CurrentGesture.AccumulatedTime += TimeSinceLastTracking;

   /* Discard moves with no time inbetween */
   if (TimeSinceLastTracking > 0ms) {

      /* Calculate speed of last move */
      QPointF GestureSpeed = MovementSinceLastTracking / static_cast<double>(TimeSinceLastTracking.count());

      /* Store current position for next round */
      LastPosition =  Position;
      LastPositionTimeStamp = Timestamp;

      /* Calculate acceleration of last move */
      QPointF SpeedChangeSinceLastTracking = GestureSpeed - LastSpeed;
      QPointF GestureAcceleration = SpeedChangeSinceLastTracking / static_cast<double>(TimeSinceLastTracking.count());
      LastSpeed = GestureSpeed;

      /* Calculate Ruck of last move */
      QPointF AccelerationChangeSinceLastTracking = GestureAcceleration - LastAcceleration;
      QPointF GestureRuck = AccelerationChangeSinceLastTracking / static_cast<double>(TimeSinceLastTracking.count());
      LastAcceleration = GestureAcceleration;

      /* Accumulate speed and squared speed */
      /* needed to detect shaking */
      CurrentGesture.AccumulatedSpeed += GestureSpeed;
      CurrentGesture.AccumulatedSquaredSpeed += QPointF(GestureSpeed.x() * GestureSpeed.x(), GestureSpeed.y()*GestureSpeed.y() );

      /* Accumulate acceleration and absolute acceleration */
      /* needed to detect shaking */
      CurrentGesture.AccumulatedAcceleration += GestureAcceleration;
      CurrentGesture.AccumulatedAbsolutesOfAcceleration += QPointF(abs(GestureSpeed.x()), abs(GestureSpeed.y()));

      /* Accumulate ruck and absolute ruck */
      /* needed to detect shaking */
      CurrentGesture.AccumulatedRuck += GestureRuck;
      CurrentGesture.AccumulatedAbsolutesOfRuck += QPointF(abs(GestureRuck.x()), abs(GestureRuck.y()));
   }
   MyTimer.start(std::chrono::milliseconds(static_cast<int>(Settings.GestureTrackerTimeout)));

}
/*****************************************************************************/
/*  End  Method : Trackmovement                                              */
/*****************************************************************************/
template<class T>
void PrintPoint(const char *Text, const T &Point) {
   std::cout << Text << ": (" << Point.x() << "; " << Point.y() << ")" << std::endl;
}

void GestureTrackerClass::Timeout()
{

   GestureFinished = true;
   CurrentGesture.EndPosition = LastPosition;
   CurrentGesture.EndPositionTimeStamp = LastPositionTimeStamp;
   LastGesture = CurrentGesture;
   auto t = LastGesture.AccumulatedTime.count() / 1000.0;
   PrintPoint("Gesture abs Ruck:", LastGesture.AccumulatedAbsolutesOfRuck/t);
   PrintPoint("Gesture Ruck:", LastGesture.AccumulatedRuck/t);
   PrintPoint("Gesture abs Acc:", LastGesture.AccumulatedAbsolutesOfAcceleration/t);
   PrintPoint("Gesture Acc:", LastGesture.AccumulatedAcceleration/t);
   PrintPoint("Gesture Speed:", LastGesture.AccumulatedSpeed/t);
   PrintPoint("Gesture Squared Speed:", LastGesture.AccumulatedSquaredSpeed/t);
   std::cout << "Length: " << LastGesture.AccumulatedLength << std::endl;
   std::cout << "Time: " << LastGesture.AccumulatedTime.count() << std::endl;
}


/*****************************************************************************/
/*  Method      : GetCurrentSpeed                                            */
/*****************************************************************************/
/*                                                                           */
/*  Function    : Returns the speed of the last two moves                    */
/*                                                                           */
/*  Type        : Public                                                     */
/*                                                                           */
/*  Input Para  : None                                                       */
/*                                                                           */
/*  Output Para : Speed of the last two moves, 0 if not calculable           */
/*                                                                           */
/*  Author      : I. Oesch                                                   */
/*                                                                           */
/*  History     : 05.11.2020  IO  Created                                    */
/*                                                                           */
/*****************************************************************************/
float GestureTrackerClass::GetCurrentSpeed()
{
   /* Method data declaration      */

   /* Method code declaration      */

   if ( (DeltaTimeLastDistance + DeltaTimeCurrentDistance) > 0ms) {
        return (static_cast<float>(LastDistance + CurrentDistance) / static_cast<float>((DeltaTimeLastDistance + DeltaTimeCurrentDistance).count()));
   }
   return 0;
}
/*****************************************************************************/
/*  End  Method : GetCurrentSpeed                                            */
/*****************************************************************************/
bool GestureTrackerClass::IsThrowing()
{
   /* Method data declaration      */

   /* Method code declaration      */
   return GetCurrentSpeed() > Settings.ThrowingSpeedLimit;
}
/*****************************************************************************/
/*  Method      : IsFastShaking                                              */
/*****************************************************************************/
/*                                                                           */
/*  Function    : Returns true if current movement is a fast shaking         */
/*                                                                           */
/*  Type        : Public                                                     */
/*                                                                           */
/*  Input Para  : None                                                       */
/*                                                                           */
/*  Output Para : True on fast shaking, False otherwise                      */
/*                                                                           */
/*  Author      : I. Oesch                                                   */
/*                                                                           */
/*  History     : 05.11.2020  IO  Created                                    */
/*                                                                           */
/*****************************************************************************/
bool GestureTrackerClass::IsFastShaking() {
   /* Method data declaration      */

   /* Method code declaration      */

   /* On fast shaking speed cancels out (positive and negative values) */
   /* Squared speed summs up */
   return (CurrentGesture.AccumulatedSpeed.manhattanLength()*10) < (CurrentGesture.AccumulatedSquaredSpeed.manhattanLength());
}
/*****************************************************************************/
/*  End  Method : IsFastShaking                                              */
/*****************************************************************************/

/*****************************************************************************/
/*  End Class   : GestureTrackerClass                                        */
/*****************************************************************************/






void GestureTrackerClass::GestureInfo::Clear()
{
   AccumulatedAbsolutesOfAcceleration = QPointF(0, 0);
   AccumulatedAcceleration = QPointF(0, 0);

   AccumulatedAbsolutesOfRuck = QPointF(0, 0);
   AccumulatedRuck = QPointF(0, 0);

   AccumulatedLength = 0;
   AccumulatedTime = 0ms;

   AccumulatedSpeed = QPointF(0, 0);
   AccumulatedSquaredSpeed = QPointF(0, 0);

   StartPosition = QPointF(0, 0);
   StartPositionTimeStamp = 0ms;
   EndPosition = QPointF(0, 0);
   EndPositionTimeStamp = 0ms;

}
