#include "facerec.h"

cv::Mat fft2(cv::Mat I)
{
	cv::Mat padded;                            //expand input image to optimal size

	int m = cv::getOptimalDFTSize( I.rows );
	int n = cv::getOptimalDFTSize( I.cols ); // on the border add zero values

	cv::copyMakeBorder(I, padded, 0, m - I.rows, 0, n - I.cols, cv::BORDER_CONSTANT, cv::Scalar::all(0));

	cv::Mat planes[] = {cv::Mat_<double>(padded), cv::Mat::zeros(padded.size(), CV_64F)};

	cv::Mat complexI(padded.size(), CV_64FC2);

	cv::merge(planes, 2, complexI);         // Add to the expanded another plane with zeros

	cv::dft(complexI, complexI);            // this way the result may fit in the source matrix

	return complexI;

	
}

cv::Mat ifft2(cv::Mat imageFFT)
{
    cv::Mat invFFT;
	cv::idft(imageFFT, invFFT, cv::DFT_COMPLEX_OUTPUT);
	
	return invFFT;
}

// 2-D Meshgrid formation
void meshgrid(std::vector<double> firstParam, std::vector<double> secondParam, cv::Mat &x, cv::Mat &y)
{
	std::ofstream fout;
	int c = firstParam.size();
	int r = secondParam.size();

	for(int i = 0; i < r; i++)
	{
		for(int j = 0; j < c; j++)
		{
			x.at<double>(i,j) = firstParam[j];
		}
	}

	for(int i = 0; i < r; i++)
	{
		for(int j = 0; j < c; j++)
		{
			y.at<double>(j,i)= secondParam[j];
		}
	}

	/*fout.open("x2.txt");
	for(int i = 0; i < x.rows; i++)
	{
		for(int j = 0; j < x.cols; j++)
			fout << x.at<double>(i,j) << " ";
		fout << std::endl;
	}

	fout.close();
	fout.open("y2.txt");
	for(int i = 0; i < y.rows; i++)
	{
		for(int j = 0; j < y.cols; j++)
			fout << y.at<double>(i,j) << " ";
		fout << std::endl;
	}*/

}

// fftshift for 2D matrices
cv::Mat fftshift(cv::Mat x)
{
	int m, n;      // FFT row and column dimensions might be different
	int m2, n2;

	m = x.rows-1; n = x.cols-1;
	m2 = m / 2;    // half of row dimension
	n2 = n / 2;    // half of column dimension

	// interchange entries in 4 quadrants, 1 <--> 3 and 2 <--> 4
	std::vector<int> idx1;
	for(int i = m2+1; i <= m; i++)
	{
		idx1.push_back(i);
	}
	for(int i = 0; i <=m2; i++)
	{
		idx1.push_back(i);
	}

	std::vector<int> idx2;
	for(int i = n2+1; i <=n; i++)
	{
		idx2.push_back(i);
	}
	for(int i = 0; i <=n2; i++)
	{
		idx2.push_back(i);
	}

	cv::Mat y(x.rows, x.cols, CV_64F);
	for (unsigned int i = 0; i < idx1.size(); i++)
	{
		 for (unsigned int j = 0; j < idx2.size(); j++)
		 {
			 y.at<double>(i,j) = x.at<double>(idx1[i],idx2[j]);
		 }
	}

	return y;
}

// histogram intersection function
double hist_int(cv::Mat LH1, cv::Mat LH2, int region, int bin)
{
	double sum = 0;
	for (int r = 0; r < region; r++)
	{
		for(int b = 0; b < bin; b++)
		{
			if(LH1.at<uchar>(b,r)<=LH2.at<uchar>(b,r))
			{
				sum = sum + LH1.at<uchar>(b,r);
			}
			else
			{
				sum = sum + LH2.at<uchar>(b,r);
			}
		}
	}
	return sum;
}

cv::Mat local_histogram(cv::Mat lgbp, int nregion, int nbin)
{
	int rsize = (lgbp.rows * lgbp.cols) / nregion; // 256=16x16
	int rrows = (int)sqrt((float)rsize); // 16
	int rcols = (int)sqrt((float)rsize); // 16

	cv::Mat LH;
	LH = cv::Mat::zeros(nbin, nregion, CV_8U); //64x256

	int rgnPerRow = (int)(lgbp.rows/rrows);
	int x = 0, y = 0;
	int r = 0;
	while(r < nregion)
	{		
		for(int i = x; i < x+rrows; i++)
		{
			for(int j = y; j < y+rcols; j++)
			{
				// lgbp_r.at<double>(i, j, r) = lgbp.at<double>(rw,cl);				
				int bin = (int)lgbp.at<double>(i,j);				
				LH.at<uchar>(bin,r)++;
			}
		} // rrows loop

		r++;

		if(r % rgnPerRow == 0) 
		{
			x = x + rrows;
			y = 0;
		}
		else
		{
			y = y + rcols;
		}
	} // nregion loop

	return LH;
}

cv::Mat imageToLGBP(cv::Mat image) 
{
	cv::Mat lgbp, paddedimage;
	
	lgbp.create(image.rows, image.cols, CV_64F);	
	paddedimage = cv::Mat::zeros(image.rows+2, image.cols+2, CV_64F);

	// store image within zero pad
	for(int i = 1; i < image.rows + 1; i++)
	{
		for ( int j = 1; j < image.cols + 1; j++)
		{
			paddedimage.at<double>(i,j) = image.at<double>(i-1,j-1);
		}
	}

	// calculate LGBP image
	for(int i = 1; i < image.rows + 1; i++)
	{
		for ( int j = 1; j < image.cols + 1; j++)
		{
		   lgbp.at<double>(i-1,j-1) = (paddedimage.at<double>(i-1,j-1) >= paddedimage.at<double>(i,j))      +
									  (paddedimage.at<double>(i-1,j)   >= paddedimage.at<double>(i,j)) * 2  +
									  (paddedimage.at<double>(i-1,j+1) >= paddedimage.at<double>(i,j)) * 4  +
									  (paddedimage.at<double>(i,  j+1) >= paddedimage.at<double>(i,j)) * 8  +
									  (paddedimage.at<double>(i+1,j+1) >= paddedimage.at<double>(i,j)) * 16 +
									  (paddedimage.at<double>(i+1,j)   >= paddedimage.at<double>(i,j)) * 32 +
									  (paddedimage.at<double>(i+1,j-1) >= paddedimage.at<double>(i,j)) * 64 +
									  (paddedimage.at<double>(i,  j-1) >= paddedimage.at<double>(i,j)) * 128;
		}
	}

	return lgbp;
}

