#include "math.h"
#include "logifit.h"
#include <iostream>
double logifit(double* x, double* y, int m)
{

	if (m < 4)
		return 9999;
	double p[4] = { 0 };            //logistic 4参数
	p[3] = y[0];
	p[2] = y[m - 1] - y[0];
	if (p[2] == 0)
	{
		return 9999;
	}
	double *lny=new double [m];
	for (int i = 0; i < m; i++)
	{
		lny[i] = log(p[2] / (y[i] - p[3] + 0.00001) - 1.0+0.00001);
	//	std::cout << lny[i] << "  ";
	}
	double sxy = 0.;
	double sx = 0.;
	double sy = 0.;
	double sx2 = 0.;
	for (int i = 0; i < m; i++)
	{
		sx += x[i];
		sy += lny[i];
		sxy += x[i] * lny[i];
		sx2 += x[i] * x[i];
	}
	p[1] = (m*sxy - sx*sy) / (m*sx2 - sx*sx+0.00001);                //最小二乘拟合
	p[0] = sy / m - p[1] * sx / m;
//	const double intv = 1;
	int co = (x[m - 1] - x[0])/1;
	double *tx = new double[co];
	double *tz = new double[co];
	double *tK = new double[co];
	for (int i = 0; i < co; i++)
	{
		tx[i] = x[0]+i;
		tz[i] = exp(p[0] + p[1] * tx[i]);
		double k1, k2, k3;                               //临时参数
		k1 = pow(1. + tz[i], 4.) + pow(p[1] * p[2] * tz[i], 2.);
		k2 = 3 * tz[i] * (1 - tz[i])*pow(1 + tz[i], 3.)*(2 * pow(1 + tz[i], 3.) + p[1] * p[1] * p[2] * p[2] * tz[i]);
		k3 = (1 + tz[i])*(1 + tz[i])*(1 + 2 * tz[i] - 5 * tz[i] * tz[i]);
		tK[i] = pow(p[1], 3.)*p[2] * tz[i] * (k2 / (pow(k1, 2.5)) - k3 / (pow(k1, 1.5)));

	}
	/*for (int i = 0; i < co; i++)
	{
		
		std::cout << tK[i] << "  ";
	}*/
	int maxindex1 = 0;
	int maxindex2 = 0;
	double kmax = tK[0];
	for (int i = 0; i < co; i++)
	{
		
		if (kmax < tK[i])
		{
			kmax = tK[i];
			maxindex1 = i;
		}
	}
	tK[maxindex1] = 0;
	kmax = 0;               //判断第二大的数
	for (int i = 0; i < co; i++)
	{

		if (kmax < tK[i])
		{
			kmax = tK[i];
			maxindex2 = i;
		}
	}
	delete[]lny;
	delete[]tx;
	delete[]tz;
	delete[]tK;
	if (maxindex1 > maxindex2)
		return maxindex2+x[0];
	else
		return maxindex1+x[0];
}

double logi(double x,double *p)
{
	double z = exp(p[0] + p[1] * x);
	double re = p[2] / (1 + z) + p[3];
	return re;
}