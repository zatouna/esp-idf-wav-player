# File Manager Component

A file system management component for ESP32 that provides unified access to SPIFFS, FAT, and other file systems with additional utilities for file operations.

## Features

- Unified file system interface
- Support for multiple file systems:
  - SPIFFS
  - FAT
  - [Other supported file systems]
- File operation utilities:
  - List files
  - Create/Delete files
  - Read/Write operations
  - Directory management
- Error handling and reporting

## Installation

```bash
idf.py add-dependency "your-username/file_manager^1.0.0"
```

## API Reference

[Document your file manager API functions here]

## Example Usage

```c
#include "file_manager.h"

void file_operations(void) {
    // Initialize file system
    file_manager_init();
    
    // List files in directory
    file_manager_list_files("/spiffs");
    
    // Read file
    char buffer[1024];
    file_manager_read_file("/spiffs/data.txt", buffer, sizeof(buffer));
}
```

## Configuration

Configure the file manager through menuconfig:
- File system type
- Partition size
- Mount points
- Cache settings

## Dependencies

- ESP-IDF v5.0 or later
- File system drivers (SPIFFS, FAT, etc.)

## License

[Add your license information here]
