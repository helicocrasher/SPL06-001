#ifndef SPL06_001_GLUE_H
#define SPL06_001_GLUE_H
#include "user_main.h"

#ifdef TARGET_BLUEPILL
#include "stm32f1xx_hal.h"
#endif

#ifdef TARGET_MATEKSYS_CRSF_PWM_V10
#include "stm32g0xx_hal.h"
#endif

#ifdef TARGET_WEACT_G031
#include "stm32g0xx_hal.h"
#endif

#ifdef __cplusplus
extern "C" {
#endif

extern I2C_HandleTypeDef __HI2C; 
extern volatile uint8_t i2cWriteComplete;  
#ifdef __cplusplus
}
#endif

//#include "platform_abstraction.h" // For SerialI2CDebug macros
#include <stdint.h>
#include <stdio.h>
#include <stddef.h>
#include <string.h>

// Arduino compatibility
typedef uint8_t byte;

// Forward declaration for SPL06 class
class SPL06;

// Minimal Adafruit_Sensor stub
class Adafruit_Sensor {
public:
    virtual bool getEvent(void *) { return false; }
    virtual void getSensor(void *) {}
};

// Minimal I2C device wrapper for STM32 HAL (I2C2, non-blocking)
class Adafruit_I2CDevice {
public:
    Adafruit_I2CDevice(uint8_t addr, void *wire = nullptr) : _addr(addr << 1) { (void)wire; }

    // Non-blocking transmit (interrupt mode) - used for writing cfg registers 2 bytes: first=register address, second=value
    bool write(uint8_t *data, size_t len) {
        return HAL_I2C_Master_Transmit(&__HI2C, _addr, data, len,100) == HAL_OK;
    }
/*
    // Non-blocking receive (interrupt mode) - not used, but provided for completeness
    bool read(uint8_t *data, size_t len) {
        return HAL_I2C_Master_Receive_IT(&__HI2C, _addr, data, len) == HAL_OK;
    }
*/
/*
    // Blocking version of write (not used, but provided for completeness)
    bool writeBlocking(uint8_t *data, size_t len) {
        //Serial2Debug_println((char*)"write blocking");
        return HAL_I2C_Master_Transmit(&__HI2C, _addr, data, len, 100) == HAL_OK;
    }
*/
/*  // Blocking version of read (not used, but provided for completeness) 
    bool readBlocking(uint8_t *data, size_t len) {
        //Serial2Debug_println((char*)"read blocking"); 
        return HAL_I2C_Master_Receive(&__HI2C, _addr, data, len, 100) == HAL_OK;
    }
*/
    // SPL06-001 expects this for initialization
    bool begin() { return true; }

    // Blocking version of write_then_read heavily used for SPL06-001 register reads (sensor ID, coefficients, pressure/temp data)
    // SPL06-001 expects this for register read operations (write register address, then read data) 
    // used for reading sensor ID and coefficients, and for reading pressure/temp data -byte by byte 
    bool write_then_readBlocking(uint8_t *wbuf, size_t wlen, uint8_t *rbuf, size_t rlen) {
      int status;
        status = HAL_I2C_Master_Transmit(&__HI2C, _addr, wbuf, wlen, 100);
        if (status != HAL_OK){
          return false;
        }
        status=HAL_I2C_Master_Receive(&__HI2C, _addr, rbuf, rlen, 100);
        if (status != HAL_OK) {
            HAL_I2C_DeInit(&__HI2C);
            HAL_I2C_Init(&__HI2C);
          return false;
        }
        return true;
    }

private:
    uint8_t _addr;
}; // End of Adafruit_I2CDevice stub class

// Minimal SPI device stub (not used, but needed for SPL06-001)

// SPI constants for compatibility
#define SPI_BITORDER_MSBFIRST 0
#define SPI_MODE0 0

// Minimal Adafruit_SPIDevice stub for all constructor signatures and methods
class Adafruit_SPIDevice {
public:
    Adafruit_SPIDevice(int8_t, uint32_t = 0, int = 0, int = 0, void* = nullptr) {}
    Adafruit_SPIDevice(int8_t, int8_t, int8_t, int8_t) {}
    bool begin() { return true; }
    bool write(uint8_t*, size_t) { return true; }
    bool write_then_read(uint8_t*, size_t, uint8_t*, size_t) { return true; }
}; // End of Adafruit_SPIDevice stub class

// Math and NAN compatibility
#include <math.h>
#ifndef NAN
#define NAN (__builtin_nanf("") )
#endif

// Minimal TwoWire stub for compatibility
#ifndef SDA2
#define SDA2 0
#endif
#ifndef SCL2
#define SCL2 1
#endif

// Use TwoWire and SPIClass from stm32_arduino_compatibility.h if available
// Otherwise define local stubs for standalone use
#ifndef STM32_ARDUINO_COMPATIBILITY_H

// Define local TwoWire for this module (only if stm32_arduino_compatibility.h not included)
struct TwoWire {
    TwoWire(int sda = 0, int scl = 1) { (void)sda; (void)scl; }
    bool begin() { return true; }
};  

static TwoWire Wire;

// Minimal SPIClass stub for compatibility
struct SPIClass {};
static SPIClass SPI;

#endif  // STM32_ARDUINO_COMPATIBILITY_H


// Minimal sensor_t and sensors_event_t stubs for compatibility
typedef struct {
    char name[12];
    int32_t version;
    int32_t sensor_id;
    int32_t type;
    float min_value;
    float max_value;
    float resolution;
    int32_t min_delay;
} sensor_t;

typedef struct {
    int32_t version;
    int32_t sensor_id;
    int32_t type;
    int32_t timestamp;
    union {
        float temperature;
        float pressure;
    };
} sensors_event_t;

#define SENSOR_TYPE_AMBIENT_TEMPERATURE 1
#define SENSOR_TYPE_PRESSURE 2

// millis() stub for compatibility
// #include "stm32g0xx_hal.h"
inline uint32_t millis() { return HAL_GetTick(); }

#define delay HAL_Delay

#endif // SPL06_001_GLUE_H
