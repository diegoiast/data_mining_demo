#ifndef DATASET_H
#define DATASET_H

#include <QVector>
#include <QList>
#include <QPointF>

/// class which represents a single node in the dataset
/// this is just a POD (plain old data) class, does not do anything smart
class DataSetItem
{
public:
	DataSetItem( double xx, double yy, int cc=-1, double dd=-1 ) {
		m_coordinates << xx;
		m_coordinates << yy;
		m_cluster = cc;
		m_distance = dd;
	};
	DataSetItem( QVector<double> coordinates_in, int cc=-1, double dd=-1 ) {
		m_coordinates = coordinates_in;
		m_cluster = cc;
		m_distance = dd;
	};
	
	/// return the 2D coordinates of this data set item
	/// note that this is not always valid
	QPointF location() {
		return QPointF( m_coordinates[0],m_coordinates[0] );
	}
	
	/// the coordinates of this node. is not ganranteed to be a 2D
	/// dataset item
	QVector<double> m_coordinates;
	
	/// the cluster this item belongs to. -1 notes it has not been
	/// assigned a cluster 
	int m_cluster;
	
	/// the distance to the closest cluster
	double m_distance;
};

class DataSet
{

public:
	DataSet();
	~DataSet();
	
	void clear();
	
	int		getItemCount();
	void		addItem( double x, double y );
	void		addItem( QVector<double> coordinates );
	DataSetItem*	getItem( int number );
	double		distance( QVector<double> &p1, QVector<double> &p2 );
	double		distance_euclid( QVector<double> &p1, QVector<double> &p2 );
	double		distance_manhattan( QVector<double> &p1, QVector<double> &p2 );
	
	QVector<double>	getCentroid( int number );
	QPointF		getCentroidPF( int number );
	int		getCentroidCount();

	/// calculate the best association of each item to the closest centroid 
	void calculateAssociations();

	/// kmeans algorithm - initialize the centroids to a random items
	void KMeans_init( int centroidsNumber );	
	/// calculate a new set of centroids by calculating the center of each centroid
	void KMeans_calculateNewCentroids();
	
private:
	QList <DataSetItem*>		m_items;
	QList< QVector<double> >	m_centroids;
};

#endif
