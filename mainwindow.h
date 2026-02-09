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

#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QList>
#include <QMainWindow>
#include "Settings.hpp"
#include <QTimer>
#include <QFileInfo>
#include <QDir>

class ScribbleArea;
class QActionGroup;

//! [0]
class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow();

signals:
    void ValueChanged();


protected:
    void closeEvent(QCloseEvent *event) Q_DECL_OVERRIDE;

private slots:
    void open();
    void Export();
    void ImportToLayer();
    void penColor();
    void BackGroundColorColor();
    void BackGroundColorWhiteBoard();
    void BackGroundColorOHP();
    void BackGroundColorBlackBoard();
    void penWidth();
    void about();

    void Save();
    void ProtectImage();
    void DirectSelect();
    void ShowPostitsFrame();
    void ShowCursors();
    void createLayerActions(int Number);
    void SetVisibilityIndicatorOfLayer(int Layer, bool Visibility);
    void ShowOverviewChanged(bool OverviewEnabled);
    void SwitchToNextPenColor();

    void ShowSettings();



    void UpdatePasteState();
    void Paste();
private:
    void createActions();
    void createMenus();
    bool maybeSave();
    bool ExportFile(const QByteArray &fileFormat);

    //QString DefaultFileName;
    static constexpr auto DefaultExtension = "ohp";
    static constexpr auto AutosaveExtension = ".autosave";

    QFileInfo CurrentFile;
    QFileInfo CurrentAutosaveFile;

    ScribbleArea *scribbleArea;
    class SettingClass Settings;


    QMenu *saveAsMenu;
    QMenu *fileMenu;
    QMenu *optionMenu;
    QMenu *DebugMenu;
    QMenu *helpMenu;

    QToolBar * LayerToolBar;

    static constexpr auto NumberOfPens = 8;
    struct PenInfo {
       QAction *SetPenColorAct;
       QColor  PenColor;
    };
    QActionGroup *PenColorGroup;

    PenInfo PenInfo[NumberOfPens];
    QColor BackgroundColor;
    QColor ScrollHintColor;
    QColor SelectionHintColor;
    QColor PostItBackgroundColor;


    QAction *openAct;
    QAction *saveAct;
    QList<QAction *> saveAsActs;
    QAction *ImportAsLayerAct;
    QAction *PasteAct;
    QAction *CopyAct;
    QAction *exitAct;
    QAction *penColorAct;
    QAction *BackGroundColorAct;
    QAction *WhiteBoardColorAct;
    QAction *OHPColorAct;
    QAction *BlackBoardColorAct;
    QAction *penWidthAct;
    QAction *DirectPostitSelectAct;
    QAction *ShowPostitsFrameAct;
    QAction *ShowGestureInfosAct;
    QAction *ShowCursorsAct;
    QAction *ShowOverviewAct;
    QAction *printAct;
    QAction *clearScreenAct;
    QAction *SettingsAct;
    QAction *aboutAct;
    QAction *aboutQtAct;


    bool SaveFile(const QByteArray &fileFormat);
    void SetWhiteBoardColors();
    void SetOHPColors();

    void readSettings();
    void writeSettings();
    void SetBlackBoardColors();
    void UpdateColors();
    void SetMatchingPostitColor(QColor &newColor);


    // QWidget interface
    void ShowGestureInfos();
};
//! [0]

#endif
