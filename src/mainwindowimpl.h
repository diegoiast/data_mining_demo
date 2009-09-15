#ifndef MAINWINDOWIMPL_H
#define MAINWINDOWIMPL_H

#include <QMainWindow>
#include <QList>
#include "ui_mainwindow.h"

class QwtPlotCurve;

class MainWindowImpl : public QMainWindow, public Ui::MainWindow
{
Q_OBJECT
public:
	MainWindowImpl( QWidget * parent = 0, Qt::WFlags f = 0 );
	void setRandomData( int dimentions );
private slots:
	void on_actionRandom2D_triggered(bool checked);
	void on_actionRandom3D_triggered();
	void on_actionProcess_triggered();
private:
	void updateData();
	void updatePlots();
	void calculateKMeans();
	
	QwtPlotCurve* kmeansSamples;
	QwtPlotCurve* claraSamples;
	QList<QObject*> kmeansList; 
	QList<QObject*> claraList; 
};
#endif





