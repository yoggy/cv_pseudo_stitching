#pragma once

class CvFeature
{
public:
	CvFeature(void);
	CvFeature(const int &threshold);
	CvFeature(const CvFeature &image_feature);

	virtual ~CvFeature(void);

	bool empty() const;
	void clear();
	void copy_to(CvFeature &dst) const;

	int threshold() const;
	void threshold(const int &val);

	cv::Mat image() const;
	void image(cv::Mat image);

	cv::Mat debugImage();

	std::vector<cv::KeyPoint> keypoints() const;
	cv::Mat descriptors() const;

	std::vector<cv::Point2f> pts() const;

	bool process();

	bool findHomography(const CvFeature &dst, cv::Mat &homography_mat, const double &min_distance_threshold = 0.20, const double &good_matches_distance_threshold = 3.0, const int &matches_count_threshold = 10);

	void drawKeypoints(cv::Mat &canvas);

	static void drawKeypoints(cv::Mat &canvas, std::vector<cv::KeyPoint> &keypoints);

protected:
	int threshold_;
	cv::Mat image_;
	std::vector<cv::KeyPoint> keypoints_;
	cv::Mat descriptors_;
};

