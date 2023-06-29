#include "animatedcursorclass.h"

#include <QPainter>
#include <QWidget>



AnimatedCursorClass::AnimatedCursorClass(size_t Width, size_t Height, size_t NumberOfFramesToPlay, QString FileName, int Hot_x, int Hot_y)
   : AnimatedCursorClass( Width,  Height,  NumberOfFramesToPlay, QPixmap(FileName).scaled(Width, Height),  Hot_x,  Hot_y, QPixmap())
{

}

AnimatedCursorClass::AnimatedCursorClass(size_t Width, size_t Height, size_t NumberOfFramesToPlay, QPixmap IconPicture, int Hot_x, int Hot_y)
   : AnimatedCursorClass( Width,  Height,  NumberOfFramesToPlay, IconPicture,  Hot_x,  Hot_y, QPixmap())
{

}

AnimatedCursorClass::AnimatedCursorClass(size_t Width, size_t Height, size_t NumberOfFramesToPlay, QString FileName, int Hot_x, int Hot_y, QString FileNameInitialPointer, int Hot_x_initial, int Hot_y_initial)
   : AnimatedCursorClass( Width,  Height,  NumberOfFramesToPlay, QPixmap(FileName).scaled(Width, Height), Hot_x, Hot_y, QPixmap(FileNameInitialPointer).scaled(Width, Height),  Hot_x_initial,  Hot_y_initial)
{

}


AnimatedCursorClass::AnimatedCursorClass(size_t Width, size_t Height, size_t NumberOfFramesToPlay, QPixmap IconPicture, int Hot_x, int Hot_y, QPixmap ImageInitialPointer, int Hot_x_initial, int Hot_y_initial)
   : HotSpotX(Hot_x), HotSpotY(Hot_y), HotSpotInitialX(Hot_x_initial), HotSpotInitialY(Hot_y_initial)
{
   this->NumberOfFramesToPlay = NumberOfFramesToPlay;
   AnimatedPointerCursor.resize(NumberOfFramesToPlay);
   //QPixmap BasePicture = QPixmap(FileName).scaled(Width, Height);
   //QPixmap IconPicture = QPixmap(FileName).scaled(Width, Height);

   BasePicture = QPixmap(32, 32);
   BasePicture.fill(QColor(0,0,0,0));
   QPainter IconPainter(&BasePicture);
   IconPainter.drawPixmap(0, 0, IconPicture);

   if (ImageInitialPointer.isNull()) {
      InitialPicture = BasePicture;
   } else {
      InitialPicture = QPixmap(32, 32);
      InitialPicture.fill(QColor(0,0,0,0));
      QPainter IconPainter2(&InitialPicture);
      IconPainter2.drawPixmap(0, 0, ImageInitialPointer);
   }

   for (size_t i = 0; i < NumberOfFramesToPlay; i++) {
      int TimerPosX = 16;
      int TimerPosY = 16;
      int TimerWidth = 10;
      int TimerHeight = 10;
      AnimatedPointerCursor[i] = BasePicture;
      QPainter IconPainter(&AnimatedPointerCursor[i]);
      IconPainter.setPen(Qt::white);
      IconPainter.setBrush(QBrush(Qt::white));
      IconPainter.drawEllipse(TimerPosX, TimerPosY, TimerWidth, TimerHeight);
      IconPainter.setPen(Qt::black);
      IconPainter.setBrush(QBrush(Qt::black));
      int StartAngle = 0*16;
      int SpanAngle = 16*360*i/NumberOfFramesToPlay;

      IconPainter.drawPie(TimerPosX+1, TimerPosY+1, TimerWidth-2, TimerHeight-2, StartAngle, SpanAngle);
      //IconPainter.drawEllipse(6,6,8,8);
   }
   CurrentAnimatedPointerShape = 0;
}





