# License Plate Detection Program

This is a Python program for detecting and extracting license plates from images. The program can automatically detect license plate regions in images containing vehicles and return a cropped image containing only the license plate.

## Features

- 🚗 Automatically detect license plate regions
- 🎯 Precise recognition based on color and shape features
- 📸 Support for multiple image formats
- 💾 Automatically save extracted license plate images
- 🔧 Adjustable detection parameters

## Installation

```bash
pip install -r requirements.txt
```

## Usage

### Command Line

```bash
# Basic usage
python license_plate_detector.py input.jpg

# Specify output path
python license_plate_detector.py input.jpg -o output_license_plate.jpg

# Show detection results
python license_plate_detector.py input.jpg --show
```

### Programmatic Usage

```python
from license_plate_detector import LicensePlateDetector

# Create detector instance
 detector = LicensePlateDetector()

# Detect license plate
license_plate = detector.detect_license_plate("input.jpg")

if license_plate is not None:
    # Save result
    detector.save_license_plate(license_plate, "output.jpg")
```

### Testing

```bash
# Use the improved test script
python test_improved_detector.py

# Or use the example script
python example_usage.py
```

```bash
# 使用参考图片测试
python test_detector.py
```

## 算法原理

1. **颜色检测**: 使用HSV颜色空间检测蓝色区域
2. **形态学处理**: 去除噪声，连接断开的区域
3. **轮廓分析**: 查找符合车牌长宽比的矩形轮廓
4. **区域提取**: 根据检测到的轮廓提取车牌区域

## 参数调整

程序中的关键参数可以根据实际需求进行调整：

- `lower_blue` / `upper_blue`: 蓝色车牌的颜色范围
- `min_aspect_ratio` / `max_aspect_ratio`: 车牌长宽比范围
- `min_area` / `max_area`: 车牌面积范围

## 注意事项

- 程序专门针对中国蓝色车牌优化
- 建议输入图片中车牌清晰可见
- 光照条件会影响检测效果
- 程序会自动添加边距以确保完整提取车牌

## 故障排除

如果检测效果不理想，可以尝试：

1. 调整HSV颜色范围参数
2. 修改长宽比和面积阈值
3. 对输入图片进行预处理（如调整亮度和对比度）
4. 确保车牌在图片中足够大且清晰
