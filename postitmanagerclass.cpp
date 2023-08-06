#include "postitmanagerclass.h"
#include "selectionclass.h"

#include <QPainter>

int PostitManagerClass::PostIt::NextId = 0;


PostitManagerClass::PostitManagerClass()
{
   PostItBackgroundColor = QColor(100, 0, 0, 50);
   ShowPostitsFrame = false;

   SelectedPostit.clear();

}

void PostitManagerClass::CreatePostitAndSelect(QImage BackgroundImage, QImage Image, QPointF Position, BoundingBoxClass Box, QPainterPath Path)
{
   QImage NewPostit(BackgroundImage);
   // Here we could add a different background for postits
   QPainter painter(&NewPostit);
   painter.setCompositionMode(QPainter::CompositionMode_SourceIn);
   painter.setBrush(QBrush(PostItBackgroundColor));
   painter.drawRect(Image.rect());
   painter.setCompositionMode(QPainter::CompositionMode_SourceOver);
   painter.drawImage(0,0,Image);
   //BoundingBoxClass TranslatedBoundingBox (SelectedImageBoundingBox);
   //TranslatedBoundingBox.Move(PositionClass(Origin.x(), Origin.y()));
   PostIts.push_back(PostIt(NewPostit, Position, Box, Path));
   SelectedPostit.push_back({std::prev(PostIts.end()), PostIts.back().Position});
}
/*
void DatabaseClass::CreeatePostitFromSelection()
{
   //std::cout << "new postit (" << PostIts.size() << ")" << std::flush;

   BoundingBoxClass TranslatedBoundingBox (CurrentSeelection.getBoundingBox());
   TranslatedBoundingBox.Move(PositionClass(Origin.x(), Origin.y()));
   Postits.CreatePostitAndSelect(CurrentSeelection.HintSelectedImagePart, SelectedImagePart, Origin + SelectedCurrentPosition+SelectedOffset, TranslatedBoundingBox, SelectedImagePartPath);
   SetModified();
}
*/
void PostitManagerClass::CreatePostitAndSelect(SelectionClass &Selection, QPointF Origin)
{
   QImage NewPostit(Selection.getHintSelectedImagePart());
   // Here we could add a different background for postits
   QPainter painter(&NewPostit);
   painter.setCompositionMode(QPainter::CompositionMode_SourceIn);
   painter.setBrush(QBrush(PostItBackgroundColor));
   painter.drawRect(Selection.getSelectedImagePart().rect());
   painter.setCompositionMode(QPainter::CompositionMode_SourceOver);
   painter.drawImage(0,0,Selection.getHintSelectedImagePart());
   BoundingBoxClass TranslatedBoundingBox (Selection.getBoundingBox());
   TranslatedBoundingBox.Move(PositionClass(Origin.x(), Origin.y()));
   PostIts.push_back(PostIt(NewPostit, Origin + Selection.getSelectedCurrentPosition()+Selection.getSelectedOffset(), TranslatedBoundingBox, Selection.getSelectedImagePartPath()));
   SelectedPostit.push_back({std::prev(PostIts.end()), PostIts.back().Position});
}

void PostitManagerClass::MoveAllPostits(QPoint Offset)
{
   // Now adjust all postits
   for (auto &Picture: PostIts) {
      Picture.Position += Offset;
      Picture.Box.Move(PositionClass(Offset.x(), Offset.y()));
   }
}

bool PostitManagerClass::FindSelectedPostIts(QPointF Position, SelectMode Mode)
{
   bool Found = false;
   SelectedPostit.clear();
   //Position += Origin;
#if 0
   std::list<PostIt>::iterator Start = PostIts.begin();
   auto End = PostIts.end();
   if (Mode == Last) {
      Start = PostIts.rbegin();
      End = PostIts.rend();
   }
#endif
   for (auto PostIt = PostIts.begin(); PostIt != PostIts.end(); PostIt++) {
     if (PostIt->Box.IsInside(PositionClass(Position.x(), Position.y()))) {
        SelectedPostit.push_back({PostIt, PostIt->Position});
        Found = true;
        if (Mode == SelectMode::First) {
           // Only select first (Bottom one)
           return true;
        }
        // Select last not implemented yet (Probably use reverse iterator for this case)
     }
   }
   return Found;
}


bool PostitManagerClass::IsInsideAnyPostIt(QPointF Position)
{
   //Position += Origin;
   for (auto &PostIt: PostIts) {
     if (PostIt.Box.IsInside(PositionClass(Position.x(), Position.y()))) {
        return true;
     }
   }
   return false;
}

void PostitManagerClass::DrawAll(QPainter &painter, const QPointF &Offset)
{
   for (auto &Picture: PostIts) {
      painter.drawImage(Picture.Position-Offset, Picture.Image);
      if (ShowPostitsFrame == true) {
         painter.setBrush(QBrush(Qt::NoBrush));
         painter.setPen(Markercolor);
         painter.drawRect(Picture.Box.QRectangle().translated(-Offset.toPoint()));
         painter.drawText(Picture.Box.QRectangle().translated(-Offset.toPoint()), QString::number(Picture.Id));
      }
   }
}

void PostitManagerClass::MoveSelectedPostits(QPointF DeltaPosition)
{
   for(auto &r: SelectedPostit) {
      //SelectedPostit->Position = Position;
      QPointF LastPosition = r.postit->Position;
      r.postit->Position = r.StartPosition + (DeltaPosition);

      r.postit->Box.Move(PositionClass(r.postit->Position.x()-LastPosition.x(), r.postit->Position.y()-LastPosition.y()));
  //  SelectedPostit->Position = SelectedPostit->Position  Origin + Position;
   }
}

void PostitManagerClass::FinishMovingSelectedPostits(QPointF DeltaPosition)
{
   for (auto &r: SelectedPostit) {
      QPointF LastPosition = r.postit->Position;
      r.postit->Position = r.StartPosition + (DeltaPosition);

      r.postit->Box.Move(PositionClass(r.postit->Position.x()-LastPosition.x(), r.postit->Position.y()-LastPosition.y()));
      // Place moved postits on top of each other
      PostIts.splice( PostIts.end(), PostIts, r.postit);
   }
}

void PostitManagerClass::DuplicateSelectedPostits()
{
   for (auto &r: SelectedPostit) {
      PostIts.push_back(*r.postit);
   }
}

void PostitManagerClass::DeleteSelectedPostits()
{
   for (auto &r: SelectedPostit) {
      PostIts.erase(r.postit);
   }
}


void PostitManagerClass::Save(QDataStream &out)
{
   out << static_cast<quint32>(PostIts.size());
   for (auto &&Picture: PostIts) {
      out << Picture.Position;
      out << Picture.Image;
      out << Picture.Box;
      out << Picture.BorderPath;
   }
}

void PostitManagerClass::Load(QDataStream &in)
{
   qint32 NumberOfSavedPostits = 0;
   in >> NumberOfSavedPostits;
   QImage NewImage;
   QPoint Position;
   BoundingBoxClass NewBox;
   QPainterPath BorderPath;
   PostIts.clear();
   for (int i = 0; i < NumberOfSavedPostits; i++) {
      in >> Position;
      in >> NewImage;
      in >> NewBox;
      in >> BorderPath;
      PostIts.push_back(PostIt(NewImage, Position, NewBox, BorderPath));
   }
}


