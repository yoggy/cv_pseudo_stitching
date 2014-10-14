#include "StdAfx.h"
#include "CvFeature.h"
#include <algorithm>
#include <iterator> 

CvFeature::CvFeature(void) : threshold_(2000)
{
}

CvFeature::CvFeature(const int &threshold) : threshold_(threshold)
{
	
}

CvFeature::CvFeature(const CvFeature &image_feature)
{
	image_feature.copy_to(*this);
}

CvFeature::~CvFeature(void)
{
	clear();
}

bool CvFeature::empty() const
{
	return image_.empty();
}

void CvFeature::clear()
{
	image_.release();
	keypoints_.clear();
	descriptors_.release();
}

void CvFeature::copy_to(CvFeature &dst) const
{
	this->image_.copyTo(dst.image_);

	dst.keypoints_.clear();
	std::copy(this->keypoints_.begin(), this->keypoints_.end(), std::back_inserter(dst.keypoints_));

	this->descriptors_.copyTo(dst.descriptors_);
}

int CvFeature::threshold() const
{
	return threshold_;
}

void CvFeature::threshold(const int &val)
{
	threshold_ = val;
	process();
}

cv::Mat CvFeature::image() const
{
	return image_;
}

void CvFeature::image(cv::Mat image)
{
	image.copyTo(image_);
	process();
}

cv::Mat CvFeature::debugImage()
{
	if (empty()) return cv::Mat();

	cv::Mat canvas;
	image_.copyTo(canvas);	

	canvas.create(image_.size(), CV_8UC3);
	drawKeypoints(canvas, keypoints_);

	return canvas;
}

std::vector<cv::KeyPoint> CvFeature::keypoints() const
{
	return keypoints_;
}

cv::Mat CvFeature::descriptors() const
{
	return descriptors_;
}

std::vector<cv::Point2f> CvFeature::pts() const 
{
	std::vector<cv::Point2f> pts;

	if (empty()) {
		return pts;
	}

	// counter clockwise
	pts.push_back(cv::Point2f(0.0f, 0.0f));
	pts.push_back(cv::Point2f(image_.cols - 1.0f, 0.0f));
	pts.push_back(cv::Point2f(image_.cols - 1.0f, image_.rows - 1.0f));
	pts.push_back(cv::Point2f(0.0f, image_.rows - 1.0f));

	return pts;
}

bool CvFeature::process()
{
	if (empty()) return false;

	cv::Mat gray_img;
	cv::cvtColor(image_, gray_img, CV_BGR2GRAY);
	cv::normalize(gray_img, gray_img, 0, 255, cv::NORM_MINMAX);

	cv::SurfFeatureDetector detector(threshold_);
	detector.detect(gray_img, keypoints_);

	cv::SurfDescriptorExtractor extractor;
	extractor.compute(image_, keypoints_, descriptors_);

	return true;
}

bool CvFeature::findHomography(const CvFeature &dst, cv::Mat &homography_mat, const double &min_distance_threshold, const double &matches_distance_threshold, const int &matches_count_threshold)
{
	if (dst.descriptors().empty()) return false;

	homography_mat.release();

	std::vector<cv::DMatch> matches;
	std::vector<cv::DMatch> good_matches;
	std::vector<cv::DMatch>::iterator it;

	cv::BFMatcher matcher(cv::NORM_L2);

	matcher.match(this->descriptors(), dst.descriptors(), matches);

	double min_d = DBL_MAX;
	for (it = matches.begin(); it != matches.end(); ++it) {
	    double d = it->distance;
	    if (d < min_d) min_d = d;
	}

	if (min_d > min_distance_threshold) return false;
 
	for (it = matches.begin(); it != matches.end(); ++it) {
		if (it->distance <= min_d * matches_distance_threshold) good_matches.push_back(*it);
	}

	if (good_matches.size() < (unsigned int)matches_count_threshold) return false;

	std::vector<cv::Point2f> src_ps;
	std::vector<cv::Point2f> dst_ps;
	for (it = good_matches.begin(); it != good_matches.end(); ++it) {
		src_ps.push_back(this->keypoints()[it->queryIdx].pt);		
		dst_ps.push_back(dst.keypoints()[it->trainIdx].pt);		
	}

	homography_mat = cv::findHomography(src_ps, dst_ps, cv::RANSAC);

	return true;
}

void CvFeature::drawKeypoints(cv::Mat &canvas)
{
	drawKeypoints(canvas, keypoints_);
}

void CvFeature::drawKeypoints(cv::Mat &canvas, std::vector<cv::KeyPoint> &keypoints)
{
	std::vector<cv::KeyPoint>::iterator it;

	for(it = keypoints.begin(); it != keypoints.end(); ++it) {
		cv::Point pt((int)(it->pt.x + cos(it->angle)*it->size / 3), (int)(it->pt.y + sin(it->angle)*it->size / 3));
		cv::line(canvas, it->pt, pt, CV_RGB(0, 255, 0), 2, CV_AA);
		cv::circle(canvas, it->pt, 2, CV_RGB(255, 0, 0), -1);
	}
}
