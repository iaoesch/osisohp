#ifndef TABLETAPPLICATION_H
#define TABLETAPPLICATION_H
#include <QApplication>

#include "mainwindow.h"
#include <QTabletEvent>

class TabletApplication : public QApplication
{
    Q_OBJECT
    
public:
    using QApplication::QApplication;
    //  ~TabletApplication() override;
    //TabletApplication(int &argc, char **argv);
    
    
    bool event(QEvent *event) override;
    void setCanvas(MainWindow *canvas) { MyWindow = canvas; }
    
private:
    MainWindow *MyWindow = nullptr;
};

//TabletApplication::TabletApplication(int &argc, char **argv) : QApplication(argc, argv) {}



#endif // TABLETAPPLICATION_H
