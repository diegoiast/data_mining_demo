/**
 * \file dataset.h
 * \brief Definitions of DataSet
 * \author Diego Iastrubni elcuco@kde.org
 * License GPLv3, 2008,2009
 */

#ifndef DATASET_H
#define DATASET_H

#include <vector>
#include <QList>
#include <QPointF>

/// class which represents a single node in the dataset
/// this is just a POD (plain old data) class, does not do anything smart
class DataSetItem
{
public:
	DataSetItem( double xx, double yy, int cc=-1, double dd=-1 ) {
		m_coordinates.push_back(xx);
		m_coordinates.push_back(yy);
		m_cluster = cc;
		m_distance = dd;
	};

	DataSetItem( std::vector<double> coordinates_in, int cc=-1, double dd=-1 ) {
		m_coordinates = coordinates_in;
		m_cluster = cc;
		m_distance = dd;
	};
/*	/// return the 2D coordinates of this data set item
	/// note that this is not always valid
	QPointF location() {
		return QPointF( m_coordinates[0],m_coordinates[0] );
	}
*/	
	
	/// the coordinates of this node. is not ganranteed to be a 2D
	/// dataset item
	std::vector<double> m_coordinates;
	
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
	void		addItem( std::vector<double> coordinates );
	DataSetItem*	getItem( size_t number );
	double		distance( std::vector<double> &p1, std::vector<double> &p2 );
	double		distance_euclid( std::vector<double> &p1, std::vector<double> &p2 );
	double		distance_manhattan( std::vector<double> &p1, std::vector<double> &p2 );
	
	std::vector<double>	getCentroid( size_t number );
//	QPointF		getCentroidPF( int number );
	int			getCentroidCount();
	
	// helper functions for PAM
	double		scattering();
	int		nextUnassociatedMedoid();
	
	// helper function for Clara
	DataSet*	reducedDataSet( float factor );
	

	/// calculate the best association of each item to the closest centroid
	/// used by clara and PAM 
	void calculateAssociations();

//- K-Means ---------------------------------------------------------------
	/// KMeans algorithm - initialize the centroids to a random items
	void KMeans_init( int centroidsNumber );	
	/// KMeans algorithm - calculate a new set of centroids by calculating the center of each centroid
	void KMeans_calculateNewCentroids();
	
	
//- PAM -------------------------------------------------------------------
	/// PAM algorithm - initialize the centroids to a random items
	void PAM_init( int centroidsNumber );
	/// PAM algorithm - calculate a new set of centroids by calculating the center of each centroid
	void PAM_calculateNewCentroids();
	/// PAM stage 1 - find the best medoids, still not reached k
	void PAM_stage1( int medoidToCalculate );	
	// PAM stage 2 - try all combinations of medoids/items
	void PAM_stage2();

//- Clara -----------------------------------------------------------------
	/// PAM algorithm - initialize the centroids to a random items
	void Clara_init( int centroidsNumber );
	/// PAM algorithm - calculate a new set of centroids by calculating the center of each centroid
	void Clara_calculateNewCentroids();
	
private:
	bool				m_finished;
	std::vector<DataSetItem*>		m_items;
	std::vector< std::vector<double> >	m_centroids;
	
	// PAM shite
	int m_pamStage2_medoid;
	int m_pamStage2_item;
	
	// Clara shite
	DataSet	*claraMiniPams[5]; 
};

#endif
