#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <inttypes.h>

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
#include "bmpfile.h"
#include "decode_jpeg.h"
#include "decode_png.h"
#include "pngle.h"

// M5stickC stuff
#define SCREEN_WIDTH 80
#define SCREEN_HEIGHT 160
#define OFFSET_X 26
#define OFFSET_Y 1
#define GPIO_MOSI 15
#define GPIO_SCLK 13
#define GPIO_CS 5
#define GPIO_DC 23
#define GPIO_RESET 18
#define FRAME_BUFFER true

#define INTERVAL 400

#define WAIT vTaskDelay(INTERVAL)

//If you want to continue by push button, enable this line.
//#define WAIT waitButton()

static const char *TAG = "M5StickC";

#if 0
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
#endif

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
	lcdDrawFinish(dev);
	vTaskDelay(50);
	lcdFillScreen(dev, GREEN);
	lcdDrawFinish(dev);
	vTaskDelay(50);
	lcdFillScreen(dev, BLUE);
	lcdDrawFinish(dev);

	endTick = xTaskGetTickCount();
	diffTick = endTick - startTick;
	ESP_LOGI(__FUNCTION__, "elapsed time[ms]:%"PRIu32,diffTick*portTICK_PERIOD_MS);
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
	lcdDrawFinish(dev);

	endTick = xTaskGetTickCount();
	diffTick = endTick - startTick;
	ESP_LOGI(__FUNCTION__, "elapsed time[ms]:%"PRIu32,diffTick*portTICK_PERIOD_MS);
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
	ESP_LOGD(TAG,"fontWidth=%d fontHeight=%d",fontWidth,fontHeight);

	uint16_t color;
	//lcdFillScreen(dev, WHITE);
	lcdFillScreen(dev, BLACK);
	lcdSetFontDirection(dev, 0);
	color = RED;
	uint8_t ascii[10];
	lcdDrawArrow(dev, 10, 10, 0, 0, 5, color);
	strcpy((char *)ascii, "0,0");
	lcdDrawString(dev, fx, 0, 30, ascii, color);

	color = GREEN;
	lcdDrawArrow(dev, 69, 10, 79, 0, 5, color);
	strcpy((char *)ascii, "79,0");
	lcdDrawString(dev, fx, 45, 30, ascii, color);

	color = GRAY;
	lcdDrawArrow(dev, 10, 149, 0, 159, 5, color);
	strcpy((char *)ascii, "0,159");
	lcdDrawString(dev, fx, 0, 140, ascii, color);

	color = BLUE;
	lcdDrawArrow(dev, 69, 149, 79, 159, 5, color);
	strcpy((char *)ascii, "79,159");
	lcdDrawString(dev, fx, 30, 140-fontHeight, ascii, color);
	lcdDrawFinish(dev);

	endTick = xTaskGetTickCount();
	diffTick = endTick - startTick;
	ESP_LOGI(__FUNCTION__, "elapsed time[ms]:%"PRIu32,diffTick*portTICK_PERIOD_MS);
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
	ESP_LOGD(TAG,"fontWidth=%d fontHeight=%d",fontWidth,fontHeight);

	uint16_t color;
	//lcdFillScreen(dev, WHITE);
	lcdFillScreen(dev, BLACK);
	uint8_t ascii[20];
	strcpy((char *)ascii, " M5StickC ");

	color = RED;
	lcdSetFontDirection(dev, 0);
	lcdDrawString(dev, fx, 0, fontHeight*1-1, ascii, color);
	lcdSetFontUnderLine(dev, RED);
	lcdDrawString(dev, fx, 0, fontHeight*2-1, ascii, color);
	lcdUnsetFontUnderLine(dev);

	lcdSetFontFill(dev, GREEN);
	lcdDrawString(dev, fx, 0, fontHeight*3-1, ascii, color);
	lcdSetFontUnderLine(dev, RED);
	lcdDrawString(dev, fx, 0, fontHeight*4-1, ascii, color);
	lcdUnsetFontFill(dev);
	lcdUnsetFontUnderLine(dev);

	color = BLUE;
	lcdSetFontDirection(dev, 2);
	lcdDrawString(dev, fx, width-1, height-(fontHeight*1)-1, ascii, color);
	lcdSetFontUnderLine(dev, BLUE);
	lcdDrawString(dev, fx, width-1, height-(fontHeight*2)-1, ascii, color);
	lcdUnsetFontUnderLine(dev);

	lcdSetFontFill(dev, YELLOW);
	lcdDrawString(dev, fx, width-1, height-(fontHeight*3)-1, ascii, color);
	lcdSetFontUnderLine(dev, BLUE);
	lcdDrawString(dev, fx, width-1, height-(fontHeight*4)-1, ascii, color);
	lcdDrawFinish(dev);
	lcdUnsetFontFill(dev);
	lcdUnsetFontUnderLine(dev);

	endTick = xTaskGetTickCount();
	diffTick = endTick - startTick;
	ESP_LOGI(__FUNCTION__, "elapsed time[ms]:%"PRIu32,diffTick*portTICK_PERIOD_MS);
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
	ESP_LOGD(TAG,"fontWidth=%d fontHeight=%d",fontWidth,fontHeight);

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
	lcdDrawString(dev, fx, (fontHeight*1)-1, height-1, ascii, color);
	lcdSetFontUnderLine(dev, BLUE);
	lcdDrawString(dev, fx, (fontHeight*2)-1, height-1, ascii, color);
	lcdUnsetFontUnderLine(dev);

	lcdSetFontFill(dev, YELLOW);
	lcdDrawString(dev, fx, (fontHeight*3)-1, height-1, ascii, color);
	lcdSetFontUnderLine(dev, BLUE);
	lcdDrawString(dev, fx, (fontHeight*4)-1, height-1, ascii, color);
	lcdDrawFinish(dev);
	lcdUnsetFontFill(dev);
	lcdUnsetFontUnderLine(dev);

	endTick = xTaskGetTickCount();
	diffTick = endTick - startTick;
	ESP_LOGI(__FUNCTION__, "elapsed time[ms]:%"PRIu32,diffTick*portTICK_PERIOD_MS);
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
	lcdDrawFinish(dev);

	endTick = xTaskGetTickCount();
	diffTick = endTick - startTick;
	ESP_LOGI(__FUNCTION__, "elapsed time[ms]:%"PRIu32,diffTick*portTICK_PERIOD_MS);
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
	lcdDrawFinish(dev);

	endTick = xTaskGetTickCount();
	diffTick = endTick - startTick;
	ESP_LOGI(__FUNCTION__, "elapsed time[ms]:%"PRIu32,diffTick*portTICK_PERIOD_MS);
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
	lcdDrawFinish(dev);

	endTick = xTaskGetTickCount();
	diffTick = endTick - startTick;
	ESP_LOGI(__FUNCTION__, "elapsed time[ms]:%"PRIu32,diffTick*portTICK_PERIOD_MS);
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
		color=rgb565(red, green, blue);
		uint16_t xpos=rand()%width;
		uint16_t ypos=rand()%height;
		uint16_t size=rand()%(width/5);
		lcdDrawFillRect(dev, xpos, ypos, xpos+size, ypos+size, color);
	}
	lcdDrawFinish(dev);

	endTick = xTaskGetTickCount();
	diffTick = endTick - startTick;
	ESP_LOGI(__FUNCTION__, "elapsed time[ms]:%"PRIu32,diffTick*portTICK_PERIOD_MS);
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
	lcdDrawFinish(dev);

	endTick = xTaskGetTickCount();
	diffTick = endTick - startTick;
	ESP_LOGI(__FUNCTION__, "elapsed time[ms]:%"PRIu32,diffTick*portTICK_PERIOD_MS);
	return diffTick;
}

