/****************************************************************************
**
** Copyright (C) 2013 Digia Plc and/or its subsidiary(-ies).
** Contact: http://www.qt-project.org/legal
**
** This file is part of the examples of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:BSD$
** You may use this file under the terms of the BSD license as follows:
**
** "Redistribution and use in source and binary forms, with or without
** modification, are permitted provided that the following conditions are
** met:
**   * Redistributions of source code must retain the above copyright
**     notice, this list of conditions and the following disclaimer.
**   * Redistributions in binary form must reproduce the above copyright
**     notice, this list of conditions and the following disclaimer in
**     the documentation and/or other materials provided with the
**     distribution.
**   * Neither the name of Digia Plc and its Subsidiary(-ies) nor the names
**     of its contributors may be used to endorse or promote products derived
**     from this software without specific prior written permission.
**
**
** THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
** "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
** LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
** A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
** OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
** SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
** LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
** DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
** THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
** (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
** OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE."
**
** $QT_END_LICENSE$
**
****************************************************************************/

#include <QtWidgets>

#include "mainwindow.h"
#include "scribblearea.h"
#include "SettingsDialog.h"


//! [0]
MainWindow::MainWindow()
{
    SetOHPColors();
    readSettings();
    scribbleArea = new ScribbleArea(Settings);
    setCentralWidget(scribbleArea);
    QWidget::setAttribute(Qt::WA_AcceptTouchEvents);
    //QWidget::setAttribute(Qt::WA_TouchPadAcceptSingleTouchEvents);
    grabGesture(Qt::PanGesture);
    PenColorGroup = new QActionGroup(this);
    createActions();
    createMenus();
    QToolBar * toolBar= new QToolBar("Main Window Tool Bar");
  //  toolBar->insertAction(0, new PushButtonAction(QIcon(":/Refresh.gif"), "Refresh"));

    QPixmap ToolIcon(15, 15);
#if 1
    int PenId = 0;
    for (auto &p: PenInfo) {
       ToolIcon.fill(p.PenColor);
       p.SetPenColorAct = toolBar->addAction(QIcon(ToolIcon), "PenColor");
       PenColorGroup->addAction(p.SetPenColorAct)->setCheckable(true);
       p.SetPenColorAct->setData(QVariant(p.PenColor));
    }
    PenInfo[NumberOfPens-1].SetPenColorAct->setText("MarkerYellow");
#else
    ToolIcon.fill(Qt::blue);
    toolBar->addAction(QIcon(ToolIcon), "Blue")->setChecked(true);

    ToolIcon.fill(Qt::darkGreen);
    Group->addAction(toolBar->addAction(ToolIcon, "Green"))->setCheckable(true);


    ToolIcon.fill(Qt::darkYellow);
    Group->addAction(toolBar->addAction(ToolIcon, "Yellow"))->setCheckable(true);

    ToolIcon.fill(QColor(255, 128, 0));
    Group->addAction(toolBar->addAction(ToolIcon, "Orange"))->setCheckable(true);

    ToolIcon.fill(Qt::red);
    Group->addAction(toolBar->addAction(ToolIcon, "Red"));

    ToolIcon.fill(Qt::magenta);
    Group->addAction(toolBar->addAction(ToolIcon, "Magenta"));

    ToolIcon.fill(Qt::black);
    Group->addAction(toolBar->addAction(ToolIcon, "Black"));

    ToolIcon.fill(Qt::yellow);
    toolBar->addAction(ToolIcon, "MarkerYellow");
#endif
    PenColorGroup->setExclusive(true);
    PenInfo[0].SetPenColorAct->setChecked(true);
    scribbleArea->setPenColor(PenInfo[0].PenColor);

    QActionGroup *Group = new QActionGroup(this);
    Group->setExclusive(true);
    ToolIcon.fill(Qt::yellow);
    QPainter IconPainter(&ToolIcon);
    IconPainter.setPen(Qt::black);
    IconPainter.setBrush(QBrush(Qt::black));
    IconPainter.drawEllipse(4,4,5,5);
    Group->addAction(toolBar->addAction(ToolIcon, "SmallPen"))->setCheckable(true);

    IconPainter.drawEllipse(3,3,7,7);
    Group->addAction(toolBar->addAction(ToolIcon, "MediumPen"))->setCheckable(true);

    IconPainter.drawEllipse(2,2,9,9);
    QAction *LargePen = Group->addAction(toolBar->addAction(ToolIcon, "LargePen"));
    LargePen->setCheckable(true);
    LargePen->setChecked(true);
    toolBar->addAction(QIcon(":/images/spongeicon.png"), "Sponge")->setCheckable(true);

    toolBar->addAction("Freeze")->setCheckable(true);
    toolBar->addAction("NewPlane");
    toolBar->addAction("Merge")->setToolTip("Merge visible Background");
    toolBar->addAction("ToTop")->setToolTip("Merge all visible to editable");
    QPixmap Scissoir = QPixmap(":/images/MousPointers/scissors24.png").scaled(24, 24);
    QAction *CutAct = toolBar->addAction(Scissoir, "Cut");
    CutAct->setCheckable(true);
    CutAct->setChecked(true);
    ShowOverviewAct = toolBar->addAction("ShowOverview");
    ShowOverviewAct->setCheckable(true);

    connect(toolBar, &QToolBar::actionTriggered,
            scribbleArea, &ScribbleArea::HandleToolAction);
    //  toolBar->insertAction(0, new PushButtonAction(QIcon(":/Search.gif"), "Search"));

    addToolBar(Qt::TopToolBarArea, toolBar);
    LayerToolBar= new QToolBar("Layer Tool Bar");
//    LayerToolBar->addAction("L1");
//    LayerToolBar->addAction("L2");
//    LayerToolBar->addAction("L3");
//    LayerToolBar->addAction("L4");
    addToolBar(Qt::TopToolBarArea, LayerToolBar);
    connect(LayerToolBar, &QToolBar::actionTriggered,
            scribbleArea, &ScribbleArea::HandleLayerVisibilityAction);
    connect(scribbleArea, &ScribbleArea::NumberOfLayerChanged,
            this, &MainWindow::createLayerActions);
    connect(scribbleArea, &ScribbleArea::SetVisibilityIndicatorOfLayer,
            this, &MainWindow::SetVisibilityIndicatorOfLayer);
    connect(scribbleArea, &ScribbleArea::ShowOverviewChanged,
            this, &MainWindow::ShowOverviewChanged);
    connect(scribbleArea, &ScribbleArea::SwitchToNextPenColor,
              this, &MainWindow::SwitchToNextPenColor);



    setWindowTitle(tr("Osis OHP"));
    setWindowState(Qt::WindowMaximized);
    resize(width(), height());
    UpdateColors();

}
//! [0]


