// GraphCut.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"
//#include "GrabCutApp.h"
#include "GrabCut/_GraphCutUI.h"
#include "GrabCut/MultiResolutionCut.h"


static void help()
{
	cout << "\nThis program demonstrates GrabCut segmentation -- select an object in a region\n"
		"and then grabcut will attempt to segment it out.\n"
		"Call:\n"
		"./grabcut <image_name>\n"
		"\nSelect a rectangular area around the object you want to segment\n" <<
		"\nHot keys: \n"
		"\tESC - quit the program\n"
		"\tr - restore the original image\n"
		"\tn - next iteration\n"
		"\n"
		"\tleft mouse button - set rectangle\n"
		"\n"
		"\tCTRL+left mouse button - set GC_BGD pixels\n"
		"\tSHIFT+left mouse button - set CG_FGD pixels\n"
		"\n"
		"\tCTRL+right mouse button - set GC_PR_BGD pixels\n"
		"\tSHIFT+right mouse button - set CG_PR_FGD pixels\n" << endl;
}

_GraphCutUI _CutUI;
void on_mouse( int event, int x, int y, int flags, void* param )
{

	_CutUI.mouseClick(event,x,y,flags,param);

}
int main()
{
	//3096
	string filename = "H:\\labCode\\testcase\\测试图片\\images\\test\\86016.jpg";
	MultiResolutionCut::Demo(filename,_CutUI);

	cout<<"test"<<endl;
	return 0;
}