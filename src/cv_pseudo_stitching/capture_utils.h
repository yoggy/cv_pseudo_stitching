#pragma once

extern bool init_video(const int &device);
extern bool init_image(const char *filename);
extern bool init_movie(const char *filename);
extern bool capture(cv::Mat &capture_img);
extern void finish();
