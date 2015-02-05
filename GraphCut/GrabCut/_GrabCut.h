#include <opencv2/opencv.hpp>
#include "graph.h"
#include "GMM.hpp"
//! class of the pixel in GrabCut algorithm
typedef Graph<double,double,double> GraphType;
class _GrabCut
{
public:
	_GrabCut(void);
	~_GrabCut(void);
	void grab_Cut( Mat _img, Mat& _mask, Rect& rect,
		Mat &_bgdModel, Mat& _fgdModel,
		int iterCount, int mode  );
private:
	double calcBeta( const Mat& img );
	void calcNWeights( const Mat& img, Mat& leftW, Mat& upleftW, Mat& upW, Mat& uprightW, double beta, double gamma );
	void checkMask( const Mat& img, const Mat& mask );
	void initMaskWithRect( Mat& mask, Size imgSize, Rect rect );
	void initGMMs( const Mat& img, const Mat& mask, GMM& bgdGMM, GMM& fgdGMM );
	void assignGMMsComponents( const Mat& img, const Mat& mask, const GMM& bgdGMM, const GMM& fgdGMM, Mat& compIdxs );
	void learnGMMs( const Mat& img, const Mat& mask, const Mat& compIdxs, GMM& bgdGMM, GMM& fgdGMM );
	void constructGCGraph( const Mat& img, const Mat& mask, const GMM& bgdGMM, const GMM& fgdGMM, double lambda,
		const Mat& leftW, const Mat& upleftW, const Mat& upW, const Mat& uprightW, GraphType& graph);
	void estimateSegmentation(GraphType& graph,Mat& mask );



};