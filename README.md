# esp-idf-m5stickC

Example code for driving M5StickC ST7735S display using ESP-IDF's SPI driver.

![M5StickC-2](https://user-images.githubusercontent.com/6020549/57962798-88284680-7956-11e9-862d-365667c9941d.JPG)

# Software requirements
esp-idf v4.4/v5.0.   

# How to build
```
git clone https://github.com/nopnop2002/esp-idf-m5stickC
cd esp-idf-m5stickC/
idf.py set-target esp32
idf.py menuconfig
idf.py flash -b 115200 monitor
```

\*There is no MENU ITEM where this application is peculiar.   

__You need to specify Baud rate for flashing.__   

# Graphic support
![M5StickC-3](https://user-images.githubusercontent.com/6020549/57962800-8c546400-7956-11e9-876d-795b6fe6d9e6.JPG)

![M5StickC-4](https://user-images.githubusercontent.com/6020549/57962802-8f4f5480-7956-11e9-806a-9e3c0bb7f98f.JPG)

![M5StickC-5](https://user-images.githubusercontent.com/6020549/57962803-924a4500-7956-11e9-8b30-44bfe19d136d.JPG)

![M5StickC-8](https://user-images.githubusercontent.com/6020549/57962809-9f673400-7956-11e9-998e-f2b26f3ded44.JPG)

# Fonts support
It's possible to text rotation and invert.   
![M5StickC-6](https://user-images.githubusercontent.com/6020549/57962805-96766280-7956-11e9-92a8-8778e62b7ef5.JPG)

![M5StickC-7](https://user-images.githubusercontent.com/6020549/57962808-9b3b1680-7956-11e9-8f7d-487c17d43dbf.JPG)

It's possible to indicate more than one font at the same time.   
Gothic Font.   
![M5StickC-9](https://user-images.githubusercontent.com/6020549/57962812-a2fabb00-7956-11e9-989f-b0fa90523c01.JPG)

Mincyo Font.   
![M5StickC-10](https://user-images.githubusercontent.com/6020549/57962814-a68e4200-7956-11e9-95ff-c735b336036d.JPG)


# Font File   
You can add your original font file.   
The format of the font file is the FONTX format.   
Your font file is put in font directory.   
Your font file is uploaded to SPIFFS partition using meke flash.   

Please refer [this](http://elm-chan.org/docs/dosv/fontx_e.html) page about FONTX format.   

```
FontxFile yourFont[2];
InitFontx(yourFont,"/spiffs/your_font_file_name","");
uint8_t ascii[10];
strcpy((char *)ascii, "MyFont");
lcdDrawString(dev, yourFont, x, y, ascii, color);
```

