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
#include "gesturetracker.hpp"

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
GestureTracker::GestureTracker()
{
   /* Method data declaration      */

   /* Method code declaration      */
   CurrentDistance = 0;
   LastDistance = 0;
   DeltaTLastDistance = 0;
   DeltaTCurrentDistance = 0;
}

/*****************************************************************************/
/*  End  Method : BoundingBoxClass                                           */
/*****************************************************************************/

/*****************************************************************************/
/*  Method      : StartTracking                                              */
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
void GestureTracker::StartTracking(QPoint Position, ulong Timestamp)
{
   /* Method data declaration      */

   /* Method code declaration      */

   GestureTrackerLastPosition =  Position;
   GestureTrackerLastPositionTimeStamp = Timestamp;
   GestureTrackeStartPosition =  Position;
   GestureTrackerStartPositionTimeStamp = Timestamp;
   GestureTrackerAccumulatedSpeed = QPoint(0,0);
   GestureTrackerAccumulatedSquaredSpeed =  QPoint(0,0);
   CurrentDistance = 0;
   LastDistance = 0;
   DeltaTLastDistance = 0;
   DeltaTCurrentDistance = 0;

}

/*****************************************************************************/
/*  End  Method : StartTracking                                              */
/*****************************************************************************/

/*****************************************************************************/
/*  Method      : Trackmovement                                              */
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
void GestureTracker::Trackmovement(QPoint Position, ulong Timestamp)
{
   /* Method data declaration      */
   QPoint GestureMovement = Position - GestureTrackerLastPosition;
   ulong  GestureTime = Timestamp - GestureTrackerLastPositionTimeStamp;

   /* Method code declaration      */

   LastDistance = CurrentDistance;
   DeltaTLastDistance = DeltaTCurrentDistance;
   DeltaTCurrentDistance = GestureTime;
   CurrentDistance = GestureMovement.manhattanLength();

   if (GestureTime > 0) {
      QPointF GestureSpeed = QPointF(GestureMovement) / GestureTime;
      GestureTrackerLastPosition =  Position;
      GestureTrackerLastPositionTimeStamp = Timestamp;

      GestureTrackerAccumulatedSpeed += GestureSpeed;
      GestureTrackerAccumulatedSquaredSpeed += QPointF(GestureSpeed.x() * GestureSpeed.x(), GestureSpeed.y()*GestureSpeed.y() );
   }
}

/*****************************************************************************/
/*  End  Method : Trackmovement                                              */
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

float GestureTracker::GetCurrentSpeed()
{
   /* Method data declaration      */

   /* Method code declaration      */

   if ( (DeltaTLastDistance + DeltaTCurrentDistance) > 0) {
        return ((LastDistance + CurrentDistance) / static_cast<float>(DeltaTLastDistance + DeltaTCurrentDistance));
   }
   return 0;
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
bool GestureTracker::IsFastShaking() {
   /* Method data declaration      */

   /* Method code declaration      */

   return (GestureTrackerAccumulatedSpeed.manhattanLength()*10) < (GestureTrackerAccumulatedSquaredSpeed.manhattanLength());
}

/*****************************************************************************/
/*  End  Method : IsInside                                                   */
/*****************************************************************************/

/*****************************************************************************/
/*  End Class   : BoundingBoxClass                                           */
/*****************************************************************************/





