#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/gpio.h"
#include "esp_err.h"
#include "esp_log.h"
#include "esp_system.h"
#include "esp_vfs.h"
#include "esp_spiffs.h"

#include "axp192.h"
#include "st7735s.h"
#include "fontx.h"

#define SCREEN_WIDTH	80
#define SCREEN_HEIGHT	160
#define	INTERVAL	100

#define WAIT	vTaskDelay(INTERVAL)

//If you want to continue by push button, enable thgis line.
//#define WAIT	waitButton()

static const char *TAG = "M5StickC";


// wait until button push
static void waitButton() {
	bool flag = true;
	while(flag) {
		int level = gpio_get_level(GPIO_NUM_37);
		if (level == 0) {
			ESP_LOGI(TAG, "Push Button");
			flag = false;
			while(1) {
				level = gpio_get_level(GPIO_NUM_37);
				if (level == 1) break;
				vTaskDelay(1);
			}
		}
		vTaskDelay(1);
	}
}

static void SPIFFS_Directory(char * path) {
	DIR* dir = opendir(path);
	assert(dir != NULL);
	while (true) {
		struct dirent*pe = readdir(dir);
		if (!pe) break;
		ESP_LOGI(TAG,"d_name=%s d_ino=%d d_type=%x", pe->d_name,pe->d_ino, pe->d_type);
	}
	closedir(dir);
}


TickType_t FillTest(ST7735_t * dev, int width, int height) {
	TickType_t startTick, endTick, diffTick;
	startTick = xTaskGetTickCount();

	lcdFillScreen(dev, RED);
	vTaskDelay(1);
	lcdFillScreen(dev, GREEN);
	vTaskDelay(1);
	lcdFillScreen(dev, BLUE);
	vTaskDelay(1);

	endTick = xTaskGetTickCount();
	diffTick = endTick - startTick;
	ESP_LOGI(__FUNCTION__, "elapsed time[ms]:%d",diffTick*portTICK_RATE_MS);
	return diffTick;
}

TickType_t ColorBarTest(ST7735_t * dev, int width, int height) {
	TickType_t startTick, endTick, diffTick;
	startTick = xTaskGetTickCount();

	uint16_t y1,y2;
	y1 = height/3;
	y2 = (height/3)*2;
	//ESP_LOGI(TAG, "y1=%d y2=%d",y1,y2);
	lcdDrawFillRect(dev, 0, 0, width-1, y1-1, RED);
	lcdDrawFillRect(dev, 0, y1-1, width-1, y2-1, GREEN);
	lcdDrawFillRect(dev, 0, y2-1, width-1, height-1, BLUE);

	endTick = xTaskGetTickCount();
	diffTick = endTick - startTick;
	ESP_LOGI(__FUNCTION__, "elapsed time[ms]:%d",diffTick*portTICK_RATE_MS);
	return diffTick;
}

TickType_t ArrowTest(ST7735_t * dev, FontxFile *fx, int width, int height) {
	TickType_t startTick, endTick, diffTick;
	startTick = xTaskGetTickCount();

	// get font width & height
	uint8_t buffer[FontxGlyphBufSize];
	uint8_t fontWidth;
	uint8_t fontHeight;
	GetFontx(fx, 0, buffer, &fontWidth, &fontHeight);
	//ESP_LOGI(TAG,"fontWidth=%d fontHeight=%d",fontWidth,fontHeight);

	uint16_t color;
	//lcdFillScreen(dev, WHITE);
	lcdFillScreen(dev, BLACK);
	lcdSetFontDirection(dev, 0);
	color = RED;
	uint8_t ascii[10];
	lcdDrawArrow(dev, 10, 10, 0, 0, 5, color);
	strcpy((char *)ascii, "0,0");
	lcdDrawString(dev, fx, 0, 20, ascii, color);

	color = GREEN;
	lcdDrawArrow(dev, 69, 10, 79, 0, 5, color);
	strcpy((char *)ascii, "79,0");
	lcdDrawString(dev, fx, 45, 20, ascii, color);

	color = GRAY;
	lcdDrawArrow(dev, 10, 149, 0, 159, 5, color);
	strcpy((char *)ascii, "0,159");
	lcdDrawString(dev, fx, 0, 130, ascii, color);

	color = BLUE;
	lcdDrawArrow(dev, 69, 149, 79, 159, 5, color);
	strcpy((char *)ascii, "79,159");
	lcdDrawString(dev, fx, 30, 130-fontHeight, ascii, color);

	endTick = xTaskGetTickCount();
	diffTick = endTick - startTick;
	ESP_LOGI(__FUNCTION__, "elapsed time[ms]:%d",diffTick*portTICK_RATE_MS);
	return diffTick;
}

