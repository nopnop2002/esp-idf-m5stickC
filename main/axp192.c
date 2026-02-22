#include <string.h>

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"

#include "axp192.h"

#define TAG "AXP192"
#define	I2C_AXP192	0x34
#define	SDA_AXP192	21
#define	SCL_AXP192	22
#define I2C_TICKS_TO_WAIT 100 // Maximum ticks to wait before issuing a timeout

static i2c_master_bus_handle_t bus_handle;
static i2c_master_dev_handle_t dev_handle;

uint8_t i2c_master_read(uint8_t reg) {
	uint8_t in_buf[1];
	uint8_t out_buf[1];
	out_buf[0] = reg;
	ESP_ERROR_CHECK(i2c_master_transmit_receive(dev_handle, out_buf, 1, in_buf, 1, I2C_TICKS_TO_WAIT));
	return in_buf[0];
}

void i2c_master_write(uint8_t reg, uint8_t data) {
	uint8_t out_buf[2];
	out_buf[0] = reg;
	out_buf[1] = data;
	ESP_ERROR_CHECK(i2c_master_transmit(dev_handle, out_buf, 2, I2C_TICKS_TO_WAIT));
}

void AXP192_Initialize(i2c_port_num_t port)
{
	i2c_master_bus_config_t i2c_mst_config = {};
	i2c_mst_config.clk_source = I2C_CLK_SRC_DEFAULT;
	i2c_mst_config.glitch_ignore_cnt = 7;
	i2c_mst_config.i2c_port = port;
	i2c_mst_config.scl_io_num = (gpio_num_t)SCL_AXP192;
	i2c_mst_config.sda_io_num = (gpio_num_t)SDA_AXP192;
	i2c_mst_config.flags.enable_internal_pullup = true;

	ESP_ERROR_CHECK(i2c_new_master_bus(&i2c_mst_config, &bus_handle));

	i2c_device_config_t dev_cfg = {};
	dev_cfg.dev_addr_length = I2C_ADDR_BIT_LEN_7;
	dev_cfg.device_address = I2C_AXP192;
	dev_cfg.scl_speed_hz = 1000000;

	ESP_ERROR_CHECK(i2c_master_bus_add_device(bus_handle, &dev_cfg, &dev_handle));
}

// Power On
void AXP192_PowerOn() {
	// Set LDO2 & LDO3(TFT_LED & TFT) 3.0V
	i2c_master_write(0x28, 0xcc);
	uint8_t data = i2c_master_read(0x28);
	ESP_LOGD(TAG, "data(0x28)=%x", data);

	// Set ADC sample rate to 200hz
	i2c_master_write(0x84, 0xF2);
	data = i2c_master_read(0x84);
	ESP_LOGD(TAG, "data(0x84)=%x", data);

	// Set ADC to All Enable
	i2c_master_write(0x82, 0xff);
	data = i2c_master_read(0x82);
	ESP_LOGD(TAG, "data(0x82)=%x", data);

	// Bat charge voltage to 4.2, Current 100MA
	i2c_master_write(0x33, 0xc0);
	data = i2c_master_read(0x33);
	ESP_LOGD(TAG, "data(0x33)=%x", data);

	// Depending on configuration enable LDO2, LDO3, DCDC1, DCDC3.
	data = i2c_master_read(0x12);
	ESP_LOGI(TAG, "data(0x12)=%x", data);
	data = (data & 0xEF) | 0x4D;
	ESP_LOGI(TAG, "data(0x12)=%x", data);
	i2c_master_write(0x12, data);

	// 128ms power on, 4s power off
	i2c_master_write(0x36, 0x0C);

	// Set RTC voltage to 3.3V
	i2c_master_write(0x91, 0xF0);

	// Set GPIO0 to LDO
	i2c_master_write(0x90, 0x02);

	// Disable vbus hold limit
	i2c_master_write(0x30, 0x80);

	// Set temperature protection
	i2c_master_write(0x39, 0xfc);

	// Enable RTC BAT charge 
	i2c_master_write(0x35, 0xa2);

	// Enable bat detection
	i2c_master_write(0x32, 0x46);

	// Set Power off voltage 3.0v
	data = i2c_master_read(0x31);
	ESP_LOGI(TAG, "data(0x31)=%x", data);
	data = (data & 0xF8) | (1 << 2);
	ESP_LOGI(TAG, "data(0x31)=%x", data);
	i2c_master_write(0x31 , data);
}

// Screen Brightness
void AXP192_ScreenBreath(uint8_t brightness) {
	if (brightness > 12) 
	{
		brightness = 12;
	}
	uint8_t buf = i2c_master_read(0x28);
	i2c_master_write(0x28, ((buf & 0x0f) | (brightness << 4)) );
}

// Enable Coulomb Counter
void AXP192_EnableCoulombcounter() {
	i2c_master_write(0xB8, 0x80);
}

// Disable Coulomb Counter
void AXP192_DisableCoulombcounter() {
	i2c_master_write(0xB8, 0x00);
}

// Stop Coulomb Counter
void AXP192_StopCoulombcounter() {
	i2c_master_write(0xB8, 0xC0);
}

// Clear Coulomb Counter
void AXP192_ClearCoulombcounter() {
	i2c_master_write(0xB8, 0xA0);
}
