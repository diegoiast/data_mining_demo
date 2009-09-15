/**
 * \file nodeplotcurve.h
 * \brief Definition of NodesPlotCurve
 * \author Diego Iastrubni elcuco@kde.org
 * License GPLv3, 2008,2009
 */

#ifndef NODEPLOTCURVE_H
#define NODEPLOTCURVE_H

#include "qwt_plot_curve.h"

class DataSetView;

class NodesPlotCurve : public QwtPlotCurve
{
public:
	NodesPlotCurve(const QwtText &title);
	NodesPlotCurve(const QString &title);
	
	void attachToDataSet(DataSetView*);
	DataSetView* getDataSetView();
	
	virtual void drawSymbols(QPainter *painter,
		const QwtSymbol &symbol,
		const QwtScaleMap &xMap, const QwtScaleMap &yMap,
		int from, int to) const;
		
private:
	DataSetView	*m_dataSetView;
};

#endif
