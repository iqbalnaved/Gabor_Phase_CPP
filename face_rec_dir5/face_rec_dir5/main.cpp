// Face Recognition using Gabor Wavelet: Face matched by NxN comparison of Local Histograms 
// and performing histogram intersection.

#include "facerec.h"


void main()
{
	int ploc, gloc;
	int region = 64, bin = 256;

//	ofstream fout("E:\\OpenCV_Projects\\face_rec_dir4\\gmuv_result10.txt");
//	ofstream fout("E:\\OpenCV_Projects\\face_rec_dir4\\gmuv_result100.txt");
//	ofstream fout("E:\\OpenCV_Projects\\face_rec_dir4\\gss_result10.txt");
	ofstream fout("E:\\OpenCV_Projects\\face_rec_dir4\\gss_result100.txt");
	
	vector<string> gallery_files;
	vector<string> probe_files;
	ListDirectoryContents("E:\\Face Recognition\\Gabor_Phase_Project\\db\\gallery100\\", &gallery_files);
	ListDirectoryContents("E:\\Face Recognition\\Gabor_Phase_Project\\db\\probes100\\", &probe_files);

	const int sz[] = {bin, region, gallery_files.size()};
	cv::MatND LH_List_g(3, sz, CV_8U);
	cv::MatND LH_List_p(3, sz, CV_8U);

	int k = 0;
	for(unsigned int i=0; i<gallery_files.size(); i++)
	{
		string gim_path = gallery_files[i];
		gloc = gallery_files[i].rfind("\\");
		string gfilename = gallery_files[i].substr(gloc+1, gallery_files[i].size()-gloc);
		cout << "Encoding gallery image " << gfilename << endl;
		cv::Mat gabor_pha_gim, lgbp_pha_gim, LH_g;
		cv::Mat gim = cv::imread(gim_path, 0); // flag=0, force to be a grayscale for accurte dft
		gim = gim / 255;
		gabor_pha_gim = gabor_sum_sum(gim);
//		gabor_pha_gim = gabor_mu_v(gim, 3, 3); // scale=3, orient=3
		lgbp_pha_gim = imageToLGBP(gabor_pha_gim); // 128x128
		LH_g = local_histogram(lgbp_pha_gim, region, bin);
		for(int r = 0; r < region; r++)
			for(int b = 0; b < bin; b++)
				LH_List_g.at<uchar>(b,r,k) = LH_g.at<uchar>(b,r);
		k++;
	}

	k = 0;
	for(unsigned int j = 0; j < probe_files.size(); j++)
	{
		string pim_path = probe_files[j];
		ploc = probe_files[j].rfind("\\");
		string pfilename = probe_files[j].substr(ploc+1, probe_files[j].size()-ploc);
		cout << "Encoding probe image " << pfilename << endl;
		cv::Mat gabor_pha_pim, lgbp_pha_pim , LH_p;
		cv::Mat pim = cv::imread(pim_path, 0);			
		pim = pim / 255;
		gabor_pha_pim = gabor_sum_sum(pim);	
//		gabor_pha_pim = gabor_mu_v(pim, 3, 3);	
		lgbp_pha_pim = imageToLGBP(gabor_pha_pim); // 128x128
		LH_p = local_histogram(lgbp_pha_pim, region, bin);
		for(int r = 0; r < region; r++)
			for(int b = 0; b < bin; b++)
				LH_List_p.at<uchar>(b,r,k) = LH_p.at<uchar>(b,r);
		k++;
	}

	int match = 0;
	for(unsigned int i=0; i<gallery_files.size(); i++)
	{
		string gim_path = gallery_files[i];
		double max_sm = 0;
		int indx = 0;
		string max_pid;
		gloc = gallery_files[i].rfind("\\");
		string gid = gallery_files[i].substr(gloc+1, 5);

		cv::Mat LH_g(bin, region, CV_8U);
		for(int r = 0; r < region; r++)
			for(int b = 0; b < bin; b++)
				LH_g.at<uchar>(b,r) = LH_List_g.at<uchar>(b,r,i);

		for(unsigned int j = 0; j < probe_files.size(); j++)
		{
			string pim_path = probe_files[j];

			cv::Mat LH_p(bin, region, CV_8U);
			for(int r = 0; r < region; r++)
				for(int b = 0; b < bin; b++)
					LH_p.at<uchar>(b,r) = LH_List_p.at<uchar>(b,r,j);

			double sm = hist_int(LH_g,LH_p,region,bin);

//			fout << gallery_files[i].substr(gallery_files[i].length()-21, 21) << " " 
//				 << probe_files[j].substr(probe_files[j].length()-21, 21) << " " 
//				 << sm << endl;
//			cout << gallery_files[i].substr(gallery_files[i].length()-21, 21) << " " 
//				 << probe_files[j].substr(probe_files[j].length()-21, 21) << " " 
//				 << sm << endl;			

			if(sm > max_sm)
			{
				max_sm = sm;
				ploc = probe_files[j].rfind("\\");
				max_pid = probe_files[j].substr(ploc+1, 5);
				indx = j;
			}
		}
		if(gid == max_pid)
		{
			fout << gallery_files[i].substr(gloc+1, gallery_files[i].size()-gloc) 
				 << " matched with " 
				 << probe_files[indx].substr(ploc+1, probe_files[indx].size()-ploc) << endl; 

			cout << gallery_files[i].substr(gloc+1, gallery_files[i].size()-gloc) 
				 << " matched with " 
				 << probe_files[indx].substr(ploc+1, probe_files[indx].size()-ploc) << endl;
			match++;
		}
		else
		{
			fout << gallery_files[i].substr(gloc+1, gallery_files[i].size()-gloc) 
				 << " mismatched with " 
				 << probe_files[indx].substr(ploc+1, probe_files[indx].size()-ploc) << endl;

			cout << gallery_files[i].substr(gloc+1, gallery_files[i].size()-gloc) 
				 << " mismatched with " 
				 << probe_files[indx].substr(ploc+1, probe_files[indx].size()-ploc) << endl;

		} 
	}

	fout << "Total match: " << match << endl;
	cout << "Total match: " << match << endl;

	getchar();

}