void MainWindow::SetOHPColors(void)
{
   PenInfo[0].PenColor = Qt::blue;
   PenInfo[1].PenColor = Qt::darkGreen;
   PenInfo[2].PenColor = Qt::darkYellow;
   PenInfo[3].PenColor = QColor(255, 128, 0);
   PenInfo[4].PenColor = Qt::red;
   PenInfo[5].PenColor = QColor(255, 88, 255);//Qt::magenta;
   PenInfo[6].PenColor = Qt::black;
   PenInfo[7].PenColor = Qt::yellow;
   BackgroundColor = QColor(230,230, 200,255);
   ScrollHintColor = QColor(200, 230, 200, 50);
   PostItBackgroundColor = QColor(100, 0, 0, 50);
   SelectionHintColor = QColor(200, 230, 200, 50);
}
void MainWindow::SetWhiteBoardColors(void)
{
   PenInfo[0].PenColor = QColor(100, 180, 255);
   PenInfo[1].PenColor = Qt::green;
   PenInfo[2].PenColor = QColor(225, 193, 110);
   PenInfo[3].PenColor = QColor(255, 128, 0);
   PenInfo[4].PenColor = Qt::red;
   PenInfo[5].PenColor = QColor(255, 88, 255);//Qt::magenta;
   PenInfo[6].PenColor = Qt::black;
   PenInfo[7].PenColor = Qt::yellow;
   BackgroundColor = QColor(255,255, 255, 255);
   ScrollHintColor = QColor(200, 230, 200, 50);
   PostItBackgroundColor = QColor(100, 0, 0, 50);
   SelectionHintColor = QColor(200, 230, 200, 50);
}

