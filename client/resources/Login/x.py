
import cv2
img = cv2.imread("3.jpg")
print(img.shape) #1573, 1080, 3
img2 = cv2.imread("esu.png",cv2.IMREAD_UNCHANGED)
print(img2.shape)
img2=cv2.resize(img2,(1000,1000))

for i in range(img2.shape[0]):
    for j in range(img2.shape[1]):
        if img2[i][j][3]==0:
            img2[i][j][0]=0
            img2[i][j][1]=0
            img2[i][j][2]=0
        elif img2[i][j][0]>=180 and img2[i][j][1]>=180 and img2[i][j][2]>=180:
            img2[i][j][0]=0
            img2[i][j][1]=0
            img2[i][j][2]=0
            img2[i][j][3]=0
            #pass

# 计算需要填充的边距
top = (1573 - 1000) // 2
bottom = 1573 - 1000 - top
left = (1080 - 1000) // 2
right = 1080 - 1000 - left

# 使用白色填充 (255,255,255)
img2_padded = cv2.copyMakeBorder(img2, top, bottom, left, right, 
                                cv2.BORDER_CONSTANT, value=[0,0,0,0])

cv2.imshow("img2_padded", img2_padded)
cv2.imwrite("begin.jpg",img2_padded)
cv2.waitKey(0)
