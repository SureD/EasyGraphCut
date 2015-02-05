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


	MultiResolutionCut test(img,mask);
	test.cut();
	test.getTriMap(mask);
	_CutUI.reSetMask(mask);
	_CutUI.showImage();
	waitKey(0);


	

}
MultiResolutionCut::MultiResolutionCut(Mat& img,Mat& mask)
	:_triMap(mask),_imgMat(img),_iterCount(3),_w(img.cols),_h(img.rows),_gamma(50)
{
	_lambda = _gamma*9;
}
MultiResolutionCut::~MultiResolutionCut()
{
	if(_pGraph)
		delete _pGraph;
	if(_pfGMM)
		delete _pfGMM;
	if (_pbGMM)
		delete _pbGMM;
}
void MultiResolutionCut::initialize()
{
	_beta = calcBeta();
	calcNWeights();
}
void MultiResolutionCut::buildGMMs()
{
	_pbGMM = new GMM(_bgdModel);
	_pfGMM = new GMM(_fgdModel);
	_compIdxs.create(_imgMat.size(), CV_32SC1);
	// may be need check mask

	initGMMs();
}
double MultiResolutionCut::calcBeta()
{
	double beta = 0;
	for( int y = 0; y < _imgMat.rows; y++ )
	{
		for( int x = 0; x < _imgMat.cols; x++ )
		{
			Vec3d color = _imgMat.at<Vec3b>(y,x);
			if( x>0 ) // left
			{
				Vec3d diff = color - (Vec3d)_imgMat.at<Vec3b>(y,x-1);
				beta += diff.dot(diff);
			}
			if( y>0 && x>0 ) // upleft
			{
				Vec3d diff = color - (Vec3d)_imgMat.at<Vec3b>(y-1,x-1);
				beta += diff.dot(diff);
			}
			if( y>0 ) // up
			{
				Vec3d diff = color - (Vec3d)_imgMat.at<Vec3b>(y-1,x);
				beta += diff.dot(diff);
			}
			if( y>0 && x<_imgMat.cols-1) // upright
			{
				Vec3d diff = color - (Vec3d)_imgMat.at<Vec3b>(y-1,x+1);
				beta += diff.dot(diff);
			}
		}
	}
	if( beta <= std::numeric_limits<double>::epsilon() )
		beta = 0;
	else
		beta = 1.f / (2 * beta/(4*_imgMat.cols*_imgMat.rows - 3*_imgMat.cols - 3*_imgMat.rows + 2) );

	return beta;
}
void MultiResolutionCut::calcNWeights()
{
	const double gammaDivSqrt2 = _gamma / std::sqrt(2.0f);
	_leftW.create( _imgMat.rows, _imgMat.cols, CV_64FC1 );
	_upleftW.create( _imgMat.rows, _imgMat.cols, CV_64FC1 );
	_upW.create( _imgMat.rows, _imgMat.cols, CV_64FC1 );
	_uprightW.create( _imgMat.rows, _imgMat.cols, CV_64FC1 );
	for( int y = 0; y < _imgMat.rows; y++ )
	{
		for( int x = 0; x < _imgMat.cols; x++ )
		{
			Vec3d color = _imgMat.at<Vec3b>(y,x);
			if( x-1>=0 ) // left
			{
				Vec3d diff = color - (Vec3d)_imgMat.at<Vec3b>(y,x-1);
				_leftW.at<double>(y,x) = _gamma * exp(-_beta*diff.dot(diff));
			}
			else
				_leftW.at<double>(y,x) = 0;
			if( x-1>=0 && y-1>=0 ) // upleft
			{
				Vec3d diff = color - (Vec3d)_imgMat.at<Vec3b>(y-1,x-1);
				_upleftW.at<double>(y,x) = gammaDivSqrt2 * exp(-_beta*diff.dot(diff));
			}
			else
				_upleftW.at<double>(y,x) = 0;
			if( y-1>=0 ) // up
			{
				Vec3d diff = color - (Vec3d)_imgMat.at<Vec3b>(y-1,x);
				_upW.at<double>(y,x) = _gamma * exp(-_beta*diff.dot(diff));
			}
			else
				_upW.at<double>(y,x) = 0;
			if( x+1<_imgMat.cols && y-1>=0 ) // upright
			{
				Vec3d diff = color - (Vec3d)_imgMat.at<Vec3b>(y-1,x+1);
				_uprightW.at<double>(y,x) = gammaDivSqrt2 * exp(-_beta*diff.dot(diff));
			}
			else
				_uprightW.at<double>(y,x) = 0;
		}
	}
}
void MultiResolutionCut::initGMMs()
{
	const int kMeansItCount = 10;
	const int kMeansType = KMEANS_PP_CENTERS;

	Mat bgdLabels, fgdLabels;
	vector<Vec3f> bgdSamples, fgdSamples;
	Point p;
	for( p.y = 0; p.y < _imgMat.rows; p.y++ )
	{
		for( p.x = 0; p.x < _imgMat.cols; p.x++ )
		{
			if( _triMap.at<uchar>(p) == GC_BGD || _triMap.at<uchar>(p) == GC_PR_BGD )
				bgdSamples.push_back( (Vec3f)_imgMat.at<Vec3b>(p) );
			else // GC_FGD | GC_PR_FGD
				fgdSamples.push_back( (Vec3f)_imgMat.at<Vec3b>(p) );
		}
	}
	CV_Assert( !bgdSamples.empty() && !fgdSamples.empty() );
	Mat t_bgdSamples( (int)bgdSamples.size(), 3, CV_32FC1, &bgdSamples[0][0] );
	kmeans( t_bgdSamples, GMM::componentsCount, bgdLabels,
		TermCriteria( CV_TERMCRIT_ITER, kMeansItCount, 0.0), 0, kMeansType );
	Mat t_fgdSamples( (int)fgdSamples.size(), 3, CV_32FC1, &fgdSamples[0][0] );
	kmeans( t_fgdSamples, GMM::componentsCount, fgdLabels,
		TermCriteria( CV_TERMCRIT_ITER, kMeansItCount, 0.0), 0, kMeansType );

	_pbGMM->initLearning();
	for( int i = 0; i < (int)bgdSamples.size(); i++ )
		_pbGMM->addSample( bgdLabels.at<int>(i,0), bgdSamples[i] );
	_pbGMM->endLearning();

	_pfGMM->initLearning();
	for( int i = 0; i < (int)fgdSamples.size(); i++ )
		_pfGMM->addSample( fgdLabels.at<int>(i,0), fgdSamples[i] );
	_pfGMM->endLearning();
}

