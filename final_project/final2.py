import cv2
import numpy as np
import math
import matplotlib.pyplot as plt
import datetime


def low_watch(img,bg,bg2,circle,imga):
    kernel = cv2.getStructuringElement(cv2.MORPH_ELLIPSE,(27, 27)) 
    opened = cv2.morphologyEx(img, cv2.MORPH_OPEN, kernel) 
    _,oth = cv2.threshold(opened,1,255,cv2.THRESH_BINARY)
    
    roi = cv2.bitwise_and(img,img,mask = oth)
    result = np.where(roi>215,0,roi).astype(np.uint8)
    
    mask = np.zeros(img.shape,np.uint8)
    cv2.circle(mask,(circle[0],circle[1]),int(circle[2]*0.4),255,-1)
    
    crange = np.zeros(img.shape,np.uint8)
    cv2.circle(crange,(circle[0],circle[1]),int(circle[2]*0.4),255,1)
    re = cv2.bitwise_and(result,crange)
    
    
    
    cv2.circle(mask,(circle[0],circle[1]),int(circle[2]*0.4),255,-1)
    nmask = cv2.bitwise_not(mask)
    #-----半圓點膠-----#
    cresult = cv2.bitwise_and(result,result,mask=mask)
    
    cnts, _ = cv2.findContours(cresult,cv2.RETR_TREE, cv2.CHAIN_APPROX_SIMPLE)
    if cnts:
        res = max(cnts, key = lambda x: cv2.contourArea(x))
        cv2.drawContours(bg, [res], -1, 255, -1) #繪出半圓點膠
        cv2.drawContours(imga, [res], 0, (0,255,0) , 2) #繪出半圓點膠
    #-----4個點膠------#
    fresult = cv2.bitwise_and(result,result,mask=nmask)
    cnts, _ = cv2.findContours(fresult,cv2.RETR_TREE, cv2.CHAIN_APPROX_SIMPLE)
    for c in cnts:
        cnts_size =cv2.contourArea(c)
        if cnts_size>700:
            cv2.drawContours(bg2, [c], -1, 255 , -1) #繪出所有點膠
            cv2.drawContours(imga, [c], 0, (0,255,0) , 2) #繪出所有點膠
            
    if re.mean()==0:
        return 0
    else:
        return 1

def high_watch(img,bg,bg2,circle,imga):

    kernel = cv2.getStructuringElement(cv2.MORPH_ELLIPSE,(3, 3)) 
    opened = cv2.morphologyEx(img, cv2.MORPH_OPEN, kernel) 
    _,oth = cv2.threshold(opened,20,255,cv2.THRESH_BINARY)
    kernel2 = cv2.getStructuringElement(cv2.MORPH_ELLIPSE,(7, 7)) 
    dilated = cv2.dilate(oth,kernel2) 
    
    roi = cv2.bitwise_and(img,img,mask = dilated)
    
    roi_copy = roi.copy()
    
    kernel3 = cv2.getStructuringElement(cv2.MORPH_RECT,(7, 7)) 
    erode = cv2.erode(roi_copy, kernel3) 
    
    
    crange = np.zeros(img.shape,np.uint8)
    cv2.circle(crange,(circle[0],circle[1]),int(circle[2]*0.4),255,1)
    re = cv2.bitwise_and(erode,crange)
    
    mask = np.zeros(img.shape,np.uint8)
    cv2.circle(mask,(circle[0],circle[1]),int(circle[2]*0.4),255,-1)
    nmask = cv2.bitwise_not(mask)
    #-----4個點膠------#
    cresult = cv2.bitwise_and(erode,erode,mask=nmask)

    cnts, _ = cv2.findContours(cresult,cv2.RETR_EXTERNAL, cv2.CHAIN_APPROX_SIMPLE)

    for c in cnts:
        cnts_size =cv2.contourArea(c)
        if cnts_size>2000:
            cv2.drawContours(bg2, [c], -1, 255, -1) #繪出4個點膠
            cv2.drawContours(imga, [c], 0, (0,255,0) , 2) #繪出4個點膠

    #-----半圓點膠-----#
    result = np.where(roi>240,0,roi).astype(np.uint8)
    nresult = cv2.bitwise_and(result,result,mask=mask)
    cnts, _ = cv2.findContours(nresult,cv2.RETR_EXTERNAL, cv2.CHAIN_APPROX_SIMPLE)
    res = max(cnts, key = lambda x: cv2.contourArea(x))
    cv2.drawContours(bg, [res], -1, 255, -1) #繪出半圓點膠
    cv2.drawContours(imga, [res], 0, (0,255,0) , 2) #繪出半圓點膠
    
    if re.sum()<=5:
        return 0
    else:
        return 1

