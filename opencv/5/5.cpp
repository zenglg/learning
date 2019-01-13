#include <opencv/highgui.h>

#ifndef IPL_GAUSSIAN_5x5
# define IPL_GAUSSIAN_5x5	CV_GAUSSIAN_5x5
#endif

static IplImage *doPyrDown(IplImage *in, int filter = IPL_GAUSSIAN_5x5)
{
	assert(in->width % 2 == 0 && in->height % 2 == 0);

	IplImage *out = cvCreateImage(cvSize(in->width / 2, in->height / 2),
				      in->depth, in->nChannels);

	cvPyrDown(in, out);

	return out;
}

static IplImage *doCanny(IplImage *in, double lowThresh, double highThresh,
			 double aperture)
{
	/*
	if (in->nChannels != 1)
		return NULL;
	*/

	IplImage *out = cvCreateImage(cvGetSize(in), IPL_DEPTH_8U, 1);

	cvCanny(in, out, lowThresh, highThresh, aperture);

	return out;
}

int main(int argc, char *argv[])
{
	IplImage *img = cvLoadImage(argv[1]);

	IplImage *out_pyrdown = NULL;

	IplImage *out_canny = NULL;

	cvNamedWindow("Example5-in");

	cvNamedWindow("Example5-PyrDown");

	cvNamedWindow("Example5-Canny");

	cvShowImage("Example5-in", img);

	out_pyrdown = doPyrDown(img, IPL_GAUSSIAN_5x5);
	cvShowImage("Example5-PyrDown", out_pyrdown);

	out_canny = doCanny(img, 10, 100, 3);
	cvShowImage("Example5-Canny", out_canny);

	cvWaitKey(0);

	cvDestroyWindow("Example5-Canny");

	cvDestroyWindow("Example5-PyrDown");

	cvDestroyWindow("Example5-in");

	cvReleaseImage(&out_canny);

	cvReleaseImage(&out_pyrdown);

	cvReleaseImage(&img);

	return 0;
}
