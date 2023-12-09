import os
import subprocess

# Path to the directory containing your audio files
directory_path = '/Users/juliusng/Downloads/newsounds'  # Update this path

# Target loudness in LUFS
target_loudness = -24.0

# Iterate over each file in the directory
for filename in os.listdir(directory_path):
    if filename.endswith('.wav'):  # Adjust the extension according to your file type
        file_path = os.path.join(directory_path, filename)

        # Construct the command to normalize the audio file
        command = [
            'ffmpeg-normalize', file_path, 
            '-ar', '44100',  # You can change this sample rate as needed
            '-o', os.path.join(directory_path, f"normalized_{filename}"), 
            '-f',  # Force overwrite without asking
            '-t', str(target_loudness),  # Target loudness
            '-tp', '0.0'  # True peak maximum (dBTP)
        ]

        # Run the command
        try:
            subprocess.run(command, check=True)
            print(f"Normalized {filename} to {target_loudness} LUFS")
        except subprocess.CalledProcessError as e:
            print(f"An error occurred while processing {filename}: {e}")

print("All files have been normalized.")
