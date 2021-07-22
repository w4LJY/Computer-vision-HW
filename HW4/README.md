# HW#4

# 一、 程式使用說明
## 1.	環境：visual studio2019, openCV4.4.0
## 2.	檔案：
* (1).	HW4_1.cpp：第一小題
* (2).	HW4_Bonus.cpp：Bonus
* (3).	HW3_3.cpp：bonus
* (4).	HW4_1.exe：HW4_1.cpp執行檔
* (5).	HW4_Bonus.exe：HW4_Bonus.cpp執行檔
## 3.	影像輸入：
* (1).	1.avi：第一小題倉鼠簡易移動影片
* (2).	2.avi：第二小題倉鼠複雜移動影片
## 4.	Txt檔:
* (1).	GroundTruth_1：1.avi中倉鼠的正確Bounding box
* (2).	GroundTruth_2：2.avi中倉鼠的正確Bounding box
 
# 二、	實作方法
## 1.	Find the target in the image sequence “1.avi”：
這次追蹤實作要求使用opencv 完成。我使用opencv中的tracking模組完成。Tracking中有7中追蹤演算法，分別是Boosting, MIL, KCF, TLD, MEDIANFLOW, GOTURN, MOSSE, CSRT。

對於1.avi這部影片我選擇了MedianFlow追蹤，選擇追蹤演算法的方向，就是在足夠精確的目標檢測下，不會使幀數掉太多。 

實際追蹤情形，預測的bounding box與groundTruth的Bounding box非常接近，而題目所給的GroundTruth也不是很精準的框住整隻倉鼠，因此我認為midianFlow的結果已經接近最佳解。

有了預測與實際的bounding box就能夠算出IoU(Intersection over Union) ，IoU的算法就是將交集/聯集，用程式演算法來計算，交集就是重疊的方形區域面積，聯集就是各自的bounding box面積相加後減去交集。

IoU的CDF曲線可以由類似直方圖計算的方式得到，先計數特定範圍IoU值的出現次數，接著歸一化成PDF，有了PDF累加就能得到CDF。

* A.	Average computational time per frame
    > 0.0526s per frame

* B.	IoU (Intersection over Union)
    
    ![](https://i.imgur.com/jtCXNQ8.png)

    > IoU平均值: 0.865299

* C.	Accuracy rate(number of correct frames(IoU>50%)/total frame number)
    > 100%

* D.	實際追蹤情形(每90個frame取1張) 紅框為預測、藍框為GroundTruth，上方顯示第幾個frame與對應的IoU。

    ![](https://i.imgur.com/iMMefIP.png)

    ![](https://i.imgur.com/tX4SBpQ.png)

    ![](https://i.imgur.com/QpudF0c.png)
    
    ![](https://i.imgur.com/iosq8Eo.png)
    
    ![](https://i.imgur.com/hqvLuUm.png)
    
    ![](https://i.imgur.com/u2bKhJi.png)
    
    ![](https://i.imgur.com/6TveANW.png)
    
    ![](https://i.imgur.com/gTN7vXG.png)
    
    ![](https://i.imgur.com/6yXAbFn.png)
    
    ![](https://i.imgur.com/zxr4eXp.png)
    
    ![](https://i.imgur.com/vWS8bWa.png)
    
    ![](https://i.imgur.com/Axc3eq5.png)

    ![](https://i.imgur.com/B2jOPPL.png)
    
    ![](https://i.imgur.com/TGI6vId.png)
    
    ![](https://i.imgur.com/bs1jh7U.png)
    
    ![](https://i.imgur.com/XxvUGCi.png)
    
    ![](https://i.imgur.com/sW8qfe7.png)
    
    ![](https://i.imgur.com/ZchSbzY.png)
    
    ![](https://i.imgur.com/b2MRw4O.png)
    
    ![](https://i.imgur.com/9VdyQcw.png)
    
    ![](https://i.imgur.com/DGUJiei.png)
    
    ![](https://i.imgur.com/l9UNLJM.png)
    
    ![](https://i.imgur.com/GSzVmZ1.png)
    
    ![](https://i.imgur.com/CE4PehU.png)
    
    ![](https://i.imgur.com/XpRoBOp.png)
    
    
    
        

    
    

    
    

    
    

    
    






















 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 

# 2.	Bonus
追蹤2.avi並重複第一小題的需求，大致程式與HW4_1相同。但medianFlow的追蹤無法很好的跟到倉鼠的位置，因此改用CSRT的方式，損失蠻多時間效率。但就算如此，CSRT的追蹤演算法還是會在最後100幀左右完全追丟，且大部分的IoU狀況都不是太好。

* A.	Average computational time per frame

    > 0.394054s per frame

* B.	IoU(Intersection over Union)

    ![image](https://user-images.githubusercontent.com/87016529/126630967-72764015-f17c-409e-aaee-8bd964dcc0da.png)

    > IoU平均值: 0.393155

* C.	Accuracy rate(number of correct frames(IoU>50%)/total frame number)

    > 30.785563%

* D.	實際追蹤情形
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 