TickType_t HorizontalTest(ST7735_t * dev, FontxFile *fx, int width, int height) {
	TickType_t startTick, endTick, diffTick;
	startTick = xTaskGetTickCount();

	// get font width & height
	uint8_t buffer[FontxGlyphBufSize];
	uint8_t fontWidth;
	uint8_t fontHeight;
	GetFontx(fx, 0, buffer, &fontWidth, &fontHeight);
	//ESP_LOGI(TAG,"fontWidth=%d fontHeight=%d",fontWidth,fontHeight);

	uint16_t color;
	//lcdFillScreen(dev, WHITE);
	lcdFillScreen(dev, BLACK);
	uint8_t ascii[20];
	strcpy((char *)ascii, " M5StickC ");

	color = RED;
	lcdSetFontDirection(dev, 0);
	lcdDrawString(dev, fx, 0, 0, ascii, color);
	lcdSetFontUnderLine(dev, RED);
	lcdDrawString(dev, fx, 0, 0+(fontHeight*1), ascii, color);
	lcdUnsetFontUnderLine(dev);

	lcdSetFontFill(dev, GREEN);
	lcdDrawString(dev, fx, 0, 0+(fontHeight*2), ascii, color);
	lcdSetFontUnderLine(dev, RED);
	lcdDrawString(dev, fx, 0, 0+(fontHeight*3), ascii, color);
	lcdUnsetFontFill(dev);
	lcdUnsetFontUnderLine(dev);

	color = BLUE;
	lcdSetFontDirection(dev, 2);
	lcdDrawString(dev, fx, width, height-(fontHeight*1), ascii, color);
	lcdSetFontUnderLine(dev, BLUE);
	lcdDrawString(dev, fx, width, height-(fontHeight*2), ascii, color);
	lcdUnsetFontUnderLine(dev);

	lcdSetFontFill(dev, YELLOW);
	lcdDrawString(dev, fx, width, height-(fontHeight*3), ascii, color);
	lcdSetFontUnderLine(dev, BLUE);
	lcdDrawString(dev, fx, width, height-(fontHeight*4), ascii, color);
	lcdUnsetFontFill(dev);
	lcdUnsetFontUnderLine(dev);

	endTick = xTaskGetTickCount();
	diffTick = endTick - startTick;
	ESP_LOGI(__FUNCTION__, "elapsed time[ms]:%d",diffTick*portTICK_RATE_MS);
	return diffTick;
}

