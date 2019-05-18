# esp-idf-m5stickC

Example code for driving M5StickC ST7735S display using ESP-IDF's SPI driver.

```
git clone https://github.com/nopnop2002/esp-idf-m5stickC
cd esp-idf-m5stickC/
make menuconfig
make flash
```

\*There is no MENU ITEM where this application is peculiar.   

![M5StickC-5](https://user-images.githubusercontent.com/6020549/57962134-d33e5b80-794e-11e9-95ff-649e00bb23e5.JPG)

---

# Font File   
You can add your original font file.   
The format of the font file is the FONTX format.   
Your font file is put in font directory.   
Your font file is uploaded to SPIFFS partition using meke flash.   

Please refer this page about FONTX format.   
http://elm-chan.org/docs/dosv/fontx_e.html

```
FontxFile yourFont[2];
InitFontx(yourFont,"/spiffs/your_font_file_name","");
lcdDrawString(dev, yourFont, x, y, ascii, color);
```

It's possible to text rotation.   
![M5StickC-3](https://user-images.githubusercontent.com/6020549/57962123-c883c680-794e-11e9-9598-94427df84ea1.JPG)

![M5StickC-4](https://user-images.githubusercontent.com/6020549/57962130-ce79a780-794e-11e9-961a-e195eb42a723.JPG)

It's possible to indicate more than one font at the same time.   
Gothic Font.   
![M5StickC-5](https://user-images.githubusercontent.com/6020549/57962150-eea96680-794e-11e9-895f-26ec16ca24e8.JPG)
Mincyo Font.   
![M5StickC-7](https://user-images.githubusercontent.com/6020549/57962334-7b085900-7950-11e9-8892-6189a705f150.JPG)

