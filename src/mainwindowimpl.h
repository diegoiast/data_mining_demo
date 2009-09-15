#ifndef MAINWINDOWIMPL_H
#define MAINWINDOWIMPL_H

#include <QMainWindow>
#include <QList>
#include "ui_mainwindow.h"

#include "nodecollection.h"
#include "nodeplotcurve.h"

class NodesPlotCurve;
class NodeCollection;
//class QwtPlotCurve;

class MainWindowImpl : public QMainWindow, public Ui::MainWindow
{
Q_OBJECT
public:
	MainWindowImpl( QWidget* parent = 0, Qt::WFlags f = 0 );
	void setRandomData( int dimentions );
private slots:
	void on_actionRandom2D_triggered(bool checked);
	void on_actionRandom3D_triggered();
	void on_actionProcess_triggered();
	void timerEvent(QTimerEvent *event);
private:
	NodesPlotCurve* kmeansSamples;
	NodesPlotCurve* claraSamples;
	
	NodeCollection* claraList; 
	NodeCollection* kmeansList;
	int kmeansTimer; 
};

#endif
