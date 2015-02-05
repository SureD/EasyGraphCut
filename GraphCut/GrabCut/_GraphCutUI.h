#pragma once

#include <iostream>
#include "string"
#include <opencv2/opencv.hpp>
using namespace std;
using namespace cv;
// UI resources
const Scalar RED = Scalar(0,0,255);
const Scalar PINK = Scalar(230,130,255);
const Scalar BLUE = Scalar(255,0,0);
const Scalar LIGHTBLUE = Scalar(255,255,160);
const Scalar GREEN = Scalar(0,255,0);

const int BGD_KEY = CV_EVENT_FLAG_CTRLKEY;
const int FGD_KEY = CV_EVENT_FLAG_SHIFTKEY;
//Trimap
enum TrimapValue {
	TM_BGD    = 0,  //!< background
	TM_FGD    = 1,  //!< foreground
	TM_PR_BGD = 2,  //!< most probably background  -->unknown area 
	TM_PR_FGD = 3   //!< most probably foreground  -->unknown area
};

class _GraphCutUI
{
public:

	//UI related
	enum{ NOT_SET = 0, IN_PROCESS = 1, SET = 2 };
	static const int radius = 2;
	static const int thickness = -1;
	static void getBinaryMask( const Mat& comMask, Mat& binMask );
	// UI function
	void mouseClick( int event, int x, int y, int flags, void* param );
	void setImageAndWinName( const Mat& _image, const string& _winName );
	void showImage() const;
	void getMask(Mat& _mask);
	void reSetMask(Mat& _mask){mask = _mask;};
	void reset();
private:
	
	// 加载标记函数 可共享
	void setRectInMask();
	void setLblsInMask( int flags, Point p, bool isPr );
	const string* winName;
	//  图像资源需分离
	const Mat* image;

	Mat mask;
	uchar rectState, lblsState, prLblsState;
	bool isInitialized;
	Rect rect;
	vector<Point> fgdPxls, bgdPxls, prFgdPxls, prBgdPxls;

};
