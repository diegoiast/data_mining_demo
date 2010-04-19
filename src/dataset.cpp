/**
 * \file dataset.cpp
 * \brief Implementation of DataSet
 * \author Diego Iastrubni elcuco@kde.org
 * License GPLv3, 2008,2009
 */

#include <cstdlib>
#include <cmath>
#include <QDebug>

#include "dataset.h"

// un comment this for large debug output
// #define __DATASET_DEBUG_

DataSet::DataSet()
{
	// TODO
	for( int i=0; i<5; i++ )
		claraMiniPams[i] = NULL;
}

DataSet::~DataSet()
{
	// TODO
}

void	DataSet::clear()
{
	m_items.clear();
	m_centroids.clear();
}

int	DataSet::getItemCount()
{
	return m_items.size();
}

void	DataSet::addItem( double x, double y )
{
	m_items.push_back(new DataSetItem( x, y ));
}

void	DataSet::addItem( std::vector<double> coordinates )
{
	m_items.push_back(new DataSetItem(coordinates));
}

DataSetItem*	DataSet::getItem( size_t number )
{
	if (number < m_items.size())
		return m_items.at(number);
	else
		return NULL;
}

// assumption: both p1 and p2 have the same number of
// items, oordinates
double	DataSet::distance( std::vector<double> &p1, std::vector<double> &p2 )
{
	return distance_euclid( p1, p2 );
	//return distance_manhattan( p1, p2 );
}

double	DataSet::distance_euclid( std::vector<double> &p1, std::vector<double> &p2 )
{
	double d = 0;
	std::vector<double>::iterator i1, i2;
	for( i1=p1.begin(), i2=p2.begin(); i1!=p1.end() && i2!=p2.end(); i1++, i2++) {
		double f = (*i1) - (*i2);
		d += f*f;
	}
	return sqrt(d);
}

double	DataSet::distance_manhattan( std::vector<double> &p1, std::vector<double> &p2 )
{
	double d = 0;
	
	std::vector<double>::iterator i1, i2;
	for( i1=p1.begin(), i2=p2.begin(); i1!=p1.end() && i2!=p2.end(); i1++, i2++) {
		d += fabs((*i1)-(*i2));
	}
	
	return d;
}


std::vector<double>	DataSet::getCentroid( size_t number )
{
	std::vector<double> v;
	if (number <= m_centroids.size())
		v = m_centroids.at(number);
		
	return v;
}

/*
QPointF	DataSet::getCentroidPF( int number )
{
	QPointF p;
	if (number <= m_centroids.size())
	{
                QVector<double> centroid = m_centroids.at(number);
                if (centroid.count() == 2)
                {
                    p.setX( centroid[0] );
                    p.setY( centroid[1] );
                }
                else
                    qDebug( "Warning: empty centroid at %d", number );
        }
		
	return p;
}
*/
int	DataSet::getCentroidCount()
{
	return m_centroids.size();
}

//
double	DataSet::scattering()
{
	double d = 0;
	std::vector<DataSetItem*>::iterator i;
	for(i=m_items.begin(); i!=m_items.end(); i++){
		d += (*i)->m_distance;
	}
	
	return d;
}

int	DataSet::nextUnassociatedMedoid()
{
	int n = 0;
	std::vector< std::vector<double> >::iterator i;
	for( i=m_centroids.begin(); i!=m_centroids.end(); i++){
		if ((*i).size() != 0)
			n ++;
	}
	
	return n;
}

/// calculate the best association of each item to the closest centroid 
void	DataSet::calculateAssociations()
{
	std::vector<DataSetItem*>::iterator item;
	// clear older assosiation
	for( item=m_items.begin(); item!=m_items.end(); item++ ){
		(*item)->m_distance = -1;
		(*item)->m_cluster = -1;
	}
	
	// assign each node to a cluster
	for( item=m_items.begin(); item!=m_items.end(); item++ ){
		(*item)->m_cluster  = -1;
		(*item)->m_distance = -1;
		
		std::vector< std::vector<double> >::iterator centroid;
		int k = 0;
		for( centroid=m_centroids.begin(); centroid!=m_centroids.end(); centroid++){
			// what if this centroid/medoid has not been assigned a coordinate yet?
			if ( (*centroid).size() == 0)
				continue;
				
			double d = distance((*item)->m_coordinates,*centroid);
			if (((*item)->m_distance == -1) || (d < (*item)->m_distance)) {
				(*item)->m_distance = d;
				(*item)->m_cluster  = k;
			}
			k++;
		}
	}
}

