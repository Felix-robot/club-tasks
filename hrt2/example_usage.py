#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
License Plate Detection Program Usage Example
Demonstrates how to use the LicensePlateDetector class
"""

from license_plate_detector import LicensePlateDetector
import os


def main():
    """Main function - demonstrates program usage"""
    print("=" * 60)
    print("License Plate Detection Program Usage Example")
    print("=" * 60)
    
    # Create detector instance
    detector = LicensePlateDetector()
    
    # Example 1: Process input image
    print("\nExample 1: Process input image")
    print("-" * 40)
    
    input_image = "input.png"
    output_image = "output.png"
    
    if os.path.exists(input_image):
        # Detect license plate
        license_plate = detector.detect_license_plate(input_image)
        
        if license_plate is not None:
            # Save result
            success = detector.save_license_plate(license_plate, output_image)
            if success:
                print(f"✓ Success! License plate saved to: {output_image}")
            else:
                print("✗ Save failed")
        else:
            print("✗ Failed to detect license plate")
    else:
        print(f"✗ Input file does not exist: {input_image}")
    
    # Example 2: Batch processing
    print("\nExample 2: Batch processing example")
    print("-" * 40)
    
    # Find all image files in current directory
    image_extensions = ['.jpg', '.jpeg', '.png', '.bmp', '.tiff']
    image_files = []
    
    for file in os.listdir('.'):
        if any(file.lower().endswith(ext) for ext in image_extensions):
            if not file.startswith('output'):
                image_files.append(file)
    
    if image_files:
        print(f"Found {len(image_files)} image files:")
        for img_file in image_files:
            print(f"  - {img_file}")
        
        # Process the first found image
        if len(image_files) > 0:
            first_image = image_files[0]
            print(f"\nProcessing: {first_image}")
            
            license_plate = detector.detect_license_plate(first_image)
            if license_plate is not None:
                output_name = f"output_{first_image}"
                success = detector.save_license_plate(license_plate, output_name)
                if success:
                    print(f"✓ Batch processing successful! Output: {output_name}")
                else:
                    print("✗ Save failed")
            else:
                print("✗ Failed to detect license plate")
    else:
        print("No image files found for processing")
    
    print("\n" + "=" * 60)
    print("Example demonstration completed!")
    print("=" * 60)


if __name__ == "__main__":
    main()
