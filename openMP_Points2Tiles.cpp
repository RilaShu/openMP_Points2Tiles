// openMP_Points2Tiles.cpp
// @author RilaShu
// Created at 2018/08/15
// at least millions points is useful.

#include "stdafx.h"
#include <string>
#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <cmath>
#include <ctime>
#include <omp.h>
#include <direct.h>
using namespace std;

// lat:(-90.0 ~ 90.0) , lng:(-180.0 ~ 180.0)
static double LatMin = -90.0;
static double LngMin = -180.0;
//normal tile resolution is 256*256
static int nResolution = 256;

//to store intermediate data
class Point {
public:
	double lat;//lat
	double lng;//lnt
};
class PointsTile {
public:
	int nLevel;
	int nRow;
	int nCol;
	vector <Point> Points;
};
//to store a common pointcount-tile data
class PointCount {
public:
	int nPosition;
	long nCount;
};
class PointCountTile {
public:
	int nLevel;
	int nRow;
	int nCol;
	vector <PointCount> pointCount;
};

class LevelCountMinMax {
	public:
		int nLevel;
		long nMin;
		long nMax;
};
//data
vector <Point> vcPoints; //all the points
vector <PointsTile> vcPointsTiles; //intermediate tiles
LevelCountMinMax *vcLevelCountMinMax; //poings count min-max in each leval

// the tile level to be cut, for a heatmap usage, 12 is suitable
// when intend to use high level, watch your memery 
int nLevel = 0;

void readPointsFromCSV(string pathInput, int nLatIndex, int nLngIndex, vector<Point> &vcPoints);
void cutTile(int nTotalLevel, vector<Point> points, vector <PointsTile> &vcPointsTiles);
void outputPointsTiles(string pathFile, vector <PointsTile> vcPointsTiles);

int main()
{
	clock_t start, end;
	start = clock();

	nLevel = 10;
	nResolution = 16;
	// read points from csv and cut
	readPointsFromCSV("dp_20170101_144_0_XY.csv", 1, 0, vcPoints);
	outputPointsTiles("..\\dpTile", vcPointsTiles);
	end = clock();
	cout << "Total time: " << (end - start) << " ms" << endl;
	getchar();
	return 0;
}

void SplitString(const string& s, vector<string>& v, const string& c)
{
	string::size_type pos1, pos2;
	pos2 = s.find(c);
	pos1 = 0;
	while (string::npos != pos2)
	{
		v.push_back(s.substr(pos1, pos2 - pos1));

		pos1 = pos2 + c.size();
		pos2 = s.find(c, pos1);
	}
	if (pos1 != s.length())
		v.push_back(s.substr(pos1));
}

//the .csv file input part needs to be parallelled!
void readPointsFromCSV(string pathInput, int nLatIndex, int nLngIndex, vector<Point> &vcPoints) {
	ifstream inPointsCSV(pathInput, ios::in);
	string sLine = "";
	//Read by line, split by ','
	int nCount = 1;
	vector <string> vcFile;
	while (getline(inPointsCSV, sLine))
	{
		vcFile.push_back(sLine);
		if (vcFile.size() == 100000) {
#pragma omp parallel for
			for (int i = 0; i < vcFile.size(); i++)
			{
				vector <string> vcLine;
				SplitString(vcFile[i], vcLine, ",");
				Point tPoint;
				tPoint.lat = stod(vcLine[nLatIndex]); tPoint.lng = stod(vcLine[nLngIndex]);
				if (tPoint.lat < 90.0 && tPoint.lat > -90.0 && tPoint.lng < 180.0 && tPoint.lng > -180.0) {
#pragma omp critical
					vcPoints.push_back(tPoint);
				}
			}
			vcFile.clear();
		}
	}
#pragma omp parallel for
	for (int i = 0; i < vcFile.size(); i++)
	{
		vector <string> vcLine;
		SplitString(vcFile[i], vcLine, ",");
		Point tPoint;
		tPoint.lat = stod(vcLine[nLatIndex]); tPoint.lng = stod(vcLine[nLngIndex]);
		if (tPoint.lat < 90.0 && tPoint.lat > -90.0 && tPoint.lng < 180.0 && tPoint.lng > -180.0) {
#pragma omp critical
			vcPoints.push_back(tPoint);
		}
	}
	vcFile.clear();

	cout << "Points num " << vcPoints.size() << "\n";
	cutTile(nLevel, vcPoints, vcPointsTiles);
	cout << "Cut tiles finished.\n";
}

// init the vcPointsTiles
void initTiles(int nTotalLevel, vector <PointsTile> &vcPointsTiles) {
	//Init tiles by function resize()
	int nTileTotalNum = 0;
	for (int k = 0; k < nTotalLevel; k++)
	{
		nTileTotalNum += int(pow(4.0, k + 1));
	}
	vcPointsTiles.resize(nTileTotalNum);
	//Init tiles details parallelly
	for (int i = 0; i < nTotalLevel; i++) {
		int nTileBaseNum = 0;
		for (int m = 1; m < i + 1; m++)
		{
			nTileBaseNum += int(pow(4.0, m));
		}
#pragma omp parallel for
		for (int m = 0; m < int(pow(2.0, i + 1)); m++) {
#pragma omp parallel for
			for (int n = 0; n < int(pow(2.0, i + 1)); n++) {
				int nTileNum = nTileBaseNum + m * int(pow(2.0, i + 1)) + n;
				vcPointsTiles[nTileNum].nLevel = i; vcPointsTiles[nTileNum].nRow = m; vcPointsTiles[nTileNum].nCol = n;
			}
		}
	}
}
//cut the points into vcPointsTiles
void cutTile(int nTotalLevel, vector<Point> points, vector <PointsTile> &vcPointsTiles) {
	initTiles(nTotalLevel, vcPointsTiles);
	cout << "Init Finish.\n";
	for (int i = 0; i < nTotalLevel; i++) {
		int nTileBaseNum = 0;
		for (int m = 1; m < i + 1; m++)
		{
			nTileBaseNum += int(pow(4.0, m));
		}
		//Add points to tiles parallelly.
# pragma omp parallel for 
		for (int j = 0; j < points.size(); j++) {
			int nRow = int((points[j].lat - LatMin) / 180.0 * pow(2.0, i + 1));
			int nCol = int((points[j].lng - LngMin) / 360.0 * pow(2.0, i + 1));
			if (nRow >= 0 && nCol >= 0) {
				int nTileNum = nTileBaseNum + nRow * int(pow(2.0, i + 1)) + nCol;
# pragma omp critical
				vcPointsTiles[nTileNum].Points.push_back(points[j]);
			}
		}
	}
}

//output intermediate data (which also useful)
void outputPointsTiles(string pathFile, vector <PointsTile> vcPointsTiles) {
	int flag = _mkdir(pathFile.c_str());
	#pragma omp parallel for schedule(dynamic)
	for (int i = 0; i < vcPointsTiles.size(); i++)
	{
		if (vcPointsTiles[i].Points.size() > 0)
		{
			ofstream output;
			string sFilePath = pathFile + "\\" + to_string(vcPointsTiles[i].nLevel) + "_" + \
				to_string(vcPointsTiles[i].nRow) + "_" + to_string(vcPointsTiles[i].nCol) + ".tilepoints";
			output.open(sFilePath, ios::trunc);
			for (int j = 0; j < vcPointsTiles[i].Points.size(); j++)
			{
				output << vcPointsTiles[i].Points[j].lat << "," << vcPointsTiles[i].Points[j].lng << "\n";
			}
			output.close();//关闭文件
		}
	}
	cout << "points tiles output finished\n";
}