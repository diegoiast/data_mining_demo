#include <math.h>
#include <QString>
#include "dataset.h"

DataSet::DataSet()
{
	// TODO
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
		p.setX( m_centroids.at(number)[0] );
		p.setY( m_centroids.at(number)[1] );
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

/// kmeans algorithm - initialize the centroids to a random items
void DataSet::KMeans_init( int centroidsNumber )
{
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
	
	foreach( DataSetItem *item, m_items )
	{
		m_centroids[medoidToCalculate] = item->m_coordinates;
		calculateAssociations();
		double currentScattering = scattering();
		
		if ((bestScatter == -1) || ( currentScattering < bestScatter))
		{
			bestScatter = currentScattering;
			bestMedoid = item;
		}
		else	// revert the medoid association
			m_centroids[medoidToCalculate] = bestMedoid->m_coordinates;
	}
}

void DataSet::PAM_stage2()
{
	// now, we choose a random demoid and assign it a new item. if it's better -> we win
	int k = rand() % m_centroids.count();
	int l = rand() % m_items.count();
	
	//qDebug("%s - %d: choosing a new medoid: %d->[%f:%f]", __FILE__, __LINE__, k, m_items[l]->m_coordinates[0], m_items[l]->m_coordinates[1] );

	double oldScatter = scattering();
	QVector<double> oldCoordinates = m_centroids[k];
	m_centroids[k] = m_items[l]->m_coordinates;
	calculateAssociations();
	double newScatter = scattering();
	 
	if (oldScatter < newScatter )
	{	// revert the association
		m_centroids[k] = oldCoordinates;
		calculateAssociations(); 
		qDebug("%s - %d: reverting medoid, scattering (%f->%f)", __FILE__, __LINE__, oldScatter, newScatter );
	}
	else
		qDebug("%s - %d: using medoid, scattering (%f->%f)", __FILE__, __LINE__, oldScatter, newScatter );
}
