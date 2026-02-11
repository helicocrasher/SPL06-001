#ifndef SPL06_001_GLUE_H
#define SPL06_001_GLUE_H

#include "stm32g0xx_hal.h"
//#include "i2c.h" // Ensure this header provides hi2c2
#ifdef __cplusplus
extern "C" {
#endif
extern I2C_HandleTypeDef hi2c2;
#ifdef __cplusplus
}
#endif

#include "platform_abstraction.h" // For SerialI2CDebug macros
#include <stdint.h>
#include <stdio.h>
#include <stddef.h>
#include <string.h>

extern UART_HandleTypeDef huart2;



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

    // Non-blocking transmit (interrupt mode)
    bool write(uint8_t *data, size_t len) {
      uint8_t status;
      char str_Buf[60];
        //SerialI2CDebug_print((char*)"write non blocking");
        status = HAL_I2C_Master_Transmit_IT(&hi2c2, _addr, data, len) == HAL_OK;
        snprintf (str_Buf, 60, "Transmit_IT  I2C_adr=0x%x wbuf=0x%04x Wlen=%d Status=0x%02x\n\r", _addr/2, *data, len, status); // Ensure register address is sent as single byte
        SerialI2CDebug_print(str_Buf);
        return (status);
    }
    // Non-blocking receive (interrupt mode)
    bool read(uint8_t *data, size_t len) {
        SerialI2CDebug_println((char*)"read non blocking");
        return HAL_I2C_Master_Receive_IT(&hi2c2, _addr, data, len) == HAL_OK;
    }
    // Blocking versions for initialization/config
    bool writeBlocking(uint8_t *data, size_t len) {
        SerialI2CDebug_println((char*)"write blocking");
        return HAL_I2C_Master_Transmit(&hi2c2, _addr, data, len, 100) == HAL_OK;
    }
    bool readBlocking(uint8_t *data, size_t len) {
        SerialI2CDebug_println((char*)"read blocking"); 
        return HAL_I2C_Master_Receive(&hi2c2, _addr, data, len, 100) == HAL_OK;
    }

    // SPL06-001 expects this for initialization
    bool begin() { return true; }
    // SPL06-001 expects this for register read

    bool write_then_read(uint8_t *wbuf, size_t wlen, uint8_t *rbuf, size_t rlen) {
      char str_Buf[60],str_Buf2[60];
      int status;
        snprintf (str_Buf, 60, "Transmit_Receive_I2C_adr=0x%x wbuf=0x%02x Wlen=%d Rlen=%d ", _addr/2, *wbuf, wlen, rlen); // Ensure register address is sent as single byte
        SerialI2CDebug_print(str_Buf);
        status = HAL_I2C_Master_Transmit(&hi2c2, _addr, wbuf, wlen, 100);
        if (status != HAL_OK){
          snprintf (str_Buf2, 60, "Write_Status= 0x%02x \n\r", status); // Ensure register address is sent as single byte
          SerialI2CDebug_print(str_Buf2);
          return false;
        }
        HAL_Delay(1); 
        status=HAL_I2C_Master_Receive(&hi2c2, _addr, rbuf, rlen, 100);
        if (status != HAL_OK) {
        snprintf (str_Buf2, 60, " Read_Status= 0x%02x \n\r", status); // Ensure register address is sent as single byte
        SerialI2CDebug_print(str_Buf2);
          return false;
        }
        HAL_Delay(1); 
        snprintf(str_Buf2,60,"rbuf=0x%04x Status=0x%02x\n\r", *rbuf, status);
        SerialI2CDebug_print(str_Buf2); 
        return true;

    }
private:
    uint8_t _addr;
};

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
};
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
struct TwoWire {
    TwoWire(int sda = 0, int scl = 1) { (void)sda; (void)scl; }
    bool begin() { return true; }
};
static TwoWire Wire;

// Minimal SPIClass stub for compatibility
struct SPIClass {};
static SPIClass SPI;


// SerialI2Cdebug macro/function for UART2
//#include "usart.h" // Ensure this header provides 'extern UART_HandleTypeDef huart2;'
#define SerialI2Cdebug_print(msg) HAL_UART_Transmit(&huart2, (uint8_t*)(msg), strlen(msg), 100)
#define SerialI2Cdebug_println(msg) do { \
    HAL_UART_Transmit(&huart2, (uint8_t*)(msg), strlen(msg), 100); \
    const char crlf[] = "\r\n"; \
    HAL_UART_Transmit(&huart2, (uint8_t*)crlf, 2, 100); \
} while(0)

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
#include "stm32g0xx_hal.h"
inline uint32_t millis() { return HAL_GetTick(); }

#define delay HAL_Delay

#endif // SPL06_001_GLUE_H
