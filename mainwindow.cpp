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

//! [0]
MainWindow::MainWindow()
{
    SetWhiteBoardColors();
    readSettings();
    scribbleArea = new ScribbleArea;
    setCentralWidget(scribbleArea);
    QWidget::setAttribute(Qt::WA_AcceptTouchEvents);
    QActionGroup *Group = new QActionGroup(this);
    createActions();
    createMenus();
    QToolBar * toolBar= new QToolBar("Main Window Tool Bar");
  //  toolBar->insertAction(0, new PushButtonAction(QIcon(":/Refresh.gif"), "Refresh"));

    QPixmap ToolIcon(15, 15);
#if 1
    for (auto &p: PenInfo) {
       ToolIcon.fill(p.PenColor);
       p.SetPenColorAct = toolBar->addAction(QIcon(ToolIcon), "PenColor");
       Group->addAction(p.SetPenColorAct);
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
    Group->setExclusive(true);
    PenInfo[0].SetPenColorAct->setChecked(true);

    ToolIcon.fill(Qt::yellow);
    QPainter IconPainter(&ToolIcon);
    IconPainter.setPen(Qt::black);
    IconPainter.setBrush(QBrush(Qt::black));
    IconPainter.drawEllipse(4,4,5,5);
    toolBar->addAction(ToolIcon, "SmallPen");

    IconPainter.drawEllipse(3,3,7,7);
    toolBar->addAction(ToolIcon, "MediumPen");

    IconPainter.drawEllipse(2,2,9,9);
    QAction *LargePen = toolBar->addAction(ToolIcon, "LargePen");
    LargePen->setChecked(true);

    toolBar->addAction("Freeze")->setCheckable(true);
    toolBar->addAction("Protect")->setCheckable(true);
    toolBar->addAction("Cut")->setCheckable(true);

    connect(toolBar, SIGNAL(actionTriggered(QAction *)),
            scribbleArea, SLOT(HandleToolAction(QAction * )));
    //  toolBar->insertAction(0, new PushButtonAction(QIcon(":/Search.gif"), "Search"));

    addToolBar(Qt::TopToolBarArea, toolBar);

    setWindowTitle(tr("Osis OHP"));

    resize(1500, 1500);
    UpdateColors();
}
//! [0]

void MainWindow::SetWhiteBoardColors(void)
{
   PenInfo[0].PenColor = Qt::blue;
   PenInfo[1].PenColor = Qt::darkGreen;
   PenInfo[2].PenColor = Qt::darkYellow;
   PenInfo[3].PenColor = QColor(255, 128, 0);
   PenInfo[4].PenColor = Qt::red;
   PenInfo[5].PenColor = Qt::magenta;
   PenInfo[6].PenColor = Qt::black;
   PenInfo[7].PenColor = Qt::yellow;
   BackgroundColor = QColor(230,230, 200,255);
}

void MainWindow::SetBlackBoardColors(void)
{
   PenInfo[0].PenColor = QColor(0x4991FF);
   PenInfo[1].PenColor = Qt::green;
   PenInfo[2].PenColor = Qt::yellow;
   PenInfo[3].PenColor = QColor(255, 128, 0);
   PenInfo[4].PenColor = Qt::red;
   PenInfo[5].PenColor = Qt::magenta;
   PenInfo[6].PenColor = Qt::white;
   PenInfo[7].PenColor = Qt::yellow;
   BackgroundColor = QColor(30, 30, 30, 255);
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
   scribbleArea->setPostItBackgroundColor(QColor(255-BackgroundColor.red(), 255-BackgroundColor.green(), 255-BackgroundColor.blue(), 40));
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

//! [7]
void MainWindow::penColor()
//! [7] //! [8]
{
    QColor newColor = QColorDialog::getColor(scribbleArea->penColor());
    if (newColor.isValid())
        scribbleArea->setPenColor(newColor);
}
//! [8]
void MainWindow::BackGroundColorColor()
//! [7] //! [8]
{
    QColor newColor = QColorDialog::getColor(scribbleArea->GetBackGroundColor());
    if (newColor.isValid()) {
       scribbleArea->setBackGroundColor(newColor);
       scribbleArea->setPostItBackgroundColor(QColor(255-newColor.red(), 255-newColor.green(), 255-newColor.blue(), 40));

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

    BlackBoardColorAct = new QAction(tr("&Blackboard"), this);
    connect(BlackBoardColorAct, SIGNAL(triggered()), this, SLOT(BackGroundColorBlackBoard()));


    DirectPostitSelectAct = new QAction(tr("&Direct select"), this);
    DirectPostitSelectAct->setCheckable(true);
    connect(DirectPostitSelectAct, SIGNAL(triggered()), this, SLOT(DirectSelect()));

    ShowPostitsFrameAct = new QAction(tr("&Postit Frame"), this);
    ShowPostitsFrameAct->setCheckable(true);
    connect(ShowPostitsFrameAct, SIGNAL(triggered()), this, SLOT(ShowPostitsFrame()));

    clearScreenAct = new QAction(tr("&Clear Screen"), this);
    clearScreenAct->setShortcut(tr("Ctrl+L"));
    connect(clearScreenAct, SIGNAL(triggered()),
            scribbleArea, SLOT(clearImage()));

    aboutAct = new QAction(tr("&About"), this);
    connect(aboutAct, SIGNAL(triggered()), this, SLOT(about()));

    aboutQtAct = new QAction(tr("About &Qt"), this);
    connect(aboutQtAct, SIGNAL(triggered()), qApp, SLOT(aboutQt()));
}
//! [14]

//! [15]
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
    fileMenu->addSeparator();
    fileMenu->addAction(exitAct);

    optionMenu = new QMenu(tr("&Options"), this);
    optionMenu->addAction(penColorAct);
    optionMenu->addAction(penWidthAct);
    optionMenu->addAction(BackGroundColorAct);
    optionMenu->addAction(WhiteBoardColorAct);
    optionMenu->addAction(BlackBoardColorAct);
    optionMenu->addAction(DirectPostitSelectAct);
    optionMenu->addSeparator();
    optionMenu->addAction(clearScreenAct);

    DebugMenu = new QMenu(tr("&Debug"), this);
    DebugMenu->addAction(ShowPostitsFrameAct);

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
    QSettings settings("OESCH", "OverheadProjector");

    settings.beginGroup("Drawing");
    settings.beginWriteArray("Pens");
    for (qsizetype i = 0; i < NumberOfPens; ++i) {
        settings.setArrayIndex(i);
        settings.setValue("Color", PenInfo[i].PenColor);
    }
    settings.endArray();
    settings.setValue("BackgroundColor", scribbleArea->GetBackGroundColor());

    settings.beginGroup("SensorNames");
    settings.endGroup();

    settings.endGroup();
}

void MainWindow::readSettings()
{
   QSettings settings("OESCH", "OverheadProjector");

    settings.beginGroup("Drawing");
    int size = settings.beginReadArray("Pens");
    for (int i = 0; i < size; ++i) {
        settings.setArrayIndex(i);
        if (settings.value("Color").canConvert<QString>()) {
            PenInfo[i].PenColor = settings.value("Color").value<QColor>();
        }
    }
    settings.endArray();
    BackgroundColor = settings.value("BackgroundColor").value<QColor>();
    settings.beginGroup("SensorNames");
    settings.endGroup();
    settings.endGroup();
}


//! [20]
