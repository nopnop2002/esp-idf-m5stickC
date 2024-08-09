# esp-idf-m5stickC

Example code for driving M5StickC ST7735S display using ESP-IDF's SPI driver.

![M5StickC-2](https://user-images.githubusercontent.com/6020549/57962798-88284680-7956-11e9-862d-365667c9941d.JPG)

# Software requirements
esp-idf v4.4/v5.x.   

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
When you build the firmware, the font files are uploaded to the SPIFFS partition.   

Please refer [this](http://elm-chan.org/docs/dosv/fontx_e.html) page about FONTX format.   

# Font File Editor(FONTX Editor)   
[There](http://elm-chan.org/fsw/fontxedit.zip) is a font file editor.   
This can be done on Windows 10.   
Developer page is [here](http://elm-chan.org/fsw_e.html).   

![fontx-editor-1](https://github.com/user-attachments/assets/76a8c96f-74c3-4583-a4f1-5664f0e81f3a)

This project uses the following as default fonts:   
- font/ILGH16XB.FNT // 8x16Dot Gothic
- font/ILGH24XB.FNT // 12x24Dot Gothic
- font/ILGH32XB.FNT // 16x32Dot Gothic
- font/ILMH16XB.FNT // 8x16Dot Mincyo
- font/ILMH24XB.FNT // 12x24Dot Mincyo
- font/ILMH32XB.FNT // 16x32Dot Mincyo

# Convert from TTF font to FONTX font  
step1)   
Download WFONTX64.exe from [here](https://github.com/nemuisan/WFONTX64/releases).
Developer page is [here](https://github.com/nemuisan/WFONTX64).   

step2)   
Select ttf font.   
![WFONTX64-1](https://github.com/user-attachments/assets/2193a3c4-021c-48e6-8486-2ce500bdac36)

step3)   
Enter Height, Width, FontX2 name.   
Specify half of Height for Width.   
Specify your favorite font name in the FontX2 name field using up to 8 characters.   
![WFONTX64-2](https://github.com/user-attachments/assets/c87a9ec9-8e28-4d34-8475-60b15a47fb22)


step4)   
Specify the file name to save.   
![WFONTX64-3](https://github.com/user-attachments/assets/9715d4bf-e460-41a6-9a4b-38c0f10020f7)

step5)   
Specify the font style as required.   
![WFONTX64-4](https://github.com/user-attachments/assets/0ff3072d-6a78-48ae-b855-60c692f8d771)

step6)   
Press the RUN button to convert TTF fonts to FONTX format.   
![WFONTX64-5](https://github.com/user-attachments/assets/d9797e3d-1fd6-4504-b161-c1280f1242c0)

step7)   
upload your font file to $HOME/esp-idf-m5stickC/font directory.   

step8)   
add font to use   
```
    FontxFile fx16[2];
    FontxFile fx24[2];
    FontxFile fx32[2];
    //InitFontx(fx16,"/spiffs/ILGH16XB.FNT",""); // 8x16Dot Gothic
    //InitFontx(fx24,"/spiffs/ILGH24XB.FNT",""); // 12x24Dot Gothic
    //InitFontx(fx32,"/spiffs/ILGH32XB.FNT",""); // 16x32Dot Gothic

    //InitFontx(fx16,"/spiffs/ILMH16XB.FNT",""); // 8x16Dot Mincyo
    //InitFontx(fx24,"/spiffs/ILMH24XB.FNT",""); // 12x24Dot Mincyo
    //InitFontx(fx32,"/spiffs/ILMH32XB.FNT",""); // 16x32Dot Mincyo

    InitFontx(fx16,"/spiffs/Gigi16.FNT",""); // 8x16Dot Gigi
    InitFontx(fx24,"/spiffs/Gigi24.FNT",""); // 12x24Dot Gigi
    InitFontx(fx32,"/spiffs/Gigi32.FNT",""); // 16x32Dot Gigi
```

![TTF_FONT](https://github.com/user-attachments/assets/7b8c88d8-e347-48a8-b438-1779f2601230)

# How to add your color   
Change here.   
```
#define RED    rgb565(255,   0,   0) // 0xf800
#define GREEN  rgb565(  0, 255,   0) // 0x07e0
#define BLUE   rgb565(  0,   0, 255) // 0x001f
#define BLACK  rgb565(  0,   0,   0) // 0x0000
#define WHITE  rgb565(255, 255, 255) // 0xffff
#define GRAY   rgb565(128, 128, 128) // 0x8410
#define YELLOW rgb565(255, 255,   0) // 0xFFE0
#define CYAN   rgb565(  0, 156, 209) // 0x04FA
#define PURPLE rgb565(128,   0, 128) // 0x8010
```

# Reference   
https://github.com/nopnop2002/esp-idf-m5stickC-Plus
