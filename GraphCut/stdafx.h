// stdafx.h : ��׼ϵͳ�����ļ��İ����ļ���
// ���Ǿ���ʹ�õ��������ĵ�
// �ض�����Ŀ�İ����ļ�
//

#pragma once

#include "targetver.h"

#include <stdio.h>
#include <tchar.h>
#include <assert.h>
// OpenCV
#include <opencv2/opencv.hpp>
//Trimap
enum TrimapValue {
	TM_BGD    = 0,  //!< background
	TM_FGD    = 1,  //!< foreground
	TM_PR_BGD = 2,  //!< most probably background  -->unknown area 
	TM_PR_FGD = 3,   //!< most probably foreground  -->unknown area
	TM_UNKOWN = 4
};

//typedef const Mat CMat_t;

// TODO: �ڴ˴����ó�����Ҫ������ͷ�ļ�
