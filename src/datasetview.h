#ifndef DATASETVIEW_H
#define DATASETVIEW_H

#include <qwt_data.h>

class DataSet;

class DataSetView: public QwtData  
{
	
public:
friend class NodesPlotCurve;

	DataSetView( DataSet *n );

	// implementation of needed interfaces of QwtData
	QwtData*	copy() const;
	size_t		size() const;
	double		x( size_t i ) const;
	double		y( size_t i ) const;
	int 		cluster( size_t i ) const;
	
	DataSet* getDataSet();
	
private:
	DataSet	*m_dataSet;
};

#endif