void MainWindow::SetBlackBoardColors(void)
{
   PenInfo[0].PenColor = QColor(0x4991FF);
   PenInfo[1].PenColor = Qt::green;
   PenInfo[2].PenColor = Qt::yellow;
   PenInfo[3].PenColor = QColor(255, 128, 0);
   PenInfo[4].PenColor = Qt::red;
   PenInfo[5].PenColor = QColor(255, 88, 255);//Qt::magenta;
   PenInfo[6].PenColor = Qt::white;
   PenInfo[7].PenColor = Qt::yellow;
   BackgroundColor = QColor(30, 30, 30, 255);
   ScrollHintColor = QColor(200, 230, 200, 50);
   PostItBackgroundColor = QColor(150, 250, 250, 50);
   SelectionHintColor = QColor(200, 230, 200, 50);
}

void MainWindow::UpdateColors(void)
{
   QPixmap ToolIcon(15, 15);

   for (auto &p: PenInfo) {
      ToolIcon.fill(p.PenColor);
      p.SetPenColorAct->setIcon(ToolIcon);
      p.SetPenColorAct->setData(p.PenColor);
   }
   scribbleArea->setBackGroundColor(BackgroundColor);
   scribbleArea->setPostItBackgroundColor(PostItBackgroundColor);
   scribbleArea->setScrollHintColor(ScrollHintColor);
   scribbleArea->setSelectionHintColor(SelectionHintColor);

//   if (BackgroundColor.value() > 128) {
//      QColor PostitColor = BackgroundColor.darker();
//      PostitColor.setAlpha(40);
//      scribbleArea->setPostItBackgroundColor(PostitColor);
//   } else {
//      QColor PostitColor = BackgroundColor.lighter();
//      PostitColor.setAlpha(40);
//      scribbleArea->setPostItBackgroundColor(PostitColor);
//   }
}

//! [1]
void MainWindow::closeEvent(QCloseEvent *event)
//! [1] //! [2]
{
    if (maybeSave()) {
        writeSettings();
        event->accept();
    } else {
        event->ignore();
    }
}

//! [2]

//! [3]
void MainWindow::open()
//! [3] //! [4]
{
    if (maybeSave()) {
        QString fileName = QFileDialog::getOpenFileName(this,
                                   tr("Open File"), QDir::currentPath());
        if (!fileName.isEmpty())
            scribbleArea->LoadImage(fileName);
    }
}
//! [4]

//! [5]
void MainWindow::Export()
//! [5] //! [6]
{
    QAction *action = qobject_cast<QAction *>(sender());
    QByteArray fileFormat = action->data().toByteArray();
    ExportFile(fileFormat);
}
//! [6]
void MainWindow::Save()
//! [5] //! [6]
{
   SaveFile("ohp");
}

void MainWindow::ProtectImage()
{
   scribbleArea->MoveImageToBackgroundLayer();
}

//! [7]
void MainWindow::penColor()
//! [7] //! [8]
{
    QColor newColor = QColorDialog::getColor(scribbleArea->penColor());
    if (newColor.isValid())
        scribbleArea->setPenColor(newColor);
}

void MainWindow::SwitchToNextPenColor()
//! [7] //! [8]
{
   int ActiveColor = PenColorGroup->actions().indexOf(PenColorGroup->checkedAction());
   ActiveColor++;
   if (ActiveColor >= PenColorGroup->actions().length()) {
      ActiveColor = 0;
   }
   PenColorGroup->actions().at(ActiveColor)->setChecked(true);
   PenColorGroup->actions().at(ActiveColor)->trigger();
}

