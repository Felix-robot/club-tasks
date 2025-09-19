#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
Test script for the improved license plate detector
"""

import os
import cv2
from license_plate_detector import LicensePlateDetector


def test_improved_detector():
    """Test the improved license plate detector with input image"""
    # Create detector instance
    detector = LicensePlateDetector()
    
    # Test with input image
    input_image = "input.png"
    output_image = "output.png"
    
    # Check if file exists
    if not os.path.exists(input_image):
        print(f"Error: Input image does not exist: {input_image}")
        print("Please make sure you have an 'input.png' file in the current directory.")
        return
    
    print("=" * 60)
    print("Testing improved license plate detector")
    print("=" * 60)
    print(f"Using input image: {input_image}")
    print(f"Output will be saved to: {output_image}")
    print("=" * 60)
    
    # Detect license plate
    license_plate = detector.detect_license_plate(input_image)
    
    if license_plate is not None:
        # Save result
        success = detector.save_license_plate(license_plate, output_image)
        
        if success:
            print("\n✓ Test completed successfully!")
            print(f"License plate image has been saved to {output_image}")
            
            # Show result comparison if OpenCV is available for display
            try:
                # Load images for display
                original = cv2.imread(input_image)
                result = cv2.imread(output_image)
                
                if original is not None and result is not None:
                    print(f"Original image size: {original.shape}")
                    print(f"Extracted license plate size: {result.shape}")
                    
                    # Resize for better display
                    original_resized = cv2.resize(original, (800, 600))
                    result_resized = cv2.resize(result, (400, 200))
                    
                    # Display images
                    cv2.imshow('Original Image', original_resized)
                    cv2.imshow('Extracted License Plate', result_resized)
                    
                    print("\nPress any key to close the image windows.")
                    cv2.waitKey(0)
                    cv2.destroyAllWindows()
            except Exception as e:
                print(f"\nWarning: Could not display images: {str(e)}")
        else:
            print("\n✗ Failed to save license plate image")
    else:
        print("\n✗ Failed to detect license plate")
        print("Possible reasons:")
        print("1. The image does not contain a blue license plate")
        print("2. The license plate is not clearly visible")
        print("3. Lighting conditions in the image are not optimal")
    
    print("=" * 60)
    

if __name__ == "__main__":
    test_improved_detector()