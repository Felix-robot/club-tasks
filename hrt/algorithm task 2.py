import cv2
import numpy as np
from pathlib import Path

def hsv_to_color_name(h, s, v):
    """根据 HSV 值粗略映射到英文颜色名称"""
    if v < 40:
        return "black"
    if s < 40:
        if v > 200:
            return "white"
        else:
            return "gray"

    if (h >= 0 and h <= 10) or (h >= 160 and h <= 179):
        return "red"
    elif h <= 25:
        return "orange"
    elif h <= 35:
        return "yellow"
    elif h <= 85:
        return "green"
    elif h <= 100:
        return "cyan"
    elif h <= 130:
        return "blue"
    elif h <= 145:
        return "purple"
    elif h <= 159:
        return "pink"
    else:
        return "unknown"


def extract_roi_colors(img_path: str, a: int, b: int, c: int, d: int, k: int = 3):
    img = cv2.imread(img_path)
    if img is None:
        raise FileNotFoundError(f"无法读取图像：{img_path}")

    h, w = img.shape[:2]

    # 矩形边界清理
    x1, y1 = int(a), int(b)
    x2, y2 = int(c), int(d)
    if x1 > x2: x1, x2 = x2, x1
    if y1 > y2: y1, y2 = y2, y1

    x1 = max(0, min(x1, w - 1))
    x2 = max(0, min(x2, w))
    y1 = max(0, min(y1, h - 1))
    y2 = max(0, min(y2, h))

    if x2 - x1 <= 0 or y2 - y1 <= 0:
        raise ValueError("矩形范围无效")

    roi_bgr = img[y1:y2, x1:x2]
    roi_hsv = cv2.cvtColor(roi_bgr, cv2.COLOR_BGR2HSV)

    # KMeans 聚类
    Z = roi_hsv.reshape((-1, 3)).astype(np.float32)
    criteria = (cv2.TERM_CRITERIA_EPS + cv2.TERM_CRITERIA_MAX_ITER, 20, 1.0)
    _, labels, centers = cv2.kmeans(Z, k, None, criteria, 5, cv2.KMEANS_PP_CENTERS)

    labels = labels.flatten()
    counts = np.bincount(labels, minlength=k)

    # 排序：按像素数降序
    order = np.argsort(-counts)
    centers_ordered = centers[order]
    counts_ordered = counts[order]

    total_pixels = np.sum(counts_ordered)

    dominant_colors = []
    for hsv, cnt in zip(centers_ordered, counts_ordered):
        h_val, s_val, v_val = hsv
        color_name = hsv_to_color_name(h_val, s_val, v_val)
        dominant_colors.append({
            "hsv": tuple(np.round(hsv, 2).tolist()),
            "pixels": int(cnt),
            "ratio": float(cnt) / total_pixels,
            "color_name": color_name
        })

    # === 生成颜色条，每个颜色单独一条（横向并排） ===
    bar_height = 60
    bar_width = roi_bgr.shape[1]  # 使用与 ROI 相同的宽度
    bars = []

    for color in dominant_colors:
        bar = np.zeros((bar_height, bar_width, 3), dtype=np.uint8)

        hsv_color = np.uint8([[color["hsv"]]])  # shape (1,1,3)
        bgr_color = cv2.cvtColor(hsv_color, cv2.COLOR_HSV2BGR)[0][0].tolist()

        cv2.rectangle(bar, (0, 0), (bar_width, bar_height), bgr_color, -1)

        # 在颜色条上写标签
        text = f"{color['color_name']} {color['ratio']*100:.1f}%"
        cv2.putText(bar, text, (10, bar_height // 2 + 5),
                    cv2.FONT_HERSHEY_SIMPLEX, 0.7, (255, 255, 255), 2,
                    lineType=cv2.LINE_AA)
        bars.append(bar)

    # 将颜色条纵向堆叠
    color_bar_full = np.vstack(bars)

    # === 拼接 ROI 和颜色条（纵向） ===
    roi_with_bar = np.vstack((roi_bgr, color_bar_full))
    combined_path = Path("roi_with_colorbar.png")
    cv2.imwrite(str(combined_path), roi_with_bar)

    return {
        "dominant_colors": dominant_colors,
        "roi_with_colorbar": str(combined_path.resolve())
    }


if __name__ == "__main__":
    img_path = "test.jpg"   # 替换为你自己的图片
    a, b = 150, 360                 # 左上角
    c, d = 400, 540               # 右下角

    try:
        result = extract_roi_colors(img_path, a, b, c, d, k=3)
        print("Top Colors in ROI:")
        for i, color in enumerate(result["dominant_colors"], start=1):
            print(f"  #{i}: HSV={color['hsv']}, Color={color['color_name']}, "
                  f"Pixels={color['pixels']}, Ratio={color['ratio']:.2%}")
        print("ROI + 颜色条拼接已保存到:", result["roi_with_colorbar"])
    except Exception as e:
        print("错误:", e)
