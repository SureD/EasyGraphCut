#pragma once
#include "../stdafx.h"
#include <iostream>
#include "graph.h"
#include "GMM.hpp"
#include "string"
#include "_GraphCutUI.h"
#include "../GrabCut/_GrabCut.h"
#include "CloseFormSolutionMatting.h"
using namespace std;

// UI callback function
extern  void on_mouse( int event, int x, int y, int flags, void* param );

class MultiResolutionCut
{
public:
	// construction and destruction function
	MultiResolutionCut( Mat& img, Mat& mask,const Mat* pOriginalImage, Rect& rect,
						Size curSize, Size originalSize,const Mat& originalMask);  
	~MultiResolutionCut(void);
	//static _GrabCut _grabCutObj;
	static void		Demo(const string file,_GraphCutUI& _CutUI,int mode=0);
private:
	static const int _sizeNums = 25; // the total count of the resize array
	static const int _base = 2; // base of resize function

public:
	void		Initialize();

	void		Cut();
	void		GetTriMap(Mat &map){map = m_triMap;};
	void		ShowImgAndResults() const ;
	void		ShowImgByMask(const Mat& img,Mat mask) ;
// the closed formed method
	void		ExecuteClosedFormed(Mat& originalMask);
private:

//GMM
	void		InitGMMs();   //initialization
	void		BuildGMMs();


private:


	int			m_w, m_h, m_iterCount;
	GMM			*m_pbGMM, *m_pfGMM;
	Mat			m_bgdModel,m_fgdModel;
	Mat			m_triMap, m_binaryMask,m_originalMask;
	Mat			m_img4Show;
	Rect		m_rect;
	const Mat	m_imgMat;
	const Size	m_curSize;  //  size of the resized matrix
	const Size	m_originalSize;
	const Mat*  m_pOriginalImage;



};