//! [8]
void MainWindow::SetMatchingPostitColor(QColor &newColor)
{
   PostItBackgroundColor = QColor(255-newColor.red(), 255-newColor.green(), 255-newColor.blue(), 50);
   scribbleArea->setPostItBackgroundColor(PostItBackgroundColor);
   ScrollHintColor = QColor(255-newColor.red(), 255-newColor.green(), 255-newColor.blue(), 50);
   scribbleArea->setScrollHintColor(ScrollHintColor);
   SelectionHintColor = QColor(255-newColor.red(), 255-newColor.green(), 255-newColor.blue(), 50);
   scribbleArea->setSelectionHintColor(SelectionHintColor);

}

void MainWindow::BackGroundColorColor()
//! [7] //! [8]
{
    QColor newColor = QColorDialog::getColor(scribbleArea->GetBackGroundColor());
    if (newColor.isValid()) {
       scribbleArea->setBackGroundColor(newColor);
       SetMatchingPostitColor(newColor);

//       if (newColor.value() > 128) {
//          QColor PostitColor = newColor.darker();
//          PostitColor.setAlpha(40);
//          scribbleArea->setPostItBackgroundColor(PostitColor);
//       } else {
//          QColor PostitColor = newColor.lighter();
//          PostitColor.setAlpha(40);
//          scribbleArea->setPostItBackgroundColor(PostitColor);
//       }

    }

}

void MainWindow::BackGroundColorOHP()
{
   SetOHPColors();
   UpdateColors();
}

void MainWindow::BackGroundColorWhiteBoard()
{
   SetWhiteBoardColors();
   UpdateColors();
}

void MainWindow::BackGroundColorBlackBoard()
{
   SetBlackBoardColors();
   UpdateColors();
}

//! [9]
void MainWindow::penWidth()
//! [9] //! [10]
{
    bool ok;
    int newWidth = QInputDialog::getInt(this, tr("Scribble"),
                                        tr("Select pen width:"),
                                        scribbleArea->penWidth(),
                                        1, 50, 1, &ok);
    if (ok)
        scribbleArea->setPenWidth(newWidth);
}
//! [10]
//!
//!
void MainWindow::DirectSelect()
//! [9] //! [10]
{
    scribbleArea->setDirectSelect(DirectPostitSelectAct->isChecked());
}

void MainWindow::ShowPostitsFrame()
//! [9] //! [10]
{

    scribbleArea->setShowPostitsFrame(ShowPostitsFrameAct->isChecked());
}

void MainWindow::ShowGestureInfos()
//! [9] //! [10]
{

    scribbleArea->setShowGestureInfos(ShowGestureInfosAct->isChecked());
}


void MainWindow::ShowCursors()
//! [9] //! [10]
{

    scribbleArea->setShowCursors(ShowCursorsAct->isChecked());
}



void MainWindow::Paste()
//! [9] //! [10]
{
    QImage ImageToPaste = QGuiApplication::clipboard()->image();
    if (!ImageToPaste.isNull()) {
       scribbleArea->PasteImage(ImageToPaste);
    }
}


//! [11]
void MainWindow::about()
//! [11] //! [12]
{
    QMessageBox::about(this, tr("About Osis OHP"),
            tr("<p>The <b>Osis OHP</b> (Overhead projector) is a simple drawing application with minimal "
               "user interface. Just draw with selected color. Last drawn shape serves "
               "as cutout, if pointer is placed inside and not moved for 500 ms "
               "If placed outside and selected for 500 ms the whole drawing area "
               "may be moved around. The drawingarea is virtually infinite... "));
}
//! [12]

