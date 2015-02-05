#pragma once

#include <iostream>
#include "graph.h"
#include "GMM.hpp"
#include "string"
#include "_GraphCutUI.h"
#include <opencv2/opencv.hpp>
using namespace std;

// UI callback function
extern  void on_mouse( int event, int x, int y, int flags, void* param );

class MultiResolutionCut
{
public:
	// construction and destruction function
	MultiResolutionCut(Mat& img,Mat& mask);  // need init lists
	~MultiResolutionCut(void);

	static void Demo(const string file,_GraphCutUI& _CutUI,int mode=0);

public:
	void initialize();
	void buildGMMs();
	void cut();
	void getTriMap(Mat &map){map = _triMap;};
private:
	// permanent parameters
	double calcBeta();
	void calcNWeights();

	//GMM
	void initGMMs();   //initialization

	// used for iteration
	void assignGMMsComponents();
	void learnGMMs();
	void constructGCGraph();
	// estimate results
	void estimateSegmentation();

private:
	int _w, _h, _iterCount;
	GMM *_pbGMM, *_pfGMM;
	Mat _bgdModel,_fgdModel;
	Mat _triMap, _binaryMask;
	Mat _img4Show;
	const Mat _imgMat;

	double _gamma;  //50 for grabCut
	double _lambda; // 9*gamma
	double _beta;
	GraphD *_pGraph;
	Mat _leftW, _upleftW, _upW, _uprightW;    //Weight matrix
	Mat _compIdxs;	



};