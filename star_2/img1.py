import cv2
import numpy as np

# 读取图片
img = cv2.imread('input.jpg')

# 转换到HSV颜色空间（更好识别颜色）
hsv = cv2.cvtColor(img, cv2.COLOR_BGR2HSV)

# 定义绿色范围（HSV格式）
lower_green = np.array([35, 50, 50])
upper_green = np.array([85, 255, 255])

# 创建绿色掩膜
mask = cv2.inRange(hsv, lower_green, upper_green)

# 查找轮廓
contours, _ = cv2.findContours(mask, cv2.RETR_EXTERNAL, cv2.CHAIN_APPROX_SIMPLE)

# 计算每个绿色区域重心
for cnt in contours:
    # 忽略小面积噪点（可选）
    if cv2.contourArea(cnt) > 100:
        M = cv2.moments(cnt)
        if M['m00'] != 0:
            cx = int(M['m10'] / M['m00'])
            cy = int(M['m01'] / M['m00'])
            print(f"重心坐标: ({cx}, {cy})")
            # 在图像上标记重心（可选）
            cv2.circle(img, (cx, cy), 5, (0, 0, 255), -1)

# 显示结果（可选）
# cv2.imshow('Result', img)
# cv2.waitKey(0)
# cv2.destroyAllWindows()
cv2.imwrite('result.jpg', img)
print("结果已保存为result.jpg")