/*
void main()
{
	ofstream fout;
	int region=64,bin=256;
	cv::Mat image, gaborImage, lgbp, LH;

	image = cv::imread("E:\\OpenCV_Projects\\test.pgm", CV_LOAD_IMAGE_GRAYSCALE);
//	gaborImage = gabor_mu_v(image, 3, 3);
	gaborImage = gabor_sum_sum(image);

	fout.open("E:\\OpenCV_Projects\\face_rec_dir4\\gpi.txt");
	for(int i = 0; i < gaborImage.rows; i++)
	{
		for(int j = 0; j < gaborImage.cols; j++)
		{
			fout.width(15);
			fout << gaborImage.at<double>(i,j) << " ";
		}
		fout << endl;
	}
	fout.close();

	lgbp = imageToLGBP(gaborImage); // 128x128

	fout.open("E:\\OpenCV_Projects\\face_rec_dir4\\lgbp.txt");
	for(int i = 0; i < lgbp.rows; i++)
	{
		for(int j = 0; j < lgbp.cols; j++)
		{
			fout.width(15);
			fout << lgbp.at<double>(i,j) << " ";
		}
		fout << endl;
	}
	fout.close();

	LH = local_histogram(lgbp, region, bin);
	fout.open("E:\\OpenCV_Projects\\face_rec_dir4\\LH.txt");
	for(int i = 0; i < bin; i++)
	{
		for(int j = 0; j < region; j++)
		{
			fout.width(3);
			fout << (int) LH.at<uchar>(i,j) << " ";
		}
		fout << endl;
	}
	fout.close();
	//cv::imwrite("E:\\OpenCV_Projects\\gabor_matlab\\gimg.jpg", gaborImage);
	//cv::imshow("Gabor phase image at s=3, o=3", gaborImage);
	cv::waitKey(0);
}
*/

/*
// s.m. between two images.
void main()
{
	ofstream fout;
	int region = 64;
	int bin = 256;
	cv::Mat image1, gaborImage1, lgbp1, LH1;
	cv::Mat image2, gaborImage2, lgbp2, LH2;

	image1 = cv::imread("E:\\OpenCV_Projects\\test.pgm", CV_LOAD_IMAGE_GRAYSCALE);
	gaborImage1 = gabor_sum_sum(image1);*/
	
	/*fout.open("gpi1.txt");
	for(int i = 0; i < gaborImage1.rows; i++)
	{
		for(int j = 0; j < gaborImage1.cols; j++)
		{
			fout << gaborImage1.at<double>(i,j) << " ";
		}
		fout << endl;
	}
	fout.close();*/

//	lgbp1 = imageToLGBP(gaborImage1); // 128x128
	
	/*fout.open("lgbp1.txt");
	for(int i = 0; i < lgbp1.rows; i++)
	{
		for(int j = 0; j < lgbp1.rows; j++)
		{
			fout << lgbp1.at<double>(i,j) << " ";
		}
		fout << endl;
	}
	fout.close();*/

//	LH1 = local_histogram(lgbp1, region, bin);

	/*fout.open("LH1.txt");
	for(int i = 0; i < region; i++)
	{
		for(int j = 0; j < bin; j++)
		{
			fout << (int) LH1.at<uchar>(i,j) << " ";
		}
		fout << endl;
	}
	fout.close();*/

//	image2 = cv::imread("E:\\OpenCV_Projects\\test2.pgm", CV_LOAD_IMAGE_GRAYSCALE);
//	gaborImage2 = gabor_sum_sum(image2);

	/*fout.open("gpi2.txt");
	for(int i = 0; i < gaborImage2.rows; i++)
	{
		for(int j = 0; j < gaborImage2.cols; j++)
		{
			fout << gaborImage2.at<double>(i,j) << " ";
		}
		fout << endl;
	}
	fout.close();*/

//	lgbp2 = imageToLGBP(gaborImage2); // 128x128

	/*fout.open("lgbp2.txt");
	for(int i = 0; i < lgbp2.rows; i++)
	{
		for(int j = 0; j < lgbp2.rows; j++)
		{
			fout << lgbp2.at<double>(i,j) << " ";
		}
		fout << endl;
	}
	fout.close();*/

//	LH2 = local_histogram(lgbp2, region, bin);

	/*fout.open("LH2.txt");
	for(int i = 0; i < region; i++)
	{
		for(int j = 0; j < bin; j++)
		{
			fout << (int) LH2.at<uchar>(i,j) << " ";
		}
		fout << endl;
	}
	fout.close();*/

//	double sm = hist_int(LH1,LH2,region,bin);

//	cout << "sm = " << sm;

//	cin.get();
//}

