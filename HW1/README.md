# HW#1
---

# 一、 	程式使用說明
## 1.	環境：visual studio2019, openCV4.4.0
## 2.	檔案：
* (1).	use_C.cpp：使用純C/C++完成的作業
* (2).	use_openCV.cpp：使用openCV函式完成的作業
* (3).	use_C.exe：use_C.cpp執行檔
* (4).	use_openCV.exe：use_openCV.cpp執行檔

## 3.	函式：
<font color="red">use_C.cpp</font>
* (1).	Image_RW：第一小題
* (2).	Image_rotation：第二小題
* (3).	Image_channel_interchange：第三小題
* (4).	Bouns_rotate_image：bouns

<font color="red">use_openCV.cpp</font>
### 標示題號於程式中
## 4.	圖片輸入：
* (1).	InputImage1.bmp：512x512的lena彩色圖像
* (2).	Lena64.bmp：1024x1024的lena彩色圖像
* (3).	Lena_cropped.bmp：切割InputImage1後的影像
## 5.	圖片結果：
<font color="red">use_C.cpp</font>
* (1).	InputImage1_out.bmp：讀取InputImage1.bmp後儲存(第一小題)
* (2).	Image_rotation.bmp：旋轉InputImage1.bmp(第二小題)
* (3).	Image_channel_interchange.bmp：互換顏色通道(第三小題)
* (4).	lena1024_rotation.bmp：旋轉lena1024.bmp(bouns)
* (5).	lenaCropped_rotation.bmp：旋轉lena_cropped.bmp(bouns)

<font color="red">use_openCV.cpp</font>
* (1).	InputImage1_out_opencv.bmp：讀取InputImage1.bmp後儲存(第一小題)
* (2).	Image_rotation_opencv.bmp：旋轉InputImage1.bmp(第二小題)
* (3).	Image_channel_interchange_opencv.bmp：互換顏色通道(第三小題)
* (4).	lena1024_rotation_opencv.bmp：旋轉lena1024.bmp(bouns)
* (5).	lenaCropped_rotation_opencv.bmp：旋轉lena_cropped.bmp(bouns)

# 二、	實作方法
<font color="red">use_C.cpp</font>
## 1.	圖片讀取與儲存：
因為**矩陣大小有所限制、矩陣大小一開始就必須確定的關係**，因此我使用了**一維動態記憶體**，且不須告訴程式圖片大小，圖片長寬由**bmp的標頭檔**中讀出
## 2.	圖片旋轉：
因為我使用一維來儲存圖片，因此須將三維座標轉為一維
```c=
int index = ((i * width) + j) * 3 + k;
```
 

而圖像旋轉180度，我的想法是先將圖像上下翻轉再左右翻轉
```c=
int new_index = (((height-i) * width) + (width-j)) * 3 + k;
```

結合兩條方程式，配合for迴圈便可得到結果

```c=
image_rotation[index] = Simage.image[new_index];
```


## 3.	圖片顏色通道轉換
先分別讀出三個通道RGB的一維座標
```c=
int index_B = ((i * width) + j) * 3 + 0;
int index_G = ((i * width) + j) * 3 + 1;
int index_R = ((i * width) + j) * 3 + 2;
```
接著先將**B通道丟入暫存器**、然後將**G通道蓋過B通道**、接著將**R通道蓋過G通道**、最後將**暫存器中的B通道蓋過R通道**，即可成功的將顏色通道互換
```c=
temp = Simage_rotation.image[index_B];
Simage_rotation.image[index_B] = Simage_rotation.image[index_G];
Simage_rotation.image[index_G] = Simage_rotation.image[index_R];
Simage_rotation.image[index_R] = temp;
```
    
## 4.	Bouns(旋轉lena1024.bmp與lena_cropped.bmp)
這裡就是為何要使用動態記憶體的原因，首先lena1024的陣列大小為1024*1024*3，陣列存不下。

而lena_cropeed事前並不知道其大小，要由標頭檔讀得長寬，但陣列只允許宣告常數陣列大小，因此無法實行。

而我使用動態記憶體，只須正常地將兩張圖片讀取、旋轉即可得到結果

## 5.	補充

* (1).	我將得到的圖片、標頭檔、長、寬存為struct的結構體型態，方便之後再函式間傳遞讀取
* (2).	每個小題寫為一個函式，函式使用上只需傳入檔名或結構體，使程式簡潔易讀、方便修改


<font color="red">use_openCV.cpp</font>
## 1.	圖片讀取與儲存：
openCV中只需使用imread與imwrite就可輕鬆讀與存。imread讀入的檔案會儲存成numpy陣列，並且可以設定讀取圖片格式，可以讀成彩色、灰階等形式。Imwrite檔案時可以根據不同的副檔名來決定輸出的圖檔格式，也能調整圖片的品質、壓縮率等數值。

## 2.	圖片旋轉：
使用opencv中的flip函數，flip函數常用的參數有三個，原圖、翻轉後影像、與翻轉的方式，翻轉方式設定為1代表水平翻轉、設定為0代表垂直翻轉、設定為-1代表水平垂直翻轉，而水平垂直翻轉與旋轉180度的功能是相同的。
```c=
flip(lena1024, lena1024_rotate, -1);
```

## 3.	圖片顏色通道轉換
使用opencv中的split函數分割三個顏色通道
```c=
split(image_rotate, channels);
```
接著配合一個暫存器將通道顏色互換 
```c=
temp = channels.at(0);
channels.at(0) = channels.at(1);
channels.at(1) = channels.at(2);
channels.at(2) = temp;
```

最後再用merge函數將3個通道的合而為一
```c=
Mat mergedst;
	Mat mergesrc[3] = { channels[0] , channels[1] , channels[2] };
	merge(mergesrc,3, mergedst);
```
## 4.	Bouns(旋轉lena1024.bmp與lena_cropped.bmp)
使用openCV函數旋轉圖片不須知道影像格式，因此將兩張圖片代入flip函數就可以得到旋轉180的結果。
# 三、	實作結果
<font color="red">use_C.cpp</font>
1.	讀取InputImage1.bmp後儲存(第一小題)

![](https://i.imgur.com/iwL77Ut.png)

2.	旋轉InputImage1.bmp旋轉180度(第二小題)

 ![](https://i.imgur.com/iwK5Sln.png)

3.	將旋轉後的圖像顏色通道互換(第三小題)

 ![](https://i.imgur.com/S3AoPqd.png)

4.	旋轉180度lena1024.bmp(bouns)

 ![](https://i.imgur.com/Zu4Wk7i.png)

5.	旋轉180度lena_cropped.bmp(bouns)

 ![](https://i.imgur.com/TmI2HMr.png)

 
 <font color="red">use_openCV</font>
1.	讀取InputImage1.bmp後儲存(第一小題)

 ![](https://i.imgur.com/qkwdH0c.png)

2.	旋轉InputImage1.bmp旋轉180度(第二小題)

 ![](https://i.imgur.com/6CmhRjd.png)

3.	將旋轉後的圖像顏色通道互換(第三小題)

 ![](https://i.imgur.com/uwYLtcV.png)

4.	旋轉180度lena1024.bmp(bouns)

 ![](https://i.imgur.com/HhDiG5y.png)

5.	旋轉180度lena_cropped.bmp(bouns)

 ![](https://i.imgur.com/DwCUZKs.png)