TickType_t VerticalTest(ST7735_t * dev, FontxFile *fx, int width, int height) {
	TickType_t startTick, endTick, diffTick;
	startTick = xTaskGetTickCount();

	// get font width & height
	uint8_t buffer[FontxGlyphBufSize];
	uint8_t fontWidth;
	uint8_t fontHeight;
	GetFontx(fx, 0, buffer, &fontWidth, &fontHeight);
	//ESP_LOGI(TAG,"fontWidth=%d fontHeight=%d",fontWidth,fontHeight);

	uint16_t color;
	//lcdFillScreen(dev, WHITE);
	lcdFillScreen(dev, BLACK);
	uint8_t ascii[20];
	strcpy((char *)ascii, " M5StickC ");

	color = RED;
	lcdSetFontDirection(dev, 1);
	lcdDrawString(dev, fx, width-(fontHeight*1), 0, ascii, color);
	lcdSetFontUnderLine(dev, RED);
	lcdDrawString(dev, fx, width-(fontHeight*2), 0, ascii, color);
	lcdUnsetFontUnderLine(dev);

	lcdSetFontFill(dev, GREEN);
	lcdDrawString(dev, fx, width-(fontHeight*3), 0, ascii, color);
	lcdSetFontUnderLine(dev, RED);
	lcdDrawString(dev, fx, width-(fontHeight*4), 0, ascii, color);
	lcdUnsetFontFill(dev);
	lcdUnsetFontUnderLine(dev);

	color = BLUE;
	lcdSetFontDirection(dev, 3);
	lcdDrawString(dev, fx, (fontHeight*1)-1, height, ascii, color);
	lcdSetFontUnderLine(dev, BLUE);
	lcdDrawString(dev, fx, (fontHeight*2)-1, height, ascii, color);
	lcdUnsetFontUnderLine(dev);

	lcdSetFontFill(dev, YELLOW);
	lcdDrawString(dev, fx, (fontHeight*3)-1, height, ascii, color);
	lcdSetFontUnderLine(dev, BLUE);
	lcdDrawString(dev, fx, (fontHeight*4)-1, height, ascii, color);
	lcdUnsetFontFill(dev);
	lcdUnsetFontUnderLine(dev);

	endTick = xTaskGetTickCount();
	diffTick = endTick - startTick;
	ESP_LOGI(__FUNCTION__, "elapsed time[ms]:%d",diffTick*portTICK_RATE_MS);
	return diffTick;
}


TickType_t LineTest(ST7735_t * dev, int width, int height) {
	TickType_t startTick, endTick, diffTick;
	startTick = xTaskGetTickCount();

	uint16_t color;
	//lcdFillScreen(dev, WHITE);
	lcdFillScreen(dev, BLACK);
	color=RED;
	for(int ypos=0;ypos<height;ypos=ypos+10) {
		lcdDrawLine(dev, 0, ypos, width, ypos, color);
	}

	for(int xpos=0;xpos<width;xpos=xpos+10) {
		lcdDrawLine(dev, xpos, 0, xpos, height, color);
	}

	endTick = xTaskGetTickCount();
	diffTick = endTick - startTick;
	ESP_LOGI(__FUNCTION__, "elapsed time[ms]:%d",diffTick*portTICK_RATE_MS);
	return diffTick;
}

TickType_t CircleTest(ST7735_t * dev, int width, int height) {
	TickType_t startTick, endTick, diffTick;
	startTick = xTaskGetTickCount();

	uint16_t color;
	//lcdFillScreen(dev, WHITE);
	lcdFillScreen(dev, BLACK);
	color = GRAY;
	uint16_t xpos = width/2;
	uint16_t ypos = height/2;
	for(int i=5;i<height;i=i+5) {
		lcdDrawCircle(dev, xpos, ypos, i, color);
	}

	endTick = xTaskGetTickCount();
	diffTick = endTick - startTick;
	ESP_LOGI(__FUNCTION__, "elapsed time[ms]:%d",diffTick*portTICK_RATE_MS);
	return diffTick;
}

TickType_t RoundRectTest(ST7735_t * dev, int width, int height) {
	TickType_t startTick, endTick, diffTick;
	startTick = xTaskGetTickCount();

	uint16_t color;
	//lcdFillScreen(dev, WHITE);
	lcdFillScreen(dev, BLACK);
	color = BLUE;
	for(int i=5;i<width;i=i+5) {
		if (i > (width-i-1) ) break;
		//ESP_LOGI(TAG, "i=%d, width-i-1=%d",i, width-i-1);
		lcdDrawRoundRect(dev, i, i, (width-i-1), (height-i-1), 10, color);
	}

	endTick = xTaskGetTickCount();
	diffTick = endTick - startTick;
	ESP_LOGI(__FUNCTION__, "elapsed time[ms]:%d",diffTick*portTICK_RATE_MS);
	return diffTick;
}

