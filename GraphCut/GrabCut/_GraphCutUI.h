#pragma once
#include "../stdafx.h"
#include <iostream>
#include "string"

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


class _GraphCutUI
{
public:

	//UI related
	enum{ NOT_SET = 0, IN_PROCESS = 1, SET = 2 };
	static const int radius = 2;
	static const int thickness = -1;
	static void		getBinaryMask( const Mat& comMask, Mat& binMask );
	// UI function
	void			mouseClick( int event, int x, int y, int flags, void* param );
	void			setImageAndWinName( const Mat& _image, const string& _winName );
	void			showImage() const;
	void			getMask(Mat& _mask);
	void			getRect(Rect& _rect);
	void			reSetMask(Mat& _mask){mask = _mask;};
	void			reset();
private:
	
	// get the mask from the user
	void			setRectInMask();
	void			setLblsInMask( int flags, Point p, bool isPr );
	const string*	winName;
	//  
	const Mat*		image;

	Mat				mask;
	uchar			rectState, lblsState, prLblsState;
	bool			isInitialized;
	Rect			rect;
	vector<Point>	fgdPxls, bgdPxls, prFgdPxls, prBgdPxls;

};
