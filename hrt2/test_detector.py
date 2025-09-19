#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
License Plate Detector Test Script
Used to test license plate recognition functionality
"""

import os
import cv2
import numpy as np
from license_plate_detector import LicensePlateDetector


# Note: Chinese path handling has been removed as per requirements


def test_with_reference_images():
    """Test program with input image"""
    detector = LicensePlateDetector()
    
    # Test with input image
    input_image = "input.png"
    output_image = "output.png"
    
    # Check if file exists
    if not os.path.exists(input_image):
        print(f"Error: Input image does not exist: {input_image}")
        return
    
    print("=" * 50)
    print("Starting license plate recognition test")
    print("=" * 50)
    
    # Detect license plate
    license_plate = detector.detect_license_plate(input_image)
    
    if license_plate is not None:
        # Save result
        success = detector.save_license_plate(license_plate, output_image)
        
        if success:
            print("\n✓ Test successful!")
            print(f"Input image: {input_image}")
            print(f"Output image: {output_image}")
            
            # Show result comparison
            original = cv2.imread(input_image)
            result = cv2.imread(output_image)
            
            if original is not None and result is not None:
                print(f"Original image size: {original.shape}")
                print(f"Extracted license plate size: {result.shape}")
                
                # Optional: Show image comparison
                cv2.imshow('Original Image', cv2.resize(original, (800, 600)))
                cv2.imshow('Extracted License Plate', cv2.resize(result, (400, 200)))
                cv2.waitKey(0)
                cv2.destroyAllWindows()
        else:
            print("\n✗ Failed to save result")
    else:
        print("\n✗ Failed to detect license plate")


def test_with_custom_image(image_path: str):
    """Test with custom image"""
    if not os.path.exists(image_path):
        print(f"Error: Image file does not exist: {image_path}")
        return
    
    detector = LicensePlateDetector()
    
    print(f"Testing image: {image_path}")
    
    license_plate = detector.detect_license_plate(image_path)
    
    if license_plate is not None:
        output_path = f"custom_output_{os.path.basename(image_path)}"
        success = detector.save_license_plate(license_plate, output_path)
        
        if success:
            print(f"✓ Successfully extracted license plate and saved to: {output_path}")
        else:
            print("✗ 保存失败")
    else:
        print("✗ 未能检测到车牌")


if __name__ == "__main__":
    # 测试参考图片
    test_with_reference_images()
    
    # 如果需要测试其他图片，取消下面的注释并修改路径
    # test_with_custom_image("你的图片路径.jpg")
