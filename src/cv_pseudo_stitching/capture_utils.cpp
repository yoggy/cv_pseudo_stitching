#include "StdAfx.h"
#include "capture_utils.h"

class CaptureBase {
public:
	CaptureBase() : now_t_(0), last_t_(0), diff_t_(0) {};
	virtual ~CaptureBase() {};
	virtual bool init() = 0;
	virtual void capture(cv::Mat &capture_img) = 0;
	virtual void finish() = 0;

protected:
	inline void check_pre_() {
		now_t_ = ::GetTickCount();
		diff_t_ = now_t_ - last_t_;
	}

	inline void check_post_() {
		if (diff_t_ < 33) {
			::Sleep(diff_t_);
		}
		last_t_ = ::GetTickCount();
	}

protected:
	DWORD now_t_;
	DWORD last_t_;
	DWORD diff_t_;
};

class CaptureWebCam : public CaptureBase {
public:
	CaptureWebCam(int device) : CaptureBase(), device_(device) {};
	virtual ~CaptureWebCam() {};
	virtual bool init() {
		if (capture_.open(device_) == false) {
			return false;
		}
		return true;
	}

	virtual void capture(cv::Mat &capture_img) {
		capture_ >> capture_img;
	}

	virtual void finish() {
		capture_.release();
	}

protected:
	int device_;
	cv::VideoCapture capture_;
};

class CaptureMovie : public CaptureBase {
public:
	CaptureMovie(const char *filename) : CaptureBase(), filename_(filename), last_t_(0) {};
	virtual ~CaptureMovie() {};
	virtual bool init() {
		if (capture_.open(filename_.c_str()) == false) {
			return false;
		}
		return true;
	}

	virtual void capture(cv::Mat &capture_img) {
		check_pre_();
		capture_ >> capture_img;
		check_post_();
	}

	virtual void finish() {
		capture_.release();
	}

protected:
	std::string filename_;
	cv::VideoCapture capture_;
	DWORD last_t_;
};

class CaptureImage : public CaptureBase {
public:
	CaptureImage(const char *filename) : CaptureBase(), filename_(filename), image_() {};
	virtual ~CaptureImage() {};
	virtual bool init() {
		image_ = cv::imread(filename_.c_str(), 1);
		if (image_.empty()) {
			return false;
		}
		return true;
	}

	virtual void capture(cv::Mat &capture_img) {
		check_pre_();
		image_.copyTo(capture_img);
		check_post_();
	}

	virtual void finish() {
		image_.release();
	}

protected:
	std::string filename_;
	cv::Mat image_;
};

////////////////////////////////////////////////////////////////////////////

CaptureBase *source_ = 0;

bool init_video(const int &device)
{
	if (source_ != NULL) {
		printf("error : init() has already been called...\n");
		return false;
	}

	source_ = new CaptureWebCam(device);
	if (source_->init() == false) {
		printf("error : init_video() failed...device=%d\n", device);
		finish();
		return false;
	}

	return true;
}

bool init_image(const char *filename)
{
	if (source_ != NULL) {
		printf("error : init() has already been called...\n");
		return false;
	}

	source_ = new CaptureImage(filename);
	if (source_->init() == false) {
		printf("error : init_image() failed...filename=%s\n", filename);
		finish();
		return false;
	}

	return true;
}

bool init_movie(const char *filename)
{
	if (source_ != NULL) {
		printf("error : init() has already been called...\n");
		return false;
	}

	source_ = new CaptureMovie(filename);
	if (source_->init() == false) {
		printf("error : init_movie() failed...filename=%s\n", filename);
		finish();
		return false;
	}

	return true;
}

bool capture(cv::Mat &capture_img) 
{
	if (source_ == NULL) {
		printf("error : capture() failed... init_????() function is not called ye\n");
		::Sleep(1000);
		return false;
	}

	source_->capture(capture_img);
	return true;
}

void finish()
{
	if (source_ != NULL) {
		source_->finish();
		delete source_; 
		source_ = NULL;
	}
}

