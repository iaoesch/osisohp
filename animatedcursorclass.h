#ifndef ANIMATEDCURSORCLASS_H
#define ANIMATEDCURSORCLASS_H

#include <QTimer>
#include <QPixmap>
#include <QObject>
#include <QCursor>

class AnimatedCursorClass
{
   int HotSpotX;
   int HotSpotY;
   int HotSpotInitialX;
   int HotSpotInitialY;
   unsigned int NumberOfFramesToPlay;
   QPixmap InitialPicture;
   QPixmap BasePicture;
   std::vector<QPixmap> AnimatedPointerCursor;
   int CurrentAnimatedPointerShape;

public:
   AnimatedCursorClass(size_t Width, size_t Height, size_t NumberOfFramesToPlay, QString FileName, int Hot_x = -1, int Hot_y = -1);
   AnimatedCursorClass(size_t Width, size_t Height, size_t NumberOfFramesToPlay, QPixmap Image, int Hot_x = -1, int Hot_y = -1);
   AnimatedCursorClass(size_t Width, size_t Height, size_t NumberOfFramesToPlay, QString FileName, int Hot_x, int Hot_y, QString FileNameInitialPointer, int Hot_x_Initial = -1, int Hot_y_Initial = -1);
   AnimatedCursorClass(size_t Width, size_t Height, size_t NumberOfFramesToPlay, QPixmap IconPicture, int Hot_x, int Hot_y, QPixmap ImageInitialPointer, int Hot_x_Initial = -1, int Hot_y_Initial = -1);
   AnimatedCursorClass() : NumberOfFramesToPlay(0) {}
   AnimatedCursorClass(const AnimatedCursorClass&) = default;
   AnimatedCursorClass(AnimatedCursorClass&&) = default;
   AnimatedCursorClass &operator = (const AnimatedCursorClass&) = default;
   AnimatedCursorClass &operator = (AnimatedCursorClass&&) = default;

   const QPixmap &GetBasePicture() {
      return BasePicture;
   }
   const QPixmap &GetInitialPicture() {
      return InitialPicture;
   }
   template <uint32_t Base = 8192>
   const QPixmap &GetPictureForPercentage(uint32_t Ratio)
   {
      if (NumberOfFramesToPlay > 0) {
         // + Base/2 for Rounding
         unsigned int Index = (Ratio * NumberOfFramesToPlay + Base/2)/Base;
         if (Index >= NumberOfFramesToPlay) {
            Index = NumberOfFramesToPlay-1;
         }
         return (AnimatedPointerCursor[Index]);
      } else {
         return BasePicture;
      }
   }
   int HotX() const {return HotSpotX;}
   int HotY() const {return HotSpotY;}
   int getHotSpotInitialX() const { return HotSpotInitialX; }
   int getHotSpotInitialY() const { return HotSpotInitialY; }
};

using namespace std::chrono_literals;

class CursorManager : public QObject{
   Q_OBJECT
    QWidget *Parent;

    int CurrentAnimatedPointerPercentage;
    QTimer AnimatedPointerTimer;
    static constexpr std::chrono::milliseconds TimeTic = 20ms;
    std::chrono::microseconds TotalDuration;
    std::chrono::microseconds TotalDelay;
    std::chrono::microseconds DelaytimeLeft;
    std::chrono::microseconds TimeLeft;

public:
   enum CursorType {
      StandardPointer,
      TimedPointerForScrolling,
      TimedPointerForCutting,
      TimedPointerForCreatingPostitFromCutting,
      TimedPointerForSelecting,
      TimedPointerForCreatingPostitFromSelection,
      TimedPointerForCopying,
      TimedPointerForSelectingSingle,
      TimedPointerForSelectingMultiple,
      SelectPossiblePointer,
      CutPossiblePointer,
      CopySelectionPossiblePointer,
      MovingCutoutPointer,
      DrawingPinter,
      GoingToFillTimer,
      FillingPointer,
      ScrollingPointer,
      MovingPostitPointer,
      MovingMultiplePostitPointer,
      TimedPointerForCopyPostit,
      TimedPointerForDeletePostit,
      CursorEnumEndmarker,
      NumberOfCursors = CursorEnumEndmarker
   };
   void SetCursor(CursorType Cursor, std::chrono::milliseconds Duration = 0ms, std::chrono::milliseconds StartupDelay = 0ms);
   void RestartAnimatedCursor();
   CursorManager(QWidget *p);
   virtual ~CursorManager() override;

   void TestCursors(bool On);
private:
   CursorType CurrentTestedCursor;
   bool TestMode;

private slots:
    void AnimatedPointerTimetick();

private:
    void StartAnimation();

    //std::array<AnimatedCursorClass, NumberOfCursors> AnimatedCursors;
    AnimatedCursorClass       AnimatedTimedPointerForScrolling;
    AnimatedCursorClass       AnimatedTimedPointerForCutting;
    AnimatedCursorClass       AnimatedTimedPointerForCreatingPostit;
    AnimatedCursorClass AnimatedTimedPointerForSelecting;
    AnimatedCursorClass AnimatedTimedPointerForCreatingPostitSelecting;

    AnimatedCursorClass       AnimatedTimedPointerForCopying;
    AnimatedCursorClass       AnimatedTimedPointerForSelectingSingle;
    AnimatedCursorClass       AnimatedTimedPointerForSelectingMultiple;
    AnimatedCursorClass       AnimatedTimedPointerForCopyPostit;
    AnimatedCursorClass       AnimatedTimedPointerForDeletePostit;
    AnimatedCursorClass       AnimatedGoingToFillTimer;

    AnimatedCursorClass *CurrentAnimatedCursor;
    AnimatedCursorClass *LastAnimatedCursor;

    QCursor CutPossiblePointerCursor;
    QCursor CopySelectionPossiblePointerCursor;
    QCursor MovingCutoutPointerCursor;
    QCursor DrawingPinterCursor;
    QCursor FillingPointerCursor;
    QCursor MovingPostitPointerCursor;
    QCursor MovingMultiplePostitPointerCursor;
};

#endif // ANIMATEDCURSORCLASS_H
