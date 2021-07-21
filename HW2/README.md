# HW#2


# 一、	程式使用說明
## 1.	環境：visual studio2019, openCV4.4.0
## 2.	檔案：
* (1).	use_C_vector.cpp：使用純C/C++完成的作業
* (2).	use_openCV.cpp：使用openCV函式完成的作業
* (3).	use_C_vector.exe：use_C.cpp執行檔
* (4).	use_openCV.exe：use_openCV.cpp執行檔
## 3.	函式：
<font color='red'>use_C.cpp</font>
* (1).	Color2Binary：彩色圖像轉二值化。參數:彩色圖像、二值化閥值。回傳:二值化圖像
* (2).	Connected_Components：查表法的Connected Components。參數:二值化圖像、彩色圖像、手/手指選擇，回傳:bounding box(手)、centroids(手指)
* (3).	myerode:mask形狀是rect的erode。參數:二值化影像、mask大小。回傳:二值化影像
* (4).	mydilate:mask形狀是rect的dilate。參數:二值化影像、mask大小。回傳:二值化影像
* (5).	myopening:mask形狀是rect的opening。參數:二值化影像、mask大小。回傳:二值化影像
* (6).	mysubtraction:影像相減。參數:二值化影像、二值化影像。回傳:二值化影像。
* (7).	fingerINhand：判斷手比的數字。參數:bounding box(手)、centroids(手指)、彩色影像\

<font color='red'>use_openCV.cpp</font>
* (1).	finger_num: 判斷手比的數字。參數:bounding box(手)、centroids(手指)、彩色影像
* (2).	finger_hr:畫出並計算出手指長與角度。參數:bounding box(手指)、二值化影像(手指)、彩色影像\

圖片輸入：
* (1).	hand.bmp：512x512的手部彩色圖像
 
