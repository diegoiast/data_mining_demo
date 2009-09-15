#include <QPainter>
#include <QBrush>
#include <QPoint>
#include <QDebug>

#include "qwt_scale_map.h"
#include "qwt_symbol.h"

#include "nodeplotcurve.h"
#include "nodecollection.h"

Qt::GlobalColor clusterColors[] = 
{
	Qt::red, 
	Qt::green,
	Qt::blue,
	Qt::darkCyan,
	Qt::darkMagenta,
	Qt::darkYellow,
	Qt::lightGray,
};


NodesPlotCurve::NodesPlotCurve(const QwtText &title) 
	: QwtPlotCurve (title)
{
	nodeCollection = NULL;
}


NodesPlotCurve::NodesPlotCurve(const QString &title)
	: QwtPlotCurve (title)
{
	nodeCollection = NULL;
}

void NodesPlotCurve::setNodeCollection( NodeCollection *collection )
{
	nodeCollection = collection;
	setData(*collection);
}

NodeCollection* NodesPlotCurve::getNodeCollection()
{
	return nodeCollection;
}

void NodesPlotCurve::drawSymbols(QPainter *painter,
	const QwtSymbol &symbol,
	const QwtScaleMap &xMap, const QwtScaleMap &yMap,
	int from, int to) const
{
	for (int i = from; i <= to; i++)
	{
		int clusterNumber = nodeCollection? nodeCollection->cluster(i) : -1;
		QColor c = (clusterNumber != -1) ? QColor(clusterColors[clusterNumber] ) : Qt::black;  
		const int xi = xMap.transform(x(i));
		const int yi = yMap.transform(y(i));
		QRect rect;

		rect.moveCenter(QPoint(xi, yi));
		painter->setPen( QPen(c) );
		symbol.draw(painter, rect);
	}
	if (!nodeCollection)
		return;
	
	// draw centroids
	for( int i=0; i<nodeCollection->centroids.size(); i ++ )
	{
		QPointF p = nodeCollection->centroids.at(i);
		const int xi = xMap.transform(p.x());
		const int yi = yMap.transform(p.y());
		QColor c = QColor(clusterColors[i] ).lighter();
		QRect rect;
		
		rect.moveCenter(QPoint(xi, yi));
		painter->setPen( QPen(c) );
		
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
