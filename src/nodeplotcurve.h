#ifndef NODEPLOTCURVE_H
#define NODEPLOTCURVE_H

#include "qwt_plot_curve.h"

class NodeCollection;
class NodesPlotCurve : public QwtPlotCurve
{
public:
	NodesPlotCurve(const QwtText &title);
	NodesPlotCurve(const QString &title);
	
	void setNodeCollection( NodeCollection *collection );
	NodeCollection* getNodeCollection();

	virtual void drawSymbols(QPainter *painter,
		const QwtSymbol &symbol,
		const QwtScaleMap &xMap, const QwtScaleMap &yMap,
		int from, int to) const;
		
private:
	NodeCollection *nodeCollection;
};

#endif
