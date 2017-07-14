#include "facerec.h"

// im = grayscale image [0,..,255]
// s = {1,..5}
// o = {1,..,8}
cv::Mat gabor_sum_sum(cv::Mat im)
{
	std::ofstream fout;

	int norient = 8;
	int nscale = 5;
	double sigmaOnf = .75;
	double dThetaOnSigma = 1.5;
	int wave[5] = {5, 10, 20, 40, 80};

	int rows = im.rows;
	int cols = im.cols;
	
	im.convertTo(im, CV_64F); // convert to type double

	cv::Mat imagefft(rows,cols,CV_64FC2);

	imagefft = fft2(im); // use fftw 2D DFT function here.


	/*fout.open("gss_imagefft.txt");
	for(int i = 0; i < imagefft.rows; i++)
	{
		fout.width(15);
		for(int j = 0; j < imagefft.cols; j++)
		{
			fout << imagefft.at<cv::Vec2d>(i,j)[0];
			if(imagefft.at<cv::Vec2d>(i,j)[1]>=0)
				fout << "+";
			fout << imagefft.at<cv::Vec2d>(i,j)[1] << "i ";
		}
		fout << std::endl;
	}
	fout.close();*/

	cv::Mat x(rows,cols,CV_64F), y(rows,cols,CV_64F);	
	std::vector<double> firstParam, secondParam;

	for(double i = -cols/2.0; i <=(cols/2.0)-1; i++)
	{
		firstParam.push_back(i/cols);
	}

	for(double i = -rows/2.0; i <=(rows/2.0)-1; i++)
	{
		secondParam.push_back(i/rows);
	}

	/*fout.open("gss_fp.txt");
	for(unsigned int i = 0; i < firstParam.size(); i++)
	{
		fout << firstParam[i] << " ";
	}
	fout.close();
	fout.open("gss_sp.txt");
	for(unsigned int i = 0; i < secondParam.size(); i++)
	{
		fout << secondParam[i] << " ";
	}*/

	meshgrid(firstParam, secondParam, x, y); 

	/*fout.open("gss_x.txt");
	for(int i = 0; i < x.rows; i++)
	{
		for(int j = 0; j < x.cols; j++)
			fout << x.at<double>(i,j) << " ";
		fout << std::endl;
	}
	fout.close();
	fout.open("gss_y.txt");
	for(int i = 0; i < y.rows; i++)
	{
		for(int j = 0; j < y.cols; j++)
			fout << y.at<double>(i,j) << " ";
		fout << std::endl;
	}*/

	cv::Mat radius(rows,cols,CV_64F);	
	cv::Mat xsquared(rows,cols,CV_64F), ysquared(rows,cols,CV_64F);
	cv::Mat sum(rows,cols,CV_64F);	

	cv::pow(x,2,xsquared);
	cv::pow(y,2, ysquared);
	sum = xsquared + ysquared;
	cv::sqrt(sum,radius);

	radius.at<double>((int)round((rows/2)+1),(int)round((cols/2)+1)) = 1.0f;

	/*fout.open("gss_radius.txt");
	for(int i = 0; i < radius.rows; i++)
	{
		for(int j = 0; j < radius.cols; j++)
			fout << radius.at<double>(i,j) << " ";
		fout << std::endl;
	}*/

	cv::Mat theta(rows, cols, CV_64F), sintheta(rows, cols, CV_64F), costheta(rows, cols, CV_64F);

	for(int i = 0; i < theta.rows; i++)
		for(int j = 0; j < theta.cols; j++)
		{
			theta.at<double>(i,j) = std::atan2(-y.at<double>(i,j), x.at<double>(i,j));
			sintheta.at<double>(i,j) = std::sin(theta.at<double>(i,j));
			costheta.at<double>(i,j) = std::cos(theta.at<double>(i,j));
		}
	
	/*fout.open("gss_theta.txt");
	for(int i = 0; i < theta.rows; i++)
	{
		for(int j = 0; j < theta.cols; j++)
			fout << theta.at<double>(i,j) << " ";
		fout << std::endl;
	}*/

	double pi = 3.1416;
	double thetaSigma = (pi/norient)/dThetaOnSigma;	

	cv::Mat filter(rows, cols, CV_64F, cv::Scalar(0));

	for(int o=1; o <= norient; o++)
	{
		double angl = (o-1)*pi/norient;			// input of o = {1,..,8}

		cv::Mat ds(rows, cols, CV_64F), dc(rows, cols, CV_64F);

		ds = (sintheta * cos(angl)) - (costheta * sin(angl));
		dc = (costheta * cos(angl)) + (sintheta * sin(angl));  

		/*fout.open("ds.txt");
		for(int i = 0; i < ds.rows; i++)
		{
			for(int j = 0; j < ds.cols; j++)
				fout << ds.at<double>(i,j) << " ";
			fout << std::endl;
		}
		fout.close();
		fout.open("dc.txt");
		for(int i = 0; i < dc.rows; i++)
		{
			for(int j = 0; j < dc.cols; j++)
				fout << dc.at<double>(i,j) << " ";
			fout << std::endl;
		}*/

		cv::Mat dtheta(rows,cols, CV_64F);
		for(int i = 0; i < dtheta.rows; i++)
			for(int j = 0; j < dtheta.cols; j++)
			{
				dtheta.at<double>(i,j) = std::fabs(std::atan2(ds.at<double>(i,j),dc.at<double>(i,j)));
			}
		/*fout.open("dtheta.txt");
		for(int i = 0; i < dtheta.rows; i++)
		{
			for(int j = 0; j < dtheta.cols; j++)
				fout << dtheta.at<double>(i,j) << " ";
			fout << std::endl;
		}*/		
		cv::Mat spread(rows,cols, CV_64F);
		cv::Mat dtheta_squared(rows,cols, CV_64F);

		cv::pow(dtheta,2,dtheta_squared);
		cv::exp((-dtheta_squared) / (2 * std::pow(thetaSigma,2)),spread);
		
		/*fout.open("spread.txt");
		for(int i = 0; i < spread.rows; i++)
		{
			for(int j = 0; j < spread.cols; j++)
				fout << spread.at<double>(i,j) << " ";
			fout << std::endl;
		}*/

		for(int s = 1; s <= nscale; s++)
		{
			int wavelength = wave[s-1]; // input range of s ={1,..,5}
			double fo = 1.0/wavelength;

			cv::Mat logGabor(rows,cols,CV_64F);
			cv::Mat logterm(rows,cols,CV_64F); 
			cv::Mat logterm_squared(rows,cols,CV_64F); 
			
			cv::log(radius/fo, logterm);
			/*fout.open("logterm.txt");
			fout << "fo=" << fo << std::endl;
			for(int i = 0; i < logterm.rows; i++)
			{
				for(int j = 0; j < logterm.cols; j++)
					fout << logterm.at<double>(i,j) << " ";
				fout << std::endl;
			}*/
			cv::pow(logterm, 2, logterm_squared);
			cv::Mat expterm = (-logterm_squared) / (2 * std::pow(std::log(sigmaOnf),2));
			/*fout.open("expterm.txt");
			for(int i = 0; i < expterm.rows; i++)
			{
				for(int j = 0; j < expterm.cols; j++)
					fout << expterm.at<double>(i,j) << " ";
				fout << std::endl;
			}*/

			cv::exp(expterm, logGabor);

			logGabor.at<double>((int)round((rows/2)+1),(int)round((cols/2)+1)) = 0.0;

			/*fout.open("logGabor.txt");
			for(int i = 0; i < logGabor.rows; i++)
			{
				for(int j = 0; j < logGabor.cols; j++)
					fout << logGabor.at<double>(i,j) << " ";
				fout << std::endl;
			}*/

			cv::Mat res(rows, cols, CV_64F);
			res = logGabor.mul(spread); // element-wise multiplication

			/*fout.open("res.txt");
			for(int i = 0; i < res.rows; i++)
			{
				for(int j = 0; j < res.cols; j++)
					fout << res.at<double>(i,j) << " ";
				fout << std::endl;
			}*/

			filter = filter + fftshift(res);
			
		}
	}

	/*fout.open("gss_filter.txt");
	for(int i = 0; i < filter.rows; i++)
	{
		for(int j = 0; j < filter.cols; j++)
		{
			fout.width(15);
			fout << filter.at<double>(i,j) << " ";
		}
		fout << std::endl;
	}
	fout.close();*/

	cv::Mat temp(rows, cols, CV_64FC2);
	for( int i = 0; i <imagefft.rows; i++)
		for(int j = 0; j < imagefft.cols; j++)
		{
			temp.at<cv::Vec2d>(i,j)[0] = imagefft.at<cv::Vec2d>(i,j)[0] * filter.at<double>(i,j);
			temp.at<cv::Vec2d>(i,j)[1] = imagefft.at<cv::Vec2d>(i,j)[1] * filter.at<double>(i,j);
		}

	/*fout.open("gss_temp.txt");
	for(int i = 0; i < temp.rows; i++)
	{
		for(int j = 0; j < temp.cols; j++)
		{
			fout.width(15);
			fout << temp.at<cv::Vec2d>(i,j)[0];
			if(temp.at<cv::Vec2d>(i,j)[1]>=0)
				fout << "+";
			fout << temp.at<cv::Vec2d>(i,j)[1] << "i ";
		}
		fout << std::endl;
	}
	fout.close();*/

	cv::Mat EO(rows, cols, CV_64FC2);
	EO = ifft2(temp);    

	/*fout.open("gss_eo.txt");
	for(int i = 0; i < EO.rows; i++)
	{
		fout.width(15);
		for(int j = 0; j < EO.cols; j++)
		{
			fout << EO.at<cv::Vec2d>(i,j)[0];
			if(EO.at<cv::Vec2d>(i,j)[1]>=0)
				fout << "+";
			fout << EO.at<cv::Vec2d>(i,j)[1] << "i ";
		}
		fout << std::endl;
	}
	fout.close();*/

	/*cv::Mat EO_Mag(rows, cols, CV_64F);
	for(int i = 0; i < EO_Mag.rows; i++)
		for(int j = 0; j < EO_Mag.cols; j++)
		{
			EO_Mag.at<double>(i,j) = std::sqrt(std::pow(EO.at<cv::Vec2d>(i,j)[0],2) +
									 std::pow(EO.at<cv::Vec2d>(i,j)[1],2));
		}	*/

	cv::Mat EO_Pha(rows, cols, CV_64F);

	for(int i = 0; i < EO_Pha.rows; i++)
		for(int j = 0; j < EO_Pha.cols; j++)
		{
			EO_Pha.at<double>(i,j) = std::atan2(EO.at<cv::Vec2d>(i,j)[1], EO.at<cv::Vec2d>(i,j)[0]);
		}	

	/*fout.open("gss_eo_pha.txt");
	for(int i = 0; i < EO_Pha.rows; i++)
	{
		for(int j = 0; j < EO_Pha.cols; j++)
		{
			fout.width(15);
			fout << EO_Pha.at<double>(i,j)/40 << " ";
		}
		fout << std::endl;
	}*/
	fout.close();
	return EO_Pha / 40;

}
