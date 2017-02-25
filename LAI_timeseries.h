#ifndef _LAI_TIMESERIES_H_
#define _LAI_TIMESERIES_H_

#include <vector>
using namespace std;


typedef struct{
	double date;
	double LAI;
	bool bottompoint;
	bool toppoint;
} laipoint;


class LAI_Series{
public:
	LAI_Series(double *x,double *y)
	{
	
		data.resize(92);
//		year = year;
		for (size_t i = 0; i < data.size(); ++i)
		{
			data[i].date = x[i];
			data[i].LAI = y[i];
		}
		ave_lai = 0;
		min_lai = 100;
		for (size_t i = 0; i < data.size(); ++i)
		{
			ave_lai += data[i].LAI;
			if (min_lai > data[i].LAI)
			{
				min_lai = data[i].LAI;
			}
		}
		ave_lai /= data.size();
	};

	void TopPoint();
	void BottomPoint();
	double StartDate();
	double StopDate();
	bool print_series();

private:
//	int year;
	vector <laipoint> data;
	int startd;          //生长起点
	int stopd;           //生长终点
	int period;          //生长长度
	double ave_lai;
	double min_lai;
};


#endif