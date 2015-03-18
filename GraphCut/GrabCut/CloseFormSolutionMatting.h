#pragma once
#include "../stdafx.h"
#include<vector>

using namespace cv;
struct coeffcientImages
{
	Mat * _pcoeffMatrix;
	coeffcientImages(unsigned int width, unsigned int height)
	{
	
			_pcoeffMatrix = new Mat();    // 4 channels a1,a2,a3,b;
			_pcoeffMatrix->create(Size(width,height),CV_64FC4);
	}
	~coeffcientImages()
	{

			delete _pcoeffMatrix;
	}
	/*inline Vec4f& operator()(int nRow,int nCol){return _pcoeffMatrix->at<Vec4f>(nRow,nCol);}*/

	void setElement(int dst_x/*rows*/, int dst_y/*cols*/, Mat& value){
		unsigned int k;
		for (k=0;k<4;k++)
			(*_pcoeffMatrix).at<Vec4d>(dst_x,dst_y)[k]=value.at<double>(k);
			//(*this)(dst_x,dst_y)[k] = value.at<double>(k);
	}

	void setElement(int dst_x/*rows*/, int dst_y/*cols*/,coeffcientImages* src_coeff,int src_x/*rows*/, int src_y/*cols*/ )
	{
		unsigned int k;
		for (k=0;k<4;k++)
			(*_pcoeffMatrix).at<Vec4d>(dst_x,dst_y)[k] = (src_coeff->_pcoeffMatrix)->at<Vec4d>(src_x,src_y)[k];
	}
	const Mat&		getcoeffMatrix() const {return *_pcoeffMatrix;} 
};

class closedFormSolutionMatting
{
public:
					closedFormSolutionMatting( const Mat* coarse_image,const Mat* coarse_hardSegmentation,
											    Mat &mask,const Mat* pOriginalImage);
					~closedFormSolutionMatting();
	void			 Execute(Mat &originalHardSegmentation);


private:
	void			UpSamplingAlpha(const Mat& coarse_alpha,const Mat& coarse_image, const Mat& original_image,
									const Mat& coarse_hardSegmentation, Mat& original_hardSegmentation,Mat& triMaps);

	void			GetLinearCoeff(const Mat& alpha_mat, const Mat& image, coeffcientImages *coeff);

	void			CalcAlphaMat(Mat& curAlphaMat);

	const Mat*		m_coarse_image;
	const Mat*		m_coarse_hardSegmentation;
	const Mat*		m_original_image;
	Mat*			m_original_hardSegmentation;
	Mat*			m_mask;

	int				m_original_w,m_original_h;
	int				m_coarse_w,m_coarse_h;
	double			m_epsilon;
	int				m_win_size;
	double			m_alpha_threshold;
};