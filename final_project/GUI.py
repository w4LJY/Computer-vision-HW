import tkinter as tk
from tkinter import filedialog
import cv2
import PIL.Image as Image 
from PIL import ImageTk
import os

import final1
import final2

def selectPath():
    #設置路徑到變數
    #after
    file_after =  filedialog.askopenfilename(initialdir = "/",title = "Select file",filetypes = (("jpeg files","*.jpg"), ("png files","*.png"), ("gif files","*.gif"),("all files","*.*")))
    patha.set(file_after)
    #before
    file_before = file_after.replace('after','before')
    pathb.set(file_before)
    #GT
    file_gt = file_after.replace('after','correct')
    file_gt = file_gt.replace('sample_good','sample_good_Groundtruth')
    pathg.set(file_gt)
    
    img = cv2.imread(file_after)
    rows,cols,_=img.shape

    img = cv2.resize(img,(int(cols/3),int(rows/3)))
    #resize前後大小
    # print(cols,rows)
    # print(int(cols/3),int(rows/3))
    img = cv2.cvtColor(img, cv2.COLOR_BGR2RGB)
    img = ImageTk.PhotoImage(Image.fromarray(img))
    pictureFrame.create_image(int(2590/6),int(1942/6),anchor = 'center', image = img)
    pictureFrame.img = img

def project1():
    file_after=patha.get()
    file_before=pathb.get()
    file_get=pathg.get()
    print(file_after)
    print(file_before)
    print(file_get)
    if os.path.isfile(file_get):
        imga = cv2.imread(file_after)
        imgb = cv2.imread(file_before)
        imggt = cv2.imread(file_get,0)
        final1.main1(imgb,imga,imggt)
        
def project2():
    file_after=patha.get()
    file_before=pathb.get()
    print(file_after)
    print(file_before)
    if os.path.isfile(file_after):
        imga = cv2.imread(file_after)
        imgb = cv2.imread(file_before)
        final2.main2(imgb,imga)
        
    
#GUI建立
window=tk.Tk()
window.wm_title("動畫模擬系統")


w=1300
h=700
window.geometry("{}x{}".format(w, h))
window.configure(bg='#F8F8FF')

#載入圖片
tk.Label(window,text = "輸入after圖像路徑:",bg = '#F8F8FF',fg = 'black').place(relx=0.03,rely=0.1,anchor='nw')
patha = tk.StringVar()
pathb = tk.StringVar()
pathg = tk.StringVar()
tk.Entry(window, textvariable = patha).place(relx=0.03,rely=0.15,width=200,anchor='nw')
tk.Button(window, text = "路徑選擇",bg = '#006400',fg = 'white',
                         command = lambda:selectPath()).place(relx=0.2,rely=0.15,anchor='nw')


#圖片欄
pictureFrame=tk.Canvas(window,bg='#333f50')
pictureFrame.place(relx=0.9755,rely=0.97,width=int(2590/3),heigh=int(1942/3),anchor='se')

#選擇題號
tk.Button(window, text = "PROJECT 1",bg = '#006400',fg = 'white',activebackground = '#ADFF2F',
                          command=lambda: project1()).place(relx=0.03,rely=0.3,width=200,heigh=50,anchor='nw')
tk.Button(window, text = "PROJECT 2",bg = '#006400',fg = 'white',activebackground = '#ADFF2F',
                          command=lambda: project2()).place(relx=0.03,rely=0.45,width=200,heigh=50,anchor='nw')


tk.Label(window,text = "分析與資訊顯示於Console",bg = '#F8F8FF',fg = 'black').place(relx=0.03,rely=0.85,anchor='nw')


window.mainloop()