TickType_t BMPTest(ST7735_t * dev, char * file, int width, int height) {
	TickType_t startTick, endTick, diffTick;
	startTick = xTaskGetTickCount();

	lcdSetFontDirection(dev, 0);
	lcdFillScreen(dev, BLACK);

	// open requested file
	esp_err_t ret;
	FILE* fp = fopen(file, "rb");
	if (fp == NULL) {
		ESP_LOGW(__FUNCTION__, "File not found [%s]", file);
		return 0;
	}

	// read bmp header
	bmpfile_t *result = (bmpfile_t*)malloc(sizeof(bmpfile_t));
	if (result == NULL) {
		ESP_LOGE(__FUNCTION__, "malloc fail");
		return 0;
	}
	ret = fread(result->header.magic, 1, 2, fp);
	assert(ret == 2);
	if (result->header.magic[0]!='B' || result->header.magic[1] != 'M') {
		ESP_LOGW(__FUNCTION__, "File is not BMP");
		free(result);
		fclose(fp);
		return 0;
	}
	ret = fread(&result->header.filesz, 4, 1 , fp);
	assert(ret == 1);
	ESP_LOGD(__FUNCTION__,"result->header.filesz=%"PRIu32, result->header.filesz);
	ret = fread(&result->header.creator1, 2, 1, fp);
	assert(ret == 1);
	ret = fread(&result->header.creator2, 2, 1, fp);
	assert(ret == 1);
	ret = fread(&result->header.offset, 4, 1, fp);
	assert(ret == 1);

	// read dib header
	ret = fread(&result->dib.header_sz, 4, 1, fp);
	assert(ret == 1);
	ret = fread(&result->dib.width, 4, 1, fp);
	assert(ret == 1);
	ret = fread(&result->dib.height, 4, 1, fp);
	assert(ret == 1);
	ret = fread(&result->dib.nplanes, 2, 1, fp);
	assert(ret == 1);
	ret = fread(&result->dib.depth, 2, 1, fp);
	assert(ret == 1);
	ret = fread(&result->dib.compress_type, 4, 1, fp);
	assert(ret == 1);
	ret = fread(&result->dib.bmp_bytesz, 4, 1, fp);
	assert(ret == 1);
	ret = fread(&result->dib.hres, 4, 1, fp);
	assert(ret == 1);
	ret = fread(&result->dib.vres, 4, 1, fp);
	assert(ret == 1);
	ret = fread(&result->dib.ncolors, 4, 1, fp);
	assert(ret == 1);
	ret = fread(&result->dib.nimpcolors, 4, 1, fp);
	assert(ret == 1);

	if((result->dib.depth == 24) && (result->dib.compress_type == 0)) {
		// BMP rows are padded (if needed) to 4-byte boundary
		uint32_t rowSize = (result->dib.width * 3 + 3) & ~3;
		int w = result->dib.width;
		int h = result->dib.height;
		ESP_LOGD(__FUNCTION__,"w=%d h=%d", w, h);
		int _x;
		int _w;
		int _cols;
		int _cole;
		if (width >= w) {
			_x = (width - w) / 2;
			_w = w;
			_cols = 0;
			_cole = w - 1;
		} else {
			_x = 0;
			_w = width;
			_cols = (w - width) / 2;
			_cole = _cols + width - 1;
		}
		ESP_LOGD(__FUNCTION__,"_x=%d _w=%d _cols=%d _cole=%d",_x, _w, _cols, _cole);

		int _y;
		int _rows;
		int _rowe;
		if (height >= h) {
			_y = (height - h) / 2;
			_rows = 0;
			_rowe = h -1;
		} else {
			_y = 0;
			_rows = (h - height) / 2;
			_rowe = _rows + height - 1;
		}
		ESP_LOGD(__FUNCTION__,"_y=%d _rows=%d _rowe=%d", _y, _rows, _rowe);

#define BUFFPIXEL 20
		uint8_t sdbuffer[3*BUFFPIXEL]; // pixel buffer (R+G+B per pixel)
		uint16_t *colors = (uint16_t*)malloc(sizeof(uint16_t) * w);
		if (colors == NULL) {
			ESP_LOGE(__FUNCTION__, "malloc fail");
			free(result);
			fclose(fp);
			return 0;
		}

		for (int row=0; row<h; row++) { // For each scanline...
			if (row < _rows || row > _rowe) continue;
			// Seek to start of scan line.	It might seem labor-
			// intensive to be doing this on every line, but this
			// method covers a lot of gritty details like cropping
			// and scanline padding.  Also, the seek only takes
			// place if the file position actually needs to change
			// (avoids a lot of cluster math in SD library).
			// Bitmap is stored bottom-to-top order (normal BMP)
			int pos = result->header.offset + (h - 1 - row) * rowSize;
			fseek(fp, pos, SEEK_SET);
			int buffidx = sizeof(sdbuffer); // Force buffer reload

			int index = 0;
			for (int col=0; col<w; col++) { // For each pixel...
				if (buffidx >= sizeof(sdbuffer)) { // Indeed
					fread(sdbuffer, sizeof(sdbuffer), 1, fp);
					buffidx = 0; // Set index to beginning
				}
				if (col < _cols || col > _cole) continue;
				// Convert pixel from BMP to TFT format, push to display
				uint8_t b = sdbuffer[buffidx++];
				uint8_t g = sdbuffer[buffidx++];
				uint8_t r = sdbuffer[buffidx++];
				colors[index++] = rgb565(r, g, b);
			} // end for col
			ESP_LOGD(__FUNCTION__,"lcdDrawMultiPixels _x=%d _y=%d row=%d",_x, _y, row);
			//lcdDrawMultiPixels(dev, _x, row+_y, _w, colors);
			lcdDrawMultiPixels(dev, _x, _y, _w, colors);
			_y++;
		} // end for row
		free(colors);
	} // end if
	lcdDrawFinish(dev);
	free(result);
	fclose(fp);

	endTick = xTaskGetTickCount();
	diffTick = endTick - startTick;
	ESP_LOGI(__FUNCTION__, "elapsed time[ms]:%"PRIu32,diffTick*portTICK_PERIOD_MS);
	return diffTick;
}

