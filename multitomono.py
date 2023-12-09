#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
Created on Thu Nov  9 12:25:05 2023

@author: juliusng
"""

import os
from pydub import AudioSegment

# Path to the directory containing your multichannel audio files
directory_path = '/Users/juliusng/Downloads/newsounds'  # Adjust this path to the folder containing your audio files

# Make sure the path is correct
print(f"Directory path: {directory_path}")
print(f"Files in the directory: {os.listdir(directory_path)}")

# Iterate over each file in the directory
for filename in os.listdir(directory_path):
    if filename.endswith('.wav'):  # or '.mp3' if your files are in mp3 format
        file_path = os.path.join(directory_path, filename)
        print(f"Processing file: {file_path}")
        
        # Load the multichannel audio file
        audio = AudioSegment.from_file(file_path)

        # Convert to mono
        mono_audio = audio.set_channels(1)

        # Export the result
        mono_filename = f"mono_{filename}"
        output_path = os.path.join(directory_path, mono_filename)
        mono_audio.export(output_path, format="wav")
        
        print(f"Converted {filename} to mono and saved as {output_path}")

print("All files have been converted to mono.")
