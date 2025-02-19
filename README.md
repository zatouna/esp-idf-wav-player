# ESP32-S3 WAV Player

A robust WAV file player implementation for ESP32-S3 microcontrollers using the I2S interface and SPIFFS filesystem.

## Features

- Supports both 16-bit and 24-bit WAV files
- Handles mono and stereo audio formats
- Dynamic volume control (0-100%)
- Automatic sample rate detection
- Efficient memory management with DMA buffers
- Built-in error handling and validation
- Supports SPIFFS filesystem for audio storage
- Watchdog timer integration for system stability

## Hardware Requirements

- ESP32-S3 development board
- I2S DAC or I2S amplifier
- Storage for audio files (internal flash using SPIFFS)

### Pin Configuration

- BCK (Bit Clock): GPIO 15
- LRCK (Word Select): GPIO 16
- DATA (Serial Data): GPIO 17

## Technical Specifications

- Sample Rate: 44.1 kHz (default)
- DMA Buffer Size: 1024 bytes
- Stack Size: 4096 bytes
- Supported Bit Depths: 16-bit and 24-bit
- Volume Range: 0-100 (default: 50)
- Maximum SPIFFS Partitions: 3
- File System: SPIFFS with wear leveling

## Installation

1. Clone this repository
2. Configure your ESP-IDF environment
3. Copy your WAV files to the `storage` directory
4. Build and flash the project:

```bash
idf.py build
idf.py -p [PORT] flash
```

## Usage

The system will automatically:
1. Initialize SPIFFS
2. Mount the filesystem
3. Scan for WAV files
4. Play each WAV file sequentially

### Volume Control

Use the following functions to control playback volume:

```c
void set_volume(int volume);           // Set volume directly (0-100)
void increase_volume(int amount);      // Increase volume by amount
void decrease_volume(int amount);      // Decrease volume by amount
```

## WAV File Requirements

- Format: Uncompressed WAV
- Sample Rates: Up to 44.1 kHz
- Bit Depth: 16-bit or 24-bit
- Channels: Mono or Stereo
- File names must have `.wav` extension
- Files must be stored in SPIFFS partition

## Technical Details

### Memory Management

- Uses DMA for efficient data transfer
- Dynamic buffer allocation based on bit depth
- Automatic cleanup of resources
- Memory-efficient mono to stereo conversion

### Error Handling

- WAV header validation
- File system mount verification
- Buffer allocation checks
- I2S write operation validation
- Watchdog timer integration

### Performance Optimization

- Efficient 24-bit to 16-bit conversion
- Optimized buffer sizes for DMA transfers
- Configurable task delays for system stability
- Automatic sample rate adaptation

## Troubleshooting

Common issues and solutions:

1. No Sound Output
   - Verify I2S pin connections
   - Check WAV file format compatibility
   - Ensure volume level is above 0

2. Distorted Audio
   - Verify power supply stability
   - Check for correct sample rate
   - Ensure WAV file isn't corrupted

3. System Crashes
   - Increase stack size if needed
   - Verify sufficient heap memory
   - Check for valid WAV file formats

## Contributing

1. Fork the repository
2. Create a feature branch
3. Commit your changes
4. Push to the branch
5. Create a Pull Request

## License

This project is licensed under the MIT License - see the LICENSE file for details.
```

</rewritten_file>