# 二、	實作方法
<font color='red'>use_C.cpp</font>
##  1.	Connected components：
### (a).	Generate a binarized image
將**三通道顏色取平均**轉為灰階，並找到合適的閥值將圖像二值化。

 ![](https://i.imgur.com/ejl7eNX.png)
 二值化的hand.bmp

### (b).	Label the five hands with 4-connected neighbor
使用掃兩遍的方式，第一遍將二值化影像轉換為標籤圖，並將衝突表紀錄起來。第二遍依據衝突表整理出的各類標籤，將連通元件做分類。

最後根據分類結果，畫出bounding box，算出area與centroid。
#### (1).	第一遍標記:
由左上往右下掃，二值化圖像只有0與255兩種情況，遇到0跳過，遇到255時考慮上、左兩邊的值，若過邊界則視為0，而根據考慮的值分為三種情況。
* A.	兩者都為0：建立一個新的標籤值
* B.	其中之一為0或兩者相同(但不為0):取較大的標籤複製
* C.	兩者不同(皆不為0):取較小的標籤複製，且將兩個標籤放入衝突表，並確保沒有重複放入相同標籤對。
#### (2).	整理衝突表:
將衝突表中有關連性的分為同一類，將結果的類別數加上完全沒有起衝突的標籤數，就是最終連通元件的數量。
#### (3).	第二遍標記:
根據整理出來的連通元件類別，將舊的標記改為不同類別不同標記。得出結果標記圖。
#### (4).	畫出bounding box:
找出各個標記中最小的x,y就是bounding box 的左上角，最大的x,y就是bounding box的右上角。最後再用opencv的rectangle畫出紅框。
#### (5).	算出area與centroid:
面積(area)的算法就是計算各個標記的總數量。重心(Centroid)的算法就是計算各個標記的所有x總和與y總和，在分別除上總數量。最後我將centroid座標用opencv的circle函式畫出紅點。

![](https://i.imgur.com/tkdZ94h.png)
手部面積

![](https://i.imgur.com/WXHJdjI.png)
手部Bounding box與重心

### (c).	Use the morphology algorithms to reserve the fingers
取出手指需要使用到形態學，因此必須先寫出erode與dilate的函式，並合成為openning(先erode後dilate)，形態學過程為對二值化的手部使用openning，mask為大小25x25的rect，在用原二值化的手部影像減掉openning結果，最後在使用一次openning，mask為大小11x11的rect。
#### (1).	Erode:
對二值化影像卷積，只要mask與圖像and的結果出現0，就視那點像素為0，否則為255。為了提升效率將axb大小的mask拆為ax1與1xb分別對影像卷積，效果不變，效率大增。
#### (2).	Dilate:
對二值化影像卷積，只要mask與圖像or的結果出現255，就視那點像素為255，否則為0，為了提升效率將axb大小的mask拆為ax1與1xb分別對影像卷積，效果不變，效率大增。
#### (3).	Opening:
先Erode後Dilate，目的是將連著的部分分開。

![](https://i.imgur.com/kIwVGkk.png)
取openning，mask25x25rect

![](https://i.imgur.com/qAAlznu.png)
手部二值化圖-openning結果

![](https://i.imgur.com/YEfIe41.png)
取openning，mask11x11rect

### (d).	Label the fingers with 4-connected neighbor
重複Connected components過程，finger與hand完全相同。

![](https://i.imgur.com/wOgs44U.png)
手指面積

![](https://i.imgur.com/cgLQlNh.png)
手指bounding box與重心

### (e).	Analyze and print the computational time of your program
* (1).	手部Connected Components + 畫bounding box與重心:0.864s
* (2).	整個形態學過程:2.4s
* (3).	手指Connected Components + 畫bounding box與重心:0.164s
> 我的Connected Components 和 畫bounding box與重心是寫在一起的。

## 2.	Bonus
### (a).	Report the following properties of each finger
要找出實際上的手指長度，就需要找到手指尖與手指跟的座標。找法就是先找到bounding box的兩個短邊，對兩個短邊的座標去二值化的手指影像中找，對短邊中最先遇到與最後遇到的非0灰階值取終點，就能找到手指尖與手指跟的座標。而我將每隻手指用opencv中的line函式畫出綠線。
手指長度的算法就是計算兩個座標的歐式距離，手指角度就是兩者相除後取tan-1。

![](https://i.imgur.com/7GcWinp.png)
手指長

![](https://i.imgur.com/ScxwC7C.png)
手指角度對應座標

相對於hand.bmp水平軸的角度
>類別1: 6.4度\
類別2: 82.09度\
類別3: 89.33度\
類別4: 84.17度\
類別5: 86.13度\
類別6: 82.78度\
類別7: 9.46度\
類別8: 0度\
類別9: 7.32度\
類別10: 15.95度\
類別11: 13.65度\
類別12: 69.45度\
類別13: 83.83度\
類別14: 90度\
類別15: 16.14度


![](https://i.imgur.com/Txj8o24.png)
畫上手指長

### (b).	Report the numbers indicated by each hand
要能得到手比的數字，需要手的bounding box 與手指的centroids，用條件判斷centroids是否在bounding box內，就能判定哪隻手指在哪個手的位置，更別說是判斷手比的數字。最後用opencv的putText函式在手的bounding box中寫出手指的數量。

![](https://i.imgur.com/8ZNfg7d.png)
手比的數字


<font color='red'>use_openCV.cpp</font>
# 1.	Connected components：
### (a).	Generate a binarized image
可以直接在imread讀入時直接以灰階讀入，但因為我想用彩色圖來畫，因此使用opencv中的函式cvtColor，參數分別為輸入彩色圖，輸出灰階圖，與顏色轉換格式。因為希望目標物為白色，背景為黑色，又用了bitwise_not將黑白顏色反轉。

![](https://i.imgur.com/hscq6Ti.png)
二值化手部

![](https://i.imgur.com/o9FeagJ.png)
黑白反轉

### (b).	Label the five hands with 4-connected neighbor
使用opencv的connectedComponentsWithStats，參數為輸入二值化圖像，輸出標記圖，整數資訊，浮點數資訊。回傳總類別。最後用rectangle畫出bounding box，circle畫出centroids

![](https://i.imgur.com/wsVuUac.png)
手部面積

![](https://i.imgur.com/SKFC3Vr.png)
手部bouding box與重心

#### (1).	整數資訊:
大小為類別數x5，分別是x,y,width,height,area
#### (2).	浮點數資訊:
大小為類別數x2，為重心的x,y座標。
### (c).	Use the morphology algorithms to reserve the fingers
Opencv的形態學mask有三種形狀，分別是方形rect、橢圓ellipse、十字cross，在erode時我使用大小27x27的rect，dilate時為了更符合手型用了大小27x27的ellipse，用手部灰階圖減去目前的型態學結果後，得到帶有雜訊的手指，因此用openning大小27x27的ellipse來去除雜訊，得到效果比純C寫出更好的結果。

![](https://i.imgur.com/88YAslf.png)
取erode，mask27x27的rect

![](https://i.imgur.com/SZ5dVmP.png)
取Dilate， mask27x27的ellipse

![](https://i.imgur.com/LMKYnis.png)
手部二值化圖-形態學結果

![](https://i.imgur.com/yFRmHaF.png)
取Opening，mask15x15 ellipse

### (d).	Label the fingers with 4-connected neighbor
同樣代入connectedComponentsWithStats函式得到結果

![](https://i.imgur.com/rTywP3c.png)
手指面積

![](https://i.imgur.com/eHExrr6.png)
手指bounding box與重心

### (e).	Analyze and print the computational time of your program
* (1).	手指Connected Components + 畫bounding box與重心:0.022s
* (2).	整個形態學過程:0.837s
* (3).	Connected Components + 畫bounding box與重心:0.017s
> 純Connected Components速度與加上畫bounding box與重心幾乎一樣快。

## 2.	Bonus
方法與純C相同。
### (a).	Report the following properties of each finger

![](https://i.imgur.com/vXYOpAu.png)
手指長度

![](https://i.imgur.com/wZDUVQJ.png)
手指角度對應編號

相對於hand.bmp水平軸的角度
>類別1: 4.39度\
類別2: 9.09度\
類別3: 85.91度\
類別4: 84.74度\
類別5: 88.53度\
類別6: 90度\
類別7: 89.34度\
類別8: 84.86度\
類別9: 88.51度\
類別10: 86.19度\
類別11: 15.25度\
類別12: 6.61度\
類別13: 2.09度\
類別14: 5.55度\
類別15: 12.53度

![](https://i.imgur.com/AVHkxwJ.png)
畫上手指長

### (b).	Report the numbers indicated by each hand

![](https://i.imgur.com/p9Xhhgp.png)
手比的數字

