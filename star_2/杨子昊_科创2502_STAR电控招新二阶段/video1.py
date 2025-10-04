import cv2
import numpy as np

# 打开视频文件
cap = cv2.VideoCapture('input.mp4')

# 检查视频是否成功打开
if not cap.isOpened():
    print("无法打开视频文件")
    exit()

# 获取视频参数
width = int(cap.get(cv2.CAP_PROP_FRAME_WIDTH))
height = int(cap.get(cv2.CAP_PROP_FRAME_HEIGHT))
fps = cap.get(cv2.CAP_PROP_FPS)

# 创建输出视频
fourcc = cv2.VideoWriter_fourcc(*'mp4v')  # 使用MP4格式
out = cv2.VideoWriter('output.mp4', fourcc, fps, (width, height))

frame_count = 0
print("开始处理视频...")

while cap.isOpened():
    ret, frame = cap.read()
    if not ret:
        break
    
    # 转换到HSV颜色空间
    hsv = cv2.cvtColor(frame, cv2.COLOR_BGR2HSV)
    
    # 定义绿色范围
    lower_green = np.array([35, 50, 50])
    upper_green = np.array([85, 255, 255])
    
    # 创建绿色掩膜
    mask = cv2.inRange(hsv, lower_green, upper_green)
    
    # 创建结果帧（仅保留绿色区域）
    result = cv2.bitwise_and(frame, frame, mask=mask)
    
    # 写入输出视频
    out.write(result)
    
    frame_count += 1
    if frame_count % 30 == 0:  # 每30帧打印一次进度
        print(f"已处理 {frame_count} 帧...")

# 释放资源
cap.release()
out.release()
print(f"视频处理完成！共处理 {frame_count} 帧")
print(f"输出已保存为 output.mp4")