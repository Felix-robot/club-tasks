#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
License Plate Detection and Extraction Program
Function: Input image containing license plate, automatically detect and extract the license plate region
"""

import cv2
import numpy as np
import argparse
import os
from typing import Tuple, Optional, List


class LicensePlateDetector:
    """License Plate Detector Class"""
    
    def __init__(self):
        """Initialize detector"""
        # Define HSV color range for blue license plates
        # Adjusted HSV color range for better license plate detection
        self.lower_blue = np.array([90, 40, 40])  # Lower blue threshold (wider range)
        self.upper_blue = np.array([140, 255, 255])  # Upper blue threshold (wider range)
        
        # License plate aspect ratio range
        self.min_aspect_ratio = 2.0
        self.max_aspect_ratio = 4.0
        
        # License plate minimum and maximum area
        self.min_area = 1000
        self.max_area = 50000
    
    def _read_image(self, image_path: str) -> Optional[np.ndarray]:
        """
        Read image
        Args:
            image_path: Image path
        Returns:
            Image array, None if failed
        """
        try:
            image = cv2.imread(image_path)
            if image is None:
                print(f"Error: Could not read image file {image_path}")
            return image
        except Exception as e:
            print(f"Error reading image: {str(e)}")
            return None
    
    def preprocess_image(self, image: np.ndarray) -> np.ndarray:
        """
        Preprocess image to enhance license plate detection
        Args:
            image: Input image
        Returns:
            Preprocessed image mask
        """
        # Convert to HSV color space for better color detection
        hsv = cv2.cvtColor(image, cv2.COLOR_BGR2HSV)
        
        # Create blue mask using adjusted HSV range
        blue_mask = cv2.inRange(hsv, self.lower_blue, self.upper_blue)
        
        # Apply Gaussian blur to reduce noise
        blue_mask = cv2.GaussianBlur(blue_mask, (5, 5), 0)
        
        # Morphological operations to remove noise and enhance plate features
        kernel_close = np.ones((5, 5), np.uint8)  # Larger kernel for closing
        kernel_open = np.ones((3, 3), np.uint8)   # Smaller kernel for opening
        
        # Close operation to fill gaps
        blue_mask = cv2.morphologyEx(blue_mask, cv2.MORPH_CLOSE, kernel_close)
        # Open operation to remove small objects
        blue_mask = cv2.morphologyEx(blue_mask, cv2.MORPH_OPEN, kernel_open)
        
        return blue_mask
    
    def find_contours(self, mask: np.ndarray) -> List[np.ndarray]:
        """
        Find and filter contours to identify potential license plate regions
        Args:
            mask: Binary mask image
        Returns:
            List of valid contours
        """
        # Find contours
        contours, _ = cv2.findContours(mask, cv2.RETR_EXTERNAL, cv2.CHAIN_APPROX_SIMPLE)
        
        # Filter contours based on multiple criteria
        valid_contours = []
        for contour in contours:
            # Calculate contour area
            area = cv2.contourArea(contour)
            
            # Filter out contours that are too small or too large
            if area < self.min_area or area > self.max_area:
                continue
            
            # Calculate bounding rectangle and aspect ratio
            x, y, w, h = cv2.boundingRect(contour)
            aspect_ratio = float(w) / h
            
            # License plates typically have a width-to-height ratio between 2:1 and 4:1
            if aspect_ratio < self.min_aspect_ratio or aspect_ratio > self.max_aspect_ratio:
                continue
            
            # Additional filter: check solidity (area / bounding rectangle area)
            solidity = float(area) / (w * h)
            # License plates should be relatively solid objects
            if solidity < 0.4:
                continue
            
            valid_contours.append(contour)
        
        return valid_contours
    
    def extract_license_plate_region(self, image: np.ndarray, contour: np.ndarray) -> np.ndarray:
        """
        Extract and enhance the license plate region
        Args:
            image: Original image
            contour: License plate contour
        Returns:
            Extracted license plate region with enhancements
        """
        # Get bounding rectangle of the contour
        x, y, w, h = cv2.boundingRect(contour)
        
        # Add some margin to ensure complete extraction
        margin = 10
        x = max(0, x - margin)
        y = max(0, y - margin)
        w = min(image.shape[1] - x, w + 2 * margin)
        h = min(image.shape[0] - y, h + 2 * margin)
        
        # Extract license plate region
        license_plate = image[y:y+h, x:x+w]
        
        # Apply some post-processing to enhance the license plate
        # Convert to grayscale
        gray_plate = cv2.cvtColor(license_plate, cv2.COLOR_BGR2GRAY)
        
        # Apply adaptive threshold to make text more visible
        thresh = cv2.adaptiveThreshold(gray_plate, 255, 
                                     cv2.ADAPTIVE_THRESH_GAUSSIAN_C, 
                                     cv2.THRESH_BINARY_INV, 11, 2)
        
        # Convert back to BGR for consistent output format
        enhanced_plate = cv2.cvtColor(thresh, cv2.COLOR_GRAY2BGR)
        
        return enhanced_plate
    
    def detect_license_plate(self, image_path: str) -> Optional[np.ndarray]:
        """
        检测并提取车牌
        Args:
            image_path: 输入图像路径
        Returns:
            提取的车牌图像，如果未找到则返回None
        """
        try:
            # Read image
            image = self._read_image(image_path)
            if image is None:
                print(f"Error: Could not read image file {image_path}")
                return None
              
            print(f"Processing image: {image_path}")
            print(f"Image size: {image.shape}")
            
            # 图像预处理
            mask = self.preprocess_image(image)
            
            # 查找轮廓
            contours = self.find_contours(mask)
            
            if not contours:
                print("No valid license plate contours found")
                return None
            
            # Select the largest contour as license plate region
            largest_contour = max(contours, key=cv2.contourArea)
            
            # Extract license plate region
            license_plate = self.extract_license_plate_region(image, largest_contour)
            
            print(f"Successfully detected license plate region")
            print(f"License plate size: {license_plate.shape}")
            
            return license_plate
            
        except Exception as e:
            print(f"Error processing image: {str(e)}")
            return None
    
    def save_license_plate(self, license_plate: np.ndarray, output_path: str) -> bool:
        """
        Save extracted license plate image
        Args:
            license_plate: License plate image
            output_path: Output path
        Returns:
            Whether save was successful
        """
        try:
            # Ensure output directory exists
            os.makedirs(os.path.dirname(output_path) if os.path.dirname(output_path) else '.', exist_ok=True)
            
            # Save image
            success = cv2.imwrite(output_path, license_plate)
            if success:
                print(f"License plate image saved to: {output_path}")
                return True
            else:
                print(f"Failed to save image: {output_path}")
                return False
                
        except Exception as e:
            print(f"Error saving image: {str(e)}")
            return False


def main():
    """Main function"""
    # Create command line argument parser
    parser = argparse.ArgumentParser(description='License Plate Detection and Extraction Program')
    parser.add_argument('input_image', help='Input image path')
    parser.add_argument('-o', '--output', help='Output image path', default='license_plate_output.jpg')
    parser.add_argument('--show', action='store_true', help='Show detection results')
    
    args = parser.parse_args()
    
    # Check if input file exists
    if not os.path.exists(args.input_image):
        print(f"Error: Input file does not exist: {args.input_image}")
        return
    
    # Create detector instance
    detector = LicensePlateDetector()
    
    # Detect and extract license plate
    license_plate = detector.detect_license_plate(args.input_image)
    
    if license_plate is not None:
        # Save license plate image
        detector.save_license_plate(license_plate, args.output)
        
        # Show results if requested
        if args.show:
            cv2.imshow('Detected License Plate', license_plate)
            cv2.waitKey(0)
            cv2.destroyAllWindows()
    else:
        print("Failed to detect license plate, please check input image")


if __name__ == "__main__":
    main()