DataSet* DataSet::reducedDataSet( float factor )
{
	long newItemCount = lround( factor * m_items.size()); 
	DataSet *newDataSet = new DataSet;
	
	for (int i=0; i<newItemCount; i ++ ){
		// choose a new random item
		int m = std::rand() % m_items.size();
		DataSetItem *item = new DataSetItem( m_items[m]->m_coordinates );
		newDataSet->m_items.push_back(item);
	}
	
	return newDataSet;
}

/// kmeans algorithm - initialize the centroids to a random items
void DataSet::KMeans_init( int centroidsNumber )
{
        m_finished = false;
	// choose number of centroids
	m_centroids.clear();
	if (m_items.size() != 0) {
		for(int i=0;i<centroidsNumber;i++){
			int k = rand() % m_items.size();
			DataSetItem *item = m_items.at( k );
			if (!item) {
				qDebug("%s - %d - warning, choosed a centroid index outside the dataset", __FILE__, __LINE__ );
				continue;
			}
			m_centroids.push_back(item->m_coordinates);
		}
	}
	calculateAssociations();
}

/// kmeans algorithm - calculate a new set of centroids by calculating the center of each cluster
void DataSet::KMeans_calculateNewCentroids()
{
	std::vector< std::vector<double> > newCentroids;
	
	for( size_t k=0; k<m_centroids.size(); k++ ) {
		int count = 0;
		std::vector<double> coordinates;
		coordinates.push_back(0);
		newCentroids.push_back(coordinates);
		
		std::vector<DataSetItem*>::iterator item;
		for(item=m_items.begin(); item!=m_items.end(); item++) {
			if ((*item)->m_cluster != k )
				continue;
			for( size_t j=0; j<(*item)->m_coordinates.size(); j++ ) {
				// if this new item has more coordinates then the centroid
				// add a new zero coordinate to the centroid
				if (newCentroids[k].size()-1 < j)
					newCentroids[k].push_back(0);
				newCentroids[k][j] += (*item)->m_coordinates[j];
			}
			count ++;
		}
		for( int j=0; j< newCentroids[k].size(); j++ )
			newCentroids[k][j] = newCentroids[k][j]/count;
	}
	m_centroids = newCentroids;
}

// PAM implementation

void DataSet::PAM_init( int centroidsNumber )
{
	m_finished = false;
	// choose number of centroids
	m_centroids.clear();
	if (m_items.size() == 0)
		return;
	
	// assign new medoids, with empty coordinates
	for( int i=0; i<centroidsNumber; i++ ) {
		std::vector<double> d;
		m_centroids.push_back( d );
	}
	calculateAssociations();
}

void DataSet::PAM_calculateNewCentroids()
{
	int k = nextUnassociatedMedoid();
	
	if (k != m_centroids.size()){
		// we still need to find the first k medoids
		PAM_stage1(k);
	}
	else {	
		// find better medoids
		PAM_stage2();
	}
}

void DataSet::PAM_stage1( int medoidToCalculate )
{
	
	DataSetItem *bestMedoid = NULL;
	double bestScatter = -1;
	
	std::vector<DataSetItem*>::iterator item;
	for( item=m_items.begin(); item!=m_items.end(); item++){
		m_centroids[medoidToCalculate] = (*item)->m_coordinates;
		calculateAssociations();
		double currentScattering = scattering();
		
		if ((bestScatter == -1) || ( currentScattering < bestScatter)){
#ifdef __DATASET_DEBUG_
			qDebug("%s - %d: PAM_stage1 - choosing %d as centroid %d", __FILE__, __LINE__, m, medoidToCalculate );
#endif
			bestScatter = currentScattering;
			bestMedoid = *item;
		}
		else	// revert the medoid association
			m_centroids[medoidToCalculate] = bestMedoid->m_coordinates;
	}
	
	m_pamStage2_medoid = 0;
	m_pamStage2_item   = 0;
	m_finished         = false;
}

