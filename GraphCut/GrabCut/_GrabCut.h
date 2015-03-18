#include <opencv2/opencv.hpp>
#include "graph.h"
#include "GMM.hpp"
//! class of the pixel in GrabCut algorithm
typedef Graph<double,double,double> GraphType;
class _GrabCut
{
public:

static	void grab_Cut( Mat _img, Mat& _mask, Rect& rect,
		Mat &_bgdModel, Mat& _fgdModel,
		int iterCount, int mode  );
private:
static	double calcBeta( const Mat& img );
static	void calcNWeights( const Mat& img, Mat& leftW, Mat& upleftW, Mat& upW, Mat& uprightW, double beta, double gamma );
static	void checkMask( const Mat& img, const Mat& mask );
static	void initMaskWithRect( Mat& mask, Size imgSize, Rect rect );
static	void initGMMs( const Mat& img, const Mat& mask, GMM& bgdGMM, GMM& fgdGMM );
static	void assignGMMsComponents( const Mat& img, const Mat& mask, const GMM& bgdGMM, const GMM& fgdGMM, Mat& compIdxs );
static	void learnGMMs( const Mat& img, const Mat& mask, const Mat& compIdxs, GMM& bgdGMM, GMM& fgdGMM );
static	void constructGCGraph( const Mat& img, const Mat& mask, const GMM& bgdGMM, const GMM& fgdGMM, double lambda,
		const Mat& leftW, const Mat& upleftW, const Mat& upW, const Mat& uprightW, GraphType& graph);
static	void estimateSegmentation(GraphType& graph,Mat& mask );



};