TickType_t JPEGTest(ST7735_t * dev, char * file, int width, int height) {
	TickType_t startTick, endTick, diffTick;
	startTick = xTaskGetTickCount();

	lcdSetFontDirection(dev, 0);
	lcdFillScreen(dev, BLACK);

	pixel_jpeg **pixels;
	int imageWidth;
	int imageHeight;
	esp_err_t err = decode_jpeg(&pixels, file, width, height, &imageWidth, &imageHeight);
	ESP_LOGI(__FUNCTION__, "decode_image err=%d imageWidth=%d imageHeight=%d", err, imageWidth, imageHeight);
	if (err == ESP_OK) {
		ESP_LOGD(__FUNCTION__, "imageWidth=%d imageHeight=%d", imageWidth, imageHeight);

		uint16_t jpegWidth = width;
		uint16_t offsetX = 0;
		if (width > imageWidth) {
			jpegWidth = imageWidth;
			offsetX = (width - imageWidth) / 2;
		}
		ESP_LOGD(__FUNCTION__, "jpegWidth=%d offsetX=%d", jpegWidth, offsetX);

		uint16_t jpegHeight = height;
		uint16_t offsetY = 0;
		if (height > imageHeight) {
			jpegHeight = imageHeight;
			offsetY = (height - imageHeight) / 2;
		}
		ESP_LOGD(__FUNCTION__, "jpegHeight=%d offsetY=%d", jpegHeight, offsetY);
		uint16_t *colors = (uint16_t*)malloc(sizeof(uint16_t) * jpegWidth);
		if (colors == NULL) {
			ESP_LOGE(__FUNCTION__, "malloc fail");
			release_image(&pixels, width, height);
			return 0;
		}

#if 0
		for(int y = 0; y < jpegHeight; y++){
			for(int x = 0;x < jpegWidth; x++){
				pixel_s pixel = pixels[y][x];
				uint16_t color = rgb565(pixel.red, pixel.green, pixel.blue);
				lcdDrawPixel(dev, x+offsetX, y+offsetY, color);
			}
			vTaskDelay(1);
		}
#endif

		for(int y = 0; y < jpegHeight; y++){
			for(int x = 0;x < jpegWidth; x++){
				//pixel_s pixel = pixels[y][x];
				//colors[x] = rgb565(pixel.red, pixel.green, pixel.blue);
				colors[x] = pixels[y][x];
			}
			lcdDrawMultiPixels(dev, offsetX, y+offsetY, jpegWidth, colors);
			vTaskDelay(1);
		}

		lcdDrawFinish(dev);
		free(colors);
		release_image(&pixels, width, height);
		ESP_LOGD(__FUNCTION__, "Finish");
	} else {
		ESP_LOGE(__FUNCTION__, "decode_image err=%d", err);
	}

	endTick = xTaskGetTickCount();
	diffTick = endTick - startTick;
	ESP_LOGI(__FUNCTION__, "elapsed time[ms]:%"PRIu32,diffTick*portTICK_PERIOD_MS);
	return diffTick;
}

