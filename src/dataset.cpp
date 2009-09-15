/**
 * \file dataset.cpp
 * \brief Implementation of DataSet
 * \author Diego Iastrubni elcuco@kde.org
 * License GPLv3, 2008,2009
 */

#include <math.h>
#include <QString>
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
	m_items << new DataSetItem( x, y );
}

void	DataSet::addItem( QVector<double> coordinates )
{
	m_items << new DataSetItem( coordinates );
}

DataSetItem*	DataSet::getItem( int number )
{
	if (number < m_items.size())
		return m_items.at(number);
	else
		return NULL;
}

// assumption: both p1 and p2 have the same number of
// items, oordinates
double	DataSet::distance( QVector<double> &p1, QVector<double> &p2 )
{
	return distance_euclid( p1, p2 );
	//return distance_manhattan( p1, p2 );
}

double	DataSet::distance_euclid( QVector<double> &p1, QVector<double> &p2 )
{
	double d = 0;
	
	for( int i=0; i<p1.count(); i++ )
	{
		double f = p1[i] - p2[i];
		d += f*f;
	}
	
	return sqrt(d);
}

double	DataSet::distance_manhattan( QVector<double> &p1, QVector<double> &p2 )
{
	double d = 0;
	
	for( int i=0; i<p1.count(); i++ )
	{
		d += fabs(p1[i] - p2[i]);
	}
	
	return d;
}


QVector<double>	DataSet::getCentroid( int number )
{
	QVector<double> v;
	if (number <= m_centroids.size())
		v = m_centroids.at(number);
		
	return v;
}

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

int	DataSet::getCentroidCount()
{
	return m_centroids.count();
}

//
double	DataSet::scattering()
{
	double d = 0;
	foreach( DataSetItem *item, m_items )
	{
		d += item->m_distance;
	}
	
	return d;
}

int	DataSet::nextUnassociatedMedoid()
{
	int i = 0;
	for( int k=0; k<m_centroids.count(); k++ )
	{
		if (m_centroids[k].count() != 0)
			i ++;
	}
	
	return i;
}

/// calculate the best association of each item to the closest centroid 
void	DataSet::calculateAssociations()
{
	// clear older assosiation
	foreach( DataSetItem *item, m_items )
	{
		item->m_distance = -1;
		item->m_cluster = -1;
	}
	
	// assign each node to a cluster
	foreach( DataSetItem *item,  m_items )
	{
		item->m_cluster = -1;
		item->m_distance = -1;
		
		for( int k=0; k<m_centroids.count(); k++ )
		{
			QVector<double> centroid = m_centroids.at( k );
			
			// what if this centroid/medoid has not been assigned a coordinate yet?
			if (centroid.count() == 0)
				continue;
				
			double d = distance( item->m_coordinates,centroid );			
			if ((item->m_distance == -1) || (d < item->m_distance))
			{
				item->m_distance = d;
				item->m_cluster = k;
			}
		}
	}
}

DataSet* DataSet::reducedDataSet( float factor )
{
	long newItemCount = lround( factor * m_items.count()); 
	DataSet *newDataSet = new DataSet;
	
	for (int i=0; i<newItemCount; i ++ )
	{
		// choose a new random item
		int m = rand() % m_items.count();
		
		DataSetItem *item = new DataSetItem( m_items[m]->m_coordinates );
		newDataSet->m_items << item;
	}
	
	return newDataSet;
}

/// kmeans algorithm - initialize the centroids to a random items
void DataSet::KMeans_init( int centroidsNumber )
{
        m_finished = false;
	// choose number of centroids
	m_centroids.clear();
	if (m_items.count() != 0) 
		for( int i=0; i<centroidsNumber; i++ )
		{
			int k = rand() % m_items.count();
			DataSetItem *item = m_items.at( k );
			if (!item) 
				continue;
			
			m_centroids.append( item->m_coordinates );
		}
	
	calculateAssociations();
}

