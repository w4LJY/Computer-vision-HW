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

    <details close>
    <summary>tracking frame</summary>

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
    
    ![image](https://user-images.githubusercontent.com/87016529/126631333-b0750394-5017-464d-abe8-16b5478863f6.png)
    
    ![image](https://user-images.githubusercontent.com/87016529/126631671-8f1576fd-5a5a-4f39-8bb7-196b5097439f.png)
    
    ![image](https://user-images.githubusercontent.com/87016529/126631685-8c7b78d4-344a-4b6a-ba31-47800d2c4593.png)
    
    ![image](https://user-images.githubusercontent.com/87016529/126631695-d23bc417-4e26-4fd4-9063-d4521d53a88b.png)
    
    ![image](https://user-images.githubusercontent.com/87016529/126631710-e84610fd-ec46-4536-b51d-97a2116019c9.png)
    
    ![image](https://user-images.githubusercontent.com/87016529/126631719-718f6d36-c550-4137-9fa1-43a88fcb7301.png)
    
    </details>

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

    <details close>
    <summary>tracking frame</summary>
    
    ![image](https://user-images.githubusercontent.com/87016529/126631815-3cae3dc7-2cfe-4f6a-8a9f-bcd9e4899290.png)
    
    ![image](https://user-images.githubusercontent.com/87016529/126631826-80139162-e8fa-4dbd-abc7-e4ab5c0c0b68.png)
    
    ![image](https://user-images.githubusercontent.com/87016529/126631831-c62d5063-929a-4d52-b7c9-4f32917bbb08.png)
    
    ![image](https://user-images.githubusercontent.com/87016529/126631843-2d3076d3-b60c-4fcb-9d88-08a9090acac3.png)
    
    ![image](https://user-images.githubusercontent.com/87016529/126631858-3a5685c7-10f2-4fc8-baf1-e7c816a1148a.png)
    
    ![image](https://user-images.githubusercontent.com/87016529/126631870-f7acfc60-a720-452e-83ea-1c0387aa61ce.png)
    
    ![image](https://user-images.githubusercontent.com/87016529/126631876-f2bf8371-0af5-4fbb-9fd7-905d0348099e.png)
    
    ![image](https://user-images.githubusercontent.com/87016529/126631893-6901161b-d923-4468-a596-5c07b9c3f8b5.png)
    
    ![image](https://user-images.githubusercontent.com/87016529/126631906-d2fba152-d8c0-4273-9c32-9bd95dc0e856.png)
    
    ![image](https://user-images.githubusercontent.com/87016529/126631916-16354bbf-04bb-4f06-a558-25a8b5b3b1fd.png)
    
    ![image](https://user-images.githubusercontent.com/87016529/126631932-ab63d422-5945-48c7-ac89-e22d88e8b6ec.png)
    
    ![image](https://user-images.githubusercontent.com/87016529/126631953-77c825ff-b976-4fae-8adc-4a0081b1d830.png)
    
    ![image](https://user-images.githubusercontent.com/87016529/126632009-d4e6f370-a518-4340-8ee1-bf18ad218468.png)
    
    ![image](https://user-images.githubusercontent.com/87016529/126632023-01e76dab-2dc6-475d-b4a9-861cb7c80ef2.png)
    
    ![image](https://user-images.githubusercontent.com/87016529/126632031-dc5ec8ef-b5f4-4b87-8e33-c8771091164b.png)
    
    ![image](https://user-images.githubusercontent.com/87016529/126632045-d6e32428-08b7-4f99-8a9d-b3ccebe1cfb6.png)
    
    ![image](https://user-images.githubusercontent.com/87016529/126632054-26f423ce-c30a-4112-974d-596248d3a18f.png)
    
    ![image](https://user-images.githubusercontent.com/87016529/126632071-9af68988-a487-483f-b9db-44ba0d362da7.png)
    
    ![image](https://user-images.githubusercontent.com/87016529/126632081-5873cc37-3767-42ef-bcad-316577e0b98c.png)
    
    ![image](https://user-images.githubusercontent.com/87016529/126632095-424eeafc-0b78-440c-813b-f6fc08946881.png)
    
    ![image](https://user-images.githubusercontent.com/87016529/126632105-0268b60d-034c-4981-b6cf-188ddb4817a1.png)
    
    ![image](https://user-images.githubusercontent.com/87016529/126632119-f9cce3a9-222d-4ef5-8495-fc57b4484597.png)
    
    ![image](https://user-images.githubusercontent.com/87016529/126632129-32f049f2-bd61-4e78-8e04-128847bcbf83.png)
    
    ![image](https://user-images.githubusercontent.com/87016529/126632139-57c6b333-930e-4c7b-9f46-7a89bac7c3a2.png)
    
    ![image](https://user-images.githubusercontent.com/87016529/126632145-4ba46542-4d24-4c66-b192-464ac7d8368d.png)
    
    ![image](https://user-images.githubusercontent.com/87016529/126632160-189f50d7-1475-41ad-aeca-446192e545f0.png)
    
    ![image](https://user-images.githubusercontent.com/87016529/126632168-5763c7cb-6025-4556-b3bd-aea0b7e79685.png)
    
    ![image](https://user-images.githubusercontent.com/87016529/126632203-e45987c6-6a5f-4ee3-b2b9-a759a8346edc.png)
    
    ![image](https://user-images.githubusercontent.com/87016529/126632224-b3134743-c5fc-4038-be63-3c77d0e25c59.png)
    
    ![image](https://user-images.githubusercontent.com/87016529/126632238-da6a4bf7-d6e5-4c97-8d2f-aa25bff97381.png)
    
    ![image](https://user-images.githubusercontent.com/87016529/126632248-deb08be4-55e8-4e98-b5fd-fe2604029424.png)
    
    ![image](https://user-images.githubusercontent.com/87016529/126632265-80820a7e-2547-4e35-b4a7-a4d5ba15eb13.png)


    </details>
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 

