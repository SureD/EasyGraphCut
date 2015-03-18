// stdafx.h : 标准系统包含文件的包含文件，
// 或是经常使用但不常更改的
// 特定于项目的包含文件
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

// TODO: 在此处引用程序需要的其他头文件
