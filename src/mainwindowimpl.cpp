#include <QString>
#include <QDebug>

#include "qwt_plot_curve.h"
#include "qwt_symbol.h"

#include "nodecollection.h"
#include "nodeplotcurve.h"
#include "mainwindowimpl.h"

MainWindowImpl::MainWindowImpl( QWidget * parent, Qt::WFlags f) 
	: QMainWindow(parent, f)
{
	setupUi(this);
	kmeansTimer = 0;
	connect( pushButton, SIGNAL(clicked()), this, SLOT(on_actionProcess_triggered()) ); 

	QStringList l;
	tableWidget->setRowCount( SIZE_Y );
	tableWidget->setColumnCount( SIZE_X );	
	for( int x=0; x<SIZE_X; x++ )
	{
		l << QString::number( x+1, 16 ).toUpper();
		tableWidget->columnWidth( 25 ); 
	}
	tableWidget->setHorizontalHeaderLabels(l);
	addDockWidget(Qt::RightDockWidgetArea, dockWidget);

	QwtSymbol symbol;
	symbol.setStyle(QwtSymbol::XCross);
	symbol.setSize(QSize(6, 6));

	kmeansList = new NodeCollection;
	kmeansSamples = new NodesPlotCurve("KMeans");
	kmeansSamples->setStyle(QwtPlotCurve::NoCurve);
	kmeansSamples->setSymbol(symbol);
	kmeansSamples->attach(kmeansPlot);
	kmeansPlot->setCanvasBackground(QColor(Qt::black));

	claraList = new NodeCollection;
	claraSamples = new NodesPlotCurve("Clara");
	claraSamples->setStyle(QwtPlotCurve::NoCurve);
	claraSamples->setSymbol(symbol);
	claraSamples->attach(claraPlot);
}

void MainWindowImpl::setRandomData( int dimentions )
{
	tableWidget->clear();
	for( int x=0; x<MAX_VALUE; x++ )
		for ( int y=0; y<dimentions; y++ )
			tableWidget->setItem( x, y, new QTableWidgetItem(QString::number(rand()%MAX_VALUE)));
	
	kmeansList->readData(tableWidget);
	//claraList->readData(tableWidget);
	on_actionProcess_triggered();
}

void MainWindowImpl::on_actionRandom2D_triggered(bool checked)
{
	setRandomData( 2 );
}

void MainWindowImpl::on_actionRandom3D_triggered()
{
	setRandomData( 3 );
}


void MainWindowImpl::on_actionProcess_triggered()
{
	kmeansList->KMeans_init( centroidsNumber->value() );
	kmeansSamples->setNodeCollection( kmeansList );
	kmeansPlot->replot();
	
	if (kmeansTimer!=0)
		killTimer( kmeansTimer );
	kmeansTimer = startTimer( 500 );
}

void MainWindowImpl::timerEvent(QTimerEvent *event)
{
	kmeansList->KMeans_calculateNewCentroids();
	kmeansList->calculateAssociations();
	kmeansSamples->setNodeCollection( kmeansList );
	kmeansPlot->replot();	
}
