#define _CRT_SECURE_NO_DEPRECATE
#include <opencv2\opencv.hpp>
#include <opencv2\highgui\highgui.hpp>
#include <stdio.h>
#include <iostream>
#include <vector>
#include <string>

#define PI 3.14159265359
using namespace std;
using namespace cv;


Mat Color2Binary(Mat, int); //轉灰階後二值化，可選閥值
vector<vector<int>> Connected_Components(Mat, Mat, int);

Mat myerode(Mat, int);
Mat mydilate(Mat, int);
Mat myopening(Mat, int);
Mat mysubtraction(Mat, Mat);
void fingerINhand(vector<vector<int>>, vector<vector<int>>,Mat);

int main()
{
	//Image Read
	Mat hand = imread("hand.bmp", 1);
	//彩色->灰階->二值化
	Mat Bhand = Color2Binary(hand, 230);
	Mat Bfinger = Color2Binary(hand, 230);
	//連通元件
	
	vector<vector<int>> box = Connected_Components(Bhand, hand, 0);
	
	Mat open_finger = myopening(Bfinger, 25);
	Mat sub_finger = mysubtraction(open_finger, Bfinger);
	Mat result_finger = myopening(sub_finger, 11);
	

	vector<vector<int>> centroid = Connected_Components(result_finger, hand, 1);

	fingerINhand(box,centroid,hand);
	
	imshow("hand", hand);

	waitKey(0);

	return 0;
}
Mat Color2Binary(Mat image, int threshold) {
	int height = image.rows, width = image.cols;
	//color2gray
	Mat GBimage = Mat(height, width, CV_8UC1);
	for (int i = 0; i < height; i++) {
		for (int j = 0; j < width; j++) {
			int b = image.at<Vec3b>(i, j)[0];
			int g = image.at<Vec3b>(i, j)[1];
			int r = image.at<Vec3b>(i, j)[2];
			GBimage.at<uchar>(i, j) = (b + g + r) / 3;
		}
	}
	//gray2bianry
	for (int i = 0; i < height; i++) {
		for (int j = 0; j < width; j++) {
			int g = GBimage.at<uchar>(i, j);
			GBimage.at<uchar>(i, j) = g > threshold ? 0 : 255;
		}
	}
	return GBimage;
}
vector<vector<int>> Connected_Components(Mat image, Mat original, int color) {
	int height = image.rows, width = image.cols;

	vector<vector<int> > eq_table(100, vector<int>(2)); //先預開100*2的衝突表
	vector<int> table_w(2);
	int table_add = 10;

	int way[2][2] = { {-1,0},{0,-1} }; //copy 標籤方向
	int way_label[2];
	int label = 1; //標籤數字
	int count = 0; //table表大小
	for (int i = 0; i < height; i++) {
		for (int j = 0; j < width; j++) {
			if (image.at<uchar>(i, j) == 0) continue;
			for (int k = 0; k < 2; k++) {
				int ni = i + way[k][0];
				int nj = j + way[k][1];
				//過邊界，設值為0
				if (ni < 0 || ni >= height || nj < 0 || nj >= width) {
					way_label[k] = 0;
					continue;
				}
				way_label[k] = image.at<uchar>(ni, nj);
			}
			int min_label = min(way_label[0], way_label[1]);
			int max_label = max(way_label[0], way_label[1]);
			//---狀況1:建立新label---//
			if (max_label == 0) {
				image.at<uchar>(i, j) = label;
				label++;
			}
			//---狀況2:直接複製---//
			else if (min_label == 0 || min_label == max_label) {
				image.at<uchar>(i, j) = max_label;
			}
			//---狀況3:處理衝突點---//
			else {
				image.at<uchar>(i, j) = min_label;

				//衝突狀況放入table表
				eq_table[count][0] = min_label;
				eq_table[count][1] = max_label;
				count++;
				//確認table表不重複
				
				if (count >= eq_table.size())
					eq_table.resize(eq_table.size() + table_add, table_w);
				
				for (int a = 0; a < count - 1; a++) {
					if (eq_table[a][0] == min_label && eq_table[a][1] == max_label) {
						count--;
						break;
					}
				}
			}
		}
	}

	printf("label:%d,count:%d\n", label - 1, count);
	//複製一份equivalent table

	vector<vector<int>> eq_copy(count, vector<int>(2));
	for (int i = 0; i < count; i++) {
		eq_copy[i][0] = eq_table[i][0];
		eq_copy[i][1] = eq_table[i][1];
		
	}

	//將eq_copy整理分類
	for (int i = 0; i < count; i++) {
		int target = eq_copy[i][0];
		int change = eq_copy[i][1];
		for (int j = 0; j < count; j++) {
			if (eq_copy[j][0] == change) eq_copy[j][0] = target;
			if (eq_copy[j][1] == change) eq_copy[j][1] = target;
		}
	}

	vector<vector<int>> eq_relation(10, vector<int>(count));
	vector<int> relation_w(count);
	int relation_add = 3;
	int hc = 0;
	int Nchange = 0;

	for (int i = 0; i < count; i++) {
		if (hc >= eq_relation.size())
			eq_relation.resize(eq_relation.size() + relation_add, relation_w);
		for (int j = 0; j < hc; j++) {
			if (eq_copy[i][0] == eq_relation[j][0]) {
				Nchange = 1;
				break;
			}
		}
		if (Nchange == 1) {
			Nchange = 0;
			continue;
		}
		eq_relation[hc][0] = eq_copy[i][0];
		hc++;
	}
	int* rc_count = (int*)calloc(count, sizeof(int));

	//用開頭數字下去，找到對應的值。
	for (int i = 0; i < hc; i++) {
		int rc = 1;
		for (int j = 0; j < count; j++) {
			if (eq_relation[i][0] == eq_copy[j][0]) {
				eq_relation[i][rc] = eq_table[j][0];
				rc++;
				for (int k = 0; k < rc - 1; k++) {
					if (eq_relation[i][rc - 1] == eq_relation[i][k]) {
						rc--;
						break;
					}
				}
				eq_relation[i][rc] = eq_table[j][1];
				rc++;
				for (int k = 0; k < rc - 1; k++) {
					if (eq_relation[i][rc - 1] == eq_relation[i][k]) {
						rc--;
						break;
					}
				}
			}
		}
		rc_count[i] = rc;
	}
	vector<int> self_label(label - 1);
	int label_count = 1;
	for (int i = 0; i < label - 1; i++) {
		self_label[i] = label_count;
		label_count++;
	}

	for (int i = 0; i < hc; i++) {
		for (int j = 0; j < rc_count[i]; j++) {
			int eq_num = eq_relation[i][j] - 1;
			self_label[eq_num] = 0;
		}
	}
	for (int i = 0; i < label - 1; i++) {
		if (hc >= eq_relation.size())
			eq_relation.resize(eq_relation.size() + relation_add, relation_w);
		if (self_label[i] != 0) {
			eq_relation[hc][0] = self_label[i];
			rc_count[hc] = 1;
			hc++;
		}
	}
	int Clabel;
	vector<vector<int>> draw_point(hc, vector<int>(4));
	//int** draw_point = ge_matrix(hc, 4);
	for (int i = 0; i < hc; i++) {
		for (int j = 0; j < 4; j++) {
			if (j < 2)
				draw_point[i][j] = height;
			else
				draw_point[i][j] = 0;
		}
	}
	vector<int> area(hc, 0);
	vector<vector<int>> centroid(hc, vector<int>(2, 0));
	for (int i = 0; i < height; i++) {
		for (int j = 0; j < width; j++) {
			Clabel = image.at<uchar>(i, j);
			if (Clabel == 0) continue;
			for (int x = 0; x < hc; x++) {
				for (int y = 0; y < rc_count[x]; y++) {
					if (eq_relation[x][y] == Clabel) {
						image.at<uchar>(i, j) = (x + 1);
						//Bounding box
						draw_point[x][0] = min(i, draw_point[x][0]);
						draw_point[x][1] = min(j, draw_point[x][1]);
						draw_point[x][2] = max(i, draw_point[x][2]);
						draw_point[x][3] = max(j, draw_point[x][3]);

						//property analysis
						area[x]++;
						centroid[x][0] += i;
						centroid[x][1] += j;
						goto here;
					}
				}
			}
		here:;
		}
	}

	Scalar box_color;
	box_color = color == 0 ? Scalar(0, 0, 255) : Scalar(255, 0, 0);


	for (int i = 0; i < hc; i++) {
		rectangle(original, Point(draw_point[i][1], draw_point[i][0]), Point(draw_point[i][3], draw_point[i][2]), box_color, 2);
		centroid[i][0] /= (float)area[i];
		centroid[i][1] /= (float)area[i];
		circle(original, Point(centroid[i][1], centroid[i][0]), 2, box_color, FILLED);
	}
	
	
	if (color != 0) {

		vector<vector<int>> long_axis(hc, vector<int>(4));
		vector<int> long_length(hc);
		vector<double> long_orientation(hc);
		for (int i = 0; i < hc; i++) {
			int axis1 = abs(draw_point[i][2] - draw_point[i][0]), axis2 = abs(draw_point[i][3] - draw_point[i][1]);

			int a1_1 = 0, a2_1 = 0, b1_1 = 0, b2_1 = 0;
			int a1_2 = 0, a2_2 = 0, b1_2 = 0, b2_2 = 0;
			int back = 0, back2 = 0;
			if (axis1 >= axis2) {
				for (int a = 0; a < axis2; a++) {
					int Istate_1 = image.at<uchar>(draw_point[i][0], draw_point[i][1] + a);
					if (Istate_1 != back) {
						a1_1 = draw_point[i][0];
						a2_1 = draw_point[i][1] + a;
						break;
					}
					else
						back = Istate_1;
				}
				for (int a = 0; a < axis2; a++) {
					int Istate_2 = image.at<uchar>(draw_point[i][2], draw_point[i][1] + a);
					if (Istate_2 != back2) {
						a1_2 = draw_point[i][2];
						a2_2 = draw_point[i][1] + a;
						break;
					}
					else
						back2 = Istate_2;
				}
				for (int a = 0; a < axis2; a++) {
					int Istate_1 = image.at<uchar>(draw_point[i][0], draw_point[i][3] - a);
					if (Istate_1 != back) {
						b1_1 = draw_point[i][0];
						b2_1 = draw_point[i][3] - a;
						break;
					}
					else
						back = Istate_1;
				}
				for (int a = 0; a < axis2; a++) {
					int Istate_2 = image.at<uchar>(draw_point[i][2], draw_point[i][3] - a);
					if (Istate_2 != back2) {
						b1_2 = draw_point[i][2];
						b2_2 = draw_point[i][3] - a;
						break;
					}
					else
						back2 = Istate_2;
				}
			}
			else {
				for (int a = 0; a < axis1; a++) {
					int Istate_1 = image.at<uchar>(draw_point[i][0] + a, draw_point[i][1]);

					if (Istate_1 != back) {
						a1_1 = draw_point[i][0] + a;
						a2_1 = draw_point[i][1];
						break;
					}
					else
						back = Istate_1;
				}
				for (int a = 0; a < axis1; a++) {
					int Istate_2 = image.at<uchar>(draw_point[i][0] + a, draw_point[i][3]);

					if (Istate_2 != back2) {
						a1_2 = draw_point[i][0] + a;
						a2_2 = draw_point[i][3];
						break;
					}
					else
						back2 = Istate_2;
				}
				for (int a = 0; a < axis1; a++) {
					int Istate_1 = image.at<uchar>(draw_point[i][2] - a, draw_point[i][1]);
					if (Istate_1 != back) {
						b1_1 = draw_point[i][2] - a;
						b2_1 = draw_point[i][1];
						break;
					}
					else
						back = Istate_1;
				}
				for (int a = 0; a < axis1; a++) {
					int Istate_2 = image.at<uchar>(draw_point[i][2] - a, draw_point[i][3]);
					if (Istate_2 != back2) {
						b1_2 = draw_point[i][2] - a;
						b2_2 = draw_point[i][3];
						break;
					}
					else
						back2 = Istate_2;
				}
			}
			long_axis[i][0] = int((a1_1 + b1_1) / float(2));
			long_axis[i][1] = int((a2_1 + b2_1) / float(2));
			long_axis[i][2] = int((a1_2 + b1_2) / float(2));
			long_axis[i][3] = int((a2_2 + b2_2) / float(2));
			long_length[i] = sqrt(pow(long_axis[i][0] - long_axis[i][2], 2) + pow(long_axis[i][1] - long_axis[i][3], 2));
			int orientation_x = abs(long_axis[i][0] - long_axis[i][2]);
			int orientation_y = abs(long_axis[i][1] - long_axis[i][3]);
			long_orientation[i] = atan(orientation_x/(float)orientation_y)*180/PI;
			line(original, Point(long_axis[i][1], long_axis[i][0]), Point(long_axis[i][3], long_axis[i][2]), Scalar(0, 255, 0), 1);
		}
	}

	if (color == 0)
		return draw_point;
	else
		return centroid;
}
Mat myerode(Mat image, int size) {
	int height = image.rows, width = image.cols;
	Mat himage(height, width, CV_8UC1);

	int M_half = (size - 1) / 2;

	int Vmin;

	for (int i = 0; i < height; i++) {
		for (int j = 0; j < width; j++) {
			Vmin = 255;
			for (int a = i - M_half; a <= i + M_half; a++) {
				for (int b = j; b <= j; b++) {
					if (a < 0 || b < 0 || a >= height || b >= width) continue;
					if (image.at<uchar>(a, b) == 0) {
						Vmin = 0;
						goto here;
					}
				}
			}
		here: himage.at<uchar>(i, j) = Vmin;
		}
	}

	Mat wimage(height, width, CV_8UC1);
	for (int i = 0; i < height; i++) {
		for (int j = 0; j < width; j++) {
			Vmin = 255;
			for (int a = i; a <= i; a++) {
				for (int b = j - M_half; b <= j + M_half; b++) {
					if (a < 0 || b < 0 || a >= height || b >= width) continue;
					if (himage.at<uchar>(a, b) == 0) {
						Vmin = 0;
						goto here2;
					}
				}
			}
		here2: wimage.at<uchar>(i, j) = Vmin;
		}
	}

	return wimage;
}
Mat mydilate(Mat image, int size) {
	int height = image.rows, width = image.cols;
	Mat himage(height, width, CV_8UC1);

	int M_half = (size - 1) / 2;

	int Vmax;

	for (int i = 0; i < height; i++) {
		for (int j = 0; j < width; j++) {
			Vmax = 0;
			for (int a = i - M_half; a <= i + M_half; a++) {
				for (int b = j; b <= j; b++) {
					if (a < 0 || b < 0 || a >= height || b >= width) continue;
					if (image.at<uchar>(a, b) == 255) {
						Vmax = 255;
						goto here;
					}
				}
			}
		here: himage.at<uchar>(i, j) = Vmax;
		}
	}

	Mat wimage(height, width, CV_8UC1);
	for (int i = 0; i < height; i++) {
		for (int j = 0; j < width; j++) {
			Vmax = 0;
			for (int a = i; a <= i; a++) {
				for (int b = j - M_half; b <= j + M_half; b++) {
					if (a < 0 || b < 0 || a >= height || b >= width) continue;
					if (himage.at<uchar>(a, b) == 255) {
						Vmax = 255;
						goto here2;
					}
				}
			}
		here2: wimage.at<uchar>(i, j) = Vmax;
		}
	}

	return wimage;
}
Mat myopening(Mat image, int size) {
	int height = image.rows, width = image.cols;
	Mat result(height, width, CV_8UC1);

	result = myerode(image, size);
	result = mydilate(result, size);

	return result;
}
Mat mysubtraction(Mat mor, Mat bin) {
	int height = mor.rows, width = mor.cols;
	Mat result(height, width, CV_8UC1);

	for (int i = 0; i < height; i++)
		for (int j = 0; j < width; j++)
			result.at<uchar>(i, j) = bin.at<uchar>(i, j) - mor.at<uchar>(i, j);

	return result;
}
void fingerINhand(vector<vector<int>> box, vector<vector<int>> centroid,Mat image) {
	vector<int> H_f(box.size(),0);
	for (int n = 0; n < centroid.size(); n++) {
		int x = centroid[n][0];
		int y = centroid[n][1];
		for (int i = 0; i < box.size(); i++) {
			int a = box[i][0];
			int b = box[i][1];
			int c = box[i][2];
			int d = box[i][3];
			if (x > a && x<c && y>b && y < d)
				H_f[i]++;
		}
	}
	for (int i = 0; i < H_f.size(); i++) {
		int a = box[i][0];
		int b = box[i][1];
		int c = box[i][2];
		int d = box[i][3];
		putText(image, to_string(H_f[i]), Point((b + d) / (float)2,(a+c)/(float)2),0,1,Scalar(0,0,0),3);
	}

}