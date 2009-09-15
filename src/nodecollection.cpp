#include <math.h>
#include <QTableWidget>
#include "nodecollection.h"

#include <QDebug>

void test()
{
	NodeCollection c;
}

double d( QPointF a, QPointF b)
{
	double dx = a.x() - b.x();
	double dy = a.y() - b.y();
	double distance = sqrt( (dx * dx) + (dy * dy) );
	
	//qDebug( "%s - %d: distance [%f,%f] [%f,%f] is %f", __FILE__, __LINE__, a.x(), a.y(), b.x(), b.y(), distance );
	return distance; 
}


NodeCollection::NodeCollection(  ) 
	: QwtData()
{
	// TODO
}

void NodeCollection::readData( QTableWidget * tableWidget )
{
	nodes.clear();
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

		nodes << new MyNode( vx, vy );
	}
}

void NodeCollection::calculateAssociations()
{
	// clear older assosiation
	foreach( MyNode *node,  nodes )
	{
		node->d = -1;
		node->cluster = -1;
	}
	
	// assign each node to a cluster
	foreach( MyNode *node,  nodes )
	{
		node->cluster = 0;
		
		for( int i=0; i<centroids.size(); i++ )
		{
			QPointF centroid = centroids.at( i );
			double d1 = d( node->location(),centroid );

			if ((node->d == -1) || (d1 < node->d))
			{
				node->d = d1;
				node->cluster = i;
			}
		}
	}
}

void NodeCollection::KMeans_init( int centroidsNumber )
{
	// choose number of centroids
	centroids.clear();
	for( int i=0; i<centroidsNumber; i++ )
	{
		int k = rand() % nodes.count();
		MyNode *node = nodes.at( k );
		
		if (!node) 
			continue;
		centroids << nodes.at( k )->location();
	}
	
	calculateAssociations();
}

void NodeCollection::KMeans_calculateNewCentroids()
{
	QList<QPointF> newCentroids;
	
	for( int i=0; i<centroids.size(); i++ )
	{
		double sx = 0;
		double sy = 0;
		int count = 0;
		
		foreach( MyNode *node,  nodes )
		{
			if (node->cluster != i )
				continue;
			sx += node->x;
			sy += node->y;
			count ++;
		}
		newCentroids << QPointF( sx/count, sy/count );
	}
	
	centroids = newCentroids ;
}

// implementation of needed interfaces
QwtData* NodeCollection::copy() const
{
	NodeCollection *a = new NodeCollection;
	a->nodes = nodes;
	return a;
}

size_t NodeCollection::size() const
{
	return nodes.size();
}

double NodeCollection::x( size_t i ) const
{
	if ( (int)i >= nodes.size() )
	{
		qDebug("%s - %d: asked for a point out of bounds(%d)", __FILE__, __LINE__, i );
		return 0;
	}
	return nodes[i]->x;
}

double NodeCollection::y( size_t i ) const
{
	if ( (int)i >= nodes.size() )
	{
		qDebug("%s - %d: asked for a point out of bounds(%d)", __FILE__, __LINE__, i );
		return 0;
	}
	return nodes[i]->y;
}

int NodeCollection::cluster( size_t i ) const
{
	if ( (int)i >= nodes.size() )
		return 0;
	return nodes[i]->cluster;
}
