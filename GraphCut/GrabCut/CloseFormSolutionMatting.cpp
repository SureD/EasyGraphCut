#include "CloseFormSolutionMatting.h"

closedFormSolutionMatting::closedFormSolutionMatting( const Mat* coarse_image,const Mat* coarse_hardSegmentation, Mat &mask,const Mat* pOriginalImage):
m_coarse_w(coarse_image->cols), m_coarse_h(coarse_image->rows),
m_original_w(mask.cols),m_original_h(mask.rows),
m_epsilon(0.0000001),m_win_size(1),
m_alpha_threshold(90),m_original_image(pOriginalImage)
{
	m_coarse_image = coarse_image;
	m_coarse_hardSegmentation = coarse_hardSegmentation;
	m_mask = &mask;
	m_original_hardSegmentation = new Mat(m_original_h,m_original_w,CV_8UC1,0);

}
closedFormSolutionMatting::~closedFormSolutionMatting()
{
	m_coarse_image = NULL;
	m_original_image = NULL;
	m_coarse_hardSegmentation = NULL;
	delete m_coarse_hardSegmentation;
}
void closedFormSolutionMatting::GetLinearCoeff(const Mat& alpha_mat, const Mat& image, coeffcientImages *coeff)
{
	unsigned int i, j, x, y, k, index, width, height;
	width = image.cols;
	height = image.rows;

	unsigned int neb_size = (m_win_size*2+1)*(m_win_size*2+1);
	double sqrt_epsilon = sqrt(m_epsilon);

	Mat G_data;
	G_data.create(Size(4,neb_size+3),CV_64FC1);
	Mat alpha_data;
	alpha_data.create(Size(1,neb_size+3),CV_64FC1);
	Mat coeff_data;
	coeff_data.create(Size(1,4),CV_64FC1);

	for (k=0;k<neb_size;k++)
	{
		G_data.at<double>(k,3) = 1.0;
	}
	for (k=0;k<3;k++)
	{
		G_data.at<double>(neb_size+k,k) = sqrt_epsilon;
	}
	for (i=m_win_size;i<height-m_win_size;i++)
	{
		for (j=m_win_size;j<width-m_win_size;j++)
		{
			index = 0;
			for (y = i-m_win_size;y<=i+m_win_size;y++)
			{
				for (x=j-m_win_size;x<=j+m_win_size;x++)
				{
					for (k=0;k<3;k++)
					{
						G_data.at<double>(index,k)=image.at<Vec3b>(y,x)[k];
					}
					alpha_data.at<double>(index,0)=alpha_mat.at<double>(y,x);
					index++;
				}
			}
			solve(G_data,alpha_data,coeff_data,DECOMP_SVD);
			coeff->setElement(i,j,coeff_data);
		}
	}
	for (j=0;j<width;j++)
	{
		for (k=0;k<m_win_size;k++)
		{
			coeff->setElement(k,j,coeff,m_win_size,j);
		}
		for(k=height-m_win_size;k<height;k++)
			coeff->setElement(k,j,coeff,height-m_win_size-1,j);
	}
	for(i=0;i<height;i++)
	{
		for (k=0;k<m_win_size;k++)
		{
			coeff->setElement(i,k,coeff,i,m_win_size);
		}
		for(k=width-m_win_size;k<width;k++)
		{
			coeff->setElement(i,k,coeff,i,width-m_win_size-1);
		}
	}
}

void closedFormSolutionMatting::UpSamplingAlpha(const Mat& coarse_alpha,const Mat& coarse_image, const Mat& original_image, const Mat& coarse_hardSegmentation,
												Mat& original_hardSegmenattion, Mat& triMaps)
{
	unsigned int coarse_w,coarse_h,original_w,original_h;
	coarse_w = coarse_image.cols;
	coarse_h = coarse_image.rows;
	original_w = triMaps.cols;
	original_h = triMaps.rows;

	coeffcientImages* p_coarse_coeff = new coeffcientImages(coarse_w,coarse_h);
	GetLinearCoeff(coarse_alpha,coarse_image,p_coarse_coeff);

	Mat original_coeff(original_h,original_w,CV_64FC4,Scalar::all(0));
	resize(p_coarse_coeff->getcoeffMatrix(),original_coeff,original_coeff.size());

	//Debug
	std::cout<<"original_coeff's size: "<<original_coeff.size()<<std::endl;


	TrimapValue cur_seg;	//
	for (int i=0;i<original_h;i++)
	{
		for (int j=0;j<original_w;j++)
		{
			switch (triMaps.at<uchar>(i,j))
			{
			case TM_BGD:
				original_hardSegmenattion.at<uchar>(i,j) = TM_BGD;
				break;
			case TM_FGD:
				original_hardSegmenattion.at<uchar>(i,j) = TM_BGD;
				break;
			case TM_PR_FGD:
				double t_alpha = 0.0;
				Vec3b t_originalColors = original_image.at<Vec3b>(i,j);
				Vec4d t_originalCoeffs = original_coeff.at<Vec4d>(i,j);
				for (int k=0;k<3;k++)
				{
					t_alpha += ((double)t_originalColors[k]*t_originalCoeffs[k]);
				}
				t_alpha += t_originalCoeffs[3];
				if (t_alpha<m_alpha_threshold)
				{
					cur_seg = TM_FGD;
				} 
				else
				{
					cur_seg = TM_BGD;
				}
				original_hardSegmenattion.at<uchar>(i,j) = cur_seg;
				break;
			}
		}
	}
	delete p_coarse_coeff;
}
/*
===============================================================
Calculate the original hardsegmentation result by closed formed
method

originalHardSegmentation:Mat CV_8UC1

===============================================================
*/
void closedFormSolutionMatting::Execute(Mat& originalHardSegmentation)
{
	Mat curAlphaMat;
	curAlphaMat.create(m_coarse_hardSegmentation->size(),CV_64FC1);
	CalcAlphaMat(curAlphaMat);

	//Debug
	//FileStorage fs("alphmat.txt",FileStorage::WRITE);
	//fs<<"curAlphaMat"<<curAlphaMat<<"coarse_hard"<<(*m_coarse_hardSegmentation)<<"trimap"<<(*m_mask);
	
	originalHardSegmentation.create(m_original_image->size(),CV_8UC1);

	//Debug
	std::cout<<"originalHardSegmentation's size"<<originalHardSegmentation.size()<<std::endl;


	UpSamplingAlpha(curAlphaMat,*m_coarse_image,*m_original_image,*m_coarse_image,originalHardSegmentation,*m_mask);
}
/* 
=============================================================
   calculate the alpha mat according to the hard segmentation
   cutAlphaMat :CV_64F1
=============================================================
*/  
void closedFormSolutionMatting::CalcAlphaMat(Mat& curAlphaMat)
{
	assert(curAlphaMat.size()==m_coarse_hardSegmentation->size());
	for (int i=0;i<m_coarse_hardSegmentation->rows;i++)
	{
		for (int j=0;j<m_coarse_hardSegmentation->cols;j++)
		{
			curAlphaMat.at<double>(i,j) = m_coarse_hardSegmentation->at<uchar>(i,j)==TM_PR_FGD? 1.0: 0.0;
		}
	}
}