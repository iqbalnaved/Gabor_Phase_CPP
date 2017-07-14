# Gabor Phase Project (C++)

A partial C++ port to some of the MATLAB functions used in 'Gabor Phase Project'

## List of ported functions

* cv::Mat fft2(cv::Mat)
* cv::Mat ifft2(cv::Mat)
* cv::Mat fftshift(cv::Mat x)
* void meshgrid(std::vector<double> firstParam, std::vector<double> secondParam, cv::Mat &x, cv::Mat &y)
* bool ListDirectoryContents(const char *sDir, vector<string> *file_list)
* cv::Mat imageToLGBP(cv::Mat)
* cv::Mat local_histogram(cv::Mat lgbp, int nregion, int nbin)
* double hist_int(cv::Mat LH1, cv::Mat LH2, int region, int bin)
* cv::Mat gabor_mu_v(cv::Mat image, int orient, int scale)
* cv::Mat gabor_sum_sum(cv::Mat image)
* inline double round( double d )
