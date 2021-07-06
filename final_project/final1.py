import cv2
import numpy as np
import datetime


def low_watch(img,bg,circle,imga):
    kernel = cv2.getStructuringElement(cv2.MORPH_ELLIPSE,(27, 27)) 
    opened = cv2.morphologyEx(img, cv2.MORPH_OPEN, kernel) 
    _,oth = cv2.threshold(opened,1,255,cv2.THRESH_BINARY)
    
    roi = cv2.bitwise_and(img,img,mask = oth)
    result = np.where(roi>215,0,roi).astype(np.uint8)
    
    
    mask = np.zeros(img.shape,np.uint8)
    cv2.circle(mask,(circle[0],circle[1]),int(circle[2]*0.4),255,-1)
    

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
            cv2.drawContours(bg, [c], -1, 255 , -1) #繪出所有點膠
            cv2.drawContours(imga, [c], 0, (0,255,0) , 2) #繪出所有點膠
            
    

def high_watch(img,bg,circle,imga):

    kernel = cv2.getStructuringElement(cv2.MORPH_ELLIPSE,(3, 3)) 
    opened = cv2.morphologyEx(img, cv2.MORPH_OPEN, kernel) 
    _,oth = cv2.threshold(opened,20,255,cv2.THRESH_BINARY)
    kernel2 = cv2.getStructuringElement(cv2.MORPH_ELLIPSE,(7, 7)) 
    dilated = cv2.dilate(oth,kernel2) 
    
    roi = cv2.bitwise_and(img,img,mask = dilated)
    
    roi_copy = roi.copy()
    
    kernel3 = cv2.getStructuringElement(cv2.MORPH_RECT,(7, 7)) 
    erode = cv2.erode(roi_copy, kernel3) 
    
    mask = np.zeros(img.shape,np.uint8)
    cv2.circle(mask,(circle[0],circle[1]),int(circle[2]*0.4),255,-1)
    nmask = cv2.bitwise_not(mask)
    #-----4個點膠------#
    cresult = cv2.bitwise_and(erode,erode,mask=nmask)

    cnts, _ = cv2.findContours(cresult,cv2.RETR_EXTERNAL, cv2.CHAIN_APPROX_SIMPLE)

    for c in cnts:
        cnts_size =cv2.contourArea(c)
        if cnts_size>2000:
            cv2.drawContours(bg, [c], -1, 255, -1) #繪出4個點膠
            cv2.drawContours(imga, [c], 0, (0,255,0) , 2) #繪出4個點膠

    #-----半圓點膠-----#
    result = np.where(roi>240,0,roi).astype(np.uint8)
    nresult = cv2.bitwise_and(result,result,mask=mask)
    cnts, _ = cv2.findContours(nresult,cv2.RETR_EXTERNAL, cv2.CHAIN_APPROX_SIMPLE)
    res = max(cnts, key = lambda x: cv2.contourArea(x))
    cv2.drawContours(bg, [res], -1, 255, -1) #繪出半圓點膠
    cv2.drawContours(imga, [res], 0, (0,255,0) , 2) #繪出半圓點膠
    
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
    
def IoU_score(img,gt):
    h,w=img.shape
    Iand = cv2.bitwise_and(img,gt)
    Ior = cv2.bitwise_or(img,gt)

    return Iand.sum()/float(Ior.sum())


#-----------載入圖片-----------#
def main1(imgb,imga,img_gt):
    starttime = datetime.datetime.now()
   
    #-----------轉通道-----------#
    HSVb = cv2.cvtColor(imgb, cv2.COLOR_BGR2Lab)[:,:,0]
    HSVa = cv2.cvtColor(imga, cv2.COLOR_BGR2Lab)[:,:,0]
    
    diff = HSVb-HSVa
    
    #-----------霍夫圓計算-----------#
    circle = Hough_circle(imgb,imga)
    
    #-----------亮度篩選-----------#
    bg = np.zeros(diff.shape,np.uint8)
    lu = HSVb[251:260,251:260].mean()
    if lu>128:
        high_watch(diff,bg,circle,imga)
    else:
        low_watch(diff,bg,circle,imga)
    
    
    #-----------評估-----------#     
    
    IoU_value = IoU_score(bg,img_gt)

    print("IoU閥值:"+str(IoU_value))
    endtime = datetime.datetime.now()
    print("計算時間:"+str(endtime - starttime))
    
    h,w,_ = imga.shape
    cv2.namedWindow('imga', 0);
    cv2.resizeWindow('imga',int(w/3),int(h/3));
    cv2.imshow('imga',imga)
    
    cv2.namedWindow('bg', 0);
    cv2.resizeWindow('bg',int(w/3),int(h/3));
    cv2.imshow('bg',bg)
    
    cv2.waitKey(0)
    
    cv2.destroyAllWindows()

if __name__ == '__main__': 
    findex = '4'
    fgood_b = 'sample_good/'+findex+'_before.jpg'
    fgood_a = 'sample_good/'+findex+'_after.jpg'
    
    if findex == '7':
        fgt = 'sample_good_Groundtruth/'+findex+'_correct.bmp'
    else:
        fgt = 'sample_good_Groundtruth/'+findex+'_correct.jpg'
    
    imgb = cv2.imread(fgood_b)
    imga = cv2.imread(fgood_a)
    img_gt = cv2.imread(fgt,0)
    main1(imgb,imga,img_gt)