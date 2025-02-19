# WAV Player Component

A lightweight WAV audio file player component for ESP32 that supports 16-bit and 24-bit audio files with volume control.

## Features

- Supports WAV files with:
  - 16-bit and 24-bit sample depths
  - 1 or 2 channels (mono/stereo)
  - Sample rates from 8kHz to 48kHz
- Volume control (0-100%)
- Real-time volume adjustment
- Efficient memory usage with buffered playback

## Installation

To add this component to your project, run:
```bash
idf.py add-dependency "zatouna/wav_player^1.0.0"
```

## API Reference

[Document your WAV player API functions here]

## Configuration

The WAV player can be configured through menuconfig:
- Sample rate
- Bit depth
- DMA buffer size
- I2S mode selection
- Pin assignments
