#include "MultiResolutionCut.h"
void MultiResolutionCut:: Demo(const string imgName,_GraphCutUI& _CutUI,int mode)
{
	Mat img,mask;
	// UI initialize
	img = imread(imgName);
	const string winName = "image";

	// Set window and image
	namedWindow( winName, WINDOW_AUTOSIZE );
	_CutUI.setImageAndWinName( img, winName );
    setMouseCallback( "image", on_mouse, 0 );
	_CutUI.showImage();
	waitKey(0);

	// get the mask
	_CutUI.getMask(mask);
	//FileStorage fs("mask.txt",FileStorage::WRITE);
	//fs<<"mask"<<mask;
	Mat mirrorMask(mask.size(),CV_8UC1,Scalar::all(0)); 
	// rect
	Rect rect;
	_CutUI.getRect(rect);
	//s_original_size1 = img.size();

	for (int i=0;i<_sizeNums;i++)
	{
		Mat t_resizeImg,t_resizeMask;
		Size t_size;
		Rect t_rect;
		// initialize the parameter
		t_size.height = cvRound(img.rows/(pow(_base,i/5)));
		t_size.width = cvRound(img.cols/(pow(_base,i%5)));

		t_rect.x = cvRound(rect.x/(pow(_base,i%5)));
		t_rect.y = cvRound(rect.y/(pow(_base,i/5)));
		t_rect.width = cvRound(rect.width/(pow(_base,i%5)));
		t_rect.height = cvRound(rect.height/(pow(_base,i/5)));


		t_size.height = max(t_size.height,20); t_size.width = max(t_size.width,20);
		resize(img,t_resizeImg,t_size);
		//resize(mask,t_resizeMask,t_size);
		t_resizeMask.create(t_size,CV_8UC1);
		t_resizeMask.setTo(0);
		(t_resizeMask(t_rect)).setTo( Scalar(TM_PR_FGD) );


		MultiResolutionCut test(t_resizeImg,t_resizeMask,&img,rect,t_size,img.size(),mask);
		test.ShowImgByMask(t_resizeImg,t_resizeMask);
		waitKey(0);
		test.Cut();
		test.ExecuteClosedFormed(mirrorMask);
		cout<<mirrorMask.size()<<endl;
		cout<<img.size()<<endl;
		test.ShowImgByMask(img,mirrorMask);
		//test.ShowImgAndResults();
		waitKey(0);
		
	}



	

}
MultiResolutionCut::MultiResolutionCut(Mat& img,Mat& mask,const Mat* pOriginalImage,Rect&rect,Size curSize, Size originalSize,const Mat& originalMask):
	m_triMap(mask), m_imgMat(img), m_iterCount(3),
	m_w(img.cols), m_h(img.rows),
	m_curSize(curSize), m_originalMask(originalMask), 
	m_originalSize(originalSize),m_pOriginalImage(pOriginalImage)
{
}
MultiResolutionCut::~MultiResolutionCut()
{
	if(m_pfGMM)
		delete m_pfGMM;
	if (m_pbGMM)
		delete m_pbGMM;
	m_pOriginalImage = NULL;
}
void MultiResolutionCut::Initialize()
{

}
void MultiResolutionCut::BuildGMMs()
{
	m_pbGMM = new GMM(m_bgdModel);
	m_pfGMM = new GMM(m_fgdModel);
	InitGMMs();
}

void MultiResolutionCut::InitGMMs()
{
	const int kMeansItCount = 10;
	const int kMeansType = KMEANS_PP_CENTERS;

	Mat bgdLabels, fgdLabels;
	vector<Vec3f> bgdSamples, fgdSamples;
	Point p;
	for( p.y = 0; p.y < m_imgMat.rows; p.y++ )
	{
		for( p.x = 0; p.x < m_imgMat.cols; p.x++ )
		{
			if( m_triMap.at<uchar>(p) == GC_BGD || m_triMap.at<uchar>(p) == GC_PR_BGD )
				bgdSamples.push_back( (Vec3f)m_imgMat.at<Vec3b>(p) );
			else // GC_FGD | GC_PR_FGD
				fgdSamples.push_back( (Vec3f)m_imgMat.at<Vec3b>(p) );
		}
	}
	CV_Assert( !bgdSamples.empty() && !fgdSamples.empty() );
	Mat t_bgdSamples( (int)bgdSamples.size(), 3, CV_32FC1, &bgdSamples[0][0] );
	kmeans( t_bgdSamples, GMM::componentsCount, bgdLabels,
		TermCriteria( CV_TERMCRIT_ITER, kMeansItCount, 0.0), 0, kMeansType );
	Mat t_fgdSamples( (int)fgdSamples.size(), 3, CV_32FC1, &fgdSamples[0][0] );
	kmeans( t_fgdSamples, GMM::componentsCount, fgdLabels,
		TermCriteria( CV_TERMCRIT_ITER, kMeansItCount, 0.0), 0, kMeansType );

	m_pbGMM->initLearning();
	for( int i = 0; i < (int)bgdSamples.size(); i++ )
		m_pbGMM->addSample( bgdLabels.at<int>(i,0), bgdSamples[i] );
	m_pbGMM->endLearning();

	m_pfGMM->initLearning();
	for( int i = 0; i < (int)fgdSamples.size(); i++ )
		m_pfGMM->addSample( fgdLabels.at<int>(i,0), fgdSamples[i] );
	m_pfGMM->endLearning();
}


void MultiResolutionCut::Cut()
{
	Initialize();
	BuildGMMs();
	//_GrabCut::grab_Cut(_imgMat,_triMap,_rect,_bgdModel,_fgdModel,1,1);
	cv::grabCut(m_imgMat,m_triMap,m_rect,m_bgdModel,m_fgdModel,1,1);
}

void MultiResolutionCut::ShowImgAndResults() const
{
	Mat binMask,res;
	if (!m_triMap.empty())
	{
		_GraphCutUI::getBinaryMask(m_triMap,binMask);
		m_imgMat.copyTo(res,binMask);
	}
	
	char windowname[25];
	sprintf_s(windowname,"%s%d","image");
	imshow(windowname,res);
	// Set window and image
}
void MultiResolutionCut::ExecuteClosedFormed(Mat& originalMask)
{
	closedFormSolutionMatting closedFormedSln(&m_imgMat,&m_triMap,m_originalMask,m_pOriginalImage);

	closedFormedSln.Execute(originalMask);


}
void MultiResolutionCut::ShowImgByMask(const Mat& img,Mat mask)
{
	Mat binMask,res;
	if (!mask.empty())
	{
		_GraphCutUI::getBinaryMask(mask,binMask);
		img.copyTo(res,binMask);
	}
	imshow("Segmentation By ClosedFormed",res);
}