TickType_t FillRectTest(ST7735_t * dev, int width, int height) {
	TickType_t startTick, endTick, diffTick;
	startTick = xTaskGetTickCount();

	uint16_t color;
	lcdFillScreen(dev, WHITE);

	uint16_t red;
	uint16_t green;
	uint16_t blue;
	srand( (unsigned int)time( NULL ) );
	for(int i=1;i<100;i++) {
		red=rand()%255;
		green=rand()%255;
		blue=rand()%255;
		color=rgb565_conv(red, green, blue);
		uint16_t xpos=rand()%width;
		uint16_t ypos=rand()%height;
		uint16_t size=rand()%(width/5);
		lcdDrawFillRect(dev, xpos, ypos, xpos+size, ypos+size, color);
	}

	endTick = xTaskGetTickCount();
	diffTick = endTick - startTick;
	ESP_LOGI(__FUNCTION__, "elapsed time[ms]:%d",diffTick*portTICK_RATE_MS);
	return diffTick;
}

TickType_t ColorTest(ST7735_t * dev, int width, int height) {
	TickType_t startTick, endTick, diffTick;
	startTick = xTaskGetTickCount();

	uint16_t color;
	lcdFillScreen(dev, WHITE);
	color = RED;
	uint16_t delta = height/16;
	uint16_t ypos = 0;
	for(int i=0;i<16;i++) {
		//ESP_LOGI(TAG, "color=0x%x",color);
		lcdDrawFillRect(dev, 0, ypos, width-1, ypos+delta, color);
		color = color >> 1;
		ypos = ypos + delta;
	}

	endTick = xTaskGetTickCount();
	diffTick = endTick - startTick;
	ESP_LOGI(__FUNCTION__, "elapsed time[ms]:%d",diffTick*portTICK_RATE_MS);
	return diffTick;
}



