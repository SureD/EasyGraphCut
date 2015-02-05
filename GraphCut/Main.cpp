// GraphCut.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"
//#include "GrabCutApp.h"
#include "GrabCut/_GraphCutUI.h"
#include "GrabCut/MultiResolutionCut.h"

//                            _ooOoo_  
//                           o8888888o  
//                           88" . "88  
//                           (| -_- |)  
//                            O\ = /O  
//                        ____/`---'\____  
//                      .   ' \\| |// `.  
//                       / \\||| : |||// \  
//                     / _||||| -:- |||||- \  
//                       | | \\\ - /// | |  
//                     | \_| ''\---/'' | |  
//                      \ .-\__ `-` ___/-. /  
//                   ___`. .' /--.--\ `. . __  
//                ."" '< `.___\_<|>_/___.' >'"".  
//               | | : `- \`.;`\ _ /`;.`/ - ` : | |  
//                 \ \ `-. \_ __\ /__ _/ .-` / /  
//         ======`-.____`-.___\_____/___.-`____.-'======  
//                            `=---='  
//  
//         .............................................  
//                  佛祖保佑             永无BUG 
//            本程序已经经过开光处理 绝无可能在产生BUG
//
//##################################################################
//
//3. Example usage.
//
//	This section shows how to use the library to compute
//	a minimum cut on the following graph:
//
//      SOURCE
//    /       \
//	1/         \2
//	/      3    \
//	node0 -----> node1
//	|   <-----   |
//	|      4     |
//	\            /
//	5\          /6
//	  \        /
//	    SINK
//
//	///////////////////////////////////////////////////

//#include <stdio.h>
//#include "_GrabCut.h"
//#include "graph.h"
//
//	int main()
//{
//	typedef Graph<int,int,int> GraphType;
//	GraphType *g = new GraphType(/*estimated # of nodes*/ 2, /*estimated # of edges*/ 1); 
//
//	g -> add_node(); 
//	g -> add_node(); 
//
//	g -> add_tweights( 0,   /* capacities */  1, 5 );
//	g -> add_tweights( 1,   /* capacities */  2, 6 );
//	g -> add_edge( 0, 1,    /* capacities */  3, 4 );
//
//	int flow = g -> maxflow();
//
//	printf("Flow = %d\n", flow);
//	printf("Minimum cut:\n");
//	if (g->what_segment(0) == GraphType::SOURCE)
//		printf("node0 is in the SOURCE set\n");
//	else
//		printf("node0 is in the SINK set\n");
//	if (g->what_segment(1) == GraphType::SOURCE)
//		printf("node1 is in the SOURCE set\n");
//	else
//		printf("node1 is in the SINK set\n");
//
//	delete g;
//	Mat temp;
//	GMM as(temp);
//	return 0;
//}


///////////////////////////////////////////////////
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
//GCApplication gcapp;
//static void on_mouse( int event, int x, int y, int flags, void* param )
//{
//
//	gcapp.mouseClick( event, x, y, flags, param );
//}
//typedef void(GCApplication::*PMA)(int event, int , int , int , void*); 
//int main()
//{
//	//////  Graph Cut test case
//
//	//string filename = "I:\\CodeZone\\test1\\2.jpg";
//	string filename = "H:\\labCode\\testcase\\测试图片\\images\\test\\3096.jpg";
//	if( filename.empty() )
//	{
//		cout << "\nDurn, couldn't read in " <<filename << endl;
//		return 1;
//	}
//	Mat image = imread( filename, 1 );
//	if( image.empty() )
//	{
//		cout << "\n Durn, couldn't read image filename " << filename << endl;
//		return 1;
//	}
//
//	help();
//	const string winName = "image";
//	namedWindow( winName, WINDOW_AUTOSIZE );
//	gcapp.setImageAndWinName( image, winName );
//	setMouseCallback( winName, on_mouse, 0 );
//
//	
//	gcapp.showImage();
//	for(;;)
//	{
//		int c = waitKey(0);
//		switch( (char) c )
//		{
//		case '\x1b':
//			cout << "Exiting ..." << endl;
//			goto exit_main;
//		case 'r':
//			cout << endl;
//			gcapp.reset();
//			gcapp.showImage();
//			break;
//		case 'n':
//			int iterCount = gcapp.getIterCount();
//			cout << "<" << iterCount << "... ";
//			int newIterCount = gcapp.nextIter();
//			if( newIterCount > iterCount )
//			{
//				gcapp.showImage();
//				cout << iterCount << ">" << endl;
//			}
//			else
//				cout << "rect must be determined>" << endl;
//			break;
//		}
//	}
//
//exit_main:
//	destroyWindow( winName );
//	return 0;
//
//}
_GraphCutUI _CutUI;
void on_mouse( int event, int x, int y, int flags, void* param )
{

	_CutUI.mouseClick(event,x,y,flags,param);

}
int main()
{
	string filename = "H:\\labCode\\testcase\\测试图片\\images\\test\\3096.jpg";
	MultiResolutionCut::Demo(filename,_CutUI);

	cout<<"test"<<endl;
	return 0;
}