//! [13]
void MainWindow::createActions()
//! [13] //! [14]
{
    openAct = new QAction(tr("&Open..."), this);
    openAct->setShortcuts(QKeySequence::Open);
    connect(openAct, SIGNAL(triggered()), this, SLOT(open()));

    saveAct = new QAction(tr("&Save..."), this);
    saveAct->setShortcuts(QKeySequence::Save);
    connect(saveAct, SIGNAL(triggered()), this, SLOT(Save()));

    foreach (QByteArray format, QImageWriter::supportedImageFormats()) {
        QString text = tr("%1...").arg(QString(format).toUpper());

        QAction *action = new QAction(text, this);
        action->setData(format);
        connect(action, SIGNAL(triggered()), this, SLOT(Export()));
        saveAsActs.append(action);
    }

    printAct = new QAction(tr("&Print..."), this);
    connect(printAct, SIGNAL(triggered()), scribbleArea, SLOT(print()));


    PasteAct = new QAction(tr("Paste"), this);
    PasteAct->setShortcuts(QKeySequence::Paste);
    connect(PasteAct, &QAction::triggered, this, &MainWindow::Paste);

    CopyAct = new QAction(tr("Copy"), this);
    CopyAct->setShortcuts(QKeySequence::Copy);
    connect(CopyAct, &QAction::triggered, scribbleArea, &ScribbleArea::CopyImageToClipboard);

    exitAct = new QAction(tr("E&xit"), this);
    exitAct->setShortcuts(QKeySequence::Quit);
    connect(exitAct, SIGNAL(triggered()), this, SLOT(close()));

    penColorAct = new QAction(tr("&Pen Color..."), this);
    connect(penColorAct, SIGNAL(triggered()), this, SLOT(penColor()));

    penWidthAct = new QAction(tr("Pen &Width..."), this);
    connect(penWidthAct, SIGNAL(triggered()), this, SLOT(penWidth()));

    BackGroundColorAct = new QAction(tr("&bg Color..."), this);
    connect(BackGroundColorAct, SIGNAL(triggered()), this, SLOT(BackGroundColorColor()));

    WhiteBoardColorAct = new QAction(tr("&Whiteboard"), this);
    connect(WhiteBoardColorAct, SIGNAL(triggered()), this, SLOT(BackGroundColorWhiteBoard()));

    OHPColorAct = new QAction(tr("&OHP"), this);
    connect(OHPColorAct, SIGNAL(triggered()), this, SLOT(BackGroundColorOHP()));

    BlackBoardColorAct = new QAction(tr("&Blackboard"), this);
    connect(BlackBoardColorAct, SIGNAL(triggered()), this, SLOT(BackGroundColorBlackBoard()));


    DirectPostitSelectAct = new QAction(tr("&Direct select"), this);
    DirectPostitSelectAct->setCheckable(true);
    connect(DirectPostitSelectAct, SIGNAL(triggered()), this, SLOT(DirectSelect()));

    ShowPostitsFrameAct = new QAction(tr("&Postit Frame"), this);
    ShowPostitsFrameAct->setCheckable(true);
    connect(ShowPostitsFrameAct, SIGNAL(triggered()), this, SLOT(ShowPostitsFrame()));

    ShowCursorsAct = new QAction(tr("&Cycle Pointers"), this);
    ShowCursorsAct->setCheckable(true);
    connect(ShowCursorsAct, &QAction::triggered, this, &MainWindow::ShowCursors);


    ShowGestureInfosAct = new QAction(tr("&Gesture Infos"), this);
    ShowGestureInfosAct->setCheckable(true);
    connect(ShowGestureInfosAct, &QAction::triggered, this, &MainWindow::ShowGestureInfos);

    clearScreenAct = new QAction(tr("&Clear Screen"), this);
    clearScreenAct->setShortcut(tr("Ctrl+L"));
    connect(clearScreenAct, SIGNAL(triggered()),
            scribbleArea, SLOT(clearImage()));

    SettingsAct = new QAction(tr("SSettings"), this);
    //SettingsAct->setShortcut(tr("Ctrl+"));
    connect(SettingsAct, SIGNAL(triggered()),
            this, SLOT(ShowSettings()));

    aboutAct = new QAction(tr("&About"), this);
    connect(aboutAct, SIGNAL(triggered()), this, SLOT(about()));

    aboutQtAct = new QAction(tr("About &Qt"), this);
    connect(aboutQtAct, SIGNAL(triggered()), qApp, SLOT(aboutQt()));
}
//! [14]
void MainWindow::createLayerActions(int Number)
{
   auto Actionlist = LayerToolBar->actions();
   for (auto &a: Actionlist) {
      LayerToolBar->removeAction(a);
      delete a;
   }
   for (int i = 0; i < Number; i++) {
      auto a = LayerToolBar->addAction("L"+QString::number(i));
      a->setCheckable(true);
      a->setData(QVariant(i));
   }
}

