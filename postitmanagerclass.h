#ifndef POSTITMANAGERCLASS_H
#define POSTITMANAGERCLASS_H

#include <QImage>
#include <QPoint>
#include <QPainterPath>
#include <list>
//#include <vector>
//#include <deque>

#include "box.hpp"
//#include "databaseclass.h"

class SelectionClass;


class PostitManagerClass
{
   struct PostIt {
      static int NextId;
      QImage Image;
      QPointF Position;
      BoundingBoxClass Box;
      QPainterPath BorderPath;
      int Id;
      PostIt(const QImage &NewImage, const QPointF &Pos, BoundingBoxClass NewBox, const QPainterPath &Path) : Image(NewImage), Position(Pos), Box(NewBox), BorderPath(Path), Id(NextId++) {}
      PostIt(const PostIt &Src) : Image(Src.Image), Position(Src.Position), Box(Src.Box), BorderPath(Src.BorderPath), Id(NextId++) {}
   };

   std::list<PostIt> PostIts;
   struct PostItDescriptor{
      std::list<PostIt>::iterator postit;
      QPointF StartPosition;
   } ;
   std::list<PostItDescriptor> SelectedPostit;

   bool ShowPostitsFrame;
   QColor PostItBackgroundColor;
   QColor Markercolor;

public:
   PostitManagerClass();
   bool IsAnySelectedPostit() {return !SelectedPostit.empty();}
   void ClearSelectedPostit() {SelectedPostit.clear();}
   void setPostItBackgroundColor(const QColor &newColor) {PostItBackgroundColor = newColor;}
   void setShowPostitsFrame(bool newShowPostitsFrame)
   {
      ShowPostitsFrame = newShowPostitsFrame;
   }
   void CreatePostitAndSelect(SelectionClass &Selection, QPointF Origin);
   void CreatePostitAndSelect(QImage BackgroundImage, QImage Image, QPointF Position, BoundingBoxClass Box, QPainterPath Path);
   void MoveAllPostits(QPoint Offset);
   void Save(QDataStream &out);
   void Load(QDataStream &in);
   enum class SelectMode {All, First, Last};

   bool FindSelectedPostIts(QPointF Position, SelectMode Mode);
   bool IsInsideAnyPostIt(QPointF Position);
   void DrawAll(QPainter &painter, const QPointF &Offset);
   void MoveSelectedPostits(QPointF DeltaPosition);
   void FinishMovingSelectedPostits(QPointF DeltaPosition);
   void DuplicateSelectedPostits();
   void DeleteSelectedPostits();
   void setMarkercolor(const QColor &newMarkercolor)
   {
      Markercolor = newMarkercolor;
   }
};

#endif // POSTITMANAGERCLASS_H
