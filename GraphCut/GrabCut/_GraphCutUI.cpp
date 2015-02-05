#include "_GraphCutUI.h"
void _GraphCutUI::getBinaryMask( const Mat& comMask, Mat& binMask )
{
	if( comMask.empty() || comMask.type()!=CV_8UC1 )
		CV_Error( CV_StsBadArg, "comMask is empty or has incorrect type (not CV_8UC1)" );
	if( binMask.empty() || binMask.rows!=comMask.rows || binMask.cols!=comMask.cols )
		binMask.create( comMask.size(), CV_8UC1 );
	binMask = comMask & 1;
}
void _GraphCutUI::setImageAndWinName( const Mat& _image, const string& _winName  )
{
	if( _image.empty() || _winName.empty() )
		return;
	image = &_image;
	winName = &_winName;
	mask.create( image->size(), CV_8UC1);
	reset();
}
void _GraphCutUI::mouseClick( int event, int x, int y, int flags, void* )
{
	// TODO add bad args check
	switch( event )
	{
	case CV_EVENT_LBUTTONDOWN: // set rect or GC_BGD(GC_FGD) labels
		{
			bool isb = (flags & BGD_KEY) != 0,
				isf = (flags & FGD_KEY) != 0;
			if( rectState == NOT_SET && !isb && !isf )
			{
				rectState = IN_PROCESS;
				rect = Rect( x, y, 1, 1 );
			}
			if ( (isb || isf) && rectState == SET )
				lblsState = IN_PROCESS;
		}
		break;
	case CV_EVENT_RBUTTONDOWN: // set GC_PR_BGD(GC_PR_FGD) labels
		{
			bool isb = (flags & BGD_KEY) != 0,
				isf = (flags & FGD_KEY) != 0;
			if ( (isb || isf) && rectState == SET )
				prLblsState = IN_PROCESS;
		}
		break;
	case CV_EVENT_LBUTTONUP:
		if( rectState == IN_PROCESS )
		{
			rect = Rect( Point(rect.x, rect.y), Point(x,y) );
			rectState = SET;
			setRectInMask();
			assert( bgdPxls.empty() && fgdPxls.empty() && prBgdPxls.empty() && prFgdPxls.empty() );
			showImage();
		}
		if( lblsState == IN_PROCESS )
		{
			setLblsInMask(flags, Point(x,y), false);
			lblsState = SET;
			showImage();
		}
		break;
	case CV_EVENT_RBUTTONUP:
		if( prLblsState == IN_PROCESS )
		{
			setLblsInMask(flags, Point(x,y), true);
			prLblsState = SET;
			showImage();
		}
		break;
	case CV_EVENT_MOUSEMOVE:
		if( rectState == IN_PROCESS )
		{
			rect = Rect( Point(rect.x, rect.y), Point(x,y) );
			assert( bgdPxls.empty() && fgdPxls.empty() && prBgdPxls.empty() && prFgdPxls.empty() );
			showImage();
		}
		else if( lblsState == IN_PROCESS )
		{
			setLblsInMask(flags, Point(x,y), false);
			showImage();
		}
		else if( prLblsState == IN_PROCESS )
		{
			setLblsInMask(flags, Point(x,y), true);
			showImage();
		}
		break;
	}
}
void _GraphCutUI::showImage() const
{
	if( image->empty() || winName->empty() )
		return;

	Mat res;
	Mat binMask;
	if( !isInitialized )
		image->copyTo( res );
	else
	{
		getBinaryMask( mask, binMask );
		image->copyTo( res, binMask );
	}

	vector<Point>::const_iterator it;
	for( it = bgdPxls.begin(); it != bgdPxls.end(); ++it )
		circle( res, *it, radius, BLUE, thickness );
	for( it = fgdPxls.begin(); it != fgdPxls.end(); ++it )
		circle( res, *it, radius, RED, thickness );
	for( it = prBgdPxls.begin(); it != prBgdPxls.end(); ++it )
		circle( res, *it, radius, LIGHTBLUE, thickness );
	for( it = prFgdPxls.begin(); it != prFgdPxls.end(); ++it )
		circle( res, *it, radius, PINK, thickness );

	if( rectState == IN_PROCESS || rectState == SET )
		rectangle( res, Point( rect.x, rect.y ), Point(rect.x + rect.width, rect.y + rect.height ), GREEN, 2);

	imshow( *winName, res );
}
void _GraphCutUI::setRectInMask()
{
	assert( !mask.empty() );
	mask.setTo( TM_BGD );
	rect.x = max(0, rect.x);
	rect.y = max(0, rect.y);
	rect.width = min(rect.width, image->cols-rect.x);
	rect.height = min(rect.height, image->rows-rect.y);
	(mask(rect)).setTo( Scalar(TM_PR_FGD) );
}

void _GraphCutUI::setLblsInMask( int flags, Point p, bool isPr )
{
	vector<Point> *bpxls, *fpxls;
	uchar bvalue, fvalue;
	if( !isPr )
	{
		bpxls = &bgdPxls;
		fpxls = &fgdPxls;
		bvalue = TM_BGD;
		fvalue = TM_FGD;
	}
	else
	{
		bpxls = &prBgdPxls;
		fpxls = &prFgdPxls;
		bvalue = TM_PR_BGD;
		fvalue = TM_PR_FGD;
	}
	if( flags & BGD_KEY )
	{
		bpxls->push_back(p);
		circle( mask, p, radius, bvalue, thickness );
	}
	if( flags & FGD_KEY )
	{
		fpxls->push_back(p);
		circle( mask, p, radius, fvalue, thickness );
	}
}
void _GraphCutUI::reset()
{
	if( !mask.empty() )
		mask.setTo(Scalar::all(GC_BGD));
	bgdPxls.clear(); fgdPxls.clear();
	prBgdPxls.clear();  prFgdPxls.clear();

	isInitialized = false;
	rectState = NOT_SET;
	lblsState = NOT_SET;
	prLblsState = NOT_SET;
}
void _GraphCutUI::getMask(Mat& _mask)
{
	if(rectState==SET)
	{
		isInitialized = true;
		mask.copyTo(_mask);
	}
	else
		return;
}