/*
  Assign GMMs components for each pixel.
*/
void MultiResolutionCut::assignGMMsComponents()
{
	Point p;
	for( p.y = 0; p.y < _imgMat.rows; p.y++ )
	{
		for( p.x = 0; p.x < _imgMat.cols; p.x++ )
		{
			Vec3d color = _imgMat.at<Vec3b>(p);
			_compIdxs.at<int>(p) = _triMap.at<uchar>(p) == TM_BGD || _triMap.at<uchar>(p) == TM_PR_BGD ?
				_pbGMM->whichComponent(color) : _pfGMM->whichComponent(color);
		}
	}
}

/*
  Learn GMMs parameters.
*/
void MultiResolutionCut::learnGMMs()
{
	_pbGMM->initLearning();
	_pfGMM->initLearning();
	Point p;
	for( int ci = 0; ci < GMM::componentsCount; ci++ )
	{
		for( p.y = 0; p.y < _imgMat.rows; p.y++ )
		{
			for( p.x = 0; p.x < _imgMat.cols; p.x++ )
			{
				if( _compIdxs.at<int>(p) == ci )
				{
					if( _triMap.at<uchar>(p) == TM_BGD || _triMap.at<uchar>(p) == TM_PR_BGD )
						_pbGMM->addSample( ci, _imgMat.at<Vec3b>(p) );
					else
						_pfGMM->addSample( ci, _imgMat.at<Vec3b>(p) );
				}
			}
		}
	}
	_pbGMM->endLearning();
	_pfGMM->endLearning();
}
void MultiResolutionCut::constructGCGraph()
{
	int vtxCount = _imgMat.cols*_imgMat.rows,
		edgeCount = 2*(4*_imgMat.cols*_imgMat.rows - 3*(_imgMat.cols + _imgMat.rows) + 2);
	_pGraph = new GraphD(vtxCount,edgeCount);
	Point p;
	int vtxIdx = -1;
	for( p.y = 0; p.y < _imgMat.rows; p.y++ )
	{
		for( p.x = 0; p.x < _imgMat.cols; p.x++)
		{
			// add node
			vtxIdx++;
			_pGraph->add_node();

			//int vtxIdx = graph.addVtx();
			Vec3b color = _imgMat.at<Vec3b>(p);

			// set t-weights
			double fromSource, toSink;
			if( _triMap.at<uchar>(p) == TM_PR_BGD || _triMap.at<uchar>(p) == TM_PR_FGD )
			{
				fromSource = -log( (*_pbGMM)(color) );
				toSink = -log( (*_pfGMM)(color) );
			}
			else if( _triMap.at<uchar>(p) == TM_BGD )
			{
				fromSource = 0;
				toSink = _lambda;
			}
			else // GC_FGD
			{
				fromSource = _lambda;
				toSink = 0;
			}
			//graph.addTermWeights( vtxIdx, fromSource, toSink );
			_pGraph->add_tweights(vtxIdx,fromSource,toSink);
			// set n-weights
			if( p.x>0 )
			{
				double w = _leftW.at<double>(p);
				//graph.addEdges( vtxIdx, vtxIdx-1, w, w );
				_pGraph->add_edge(vtxIdx,vtxIdx-1,w,w);
			}
			if( p.x>0 && p.y>0 )
			{
				double w = _upleftW.at<double>(p);
				//graph.addEdges( vtxIdx, vtxIdx-img.cols-1, w, w );
				_pGraph->add_edge(vtxIdx,vtxIdx-_imgMat.cols-1,w,w);
			}
			if( p.y>0 )
			{
				double w = _upW.at<double>(p);
				//graph.addEdges( vtxIdx, vtxIdx-img.cols, w, w );
				_pGraph->add_edge(vtxIdx,vtxIdx-_imgMat.cols,w,w);
			}
			if( p.x<_imgMat.cols-1 && p.y>0 )
			{
				double w = _uprightW.at<double>(p);
				//graph.addEdges( vtxIdx, vtxIdx-img.cols+1, w, w );
				_pGraph->add_edge(vtxIdx,vtxIdx-_imgMat.cols+1,w,w);
			}
		}
	}
}
void MultiResolutionCut::estimateSegmentation()
{
	_pGraph->maxflow();
	Point p;
	for( p.y = 0; p.y < _triMap.rows; p.y++ )
	{
		for( p.x = 0; p.x < _triMap.cols; p.x++ )
		{
			if( _triMap.at<uchar>(p) == TM_PR_BGD || _triMap.at<uchar>(p) == TM_PR_FGD )
			{
				//if( graph.inSourceSegment( p.y*mask.cols+p.x /*vertex index*/ ) )
				if(_pGraph->what_segment(p.y*_triMap.cols+p.x /*vertex index*/ ) == GraphF::SOURCE)
					_triMap.at<uchar>(p) = TM_PR_FGD;
				else
					_triMap.at<uchar>(p) = TM_PR_BGD;
			}
		}
	}
}

void MultiResolutionCut::cut()
{
	initialize();
	buildGMMs();
	for (int i=0;i<1;i++)
	{
		assignGMMsComponents();
		learnGMMs();
		constructGCGraph();
		estimateSegmentation();
		_pGraph->reset();
	}
}
