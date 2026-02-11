# SPL06-001 STM32 Glue Integration

This project uses a minimal glue layer to allow the Arduino/Adafruit-based SPL06-001 driver to work on STM32 (bare-metal, no Arduino core).

## Key Points

- **No Arduino/Adafruit libraries required.**
- **I2C2 is used for SPL06-001.**
- **All I2C operations after initialization are non-blocking (interrupt mode).**
- **SerialI2Cdebug output is redirected to UART2.**

## How it works

- The file `SPL06-001/spl06_001_glue.h` provides minimal stub classes and macros:
  - `Adafruit_Sensor`, `Adafruit_I2CDevice`, `Adafruit_SPIDevice`, `TwoWire`, `SPIClass`.
  - `Adafruit_I2CDevice` uses STM32 HAL I2C2 for all I2C operations.
  - `SerialI2Cdebug_print` and `SerialI2Cdebug_println` send debug output to UART2.
- The SPL06-001 library is patched to include only this glue header for compatibility.
- No changes to SPL06-001 logic; all adaptation is in the glue file.

## Usage

- Include `spl06_001_glue.h` before `SPL06-001.h` in your project.
- Ensure `usart.h` and `i2c.h` are present and provide `huart2` and `hi2c2` handles.
- Use the SPL06-001 API as documented.

## Example

```cpp
#include "spl06_001_glue.h"
#include "SPL06-001.h"

SPL06 baro;

void setup() {
    baro.begin();
}

void loop() {
    double temp = baro.readTemperature();
    double press = baro.readPressure();
    // ...
}
```

## Notes
- All I2C operations after initialization are non-blocking (interrupt mode). Ensure you handle completion in your main loop or via callbacks if needed.
- If you need to use blocking I2C, use the `writeBlocking`/`readBlocking` methods in the glue class.
