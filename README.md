# esp-idf-m5stickC

Example code for driving M5StickC ST7735S display using ESP-IDF's SPI driver.

```
git clone https://github.com/nopnop2002/esp-idf-m5stickC
cd esp-idf-m5stickC/
make menuconfig
make flash
```

\*There is no MENU ITEM where this application is peculiar.   

# Font File   
You can add your font file.   
The format of the font file is the FONTX format.   
Your font file is put in font directory.   
Your font file is uploaded to SPIFFS partition using meke flash.   

Please refer this page about FONTX format.   
http://elm-chan.org/docs/dosv/fontx_e.html