void MainWindow::SetVisibilityIndicatorOfLayer(int Layer, bool Visibility)
{
   auto Actionlist = LayerToolBar->actions();
   if (Layer < Actionlist.size()) {
      Actionlist.at(Layer)->setChecked(Visibility);
   }
}

void MainWindow::ShowOverviewChanged(bool OverviewEnabled)
{
   ShowOverviewAct->setChecked(OverviewEnabled);
}

void MainWindow::ShowSettings()
{
#if 0
   static struct Settings {
    bool Value1 = true;
    int Value2 = 128;
    double Value3 = 3.14;
    std::string Value4 = "Fritz";
    std::string Value5 = "guess";
   } MyLocalSettings;
    TabDialogDescriptor Descriptor("Test");
    Descriptor.AddTab("Tab1");
    Descriptor.GetTab().AddEntry("Val 1", "Help for Val 1", MyLocalSettings.Value1, false);
    Descriptor.GetTab().AddEntry("Val 2", "Help for Val 2", MyLocalSettings.Value2, 0);
    Descriptor.GetTab().AddEntry("Val 3", "Help for Val 3", MyLocalSettings.Value3, 0.0);
    Descriptor.GetTab().AddEntry("Val 4", "Help for Val 4", MyLocalSettings.Value4, std::string(""));
    Descriptor.AddTab("Tab2");
    Descriptor.GetTab().AddEntry("Val 1", "Help for Val 1", MyLocalSettings.Value5, std::string(""));
    Descriptor.AddTab("Timings");
    this->Settings.getSettings(Descriptor.GetTab());
#else
   TabDialogDescriptor Descriptor("Preferences");
   Descriptor.AddTab( Settings.getSettings());
#endif
   Descriptor.Fetch();
    SettingsDialog MySettings(Descriptor);
    auto Result  = MySettings.exec();
    if (Result == QDialog::Accepted) {
       Descriptor.Update();
    }

}

//! [15]
void MainWindow::UpdatePasteState()
{
   if (QGuiApplication::clipboard()->mimeData()->hasImage()) {
      PasteAct->setEnabled(true);
   } else {
      PasteAct->setEnabled(false);
   }
}


