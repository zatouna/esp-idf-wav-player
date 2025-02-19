# I2S Manager Component

ESP32 I2S (Inter-IC Sound) manager component for audio input/output operations with support for various I2S modes and configurations.

## Features

- Supports both I2S input and output
- Configurable sample rates and bit depths
- DMA buffer management
- Support for standard I2S, PDM, and TDM modes
- Easy-to-use API for audio streaming

## Installation

To add this component to your project, run:
```bash
idf.py add-dependency "your-username/i2s_manager^1.0.0"
```

## API Reference

[Document your I2S manager API functions here]

## Configuration

The I2S manager can be configured through menuconfig:
- Sample rate
- Bit depth
- DMA buffer size
- I2S mode selection
- Pin assignments

## Example Usage

```c
#include "i2s_manager.h"

void audio_init(void) {
    i2s_config_t config = {
        // Add your I2S configuration
    };
    
    i2s_manager_init(&config);
}

void audio_output(const uint8_t* data, size_t size) {
    i2s_manager_write(data, size);
}
```

## Dependencies

- ESP-IDF v5.0 or later
- ESP32 with I2S support

## License

[Add your license information here] 