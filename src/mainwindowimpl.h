/**
 * \file mainwindowimpl.h
 * \brief Definition of the main window
 * \author Diego Iastrubni elcuco@kde.org
 * License GPLv3, 2008,2009
 */


#ifndef MAINWINDOWIMPL_H
#define MAINWINDOWIMPL_H

#include <QMainWindow>
#include <QList>

#include "ui_mainwindow.h"

class QDockWidget;

class NodesPlotCurve;
class DataSet;
class DataSetView;

class MainWindowImpl : public QMainWindow, public Ui::MainWindow
{
Q_OBJECT
public:
	MainWindowImpl( QWidget* parent = 0, Qt::WFlags f = 0 );
	void setRandomData( int dimentions );
	void populateDataSet();
        void stopTheTimer();
	
private slots:
	void on_actionAbout_triggered();
	void on_actionLoad_triggered();
	void on_actionClearDataSet_triggered();
	void on_actionSwitchView_triggered();
        void on_actionRandom2D_triggered();
	void on_actionRandom3D_triggered();
	void on_actionProcess_triggered();
	void timerEvent(QTimerEvent *event);
private:
	int updateTimer; 
	
	// new code, MVC
	DataSet*	kmeansDataSet;
	DataSetView*	kmeansDataSetView;
	NodesPlotCurve* kmeansPlotCurve;

	DataSet*	claraDataSet;
	DataSetView*	claraDataSetView;
	NodesPlotCurve* claraPlotCurve;

	DataSet*	pamDataSet;
	DataSetView*	pamDataSetView;
	NodesPlotCurve* pamPlotCurve;
};

#endif