/// kmeans algorithm - calculate a new set of centroids by calculating the center of each cluster
void DataSet::KMeans_calculateNewCentroids()
{
	QList< QVector<double> > newCentroids;
	
	for( int k=0; k<m_centroids.size(); k++ )
	{
		int count = 0;
		QVector<double> d;
		
		// make a new meta centroid
		newCentroids.append( d );
		
		foreach( DataSetItem *item, m_items )
		{
			if (item->m_cluster != k )
				continue;
		
			for( int j=0; j< item->m_coordinates.size(); j++ )
			{
				// if this new item has more coordinates then the centroid
				// add a new zero coordinate to the centroid
				if (newCentroids[k].size()-1 < j)
					newCentroids[k].append( 0 );
				
				newCentroids[k][j] += item->m_coordinates[j];
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
	if (m_items.count() == 0)
		return;
 
 	// assign new medoids, with empty coordinates
	for( int i=0; i<centroidsNumber; i++ )
	{
		QVector<double> d;
		m_centroids.append( d );
	}
	
	
	calculateAssociations();
}

void DataSet::PAM_calculateNewCentroids()
{
	int k = nextUnassociatedMedoid();
	
	if (k != m_centroids.count())
	{	// we still need to find the first k medoids
		PAM_stage1( k );
	}
	else
	{	// find better medoids
		PAM_stage2();
	}
}

void DataSet::PAM_stage1( int medoidToCalculate )
{
	
	DataSetItem *bestMedoid = NULL;
	double bestScatter = -1;
	
	//foreach( DataSetItem *item, m_items )
	for( int m=0; m<m_items.count(); m++ )
	{
		DataSetItem *item = m_items[m];
		m_centroids[medoidToCalculate] = item->m_coordinates;
		calculateAssociations();
		double currentScattering = scattering();
		
		if ((bestScatter == -1) || ( currentScattering < bestScatter))
		{
#ifdef __DATASET_DEBUG_
                        qDebug("%s - %d: PAM_stage1 - choosing %d as centroid %d", __FILE__, __LINE__, m, medoidToCalculate );
#endif
			bestScatter = currentScattering;
			bestMedoid = item;
		}
		else	// revert the medoid association
			m_centroids[medoidToCalculate] = bestMedoid->m_coordinates;
	}
	
	m_pamStage2_medoid = 0;
	m_pamStage2_item = 0;
	m_finished = false;
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
	QVector<double> oldCoordinates = m_centroids[k];
	m_centroids[k] = m_items[l]->m_coordinates;
	calculateAssociations();
	double newScatter = scattering();
	 
	if (oldScatter < newScatter )
	{	// revert the association
		m_centroids[k] = oldCoordinates;
		calculateAssociations(); 

#ifdef __DATASET_DEBUG_
		qDebug("%s - %d: pam stage 2, reverting medoid, medoids: %d/%d, item: %d/%d, scattering (%f->%f)", __FILE__, __LINE__, k, m_centroids.count(), l, m_items.count(), oldScatter, newScatter );
#endif
	}
	else
        {
#ifdef __DATASET_DEBUG_
                qDebug("%s - %d: pam stage 2, using medoid, medoids: %d/%d, item: %d/%d, scattering (%f->%f)", __FILE__, __LINE__, k, m_centroids.count(), l, m_items.count(), oldScatter, newScatter );
#endif
        }
	
	m_pamStage2_item ++;
	if (m_pamStage2_item == m_items.count())
	{
		m_pamStage2_item = 0;
		m_pamStage2_medoid++;
		
		if (m_pamStage2_medoid == m_centroids.count())
		{
			m_finished = true;
#ifdef __DATASET_DEBUG_
                        qDebug("%s - %d: Finished PAM", __FILE__, __LINE__ );
#endif
                }
	}
}

void DataSet::Clara_init( int centroidsNumber )
{
        for( int i=0; i<5; i++ )
	{
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
	
	for( int i=1; i<5; i++ )
	{
		claraMiniPams[i]->PAM_calculateNewCentroids();
		double newScattering = claraMiniPams[i]->scattering();
#ifdef __DATASET_DEBUG_
                qDebug("%s - %d: %s, using miniPam[%d] for calculations, scattering: %f->%f", __FILE__, __LINE__, __FUNCTION__, i, bestScattering, newScattering );
#endif
                if ( newScattering < bestScattering )
		{
			bestScattering = newScattering;
			bestMiniPam = i;
			m_centroids = claraMiniPams[bestMiniPam]->m_centroids;
			calculateAssociations();
		}
		
		m_finished &= claraMiniPams[i]->m_finished;
	}
}
