
#include "LAI_timeseries.h"
#include "logifit.h"
#include <iostream>


void LAI_Series::TopPoint()
{
	for (size_t i = 2; i < data.size() - 2; ++i)
	{
		if (data[i].LAI > data[i - 1].LAI&&data[i].LAI > data[i + 1].LAI)
		{
			data[i].toppoint = true;
		}
	}
	for (size_t i = 2; i < data.size() - 2; ++i)
	{

		if (data[i].LAI >= data[i - 1].LAI&&data[i].LAI >= data[i + 1].LAI&&data[i].LAI>min_lai)       //寻找峰值点

		{
			bool flag1 = false;
			bool flag2 = false;
			int j = i;
			int p = 1;
			if (data[j].LAI > data[j - 1].LAI)
			{
				flag1 = true;
			}
			if (data[j].LAI == data[j - p].LAI)
			{
				while (data[j].LAI == data[j - p].LAI)
				{
					p++;
					if ((j - p) <=0)
						break;
				}
				if (data[j].LAI > data[j - p].LAI)
				{
					flag1 = true;
				}
			}
			j = i;
			p = 1;
			if (data[j].LAI > data[j + 1].LAI)
			{
				flag2 = true;
			}
			if (data[j].LAI == data[j + p].LAI)
			{
				while (data[j].LAI == data[j + p].LAI)
				{
					p++;
					if ((j + p) >= (data.size() - 1))
						break;
				}
				if (data[j].LAI > data[j + p].LAI)
				{
					flag2 = true;
				}
			}

			if (flag1 == true && flag2 == true)
			{
				data[i].toppoint = true;
			}
		}
	}


}
void LAI_Series::BottomPoint()
{

	for (size_t i = 1; i < data.size() - 1; ++i)
	{
		if (data[i].toppoint == true )
		{
			int tmpend = 0;
			for (size_t j = i - 1; j > 0; j--)          //decide search interval
			{
				if (data[j].toppoint == true)
				{
					tmpend = j;
					break;
				}
				else
				{
					tmpend = 0;
				}
			}
			int min = 100;
			int mindex = 0;
			for (size_t j = i - 1; j > tmpend; j--)
			{

				if (min > data[j].LAI)
				{
					min = data[j].LAI;
					mindex = j;
				}
			}
			data[mindex].bottompoint = true;
			///////////////////////////////////////////////////////////////////////
			for (size_t j = i + 1; j < data.size() - 1; j++)
			{
				if (data[j].toppoint == true)
				{
					tmpend = j;
					break;
				}
				else
				{
					tmpend = data.size();
				}
			}
			min = 100;
			mindex = 0;
			for (size_t j = i + 1; j < tmpend; j++)
			{

				if (min > data[j].LAI)
				{
					min = data[j].LAI;
					mindex = j;
				}
			}
			data[mindex].bottompoint = true;
		}

	}
}

double LAI_Series::StartDate()
{
	int startd=0;
	int stopd=0;
	int number;
	/*if (year==2001)
	{ }*/
	for (int i = 23; i < 69; ++i)
	{
		if (data[i].toppoint == true && data[i].LAI>ave_lai)            //找第一个峰值点，反向搜索对于的谷点
		{
			for (int j = i; j > 0; j--)
			{
				if (data[j].bottompoint == true)
				{
					startd = j; stopd = i;
					break;
				}
			}
			break;
		}
	}
	number = stopd - startd+1;
	double*x = new double[number];
	double *y = new double[number];
	for (int i = 0; i < number; ++i)
	{
		x[i] = data[startd + i].date;
		y[i] = data[startd + i].LAI;
	}

	//计算生长起点
	double startpoint=0;
	startpoint = logifit(x, y, number);

	delete[]x;
	delete[]y;
	return startpoint;
}
double LAI_Series::StopDate()
{
	int startd = 0;
	int stopd = 0;
	int number;
	/*if (year == 2001)
	{
	}*/
	for (int i = 68; i > 22; --i)
	{
		if (data[i].toppoint == true && data[i].LAI>ave_lai)            //找第一个峰值点，反向搜索对于的谷点
		{
			for (int j = i; j < 92; ++j)
			{
				if (data[j].bottompoint == true)
				{
					startd = i; stopd = j;   //.............
					break;
				}
			}
			break;
		}
	}
	number = stopd - startd + 1;
	double*x = new double[number];
	double *y = new double[number];
	for (int i = 0; i < number; ++i)
	{
		x[i] = 366-data[stopd - i].date;
		y[i] = data[stopd - i].LAI;
	}

	//计算生长起点
	double stoppoint = 0;
	stoppoint = logifit(x, y, number);
	delete[]x;
	delete[]y;
	if (stoppoint == 9999)
		return 9999;
	return 366-stoppoint;
}

bool LAI_Series::print_series()
{
	if (data.size() == 0)
		return false;
	for (int i = 0; i < data.size(); i++)
	{
		std::cout << data[i].date << "  " << data[i].LAI << "  " 
			<< data[i].bottompoint << "  " << data[i].toppoint << endl;
	}
	return true;
}

