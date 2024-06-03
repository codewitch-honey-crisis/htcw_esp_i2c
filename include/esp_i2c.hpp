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
    static i2c_port_t instance;
    static i2c_port_t get_instance() {
        i2c_config_t conf;
        memset(&conf,0,sizeof(conf));
        conf.mode = I2C_MODE_MASTER;
        conf.sda_io_num = I2C_MASTER_SDA_IO;         // select GPIO specific to your project
        conf.sda_pullup_en = GPIO_PULLUP_ENABLE;
        conf.scl_io_num = I2C_MASTER_SCL_IO;         // select GPIO specific to your project
        conf.scl_pullup_en = GPIO_PULLUP_ENABLE;
        conf.master.clk_speed = 100*1000;  // select frequency specific to your project
        // .clk_flags = 0,          /*!< Optional, you can use I2C_SCLK_SRC_FLAG_* flags to choose i2c source clock here. */
        i2c_param_config((i2c_port_t)Port,&conf);
        i2c_driver_install((i2c_port_t)Port,I2C_MODE_MASTER,0,0,0);
        return (i2c_port_t)Port;
    }
#endif
#endif
};
template<int Port, uint8_t SdaPin, uint8_t SclPin>
#ifdef ARDUINO
TwoWire& 
#else
#if ESP_IDF_VERSION >= ESP_IDF_VERSION_VAL(5, 0, 0)
i2c_master_bus_handle_t 
#else
i2c_port_t
#endif
#endif
esp_i2c<Port, SdaPin, SclPin>::instance = get_instance();
}
//#endif