TickType_t PNGTest(ST7735_t * dev, char * file, int width, int height) {
	TickType_t startTick, endTick, diffTick;
	startTick = xTaskGetTickCount();

	lcdSetFontDirection(dev, 0);
	lcdFillScreen(dev, BLACK);

	// open PNG file
	FILE* fp = fopen(file, "rb");
	if (fp == NULL) {
		ESP_LOGW(__FUNCTION__, "File not found [%s]", file);
		return 0;
	}

	char buf[1024];
	size_t remain = 0;
	int len;

	pngle_t *pngle = pngle_new(width, height);
	ESP_LOGD(__FUNCTION__, "pngle=%p", pngle);
	if (pngle == NULL) {
		fclose(fp);
		return 0;
	}

	pngle_set_init_callback(pngle, png_init);
	pngle_set_draw_callback(pngle, png_draw);
	pngle_set_done_callback(pngle, png_finish);

	double display_gamma = 2.2;
	pngle_set_display_gamma(pngle, display_gamma);


	while (!feof(fp)) {
		if (remain >= sizeof(buf)) {
			ESP_LOGE(__FUNCTION__, "Buffer exceeded");
			while(1) vTaskDelay(1);
		}

		len = fread(buf + remain, 1, sizeof(buf) - remain, fp);
		if (len <= 0) {
			//printf("EOF\n");
			break;
		}

		int fed = pngle_feed(pngle, buf, remain + len);
		if (fed < 0) {
			ESP_LOGE(__FUNCTION__, "ERROR; %s", pngle_error(pngle));
			pngle_destroy(pngle, width, height);
			return 0;
		}

		remain = remain + len - fed;
		if (remain > 0) memmove(buf, buf + fed, remain);
	}
	fclose(fp);

	uint16_t pngWidth = width;
	uint16_t offsetX = 0;
	if (width > pngle->imageWidth) {
		pngWidth = pngle->imageWidth;
		offsetX = (width - pngle->imageWidth) / 2;
	}
	ESP_LOGD(__FUNCTION__, "pngWidth=%d offsetX=%d", pngWidth, offsetX);

	uint16_t pngHeight = height;
	uint16_t offsetY = 0;
	if (height > pngle->imageHeight) {
		pngHeight = pngle->imageHeight;
		offsetY = (height - pngle->imageHeight) / 2;
	}
	ESP_LOGD(__FUNCTION__, "pngHeight=%d offsetY=%d", pngHeight, offsetY);
	uint16_t *colors = (uint16_t*)malloc(sizeof(uint16_t) * pngWidth);
	if (colors == NULL) {
		ESP_LOGE(__FUNCTION__, "malloc fail");
		pngle_destroy(pngle, width, height);
		return 0;
	}

#if 0
	for(int y = 0; y < pngHeight; y++){
		for(int x = 0;x < pngWidth; x++){
			pixel_png pixel = pngle->pixels[y][x];
			uint16_t color = rgb565(pixel.red, pixel.green, pixel.blue);
			lcdDrawPixel(dev, x+offsetX, y+offsetY, color);
		}
	}
#endif

	for(int y = 0; y < pngHeight; y++){
		for(int x = 0;x < pngWidth; x++){
			//pixel_png pixel = pngle->pixels[y][x];
			//colors[x] = rgb565(pixel.red, pixel.green, pixel.blue);
			colors[x] = pngle->pixels[y][x];
		}
		lcdDrawMultiPixels(dev, offsetX, y+offsetY, pngWidth, colors);
		vTaskDelay(1);
	}
	lcdDrawFinish(dev);
	free(colors);
	pngle_destroy(pngle, width, height);

	endTick = xTaskGetTickCount();
	diffTick = endTick - startTick;
	ESP_LOGI(__FUNCTION__, "elapsed time[ms]:%"PRIu32,diffTick*portTICK_PERIOD_MS);
	return diffTick;
}