void MainWindow::createMenus()
//! [15] //! [16]
{
    saveAsMenu = new QMenu(tr("&Export As"), this);
    foreach (QAction *action, saveAsActs)
        saveAsMenu->addAction(action);

    fileMenu = new QMenu(tr("&File"), this);
    fileMenu->addAction(openAct);
    fileMenu->addAction(saveAct);
    fileMenu->addMenu(saveAsMenu);
    fileMenu->addAction(printAct);
    fileMenu->addAction(CopyAct);
    fileMenu->addAction(PasteAct);
    fileMenu->addSeparator();
    fileMenu->addAction(exitAct);

    connect(fileMenu, &QMenu::aboutToShow, this, &MainWindow::UpdatePasteState);

    optionMenu = new QMenu(tr("&Options"), this);
    optionMenu->addAction(penColorAct);
    optionMenu->addAction(penWidthAct);
    optionMenu->addAction(BackGroundColorAct);
    optionMenu->addAction(WhiteBoardColorAct);
    optionMenu->addAction(OHPColorAct);
    optionMenu->addAction(BlackBoardColorAct);
    optionMenu->addAction(DirectPostitSelectAct);
    optionMenu->addAction(SettingsAct);
    optionMenu->addSeparator();
    optionMenu->addAction(clearScreenAct);



    DebugMenu = new QMenu(tr("&Debug"), this);
    DebugMenu->addAction(ShowPostitsFrameAct);
    DebugMenu->addAction(ShowCursorsAct);
    DebugMenu->addAction(ShowGestureInfosAct);
    DebugMenu->addAction(SettingsAct);


    helpMenu = new QMenu(tr("&Help"), this);
    helpMenu->addAction(aboutAct);
    helpMenu->addAction(aboutQtAct);

    menuBar()->addMenu(fileMenu);
    menuBar()->addMenu(optionMenu);
    menuBar()->addMenu(DebugMenu);

    menuBar()->addMenu(helpMenu);
}
//! [16]

//! [17]
bool MainWindow::maybeSave()
//! [17] //! [18]
{
    if (scribbleArea->isModified()) {
       QMessageBox::StandardButton ret;
       ret = QMessageBox::warning(this, tr("Scribble"),
                          tr("The image has been modified.\n"
                             "Do you want to save your changes?"),
                          QMessageBox::Save | QMessageBox::Discard
                          | QMessageBox::Cancel);
        if (ret == QMessageBox::Save) {
            return SaveFile("ohp");
        } else if (ret == QMessageBox::Cancel) {
            return false;
        }
    }
    return true;
}
//! [18]

//! [19]
bool MainWindow::ExportFile(const QByteArray &fileFormat)
//! [19] //! [20]
{
    QString initialPath = QDir::currentPath() + "/untitled." + fileFormat;

    QString fileName = QFileDialog::getSaveFileName(this, tr("Save As"),
                               initialPath,
                               tr("%1 Files (*.%2);;All Files (*)")
                               .arg(QString::fromLatin1(fileFormat.toUpper()))
                               .arg(QString::fromLatin1(fileFormat)));
    if (fileName.isEmpty()) {
        return false;
    } else {
        return scribbleArea->ExportImage(fileName, fileFormat.constData());
    }
}
bool MainWindow::SaveFile(const QByteArray &fileFormat)
//! [19] //! [20]
{
    QString initialPath = QDir::currentPath() + "/untitled." + fileFormat;

    QString fileName = QFileDialog::getSaveFileName(this, tr("Save As"),
                               initialPath,
                               tr("%1 Files (*.%2);;All Files (*)")
                               .arg(QString::fromLatin1(fileFormat.toUpper()))
                               .arg(QString::fromLatin1(fileFormat)));
    if (fileName.isEmpty()) {
        return false;
    } else {
        return scribbleArea->SaveImage(fileName);
    }
}

