#ifndef ANIMATEDCURSORCLASS_H
#define ANIMATEDCURSORCLASS_H

#include <QTimer>
#include <QPixmap>
#include <QObject>

class AnimatedCursorClass
{
   int HotSpotX;
   int HotSpotY;
   unsigned int NumberOfFramesToPlay;
   std::vector<QPixmap> AnimatedPointerCursor;
   int CurrentAnimatedPointerShape;

public:
   AnimatedCursorClass(size_t Width, size_t Height, size_t NumberOfFramesToPlay, QString FileName, int Hot_x = -1, int Hot_y = -1);
   AnimatedCursorClass() : NumberOfFramesToPlay(0) {}
   const QPixmap &GetPictureForPercentage(unsigned int Percentage)
   {
      // + 50 for Rounding
      unsigned int Index = (Percentage * NumberOfFramesToPlay + 50u)/100u;
      if (Index >= NumberOfFramesToPlay) {
         Index = NumberOfFramesToPlay-1;
      }
      return (AnimatedPointerCursor[Index]);
   }
   int HotX() {return HotSpotX;}
   int HotY() {return HotSpotY;}
};

class CursorManager : public QObject{
   Q_OBJECT
    QWidget *Parent;

    int CurrentAnimatedPointerPercentage;
    QTimer AnimatedPointerTimer;


public:
   enum CursorType {
      StandardPointer,
      TimedPointerForScrolling,
      TimedPointerForCutting,
      TimedPointerForCopying,
      TimedPointerForSelectingSingle,
      TimedPointerForSelectingMultiple,
      SelectPossiblePointer,
      CutPossiblePointer,
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
   void SetCursor(CursorType Cursor);
   CursorManager(QWidget *p);
   virtual ~CursorManager() override;

private slots:
    void AnimatedPointerTimetick();

private:
    void StartAnimation();

    std::array<AnimatedCursorClass, NumberOfCursors> AnimatedCursors;
    AnimatedCursorClass *CurrentAnimatedCursor;

};

#endif // ANIMATEDCURSORCLASS_H
