#pragma once
#ifndef ESP_PLATFORM
#error "This library requires an ESP32 line MCU"
#endif
#if __has_include(<Arduino.h>)
#include <Arduino.h>
#include <Wire.h>

#else
#include <esp_idf_version.h>
#include <stddef.h>
#include <stdint.h>
#include <memory.h>
#if ESP_IDF_VERSION >= ESP_IDF_VERSION_VAL(5, 0, 0)
#include <driver/i2c_master.h>
#else
#include <driver/i2c.h>
#endif
#endif
#ifdef ARDUINO
namespace arduino {
#else
namespace esp_idf {
#endif
template<int Port, uint8_t SdaPin, uint8_t SclPin>
class esp_i2c {
#if defined(ARDUINO) || ESP_IDF_VERSION >= ESP_IDF_VERSION_VAL(5, 0, 0)
    static bool m_initialized;
#endif
public:
#ifdef ARDUINO
    static TwoWire& get_instance() {
        if(Port==1) {
            Wire1.begin(SdaPin,SclPin);
            return Wire1;
        }
        Wire.begin(SdaPin,SclPin);
        return Wire;
    }
    static TwoWire& instance;
#else
#if ESP_IDF_VERSION >= ESP_IDF_VERSION_VAL(5, 0, 0)
    static i2c_master_bus_handle_t get_instance() {
        i2c_master_bus_config_t i2c_mst_config;
        memset(&i2c_mst_config,0,sizeof(i2c_mst_config));
        i2c_mst_config.clk_source = I2C_CLK_SRC_DEFAULT;
        i2c_mst_config.i2c_port = Port==1?I2C_NUM_1:I2C_NUM_0;
        i2c_mst_config.scl_io_num = (gpio_num_t)SclPin;
        i2c_mst_config.sda_io_num = (gpio_num_t)SdaPin;
        i2c_mst_config.glitch_ignore_cnt = 7;
        i2c_mst_config.flags.enable_internal_pullup = true;
        i2c_master_bus_handle_t bus_handle;
        if(ESP_OK!=i2c_new_master_bus(&i2c_mst_config, &bus_handle)) {
            return nullptr;
        }
        return bus_handle;
    }
    static i2c_master_bus_handle_t instance;
#else
    constexpr static const i2c_port_t instance = (i2c_port_t)Port;
#endif
#endif
};
#if defined(ARDUINO) || ESP_IDF_VERSION >= ESP_IDF_VERSION_VAL(5, 0, 0)
template<int Port, uint8_t SdaPin, uint8_t SclPin>
bool esp_i2c<Port, SdaPin, SclPin>::m_initialized = false;
#endif
#if defined(ARDUINO) || ESP_IDF_VERSION >= ESP_IDF_VERSION_VAL(5, 0, 0)
template<int Port, uint8_t SdaPin, uint8_t SclPin>
#ifdef ARDUINO
TwoWire& 
#else
i2c_master_bus_handle_t 
#endif
esp_i2c<Port, SdaPin, SclPin>::instance = get_instance();
}
#endif