TickType_t CodeTest(ST7735_t * dev, FontxFile *fx, int width, int height, uint16_t start, uint16_t end) {
	TickType_t startTick, endTick, diffTick;
	startTick = xTaskGetTickCount();

	// get font width & height
	uint8_t buffer[FontxGlyphBufSize];
	uint8_t fontWidth;
	uint8_t fontHeight;
	GetFontx(fx, 0, buffer, &fontWidth, &fontHeight);
	//ESP_LOGI(__FUNCTION__,"width=%d height=%d",width,height);
	//ESP_LOGI(__FUNCTION__,"fontWidth=%d fontHeight=%d",fontWidth,fontHeight);
	//uint8_t xmoji = width / fontWidth;
	//uint8_t ymoji = height / fontHeight;
	uint8_t ymoji = width / fontWidth;
	uint8_t xmoji = height / fontHeight;
	//ESP_LOGI(__FUNCTION__,"xmoji=%d ymoji=%d",xmoji, ymoji);

	uint16_t color;
	lcdFillScreen(dev, BLACK);
	uint8_t code;

	color = CYAN;
	//lcdSetFontDirection(dev, 0);
	lcdSetFontDirection(dev, 1);
	code = start;
	for(int y=0;y<ymoji;y++) {
		uint16_t xpos = width - fontHeight*(y+1)-1;;
		uint16_t ypos = 0;
		if (fontWidth*xmoji < height) {
			ypos = ((height - (fontWidth*xmoji)) / 2) - 1;
		}
		for(int x=0;x<xmoji;x++) {
			//ESP_LOGI(__FUNCTION__,"xpos=%d ypos=%d", xpos, ypos);
			ypos = lcdDrawCode(dev, fx, xpos, ypos, code, color);
			if (code == 0xFF) break;
			code++;
			if (code > end) break;
		}
		if (code == 0xFF) break;
		if (code > end) break;
	}
	lcdDrawFinish(dev);

	endTick = xTaskGetTickCount();
	diffTick = endTick - startTick;
	ESP_LOGI(__FUNCTION__, "elapsed time[ms]:%"PRIu32,diffTick*portTICK_PERIOD_MS);
	return diffTick;
}


