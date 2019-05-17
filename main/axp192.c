#include <string.h>

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include "driver/i2c.h"
#include "esp_log.h"

//#include "ssd1306.h"

#define tag "AXP192"
#define	I2C_AXP192	0x34
#define	SDA_AXP192	21
#define	SCL_AXP192	22

void i2c_master_init()
{
    esp_err_t ret;

	i2c_config_t i2c_config = {
		.mode = I2C_MODE_MASTER,
		.sda_io_num = SDA_AXP192,
		.scl_io_num = SCL_AXP192,
		.sda_pullup_en = GPIO_PULLUP_ENABLE,
		.scl_pullup_en = GPIO_PULLUP_ENABLE,
		.master.clk_speed = 1000000
	};
	ret = i2c_param_config(I2C_NUM_0, &i2c_config);
    assert(ret==ESP_OK);
	ret = i2c_driver_install(I2C_NUM_0, I2C_MODE_MASTER, 0, 0, 0);
    assert(ret==ESP_OK);
}

void i2c_write(uint8_t reg, uint8_t data) {
	esp_err_t espRc;
	
	i2c_cmd_handle_t cmd = i2c_cmd_link_create();

	i2c_master_start(cmd);
	i2c_master_write_byte(cmd, (I2C_AXP192 << 1) | I2C_MASTER_WRITE, true);
	i2c_master_write_byte(cmd, reg, true);
	i2c_master_write_byte(cmd, data, true);
	i2c_master_stop(cmd);

	espRc = i2c_master_cmd_begin(I2C_NUM_0, cmd, 10/portTICK_PERIOD_MS);
	if (espRc == ESP_OK) {
		ESP_LOGD(tag, "AXP192 configured successfully");
	} else {
		ESP_LOGE(tag, "AXP192 configuration failed. code: 0x%.2X", espRc);
	}
	i2c_cmd_link_delete(cmd);
}

// Power On
void AXP192_PowerOn() {
	i2c_write(0x10, 0xff);
	i2c_write(0x28, 0xff);
	i2c_write(0x82, 0xff);
	i2c_write(0x33, 0xC0);
	i2c_write(0xB8, 0x80);
	i2c_write(0x12, 0x4D);
	i2c_write(0x36, 0x5C);
	i2c_write(0x90, 0x02);
	i2c_write(0x31, 0x04);
}

// Screen Brightness
void AXP192_ScreenBreath(uint8_t brightness) {
	i2c_write(0x28, ((brightness & 0x0f) << 4));
}

// Enable Coulombcounter
void AXP192_EnableCoulombcounter() {
	i2c_write(0xB8, 0x80);
}

// Disable Coulombcounter
void AXP192_DisableCoulombcounter() {
	i2c_write(0xB8, 0x00);
}

// Stop Coulombcounter
void AXP192_StopCoulombcounter() {
	i2c_write(0xB8, 0xC0);
}

// Clear Coulombcounter
void AXP192_ClearCoulombcounter() {
	i2c_write(0xB8, 0xA0);
}
