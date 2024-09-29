# SD Card Data Logger using ESP32

This project demonstrates a simple data logging system using an ESP32 microcontroller and an SD card. The system writes binary data to the SD card and retrieves the stored data for historical review. This can be a useful starting point for data logging applications in IoT, industrial monitoring, and sensor-based systems.

## Features

- **ESP32 Integration**: Uses the ESP-IDF framework for seamless SD card integration with ESP32.
- **SPI Interface**: The SD card is connected via SPI interface, with customizable pin configurations.
- **File Operations**: Supports reading from and writing to the SD card, handling up to 5 open files concurrently.
- **Error Handling**: Robust error handling for file operations, ensuring data consistency.
- **Data Logging**: Writes sample binary data (30 bytes) to multiple files (up to 600 files), simulating a data logging application.
- **Data Retrieval**: Reads historical data from the SD card and displays it via the console.

## Getting Started

### Prerequisites

To work with this project, you'll need:

- **ESP32** development board
- **SD card module** (SPI-based)
- **SD card** formatted as FAT32
- **ESP-IDF environment** set up on your machine. Follow the [ESP-IDF Setup Guide](https://docs.espressif.com/projects/esp-idf/en/latest/esp32/get-started/) to install the required tools.

### Hardware Configuration

Ensure the following GPIO pin configuration for the SPI interface:

- **MISO**: Configurable (default: `CONFIG_EXAMPLE_PIN_MISO`)
- **MOSI**: Configurable (default: `CONFIG_EXAMPLE_PIN_MOSI`)
- **CLK**: Configurable (default: `CONFIG_EXAMPLE_PIN_CLK`)
- **CS**: Configurable (default: `CONFIG_EXAMPLE_PIN_CS`)

These pins can be customized in the `menuconfig` interface provided by ESP-IDF.

### Installing

Clone this repository and compile the code using the ESP-IDF toolchain.

```bash
git clone https://github.com/Asadalvi007/sd_card_history_module.git
cd sd-card-data-logger-esp32
idf.py build
idf.py flash
```


Check out my profile [Muhammad Asad Ali](https://github.com/Asadalvi007) for more information.