void MainWindow::writeSettings()
{
    QSettings SettingsManager("OESCH", "OverheadProjector");
#ifdef UseFlatSettings
#error "Old variant, just for reference, throw away soon"
    GroupDescriptor MySettings("timings");
    Settings.getSettings(MySettings);
    SettingsManager.beginGroup("timings");
#else
    auto GroupedSettings = Settings.getSettings();
    for (auto MySettings: GroupedSettings) {
       MySettings.Fetch();
    SettingsManager.beginGroup(QString::fromStdString(MySettings.GetGroupName()));
#endif

    for (auto &d: MySettings.getEntries()) {
       // Switch on type in variant
       switch(d.CurrentTypeId()) {
          case EntityDescriptor::IdOf<bool>():
                SettingsManager.setValue(QString::fromStdString(d.Title), d.GetValue<bool>());
          break;
          case EntityDescriptor::IdOf<int>():
                SettingsManager.setValue(QString::fromStdString(d.Title), d.GetValue<int>());
             break;
          case EntityDescriptor::IdOf<double>():
                SettingsManager.setValue(QString::fromStdString(d.Title), d.GetValue<double>());
          break;
          case EntityDescriptor::IdOf<std::string>():
                SettingsManager.setValue(QString::fromStdString(d.Title), QString::fromStdString(d.GetValue<std::string>()));
        }
    }

#ifdef UseFlatSettings
    SettingsManager.endGroup();
#else
    SettingsManager.endGroup();
    }
#endif

    SettingsManager.beginGroup("Drawing");
    SettingsManager.beginWriteArray("Pens");
    for (qsizetype i = 0; i < NumberOfPens; ++i) {
        SettingsManager.setArrayIndex(i);
        SettingsManager.setValue("Color", PenInfo[i].PenColor);
    }
    SettingsManager.endArray();
    SettingsManager.setValue("BackgroundColor", BackgroundColor);
    SettingsManager.setValue("ScrollHintColor", ScrollHintColor);
    SettingsManager.setValue("SelectionHintColor", SelectionHintColor);
    SettingsManager.setValue("PostItBackgroundColor", PostItBackgroundColor);

    SettingsManager.beginGroup("SensorNames");
    SettingsManager.endGroup();

    SettingsManager.endGroup();
}

void MainWindow::readSettings()
{
   QSettings SettingsManager("OESCH", "OverheadProjector");
#ifdef UseFlatSettings
#error "Old variant, just for reference, throw away soon"

   GroupDescriptor MySettings("timings");
   Settings.getSettings(MySettings);
   SettingsManager.beginGroup("timings");
#else
    auto GroupedSettings = Settings.getSettings();
    for (auto MySettings: GroupedSettings) {

    SettingsManager.beginGroup(QString::fromStdString(MySettings.GetGroupName()));
#endif


   for (auto &d: MySettings.getEntries()) {
      // Switch on type in variant
      switch(d.CurrentTypeId()) {
         case EntityDescriptor::IdOf<bool>():
               d.SetValue<bool>(SettingsManager.value(QString::fromStdString(d.Title), QVariant(d.GetDefaultValue<bool>())).value<bool>());
         break;
         case EntityDescriptor::IdOf<int>():
            d.SetValue<int>(SettingsManager.value(QString::fromStdString(d.Title), QVariant(d.GetDefaultValue<int>())).value<int>());
            break;
         case EntityDescriptor::IdOf<double>():
            d.SetValue<double>(SettingsManager.value(QString::fromStdString(d.Title), QVariant(d.GetDefaultValue<double>())).value<double>());
         break;
         case EntityDescriptor::IdOf<std::string>():
            d.SetValue<std::string>(SettingsManager.value(QString::fromStdString(d.Title), QVariant(QString::fromStdString(d.GetDefaultValue<std::string>()))).value<std::string>());
       }
   }
#ifdef UseFlatSettings
    SettingsManager.endGroup();
    MySettings.Update();
#else
    SettingsManager.endGroup();
    MySettings.Update();
    }
#endif

    SettingsManager.beginGroup("Drawing");
    int size = SettingsManager.beginReadArray("Pens");
    for (int i = 0; i < size; ++i) {
        SettingsManager.setArrayIndex(i);
        if (SettingsManager.value("Color").canConvert<QString>()) {
            PenInfo[i].PenColor = SettingsManager.value("Color").value<QColor>();
        }
    }
    SettingsManager.endArray();
    BackgroundColor = SettingsManager.value("BackgroundColor").value<QColor>();
    ScrollHintColor = SettingsManager.value("ScrollHintColor").value<QColor>();
    SelectionHintColor = SettingsManager.value("SelectionHintColor").value<QColor>();
    PostItBackgroundColor = SettingsManager.value("PostItBackgroundColor").value<QColor>();

    SettingsManager.beginGroup("SensorNames");
    SettingsManager.endGroup();
    SettingsManager.endGroup();
}


//! [20]
