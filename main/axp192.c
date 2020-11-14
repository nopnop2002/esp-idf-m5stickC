#include <string.h>

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include "driver/i2c.h"
#include "esp_log.h"

//#include "ssd1306.h"

#define TAG "AXP192"
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

uint8_t i2c_read(uint8_t reg) {
	esp_err_t espRc;
	uint8_t data[1];
	
	i2c_cmd_handle_t cmd = i2c_cmd_link_create();

	i2c_master_start(cmd);
	i2c_master_write_byte(cmd, (I2C_AXP192 << 1) | I2C_MASTER_WRITE, true);
	i2c_master_write_byte(cmd, reg, true);

	i2c_master_start(cmd);
	i2c_master_write_byte(cmd, (I2C_AXP192 << 1) | I2C_MASTER_READ, true);
	i2c_master_read_byte(cmd, data, I2C_MASTER_LAST_NACK);
	i2c_master_stop(cmd);

	espRc = i2c_master_cmd_begin(I2C_NUM_0, cmd, 10/portTICK_PERIOD_MS);
	if (espRc == ESP_OK) {
		ESP_LOGD(TAG, "AXP192 configured successfully");
	} else {
		ESP_LOGE(TAG, "AXP192 configuration failed. code: 0x%.2X", espRc);
	}
	i2c_cmd_link_delete(cmd);
	return data[0];
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
		ESP_LOGD(TAG, "AXP192 configured successfully");
	} else {
		ESP_LOGE(TAG, "AXP192 configuration failed. code: 0x%.2X", espRc);
	}
	i2c_cmd_link_delete(cmd);
}

// Power On
void AXP192_PowerOn() {
#if 0
	i2c_write(0x10, 0xff);
	i2c_write(0x28, 0xff);
	i2c_write(0x82, 0xff);
	i2c_write(0x33, 0xC0);
	i2c_write(0xB8, 0x80);
	i2c_write(0x12, 0x4D);
	i2c_write(0x36, 0x5C);
	i2c_write(0x90, 0x02);
	i2c_write(0x31, 0x04);
#endif
	// Set LDO2 & LDO3(TFT_LED & TFT) 3.0V
	i2c_write(0x28, 0xcc);
	uint8_t data = i2c_read(0x28);
	ESP_LOGD(TAG, "data(0x28)=%x", data);

	// Set ADC sample rate to 200hz
	i2c_write(0x84, 0xF2);
	data = i2c_read(0x84);
	ESP_LOGD(TAG, "data(0x84)=%x", data);

	// Set ADC to All Enable
	i2c_write(0x82, 0xff);
	data = i2c_read(0x82);
	ESP_LOGD(TAG, "data(0x82)=%x", data);

	// Bat charge voltage to 4.2, Current 100MA
	i2c_write(0x33, 0xc0);
	data = i2c_read(0x33);
	ESP_LOGD(TAG, "data(0x33)=%x", data);

	// Depending on configuration enable LDO2, LDO3, DCDC1, DCDC3.
	data = i2c_read(0x12);
	ESP_LOGI(TAG, "data(0x12)=%x", data);
	data = (data & 0xEF) | 0x4D;
	ESP_LOGI(TAG, "data(0x12)=%x", data);
	i2c_write(0x12, data);

	// 128ms power on, 4s power off
	i2c_write(0x36, 0x0C);

	// Set RTC voltage to 3.3V
	i2c_write(0x91, 0xF0);

	// Set GPIO0 to LDO
	i2c_write(0x90, 0x02);

	// Disable vbus hold limit
	i2c_write(0x30, 0x80);

	// Set temperature protection
	i2c_write(0x39, 0xfc);

	// Enable RTC BAT charge 
	i2c_write(0x35, 0xa2);

	// Enable bat detection
	i2c_write(0x32, 0x46);

	// Set Power off voltage 3.0v
	data = i2c_read(0x31);
	ESP_LOGI(TAG, "data(0x31)=%x", data);
	data = (data & 0xF8) | (1 << 2);
	ESP_LOGI(TAG, "data(0x31)=%x", data);
	i2c_write(0x31 , data);
}

// Screen Brightness
void AXP192_ScreenBreath(uint8_t brightness) {
	if (brightness > 12) 
	{
		brightness = 12;
	}
	uint8_t buf = i2c_read(0x28);
	i2c_write(0x28, ((buf & 0x0f) | (brightness << 4)) );
}

// Enable Coulomb Counter
void AXP192_EnableCoulombcounter() {
	i2c_write(0xB8, 0x80);
}

// Disable Coulomb Counter
void AXP192_DisableCoulombcounter() {
	i2c_write(0xB8, 0x00);
}

// Stop Coulomb Counter
void AXP192_StopCoulombcounter() {
	i2c_write(0xB8, 0xC0);
}

// Clear Coulomb Counter
void AXP192_ClearCoulombcounter() {
	i2c_write(0xB8, 0xA0);
}