void CursorManager::SetCursor(CursorType Cursor, std::chrono::milliseconds Duration, std::chrono::milliseconds StartupDelay)
{
   if (!TestMode) {
   AnimatedPointerTimer.stop();
   CurrentAnimatedCursor = nullptr;
   DelaytimeLeft = StartupDelay;
   TimeLeft      = Duration;
   TotalDuration = Duration;
   TotalDelay    = StartupDelay;

   switch(Cursor) {
      case CursorManager::TimedPointerForCopying:
         //Parent->setCursor(QCursor(Qt::DragCopyCursor));
         CurrentAnimatedCursor = &AnimatedTimedPointerForCopying;
         StartAnimation();
         break;
      case CursorManager::TimedPointerForCreatingPostit:
         //Parent->setCursor(QCursor(Qt::DragCopyCursor));
         CurrentAnimatedCursor = &AnimatedTimedPointerForCreatingPostit;
         StartAnimation();
         break;
      case CursorManager::MovingMultiplePostitPointer:
         Parent->setCursor(MovingMultiplePostitPointerCursor);
         break;
      case CursorManager::StandardPointer:
         Parent->setCursor(QCursor(Qt::ArrowCursor));
         break;
      case CursorManager::TimedPointerForScrolling:
         CurrentAnimatedCursor = &AnimatedTimedPointerForScrolling;
         StartAnimation();
//         Parent->setCursor(QCursor(Qt::SizeAllCursor));
         break;
      case CursorManager::TimedPointerForCutting:
         CurrentAnimatedCursor = &AnimatedTimedPointerForCutting;
         StartAnimation();
         break;

      case CursorManager::TimedPointerForSelectingSingle:
         CurrentAnimatedCursor = &AnimatedTimedPointerForSelectingSingle;
         StartAnimation();
         break;
      case CursorManager::TimedPointerForSelectingMultiple:
         CurrentAnimatedCursor = &AnimatedTimedPointerForSelectingMultiple;
         StartAnimation();
         //Parent->setCursor(QCursor(Qt::ClosedHandCursor));
         break;
      case CursorManager::SelectPossiblePointer:
         Parent->setCursor(QCursor(Qt::PointingHandCursor));
         break;
      case CursorManager::CutPossiblePointer:
         Parent->setCursor(CutPossiblePointerCursor);
         //Parent->setCursor(QCursor(Qt::UpArrowCursor));
         break;
      case CursorManager::MovingCutoutPointer:
         Parent->setCursor(QCursor(Qt::ClosedHandCursor));
         break;
      case CursorManager::DrawingPinter:
         Parent->setCursor(DrawingPinterCursor);
         break;
      case CursorManager::GoingToFillTimer:
         CurrentAnimatedCursor = &AnimatedGoingToFillTimer;
         StartAnimation();
         break;
      case CursorManager::FillingPointer:
         Parent->setCursor(FillingPointerCursor);
         break;
      case CursorManager::ScrollingPointer:
         Parent->setCursor(QCursor(Qt::SizeAllCursor));
         break;
      case CursorManager::MovingPostitPointer:
         Parent->setCursor(QCursor(Qt::ClosedHandCursor));
         break;
      case CursorManager::TimedPointerForCopyPostit:
         CurrentAnimatedCursor = &AnimatedTimedPointerForCopyPostit;
         StartAnimation();
         break;
      case CursorManager::TimedPointerForDeletePostit:
         CurrentAnimatedCursor = &AnimatedTimedPointerForDeletePostit;
         StartAnimation();
         break;

   }
   LastAnimatedCursor = CurrentAnimatedCursor;
   }

}

void CursorManager::RestartAnimatedCursor()
{
   if (!TestMode)
 {   AnimatedPointerTimer.stop();
   CurrentAnimatedCursor = LastAnimatedCursor;
   DelaytimeLeft = TotalDelay;
   TimeLeft      = TotalDuration;
   StartAnimation();
   }
}

