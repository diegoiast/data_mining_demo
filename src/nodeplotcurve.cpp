#include <QPainter>
#include <QBrush>
#include <QPoint>
#include <QDebug>

#include "qwt_scale_map.h"
#include "qwt_symbol.h"
#include "nodeplotcurve.h"

#include "datasetview.h"
#include "dataset.h"


Qt::GlobalColor clusterColors[] = 
{
	Qt::red, 
	Qt::green,
	Qt::blue,
	Qt::darkCyan,
	Qt::darkMagenta,
	Qt::darkYellow,
	Qt::lightGray,
	Qt::yellow,
	Qt::magenta,
	Qt::cyan,
};


NodesPlotCurve::NodesPlotCurve(const QwtText &title) 
	: QwtPlotCurve (title)
{
	m_dataSetView = NULL;
}


NodesPlotCurve::NodesPlotCurve(const QString &title)
	: QwtPlotCurve (title)
{
	m_dataSetView = NULL;
}

void NodesPlotCurve::attachToDataSet(DataSetView *newDataSetView)
{
	m_dataSetView = newDataSetView;
	setData( *newDataSetView );
}

DataSetView* NodesPlotCurve::getDataSetView()
{
	return m_dataSetView;
}

void NodesPlotCurve::drawSymbols(QPainter *painter,
	const QwtSymbol &symbol,
	const QwtScaleMap &xMap, const QwtScaleMap &yMap,
	int from, int to) const
{
	for (int i = from; i <= to; i++)
	{
		int clusterNumber= -1;
		if (m_dataSetView)
			clusterNumber = m_dataSetView->cluster( i );
		
		QColor c = (clusterNumber != -1) ? QColor(clusterColors[clusterNumber] ) : Qt::white;  
		const int xi = xMap.transform(x(i));
		const int yi = yMap.transform(y(i));
		QRect rect;

		rect.moveCenter(QPoint(xi, yi));
		painter->setPen( QPen(c) );
		symbol.draw(painter, rect);
	}
	
	// draw centroids
	int centroidsCount = 0;
	if ((m_dataSetView) && (m_dataSetView->getDataSet()))
		centroidsCount = m_dataSetView->getDataSet()->getCentroidCount();
	else
		return;
		
	for( int i=0; i<centroidsCount; i ++ )
	{
		QPointF p;
		p = m_dataSetView->getDataSet()->getCentroidPF( i );
			
		const int xi = xMap.transform(p.x());
		const int yi = yMap.transform(p.y());
		QColor c = QColor(clusterColors[i] ).lighter( 175 );
		QRect rect;
		
		rect.moveCenter(QPoint(xi, yi));
		painter->setPen(c);
		
		for( int k=0; k<5; k++) 
		{
			painter->drawPoint( rect.x()-k, rect.y() );
			painter->drawPoint( rect.x()+k, rect.y() );
			painter->drawPoint( rect.x(), rect.y()+k );
			painter->drawPoint( rect.x(), rect.y()-k );
		}
		painter->drawPoint( rect.x(), rect.y() );
	}
}
