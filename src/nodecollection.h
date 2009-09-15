#ifndef NODECOLLECTION_H
#define NODECOLLECTION_H

#include <QList>
#include <qwt_data.h>

class QTableWidget;
class NodesPlotCurve;

#define SIZE_X		256
#define SIZE_Y		1024
#define MAX_VALUE	1000
 

class MyNode
{
public:
	MyNode( double xx, double yy, int cc=-1, double dd=-1 ) {
		x = xx;
		y = yy;
		cluster = cc;
		d = dd;
	};
	QPointF location() {
		return QPointF(x,y);
	}
	double x;
	double y;
	int cluster;
	double d;
};


class NodeCollection : public QwtData
{
public:
friend class NodesPlotCurve;
	NodeCollection();
	void readData( QTableWidget * );
	void calculateAssociations();
	
	// kmeans algorithm
	void KMeans_init( int centroidsNumber );
	void KMeans_calculateNewCentroids();
	
	// implementation of needed interfaces of QetData
	QwtData*	copy() const;
	size_t		size() const;
	double		x( size_t i ) const;
	double		y( size_t i ) const;
	int 		cluster( size_t ) const;
	
private:
	QList<MyNode*> nodes;
	QList<QPointF> centroids;
};

#endif