void tft(void *pvParameters)
{
	// set font file
	FontxFile fx16[2];
	FontxFile fx24[2];
	FontxFile fx32[2];
	InitFontx(fx16,"/spiffs/ILGH16XB.FNT",""); // 8x16Dot Gothic
	InitFontx(fx24,"/spiffs/ILGH24XB.FNT",""); // 12x24Dot Gothic
	InitFontx(fx32,"/spiffs/ILGH32XB.FNT",""); // 16x32Dot Gothic

	//InitFontx(fx16,"/spiffs/ILMH16XB.FNT",""); // 8x16Dot Mincyo
	//InitFontx(fx24,"/spiffs/ILMH24XB.FNT",""); // 12x24Dot Mincyo
	//InitFontx(fx32,"/spiffs/ILMH32XB.FNT",""); // 16x32Dot Mincyo

	ST7735_t dev;
	spi_master_init(&dev);
	lcdInit(&dev, SCREEN_WIDTH, SCREEN_HEIGHT);

	while (1) {
		ESP_LOGI(TAG, "Mainloop Start");
		AXP192_ScreenBreath(15);

		FillTest(&dev, SCREEN_WIDTH, SCREEN_HEIGHT);
		WAIT;
		//vTaskDelay(INTERVAL);

		ColorBarTest(&dev, SCREEN_WIDTH, SCREEN_HEIGHT);
		WAIT;
		//vTaskDelay(INTERVAL);

		ArrowTest(&dev, fx16, SCREEN_WIDTH, SCREEN_HEIGHT);
		WAIT;
		//vTaskDelay(INTERVAL);

		LineTest(&dev, SCREEN_WIDTH, SCREEN_HEIGHT);
		WAIT;

		CircleTest(&dev, SCREEN_WIDTH, SCREEN_HEIGHT);
		WAIT;

		RoundRectTest(&dev, SCREEN_WIDTH, SCREEN_HEIGHT);
		WAIT;

		HorizontalTest(&dev, fx16, SCREEN_WIDTH, SCREEN_HEIGHT);
		WAIT;

		VerticalTest(&dev, fx16, SCREEN_WIDTH, SCREEN_HEIGHT);
		WAIT;

		FillRectTest(&dev, SCREEN_WIDTH, SCREEN_HEIGHT);
		WAIT;

		ColorTest(&dev, SCREEN_WIDTH, SCREEN_HEIGHT);
		WAIT;

		// Multi Font Test
		uint16_t color;
		uint8_t ascii[20];
		uint16_t xpos;
		lcdFillScreen(&dev, BLACK);
		color = WHITE;
		lcdSetFontDirection(&dev, 1);
		xpos = SCREEN_WIDTH-16;
		strcpy((char *)ascii, "16Dot Font");
		lcdDrawString(&dev, fx16, xpos, 0, ascii, color);

		xpos = xpos - 24;
		strcpy((char *)ascii, "24Dot Font");
		lcdDrawString(&dev, fx24, xpos, 0, ascii, color);

		xpos = xpos - 32;
		strcpy((char *)ascii, "32Dot Font");
		lcdDrawString(&dev, fx32, xpos, 0, ascii, color);
		WAIT;

		// Brightness control
		lcdFillScreen(&dev, WHITE);
		color = BLACK;
		lcdSetFontDirection(&dev, 1);
		xpos = SCREEN_WIDTH-(24 * 2);
		strcpy((char *)ascii, " Push Button");
		lcdDrawString(&dev, fx24, xpos, 0, ascii, color);

		int Brightness = 15;
		while(1) {
			int level = gpio_get_level(GPIO_NUM_37);
			if (level == 0) {
				ESP_LOGI(TAG, "Push Button Brightness=%d",Brightness);
				Brightness--;
				if (Brightness == 5) break;
				AXP192_ScreenBreath(Brightness);

				while(1) {
					level = gpio_get_level(GPIO_NUM_37);
					if (level == 1) break;
					vTaskDelay(1);
				}
			}
			vTaskDelay(1);
		}

		ESP_LOGI(TAG, "Mainloop End");
		vTaskDelay(INTERVAL);
	} // end while
	
	// nerver reach
	while (1) {
		vTaskDelay(2000 / portTICK_PERIOD_MS);
	}
}


void app_main(void)
{
	ESP_LOGI(TAG, "Initializing SPIFFS");

	esp_vfs_spiffs_conf_t conf = {
		.base_path = "/spiffs",
		.partition_label = NULL,
		.max_files = 6,
		.format_if_mount_failed =true
	};

	// Use settings defined above toinitialize and mount SPIFFS filesystem.
	// Note: esp_vfs_spiffs_register is anall-in-one convenience function.
	esp_err_t ret =esp_vfs_spiffs_register(&conf);

	if (ret != ESP_OK) {
		if (ret ==ESP_FAIL) {
			ESP_LOGE(TAG, "Failed to mount or format filesystem");
		} else if (ret== ESP_ERR_NOT_FOUND) {
			ESP_LOGE(TAG, "Failed to find SPIFFS partition");
		} else {
			ESP_LOGE(TAG, "Failed to initialize SPIFFS (%s)",esp_err_to_name(ret));
		}
		return;
	}

	size_t total = 0, used = 0;
	ret = esp_spiffs_info(NULL, &total,&used);
	if (ret != ESP_OK) {
		ESP_LOGE(TAG,"Failed to get SPIFFS partition information (%s)",esp_err_to_name(ret));
	} else {
		ESP_LOGI(TAG,"Partition size: total: %d, used: %d", total, used);
	}

	SPIFFS_Directory("/spiffs");

	// power on
	i2c_master_init();
	AXP192_PowerOn();

	// set the GPIO as a input
	gpio_pad_select_gpio(GPIO_NUM_37);
	gpio_set_direction(GPIO_NUM_37, GPIO_MODE_DEF_INPUT);

	xTaskCreate(tft, "TFT", 4096, NULL, 2, NULL);
}