CursorManager::CursorManager(QWidget *p) :  QObject(p) ,Parent(p),
   AnimatedTimedPointerForScrolling(24, 24, 30, ":/images/MousPointers/all-scroll.png", 6, 0, ":/images/MousPointers/pencil.png", 3, 21),
   AnimatedTimedPointerForCutting(24, 24, 30, ":/images/MousPointers/scissors24.png", 6, 0, ":/images/MousPointers/pencil.png", 3, 21),
   AnimatedTimedPointerForCreatingPostit(24, 24, 30, ":/images/MousPointers/scissorscopy24.png", 6, 0, ":/images/MousPointers/hand1.png", 6, 0),
   AnimatedTimedPointerForCopying(24, 24, 30, ":/images/MousPointers/hand1plus.png", 6, 0, ":/images/MousPointers/hand1.png", 6, 0),
   AnimatedTimedPointerForSelectingSingle(24, 24, 30, ":/images/MousPointers/hand1.png", 6, 0, ":/images/MousPointers/left_ptr.png", 6, 0),
   AnimatedTimedPointerForSelectingMultiple(24, 24, 30, ":/images/MousPointers/hand1plus.png", 6, 0, ":/images/MousPointers/hand1.png", 6, 0),
   AnimatedTimedPointerForCopyPostit(24, 24, 30, ":/images/MousPointers/handMoveplus.png", 6, 0, ":/images/MousPointers/move.png", 6, 0),
   AnimatedTimedPointerForDeletePostit(24, 24, 30, ":/images/MousPointers/X_cursor.png", 6, 0, ":/images/MousPointers/hand1plus.png", 6, 0),
   AnimatedGoingToFillTimer(24, 24, 30, ":/images/MousPointers/bucketfill16.png", 6, 0, ":/images/MousPointers/pencil.png", 3, 21)
{
   connect(&AnimatedPointerTimer, &QTimer::timeout, this, &CursorManager::AnimatedPointerTimetick);
   AnimatedPointerTimer.setInterval(TimeTic);
   CurrentAnimatedPointerPercentage = 0;
   MovingMultiplePostitPointerCursor = QCursor(QPixmap(":/images/MousPointers/handMoveMultiple.png").scaled(24, 24));
   CutPossiblePointerCursor = QCursor(QPixmap(":/images/MousPointers/scissors24.png").scaled(24, 24));
   DrawingPinterCursor = QCursor(QPixmap(":/images/MousPointers/pencil.png").scaled(24, 24),3,21);
   FillingPointerCursor = QCursor(QPixmap(":/images/MousPointers/bucketfill16.png").scaled(24, 24));
  // MovingPostitPointerCursor = QCursor(QPixmap(":/images/MousPointers/bucketfill16.png").scaled(24, 24));
   TestMode = false;
 }

void CursorManager::AnimatedPointerTimetick()
{
   constexpr uint32_t BaseTime = 8192;
   if (CurrentAnimatedCursor != nullptr) {
      if (DelaytimeLeft > 0us) {
         DelaytimeLeft -= TimeTic;
      } else if (TimeLeft > 0us) {
         uint32_t Ratio = ((TotalDuration - TimeLeft) * BaseTime) / TotalDuration;
         Parent->setCursor(QCursor(CurrentAnimatedCursor->GetPictureForPercentage<BaseTime>(Ratio), CurrentAnimatedCursor->HotX(), CurrentAnimatedCursor->HotY()));
         TimeLeft -= TimeTic;
      } else {
         Parent->setCursor(QCursor(CurrentAnimatedCursor->GetPictureForPercentage<BaseTime>(BaseTime), CurrentAnimatedCursor->HotX(), CurrentAnimatedCursor->HotY()));
         AnimatedPointerTimer.stop();
      }
   } else if (CurrentTestedCursor != CursorEnumEndmarker) {
      CurrentTestedCursor = static_cast<CursorType>(CurrentTestedCursor + 1);
      if (CurrentTestedCursor == CursorEnumEndmarker) {
         CurrentTestedCursor = StandardPointer;
      }
      TestMode = false;
      SetCursor(CurrentTestedCursor, 500ms, 500ms);
      TestMode = true;
      AnimatedPointerTimer.start();
      CurrentAnimatedCursor = nullptr;
   }
}

void CursorManager::StartAnimation()
{
   constexpr uint32_t BaseTime = 8192;
   if (DelaytimeLeft > 0us) {
      Parent->setCursor(QCursor(CurrentAnimatedCursor->GetInitialPicture(), CurrentAnimatedCursor->getHotSpotInitialX(), CurrentAnimatedCursor->getHotSpotInitialY()));
   } else if (TimeLeft > 0us) {
      Parent->setCursor(QCursor(CurrentAnimatedCursor->GetPictureForPercentage<BaseTime>(0), CurrentAnimatedCursor->HotX(), CurrentAnimatedCursor->HotY()));
   }
   AnimatedPointerTimer.start();
}

CursorManager::~CursorManager()
{

}

void CursorManager::TestCursors(bool On)
{
   if (On) {
      AnimatedPointerTimer.stop();
      AnimatedPointerTimer.setInterval(1000ms);
      CurrentTestedCursor = StandardPointer;
      SetCursor(CurrentTestedCursor, 500ms, 500ms);
      CurrentAnimatedCursor = nullptr;
      TestMode = true;
      AnimatedPointerTimer.start();
   } else {
      SetCursor(StandardPointer);
      CurrentTestedCursor = CursorEnumEndmarker;
      AnimatedPointerTimer.stop();
      AnimatedPointerTimer.setInterval(TimeTic);
      TestMode = false;

   }
}

