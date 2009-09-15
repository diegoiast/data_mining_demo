#include <QString>
#include <QDebug>

#include "qwt_plot_curve.h"
#include "qwt_symbol.h"

#include "mainwindowimpl.h"

#define SIZE_X		256
#define SIZE_Y		1024
#define MAX_VALUE	1000

class MyNode: public QObject
{
public:
	MyNode( double xx, double yy ) {
		x = xx;
		y = yy;
		cluster = -1;
		d = -1;
	};
	double x;
	double y;
	int cluster;
	double d;
};

Qt::GlobalColor clusterColors[] = 
{
	Qt::red, 
	Qt::green,
	Qt::blue,
	Qt::darkCyan,
	Qt::darkMagenta,
	Qt::darkYellow,
	Qt::lightGray,
	Qt::black
};

MainWindowImpl::MainWindowImpl( QWidget * parent, Qt::WFlags f) 
	: QMainWindow(parent, f)
{
	setupUi(this);

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
	kmeansSamples = new QwtPlotCurve();
	kmeansSamples->setStyle(QwtPlotCurve::NoCurve);
	kmeansSamples->setSymbol(symbol);
	kmeansSamples->attach(kmeansPlot);

	claraSamples = new QwtPlotCurve();
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
	
	updateData();
	calculateKMeans();
	updatePlots();
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
	// todo	
}

void MainWindowImpl::updateData()
{
	kmeansList.clear();
	for ( int y=0; y<SIZE_Y; y++ )
	{
		bool ok;
		QVariant valX;
		QVariant valY;

		if (tableWidget->item( y, 0 ))
			valX = tableWidget->item( y, 0 )->data( Qt::DisplayRole );
		if (tableWidget->item( y, 1 ))
			valY = tableWidget->item( y, 1 )->data( Qt::DisplayRole );
		if (!valX.isValid() || ! valY.isValid())
			continue;
		double vx = valX.toDouble(&ok);
		if (!ok)
			continue;
		double vy = valY.toDouble(&ok);
		if (!ok)
			continue;

		kmeansList << new MyNode( vx, vy );
		claraList << new MyNode( vx, vy );
	}

}

void MainWindowImpl::updatePlots()
{
	QPolygonF polygon;
	
	foreach( QObject *o,  kmeansList )
	{
		MyNode *node = dynamic_cast<MyNode *>(o);
		if (node)
			polygon << QPointF( node->x, node->y );
	}
	QwtPolygonFData d(polygon);
	kmeansSamples->setData( d );
	kmeansPlot->replot();

	claraSamples->setData( d );
	claraPlot->replot();
}

double d( MyNode *a, MyNode *b)
{
	return sqrt( (a->x *a->x) + (a->y *a->y) ); 
}

void MainWindowImpl::calculateKMeans()
{
	QList<QObject*> centroids;

	qDebug() << "Kmeans start";
		
	// choose number of centroids
	for( int i=0; i<centroidsNumber->value(); i++ )
	{
		int k = rand() % kmeansList.count();
		MyNode *node = dynamic_cast<MyNode *>(kmeansList.at( k ));
		
		if (!node) 
			continue;
		qDebug() << "choosing item (" << node->x << "," << node->y << ") as centroid"; 
		centroids << kmeansList.at( k );
	}
	
	// assign each node to a cluster
	foreach( QObject *o,  kmeansList )
	{
		MyNode *node = dynamic_cast<MyNode *>(o);
		// just to be safe
		if (!node)
			continue;
		
		node->cluster = 0;
		
		for( int i=0; i<centroidsNumber->value(); i++ )
		{
			MyNode *centroid = dynamic_cast<MyNode *>(kmeansList.at(i ));			
			if (!centroid) 
				continue;
				
			double d1 = d( node,centroid );
			if (d1 < node->d)
			{
				node->d = d1;
				node->cluster = i;
			}
		}
	}
	
}