void DataSet::PAM_stage2()
{
	if (m_finished)
		return;
		
	// now, we choose a random demoid and assign it a new item. if it's better -> we win
	//int k = rand() % m_centroids.count();
	//int l = rand() % m_items.count();

	int k = m_pamStage2_medoid;
	int l = m_pamStage2_item;
	
	//qDebug("%s - %d: choosing a new medoid: %d->[%f:%f]", __FILE__, __LINE__, k, m_items[l]->m_coordinates[0], m_items[l]->m_coordinates[1] );
	//qDebug("%s - %d: pam stage 2, medoids: %d/%d, item: %d/%d", __FILE__, __LINE__, k, m_centroids.count(), l, m_items.count() );

	double oldScatter = scattering();
	std::vector<double> oldCoordinates = m_centroids[k];
	m_centroids[k] = m_items[l]->m_coordinates;
	calculateAssociations();
	double newScatter = scattering();
	 
	if (oldScatter < newScatter ) {
		// revert the association
		m_centroids[k] = oldCoordinates;
		calculateAssociations(); 
#ifdef __DATASET_DEBUG_
		qDebug("%s - %d: pam stage 2, reverting medoid, medoids: %d/%d, item: %d/%d, scattering (%f->%f)", __FILE__, __LINE__, k, m_centroids.count(), l, m_items.count(), oldScatter, newScatter );
#endif
	}
	else {
#ifdef __DATASET_DEBUG_
		qDebug("%s - %d: pam stage 2, using medoid, medoids: %d/%d, item: %d/%d, scattering (%f->%f)", __FILE__, __LINE__, k, m_centroids.count(), l, m_items.count(), oldScatter, newScatter );
#endif
	}
	
	m_pamStage2_item ++;
	if (m_pamStage2_item == m_items.size()) {
		m_pamStage2_item = 0;
		m_pamStage2_medoid++;
		
		if (m_pamStage2_medoid == m_centroids.size()) {
			m_finished = true;
#ifdef __DATASET_DEBUG_
			qDebug("%s - %d: Finished PAM", __FILE__, __LINE__ );
#endif
		}
	}
}

void DataSet::Clara_init( int centroidsNumber )
{
	for (int i=0; i<5; i++ ) {
		claraMiniPams[i] = reducedDataSet( 0.2 );
		claraMiniPams[i]->PAM_init( centroidsNumber );
	}
	PAM_init( centroidsNumber );
}

void DataSet::Clara_calculateNewCentroids()
{
	if (m_finished)
		return;
		
	claraMiniPams[0]->PAM_calculateNewCentroids();
	m_finished &= claraMiniPams[0]->m_finished;
	int bestMiniPam = 0;
	double bestScattering = claraMiniPams[bestMiniPam]->scattering();
	m_centroids = claraMiniPams[bestMiniPam]->m_centroids;
	calculateAssociations();
	
	for( int i=1; i<5; i++ ) {
		claraMiniPams[i]->PAM_calculateNewCentroids();
		double newScattering = claraMiniPams[i]->scattering();
#ifdef __DATASET_DEBUG_
		qDebug("%s - %d: %s, using miniPam[%d] for calculations, scattering: %f->%f", __FILE__, __LINE__, __FUNCTION__, i, bestScattering, newScattering );
#endif
		if ( newScattering < bestScattering ) {
			bestScattering = newScattering;
			bestMiniPam = i;
			m_centroids = claraMiniPams[bestMiniPam]->m_centroids;
			calculateAssociations();
		}
		m_finished &= claraMiniPams[i]->m_finished;
	}
}
