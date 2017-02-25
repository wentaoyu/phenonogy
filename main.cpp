#include "gdal_priv.h"
#include "LAI_timeseries.h"
#include <string>
#include <io.h>
#include <iostream>
#include <fstream>
#include<sstream>
#include <vector>
using namespace std;

void getFiles(string path, vector<string>& files)
{
	//文件句柄  
	long   hFile = 0;
	//文件信息  
	struct _finddata_t fileinfo;
	string p;
	if ((hFile = _findfirst(p.assign(path).append("*.hdf").c_str(), &fileinfo)) != -1)
	{
		do
		{
			//如果是目录,迭代之  
			//如果不是,加入列表  
			if ((fileinfo.attrib &  _A_SUBDIR))
			{
				if (strcmp(fileinfo.name, ".") != 0 && strcmp(fileinfo.name, "..") != 0)
					getFiles(p.assign(path).append(fileinfo.name), files);
			}
			else
			{
				files.push_back(p.assign(path).append(fileinfo.name));
			}
		} while (_findnext(hFile, &fileinfo) == 0);
		_findclose(hFile);
	}
}


int main(int argc, char * argv[])   //argv[1] workspace argv[2] year argv[3] HVfile path argv[4]and argv[5] query point
{
	if (argc < 4)
	{
		cout << "wrong args";
		exit(1);
	}
	if (argc < 6)
	{
		argv[4] = 0;
		argv[5] = 0;
	}
	GDALAllRegister();
	CPLSetConfigOption("GDAL_FILENAME_IS_UTF8", "NO");
	vector<string>HV;
	string temp;
	ifstream fin(argv[3]);
	if (!fin)
	{
		cerr << "hv file error!";
		exit(-1);
	}
	while (!fin.eof())
	{
		getline(fin, temp, '\n');
		istringstream temp1(temp);
		while (temp1 >> temp)
		{
			HV.push_back(temp);
		}
	}
	fin.close();
	//计算所需的产品文件路径
	for (size_t pos = 0; pos < HV.size(); pos++)
	{
		cout << "processing:"  << HV[pos];
		vector <string> laipath;
		string path;

		path = argv[1];
		path.append("MCD12Q1\\");
		char y[5];
		sprintf(y, "%d", atoi(argv[2]));
		path += y;
		path += "\\";
		vector<string> files;
		getFiles(path, files);
		for (size_t i = 0; i < files.size(); i++)
		{
			size_t pos1 = files[i].find(HV[pos]);
			if (pos1 != string::npos)
			{
				laipath.push_back(files[i]);
				break;
			}
		}
		files.clear();
		for (int i = 185; i < 365; i += 8)           //former year
		{
			path = argv[1];
			path.append("LAI\\");
			char* day = new char[4];
			sprintf(day, "%03d", i);
			char y[5];
			sprintf(y, "%d", atoi(argv[2]) - 1);
			path += y;
			path += "\\";
			path += day;
			path += "\\";

			//vector<string> files;
			getFiles(path, files);
			for (size_t i = 0; i < files.size(); i++)
			{
				size_t pos1 = files[i].find(HV[pos]);
				if (pos1 != string::npos)
				{
					laipath.push_back(files[i]);
					break;
				}
			}
			files.clear();
		
		}
		for (int i = 1; i < 365; i += 8)           //current year
		{
			path = argv[1];
			path.append("LAI\\");
			char* day = new char[4];
			sprintf(day, "%03d", i);
			char y[5];
			sprintf(y, "%d", atoi(argv[2]));
			path += y;
			path += "\\";
			path += day;
			path += "\\";

			vector<string> files;
			getFiles(path, files);
			for (size_t i = 0; i < files.size(); i++)
			{
				size_t pos1 = files[i].find(HV[pos]);
				if (pos1 != string::npos)
				{
					laipath.push_back(files[i]);
					break;
				}
			}
			files.clear();
		
		}

		for (int i = 1; i < 184; i += 8)           //next year
		{
			path = argv[1];
			path.append("LAI\\");
			char* day = new char[4];
			sprintf(day, "%03d", i);
			char y[5];
			sprintf(y, "%d", atoi(argv[2]) + 1);
			path += y;
			path += "\\";
			path += day;
			path += "\\";

			vector<string> files;
			getFiles(path, files);
			for (size_t i = 0; i < files.size(); i++)
			{
				size_t pos1 = files[i].find(HV[pos]);
				if (pos1 != string::npos)
				{
					laipath.push_back(files[i]);
					break;
				}
			}
			files.clear();
		
		}
		/////////////////
		if (laipath.size() != 93)
		{
			cout << "行列号：" << HV[pos] << "原始数据缺失，反演失败" << endl;
			continue;
		}


		//初始化内存
		unsigned char **datablock = new unsigned char *[92];
		for (int i = 0; i < 92; i++)
		{
			datablock[i] = new unsigned char[1200 * 1200];
		}
		unsigned char *landcoverblock = new unsigned char[1200 * 1200];

		string  dstsr;
		for (int i = 1; i <= 92; i++)
		{
			GDALDataset* dataset = (GDALDataset*)GDALOpen(laipath[i].c_str(), GA_ReadOnly);
			if (dataset == NULL)
			{
				cerr << "file error！";
			}
			char** sublist = GDALGetMetadata((GDALDatasetH)dataset, "SUBDATASETS");
			//cout << sublist[0];
			string subDataset1Name = sublist[0];
			subDataset1Name = subDataset1Name.substr(subDataset1Name.find_first_of("=") + 1);
			GDALDataset* subDataset = (GDALDataset*)GDALOpen(subDataset1Name.c_str(), GA_ReadOnly);//打开该数据  
			if (subDataset == NULL)
			{
				exit(1);
			}
			GDALRasterBand *poband = subDataset->GetRasterBand(1);
			poband->RasterIO(GF_Read, 0, 0, 1200, 1200, datablock[i - 1], 1200, 1200, GDT_Byte, 0, 0);
			GDALClose(GDALDatasetH(subDataset));
			GDALClose((GDALDatasetH)dataset);
		}
		GDALDataset* dataset = (GDALDataset*)GDALOpen(laipath[0].c_str(), GA_ReadOnly);
		if (dataset == NULL)
		{
			cerr << "land cover file error！";
		}
		char** sublist = GDALGetMetadata((GDALDatasetH)dataset, "SUBDATASETS");
		//cout << sublist[0];
		string subDataset1Name = sublist[4];  //type3 scheme
		subDataset1Name = subDataset1Name.substr(subDataset1Name.find_first_of("=") + 1);
		GDALDataset* subDataset = (GDALDataset*)GDALOpen(subDataset1Name.c_str(), GA_ReadOnly);//打开该数据  
		if (subDataset == NULL)
		{
			exit(1);
		}
		double GeoTransform[6];
		const char* sr;
		sr = subDataset->GetProjectionRef();
		dstsr.append(sr);
		
		subDataset->GetGeoTransform(GeoTransform);
		GDALRasterBand *poband = subDataset->GetRasterBand(1);
		poband->RasterIO(GF_Read, 0, 0, 2400, 2400, landcoverblock, 1200, 1200, GDT_Byte, 0, 0);
		GDALClose(GDALDatasetH(subDataset));
		GDALClose((GDALDatasetH)dataset);
		///////////////////////////////
		int *startup = new int[1200 * 1200];
		int *endin = new int[1200 * 1200];
		int *length = new int[1200 * 1200];
		cout << "建立时间序列。。。";
		////////////////////////////////////////////////////
		for (int i = 0; i < 1200 * 1200; i++)
		{
			if (landcoverblock[i] == 0 || landcoverblock[i] == 5                //non-vegetated and evergreen broadleaf forest
				|| landcoverblock[i] == 9 || landcoverblock[i] == 10
				|| landcoverblock[i] == 254)
			{
				startup[i] = 9999;
				endin[i] = 9999;
				length[i] = 9999;
			}
			else
			{
				//construct time series
				double* x = new double[92];
				double *y = new double[92];
				for (int j = 0; j < 23; j++)
				{
					x[j] = -180 + j * 8;
					y[j] = datablock[j][i];
				}
				for (int j = 23; j < 69; j++)
				{
					x[j] = -183 + j * 8;
					y[j] = datablock[j][i];
				}
				for (int j = 69; j < 92; j++)
				{
					x[j] = -186 + j * 8;
					y[j] = datablock[j][i];
				}
				LAI_Series poseries( x, y);
			
				poseries.TopPoint();
				poseries.BottomPoint();
				startup[i] = poseries.StartDate();
				endin[i] = poseries.StopDate();
				if (startup[i] == 9999 || endin[i] == 9999)
				{
					length[i] = 9999;
				}
				else
				{
					length[i] = endin[i] - startup[i];
				}
				/*if (i == ((atoi(argv[5])-1) * 1200 +atoi(argv[4])-1))
				{
					poseries.print_series();
					cout << startup[i] << endl;
					cout << endin[i] << endl;
				}*/
				delete[] x;
				delete[] y;
			}
		}


		cout << "创建输出图像。";



		const char *pszFormat = "GTiff";     //创建图像
		GDALDriver *poDriver;
		char**papszMetadata;
		poDriver = GetGDALDriverManager()->GetDriverByName(pszFormat);
		if (poDriver == NULL)
			exit(1);
		papszMetadata = poDriver->GetMetadata();
		string outpath;
		outpath = argv[1];
		outpath.append("result\\");

		outpath += y;
		outpath += "\\";
		outpath.append("phenology.");
		outpath += y;
		outpath.append(HV[pos]);
		outpath.append(".tif");
		GDALDataset* poDstDS;
		poDstDS = poDriver->Create(outpath.c_str(), 1200, 1200, 3, GDT_Int32, NULL);
		GeoTransform[1] *= 2;
		GeoTransform[5] *= 2;
		poDstDS->SetGeoTransform(GeoTransform);
		poDstDS->SetProjection(dstsr.c_str());
		
		
		GDALRasterBand* dstband1 = poDstDS->GetRasterBand(1);
		GDALRasterBand* dstband2 = poDstDS->GetRasterBand(2);
		GDALRasterBand* dstband3 = poDstDS->GetRasterBand(3);
		dstband1->RasterIO(GF_Write, 0, 0, 1200, 1200, startup, 1200, 1200, GDT_Int32, 0, 0);
		dstband2->RasterIO(GF_Write, 0, 0, 1200, 1200, endin, 1200, 1200, GDT_Int32, 0, 0);
		dstband3->RasterIO(GF_Write, 0, 0, 1200, 1200, length, 1200, 1200, GDT_Int32, 0, 0);
		GDALClose(GDALDatasetH(poDstDS));




		delete[] startup;
		delete[] endin;
		delete[] length;
		delete[] landcoverblock;
		for (int i = 0; i < 92; i++)
		{
			delete[] datablock[i];
		}
		delete[]datablock;
		cout << "finish processing!" << endl;
		cout << "///////////////////////////";
	}
}