#include <opencv/highgui.h>

static void example2_4(IplImage *image)
{
	cvNamedWindow("Example4-in");

	cvNamedWindow("Example4-out");

	cvShowImage("Example4-in", image);

	IplImage *out = cvCreateImage(cvGetSize(image), IPL_DEPTH_8U, 3);

	cvSmooth(image, out, CV_GAUSSIAN, 3, 3);

	cvShowImage("Example4-out", out);

	cvReleaseImage(&out);

	cvWaitKey(0);

	cvDestroyWindow("Example4-in");

	cvDestroyWindow("Example4-out");
}

int main(int argc, char *argv[])
{
	IplImage *img = cvLoadImage(argv[1]);

	example2_4(img);

	cvReleaseImage(&img);

	return 0;
}