def Hough_circle(imgb,imga):
    #-----------霍夫圓得到表面範圍-----------#
    gray = cv2.cvtColor(imgb, cv2.COLOR_BGR2GRAY)
    circles = cv2.HoughCircles(gray,cv2.HOUGH_GRADIENT,1,2000,param1=100,param2=30,minRadius=600,maxRadius=750)
    circles = np.int64(np.around(circles))
    i = circles[0][0]
    # draw the outer circle
    cv2.circle(imga,(i[0],i[1]),i[2],(0,255,0),10)
    # draw the center of the circle
    cv2.circle(imga,(i[0],i[1]),2,(0,0,255),10)
    
    sradius = int(i[2]*0.4)
    center = i[0:2]
    
    #-----------根據表面大小畫出mask1-----------#
    cv2.circle(imga,(center[0],center[1]),sradius,(0,255,255),10)
    
    return i
    
def circle_correct(img,c):
    Nsize = 1400    
    Nimg = img[c[1]-c[2]:c[1]+c[2],c[0]-c[2]:c[0]+c[2]]
    Nimg = cv2.resize(Nimg,(Nsize, Nsize), interpolation=cv2.INTER_CUBIC)
    return Nimg
    
   
def IoU_score(img,gt):
    h,w=img.shape
    Iand = cv2.bitwise_and(img,gt)
    Ior = cv2.bitwise_or(img,gt)

    return Iand.sum()/float(Ior.sum())


def main2(imgb,imga):    
    starttime = datetime.datetime.now()
    GT = cv2.imread('project2_Groundtruth//GT.png',0)
    GT2 = cv2.imread('project2_Groundtruth//GT2.png',0)
    #-----------模糊，轉通道-----------#
    HSVb = cv2.cvtColor(imgb, cv2.COLOR_BGR2Lab)[:,:,0]
    HSVa = cv2.cvtColor(imga, cv2.COLOR_BGR2Lab)[:,:,0]
    
    diff = HSVb-HSVa
    
    #-----------霍夫圓計算-----------#
    circle = Hough_circle(imgb,imga)
    
    #-----------亮度篩選-----------#
    bg = np.zeros(diff.shape,np.uint8)
    bg2 = np.zeros(diff.shape,np.uint8)
    lu = HSVb[251:260,251:260].mean()
    if lu>128:
        flag=high_watch(diff,bg,bg2,circle,imga)
    else:
        flag=low_watch(diff,bg,bg2,circle,imga)
    
    #-----------校正區域-----------#
    roi = circle_correct(bg,circle)
    roi2 = circle_correct(bg2,circle)
    
    #-----------評估-----------#
        
    IoU_value = IoU_score(roi,GT)
    IoU_value2 = IoU_score(roi2,GT2)
    if flag==1:
        print("半圓膠溢出")
    print("半圓膠IoU閥值:"+str(IoU_value))
    print("4點膠IoU閥值:"+str(IoU_value2))
    
    if IoU_value2<0.35 or IoU_value<0.44 or flag==1:
        print("bad")
    else:
        print("good")
        
    endtime = datetime.datetime.now()
    print("計算時間:"+str(endtime - starttime))
    
    h,w,_ = imga.shape
    
    cv2.namedWindow('imga', 0);
    cv2.resizeWindow('imga',int(w/3),int(h/3));
    cv2.imshow('imga',imga)
    
    # cv2.namedWindow('bg', 0);
    # cv2.resizeWindow('bg',int(w/3),int(h/3));
    # cv2.imshow('bg',bg)
    
    # cv2.namedWindow('bg2', 0);
    # cv2.resizeWindow('bg2',int(w/3),int(h/3));
    # cv2.imshow('bg2',bg2)
    
    # h,w= roi.shape
    # cv2.namedWindow('roi', 0);
    # cv2.resizeWindow('roi',int(w/3),int(h/3));
    # cv2.imshow('roi',roi)
    
    # cv2.namedWindow('roi2', 0);
    # cv2.resizeWindow('roi2',int(w/3),int(h/3));
    # cv2.imshow('roi2',roi2)
    
    cv2.waitKey(0)
    
    cv2.destroyAllWindows()

if __name__ == '__main__': 
    findex = '5'
    fgood_b = 'sample_good/'+findex+'_before.jpg'
    fgood_a = 'sample_good/'+findex+'_after.jpg'

    fbad_b ='sample_bad/'+findex+'b_before.jpg'
    fbad_a ='sample_bad/'+findex+'b_after.jpg'
    imgb = cv2.imread(fbad_b)
    imga = cv2.imread(fbad_a)
    main2(imgb,imga)