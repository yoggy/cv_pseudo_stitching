#include "stdafx.h"
#include "capture_utils.h"
#include "CvFeature.h"

cv::Mat capture_img;

cv::Mat center_img;
cv::Mat target_img;
cv::Mat canvas_img;
cv::Mat perspective_img;
cv::Mat mask_img;
cv::Mat white_img;

CvFeature center_img_feature;

cv::Mat homograpy_mat;

bool running = false;

void start() {
	running = true;
	target_img.copyTo(center_img);
	center_img_feature.image(center_img);
}

void clear() {
	running = false;
	canvas_img = CV_RGB(0, 0, 0);
	mask_img = 0;
	white_img = 0;
	
	cv::Rect roi(161, 121, 318, 238);
	white_img(roi) = 255;
}

void process()
{
	bool rv;
	CvFeature target_img_feature;

	cv::resize(capture_img, capture_img, cv::Size(320, 240));

	target_img = 0;
	cv::Rect roi(160, 120, 320, 240);
	capture_img.copyTo(target_img(roi));

	if (running) {
		target_img_feature.image(target_img);
		
		rv = target_img_feature.findHomography(center_img_feature, homograpy_mat);

		if (rv == true) {
			cv::warpPerspective(target_img, perspective_img, homograpy_mat, perspective_img.size());
			cv::warpPerspective(white_img, mask_img, homograpy_mat, mask_img.size());
			perspective_img.copyTo(canvas_img, mask_img);
		}
	}
	else {
		target_img.copyTo(canvas_img);
	}
}

int main(int argc, char* argv[])
{
	bool rv;

	rv = init_video(0);
	//rv = init_image("source.png");
	//rv = init_movie("source.m4v");
	if (rv == false) return -1;

	// 
	target_img.create(cv::Size(640, 480), CV_8UC3);
	canvas_img.create(cv::Size(640, 480), CV_8UC3);
	mask_img.create(cv::Size(640, 480), CV_8UC1);
	white_img.create(cv::Size(640, 480), CV_8UC1);

	clear();

	while(true) {
		capture(capture_img);
		process();

		cv::imshow("canvas_img", canvas_img);

		int c = cv::waitKey(1);
		if (c == 27) {
			break;
		}
		else if (c == ' ') {
			start();
		}
		else if (c == 'c') {
			clear();
		}
	}

	finish();

	cv::destroyAllWindows();

	return 0;
}