TickType_t ScrollTest(ST7735_t * dev, FontxFile *fx, int width, int height) {
	TickType_t startTick, endTick, diffTick;
	startTick = xTaskGetTickCount();

	// get font width & height
	uint8_t buffer[FontxGlyphBufSize];
	uint8_t fontWidth;
	uint8_t fontHeight;
	GetFontx(fx, 0, buffer, &fontWidth, &fontHeight);
	ESP_LOGD(TAG,"fontWidth=%d fontHeight=%d",fontWidth,fontHeight);

	uint16_t color;
	uint8_t ascii[30];

	typedef struct {
		bool enable;
		uint16_t color;;
		char line[30];
	} SAVE_t;


	int lines = (height - fontHeight) / fontHeight;
	ESP_LOGD(TAG, "height=%d fontHeight=%d lines=%d", height, fontHeight, lines);
	SAVE_t save[10];
	for(int i=0;i<lines;i++) {
		save[i].enable = false;
	}

	lcdSetFontDirection(dev, 0);
	lcdFillScreen(dev, BLACK);

	strcpy((char *)ascii, "Scroll");
	lcdDrawString(dev, fx, 0, fontHeight-1, ascii, RED);

	color = CYAN;
	for(int i=0;i<20;i++) {
		sprintf((char *)ascii, "Line %d", i);
		int last = -1;
		bool redraw = false;
		for(int j=(lines-1);j>=0;j--) {
			if (save[j].enable == false) last = j;
		}
		if (last == -1) {
			last = lines-1;
			redraw = true;
			for(int j=0;j<lines-1;j++) {
				save[j].enable = save[j+1].enable;
				save[j].color = save[j+1].color;
				strcpy(save[j].line, save[j+1].line);
			}
		}
		save[last].enable = true;
		save[last].color = color;
		strcpy(save[last].line,  (char*)ascii);
		
		if (redraw) {
			//lcdDrawFillRect(dev, 0, fontHeight, width-1, height-1, BLACK);
			for(int j=0;j<lines;j++) {
				ESP_LOGD(TAG, "enable[%d]=%d",j, save[j].enable);
				lcdDrawFillRect(dev, 0, fontHeight*(j+1), width-1, fontHeight*(j+2)-1, BLACK);
				lcdDrawString(dev, fx, 0, fontHeight*(j+2)-1, (uint8_t *)save[j].line, save[j].color);
			}
		} else {
			lcdDrawString(dev, fx, 0, fontHeight*(last+2)-1, (uint8_t *)save[last].line, save[last].color);
		}
		lcdDrawFinish(dev);
		vTaskDelay(25);
	}

	endTick = xTaskGetTickCount();
	diffTick = endTick - startTick;
	ESP_LOGI(__FUNCTION__, "elapsed time[ms]:%"PRIu32,diffTick*portTICK_PERIOD_MS);
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

	FontxFile fx32E[2];
	InitFontx(fx32E,"/spiffs/emoticons21.fnt",""); // 24x24Dot Smile

	FontxFile fx32S[2];
	InitFontx(fx32S,"/spiffs/Scroll-o-Sprites.fnt",""); // 16x16Dot Emoji

	ST7735_t dev;
	spi_master_init(&dev, GPIO_MOSI, GPIO_SCLK, GPIO_CS, GPIO_DC, GPIO_RESET);
	lcdInit(&dev, SCREEN_WIDTH, SCREEN_HEIGHT, OFFSET_X, OFFSET_Y, FRAME_BUFFER);

#if 0
	//For TEST
	while(1) {
		FillTest(&dev, SCREEN_WIDTH, SCREEN_HEIGHT);
		WAIT;
		ArrowTest(&dev, fx16, SCREEN_WIDTH, SCREEN_HEIGHT);
		WAIT;
		CodeTest(&dev, fx32E, SCREEN_WIDTH, SCREEN_HEIGHT, 0x20, 0x37);
		WAIT;
		CodeTest(&dev, fx32S, SCREEN_WIDTH, SCREEN_HEIGHT, 0x00, 0x1F);
		WAIT;
	}
#endif

	while (1) {
		ESP_LOGD(TAG, "Mainloop Start");
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

		ScrollTest(&dev, fx16, SCREEN_WIDTH, SCREEN_HEIGHT);
		WAIT;

		char file[32];
		strcpy(file, "/spiffs/esp32_80.bmp");
		BMPTest(&dev, file, SCREEN_WIDTH, SCREEN_HEIGHT);
		WAIT;

		strcpy(file, "/spiffs/esp32.jpeg");
		JPEGTest(&dev, file, SCREEN_WIDTH, SCREEN_HEIGHT);
		WAIT;

		strcpy(file, "/spiffs/esp_logo.png");
		PNGTest(&dev, file, SCREEN_WIDTH, SCREEN_HEIGHT);
		WAIT;

		CodeTest(&dev, fx32E, SCREEN_WIDTH, SCREEN_HEIGHT, 0x20, 0x37);
		WAIT;

		CodeTest(&dev, fx32S, SCREEN_WIDTH, SCREEN_HEIGHT, 0x00, 0x1F);
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
		lcdDrawFinish(&dev);
		WAIT;

		// Brightness control
		lcdFillScreen(&dev, WHITE);
		color = BLACK;
		lcdSetFontDirection(&dev, 1);
		xpos = SCREEN_WIDTH-(24 * 2);
		strcpy((char *)ascii, " Fade Out...");
		lcdDrawString(&dev, fx24, xpos, 0, ascii, color);

		int Brightness = 15;
		while(1){
			AXP192_ScreenBreath(Brightness);
			vTaskDelay(50);
			Brightness--;
			if (Brightness == 5) break;
		}

#if 0
		while(1) {
			// wait until button is pressed
			int level = gpio_get_level(GPIO_NUM_37);
			if (level == 0) {
				ESP_LOGI(TAG, "Push Button Brightness=%d",Brightness);
				Brightness--;
				if (Brightness == 5) break;
				AXP192_ScreenBreath(Brightness);

				// wait until button is released
				while(1) {
					level = gpio_get_level(GPIO_NUM_37);
					if (level == 1) break;
					vTaskDelay(1);
				}
			}
			vTaskDelay(1);
		}
#endif
		ESP_LOGD(TAG, "Mainloop End");
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
		.max_files = 10,
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
	//gpio_pad_select_gpio(GPIO_NUM_37);
	gpio_reset_pin(GPIO_NUM_37);
	gpio_set_direction(GPIO_NUM_37, GPIO_MODE_DEF_INPUT);

	xTaskCreate(tft, "TFT", 4096, NULL, 2, NULL);
}

