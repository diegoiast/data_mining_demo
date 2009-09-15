/**
 * \file datasetview.cpp
 * \brief Implementation of DataSetView - a proxy to QWT plot classes
 * \author Diego Iastrubni elcuco@kde.org
 * License GPLv3, 2008,2009
 */

#include "dataset.h"
#include "datasetview.h"


DataSetView::DataSetView( DataSet *n ) 
{
	m_dataSet = n;
}

QwtData*	DataSetView::copy() const
{
	DataSetView *newView = new DataSetView( m_dataSet );
	return newView;
}

size_t	DataSetView::size() const
{
	if (m_dataSet == NULL)
		return 0;
	else
	{
		size_t i = m_dataSet->getItemCount();
		return i;
	}
		
}

double	DataSetView::x( size_t i ) const
{
	if (m_dataSet == NULL)
		return 0;

	DataSetItem *item = m_dataSet->getItem( i );
	if (item==NULL)
		return 0;
		
	if (item->m_coordinates.count()<1)
		return 0;
	
	// remember, coordinates are 0 based 
	return item->m_coordinates[0];
}

double	DataSetView::y( size_t i ) const
{
	if (m_dataSet == NULL)
		return 0;

	DataSetItem *item = m_dataSet->getItem( i );
	if (item==NULL)
		return 0;
		
	if (item->m_coordinates.count()<2)
		return 0;
	
	// remember, coordinates are 0 based 
	return item->m_coordinates[1];
}

int	DataSetView::cluster( size_t i ) const
{
	if (m_dataSet == NULL)
		return 0;

	DataSetItem *item = m_dataSet->getItem( i );
	if (item==NULL)
		return -1;
		
	return item->m_cluster;	
}

DataSet* DataSetView::getDataSet()
{
	return m_dataSet;
}
