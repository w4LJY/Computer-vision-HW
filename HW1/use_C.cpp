#define _CRT_SECURE_NO_DEPRECATE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

//bmp圖像結構，包含高、寬、圖像、標頭
struct SImage {
	int height;
	int width;
	unsigned char* image;
	unsigned char* header;
};
//建立與釋放動態記憶體
unsigned char* geImage(int, int ,int);
void reImage(unsigned char *);

//四個函式對應四個作業題目
SImage Image_RW(const char*,int);
SImage Image_rotation(SImage,const char*);
void Image_channel_interchange(SImage ,const char*);
void Bouns_rotate_image();

int main234134()
{
	
	//Image Read/Write
	SImage Simage = Image_RW("InputImage1",1);//輸入檔名，根據標頭自動判定大小，第二個參數為是否另存一個檔案

	//Image Rotation
	SImage Simage_rotation = Image_rotation(Simage,"image_rotation"); //傳入結構影像、儲存檔案的名字

	//Image Channel Interchange
	Image_channel_interchange(Simage_rotation,"lena_channel_interchange");

	//Bonus
	Bouns_rotate_image();

	reImage(Simage.image);
	reImage(Simage.header);
	reImage(Simage_rotation.image);
	system("pause");
	return 0;
}
unsigned char* geImage(int height, int width=1 , int color=1) {
	unsigned char* image = (unsigned char*)malloc((size_t)height * width * color); //開一個W x H x 3的動態記憶體
	if (image == NULL) return NULL; //動態記憶體開啟失敗
	return image;
}
void reImage(unsigned char* image) {
	free(image);
}
SImage Image_RW(const char* filename,int imageWrite = 0) {
	
	FILE* fpin;
	
	//讀取檔案名處理
	char filename_bmp[128];
	sprintf(filename_bmp, "%s.bmp", filename);
	fpin = fopen(filename_bmp, "rb");

	unsigned char* header = geImage(54);
	fread(header, sizeof(unsigned char), 54, fpin);//讀標頭

	//根據標頭檔讀出影像長寬
	int width = header[19] * 256 + header[18];
	int height = header[23] * 256 + header[22];
	
	unsigned char* image = geImage(height, width, 3);
	fread(image, sizeof(unsigned char), (size_t)(long)height* width * 3, fpin);//讀圖

	if (imageWrite == 1) {

		FILE* fpout;

		char filename_out_bmp[128];
		sprintf(filename_out_bmp, "%s_out.bmp", filename);
		fpout = fopen(filename_out_bmp, "wb");

		fwrite(header, sizeof(unsigned char), 54, fpout);
		fwrite(image, sizeof(unsigned char), (size_t)(long)height * width * 3, fpout);
		fclose(fpout);
	}

	fclose(fpin);
	

	struct SImage Simage = { height,width,image,header };

	return Simage;
}
SImage Image_rotation(SImage Simage,const char* filename) {
	
	int width = Simage.width;
	int height = Simage.height;
	
	FILE* fpout;

	char filename_out_bmp[128];
	sprintf(filename_out_bmp, "%s.bmp", filename);
	fpout = fopen(filename_out_bmp, "wb");

	unsigned char* image_rotation = geImage(height, width, 3);

	//影像旋轉180
	for (int i = 0; i < height; i++) {
		for (int j = 0; j < width; j++) {
			for (int k = 0; k < 3; k++) {
				int index = ((i * width) + j) * 3 + k;
				int new_index = (((height-i) * width) + (width-j)) * 3 + k;
				image_rotation[index] = Simage.image[new_index];

			}
		}
	}	
	fwrite(Simage.header, sizeof(unsigned char), 54, fpout);
	fwrite(image_rotation, sizeof(unsigned char), (size_t)(long)height * width * 3, fpout);

	fclose(fpout);

	struct SImage Simage_rotation = { height,width,image_rotation,Simage.header };

	return Simage_rotation;
}
void Image_channel_interchange(SImage Simage_rotation, const char* filename) {
	
	int width = Simage_rotation.width;
	int height = Simage_rotation.height;

	FILE* fpout;

	char filename_out_bmp[128];
	sprintf(filename_out_bmp, "%s.bmp", filename);
	fpout = fopen(filename_out_bmp, "wb");
	
	int temp;

	for (int i = 0; i < height; i++) {
		for (int j = 0; j < width; j++) {
			int index_B = ((i * width) + j) * 3 + 0;
			int index_G = ((i * width) + j) * 3 + 1;
			int index_R = ((i * width) + j) * 3 + 2;

			temp = Simage_rotation.image[index_B];
			Simage_rotation.image[index_B] = Simage_rotation.image[index_G];
			Simage_rotation.image[index_G] = Simage_rotation.image[index_R];
			Simage_rotation.image[index_R] = temp;
				
		}
	}
	fwrite(Simage_rotation.header, sizeof(unsigned char), 54, fpout);
	fwrite(Simage_rotation.image, sizeof(unsigned char), (size_t)(long)height * width *3, fpout);
	fclose(fpout);
}
void Bouns_rotate_image() {

	SImage lenaCropped = Image_RW("lena_cropped");
	SImage lena1024 = Image_RW("lena1024");

	SImage SlenaCropped_rotation = Image_rotation(lenaCropped,"lenaCropped_rotation");
	SImage Slena1024_rotation = Image_rotation(lena1024,"lena1024_rotation");

	reImage(lenaCropped.image);
	reImage(lena1024.image);
	reImage(lenaCropped.header);
	reImage(lena1024.header);
	reImage(SlenaCropped_rotation.image);
	reImage(Slena1024_rotation.image);
	
}