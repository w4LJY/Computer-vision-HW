# HW#3

# 一、	程式使用說明
## 1.	環境：visual studio2019, openCV4.4.0
## 2.	檔案：
* (1).	HW3_1.cpp：第一小題
* (2).	HW3_2.cpp：第二小題
* (3).	HW3_3.cpp：bonus
* (4).	HW3_1.exe：HW3_1.cpp執行檔
* (5).	HW3_2.exe：HW3_2.cpp執行檔
* (6).	HW3_3.exe：HW3_3.cpp執行檔
## 3.	圖片輸入：
* (1).	distortion.bmp：720x480的魚眼彩色圖像
* (2).	road：1024x768的道路平行視角彩色圖像
* (3).	left：510x370的風景彩色影像
* (4).	right：510x370的風景彩色影像
 
# 二、	實作方法
## 1.	Unwarp the distorted image of fish-eye camera
要將魚眼成像轉為正常視角，需要用Unwrap來處理，但若只用正向的forward warping做影像轉換，會因為4捨5入或圖像被拉大等原因，使結果圖會有缺洞。為了解決這個問題，會使用到Inverse warping，Inverse warping因為是由結果圖對應找到原圖上的像素，並不會受到4捨5入等因素的影像，一定能找到一個對應的像素。而forward warping的功用就剩下拿來估測轉換後的圖像大小。
### (a).	用forward warping找到轉換後的影像大小
魚眼校正轉換公式是根據極座標來進行運算的，因此要先將直角座標![](https://i.imgur.com/62x3nfq.png)轉換成極座標![](https://i.imgur.com/lDFKmby.png)，接著根據轉換公式

![](https://i.imgur.com/0OLOE1U.png)

  
加上已知形變參數![](https://i.imgur.com/sptEsS7.png) 可以得到![](https://i.imgur.com/SXvdc8N.png) 轉為直角座標可得到![](https://i.imgur.com/NILf8OY.png) ，整個forward warping的過程可表示為：

![](https://i.imgur.com/7GK5efJ.png)

  
而轉換後的影像大小能根據![](https://i.imgur.com/UOi1Ky3.png) 得到。 
### (b).	用Inverse warping進行圖像轉換
Sol. 因為轉換後的影像過大，會將轉換後的影像縮小r倍，縮小後的座標為![](https://i.imgur.com/nCr0IqN.png) 。
Inverse warping會由轉換後且縮小過的影像中的每個pixel映射到原圖去找反轉換的對應座標，所以第一步是將縮小後的座標轉換為縮小前的座標![](https://i.imgur.com/BCo7058.png) ，而反轉換的公式同樣須用到極座標，因此轉為極座標![](https://i.imgur.com/MxjBxDV.png) ，反轉換公式為：

![](https://i.imgur.com/kGxBGnt.png)

  
反轉完後的極座標![](https://i.imgur.com/P8dFnPH.png) 轉為直角座標![](https://i.imgur.com/ysJJHOe.png) 即可找到映射關係。
整個Inverse warping的過程可表示為![](https://i.imgur.com/RZMDjQk.png)：
  
將找到的對應座標上的![](https://i.imgur.com/RGAm2Hg.png) 的顏色強度放到縮小圖座標![](https://i.imgur.com/aa4wrUq.png) 就能完成轉換。

![](https://i.imgur.com/TlrJov4.png)

 
### (c).	局部Inverse warping
對於魚眼轉換問題來說，轉換結果會有四條尾巴，在影像處理上，通常不太需要這些部分，我們真正需要的是影像中央的主要影像，所以根據要取出的範圍設定w,h，並從結果圖的這個範圍通過Inverse warping去映射到原圖的對應座標就能完成轉換，其他部分的與(b)的步驟相同。
 
![](https://i.imgur.com/SfCRMWk.png)

![](https://i.imgur.com/sBz28Sk.png)

 
## 2.	Transfer the road image to bird’s eye view by inverse perspective mapping
道路的鳥勘座標轉換，和魚眼座標轉換類似，一樣要根據Forward warping找到轉換圖的大小，並且以Inverse warping進行轉換。
### (a).	用forward warping找到轉換後的影像大小
道路的鳥勘座標的forward warping就是image to world的公式

![](https://i.imgur.com/zUipw99.png)

  
找出x,y後根據公式

![](https://i.imgur.com/fyiJAP9.png)

就能得到轉換後的圖像大小。
Forward warping的過程就是： ![](https://i.imgur.com/r5B1tf9.png)
### (b).	用Inverse warping進行圖像轉換
道路的鳥勘座標的Inverse warping就是world to image的公式

![](https://i.imgur.com/x1vTzYA.png)

Inverse warping的步驟與魚眼還原完全相同。
結果還可發現，road.bmp圖的上半部完全沒有出現在轉換後的結果圖上，因此在運算時可以跳過這個範圍不計算，節省運算時間。

![](https://i.imgur.com/wrk8vin.png)

![](https://i.imgur.com/0vXhHjx.png)

 
### (c).	局部Inverse warping
道路影像在影像處理中，同樣不需要整張影像，而是要車前一小段距離的影像。

![](https://i.imgur.com/B2y1OPH.png)

 
將著個部分用Inverse warping找到映射座標即可完成轉換

![](https://i.imgur.com/btqffPR.png)

 
## 3.	Image stitching with the projective transform：
### (a).	圖像轉換式
圖像拼接問題要解的參數包含位移、旋轉、尺度等9個參數，第9個參數i為1的狀況下，要求解8個參數需要4組原圖與轉換後的對應點，因此轉換方程式可表示為：

![](https://i.imgur.com/qmIwAL3.png)

 
要求解這個矩陣過於複雜，因此為了簡化問題，在原圖與結果的轉換間多加入一個單位方塊。

![](https://i.imgur.com/8EISAnB.png)

這樣問題就會從4個原座標轉為對應的4個結果座標簡化為從單位方塊的四個座標轉為4個結果座標。有此可輕鬆的解出8個參數。

![](https://i.imgur.com/OAstRXI.png)

經由簡單的矩陣運算就能知道u,v對應到x,y的公式。

![](https://i.imgur.com/AlpiDNT.png)

這些就是圖上所對應的case1動作。
而case2則是由原座標對應到單位方塊座標，可以發現方向與case1是反的，因此要用到反函式。

![](https://i.imgur.com/c9cN34S.png)

而case3則是將case1計算出的![](https://i.imgur.com/IQDTaIW.png) 與case2經由反函式計算出的![](https://i.imgur.com/EQdeTHz.png) 相乘，即可得到直接由原圖到結果圖的轉換公式：

![](https://i.imgur.com/PqrnCub.png)

(b).	Inverse warping
因為事先指定轉換後的圖像大小與大致位置，不需要先用Forward warping求轉換後的影像大小。
Inverse warping的過程就是將forward warping的所有運算反過來

![](https://i.imgur.com/jbPCcn0.png)

 
由case1經由反函式計算出![](https://i.imgur.com/4j9AHen.png) ，case2計算出 ![](https://i.imgur.com/JLbiTg5.png)
就能得到原圖座標經由轉換映射到的結果圖座標

![](https://i.imgur.com/ZUWJP80.png)

![](https://i.imgur.com/vEHTwjC.png)


  
 


