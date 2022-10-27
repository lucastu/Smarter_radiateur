#define ENABLE_GxEPD2_GFX 0

#include <GxEPD2_BW.h>
#include <U8g2_for_Adafruit_GFX.h>
//Definition of the screen (from the exemple of GxEPD2)
#if defined(__AVR)
#define MAX_DISPLAY_BUFFER_SIZE 800
#define MAX_HEIGHT(EPD) (EPD::HEIGHT <= MAX_DISPLAY_BUFFER_SIZE / (EPD::WIDTH / 8) ? EPD::HEIGHT : MAX_DISPLAY_BUFFER_SIZE / (EPD::WIDTH / 8))
GxEPD2_BW<GxEPD2_213_T5D, MAX_HEIGHT(GxEPD2_213_T5D)> display(GxEPD2_213_T5D(/*CS=*/SS, /*DC=*/8, /*RST=*/9, /*BUSY=*/7));  // GDEW0213T5D 104x212, UC8151D
#endif

U8G2_FOR_ADAFRUIT_GFX u8g2Fonts;

void setup() {
  display.init();
  u8g2Fonts.begin(display);  // connect u8g2 procedures to Adafruit GFX7
  display.fillScreen(GxEPD_WHITE);
  //drawCornerTest();  //test from https://github.com/ZinggJM/GxEPD2/blob/master/examples/GxEPD2_Example/GxEPD2_Example.ino
  TestPartialUpdate1();
  TestPartialUpdate2();
}

void loop() {
delay(100000);
}


void TestPartialUpdate1() {
  display.setRotation(1);  // 0--> No rotation ,  1--> rotate 90 deg
  uint16_t bg = GxEPD_WHITE;
  uint16_t fg = GxEPD_BLACK;
  u8g2Fonts.setFontMode(1);                    // use u8g2 transparent mode (this is default)
  u8g2Fonts.setFontDirection(0);               // left to right (this is default)
  u8g2Fonts.setForegroundColor(fg);            // apply Adafruit GFX color
  u8g2Fonts.setBackgroundColor(bg);            // apply Adafruit GFX color
  u8g2Fonts.setFont(u8g2_font_logisoso32_tr);  //u8g2_font_logisoso32_tn--->numbers only to save memory ; u8g2_font_logisoso32_tr , u8g2_font_logisoso32_tf -->numbers&letters

  //display.setPartialWindow(0, 0, display.width(), display.height()); //this sets a window for the partial update, so the values can update without refreshing the entire screen.
  display.setPartialWindow(0, 0, display.width() / 2, display.height() / 2);
  //https://forum.arduino.cc/t/e-paper-partial-update-i-dont-know-how/527555/8
  //display.setPartialWindow(0, 90, 128, 25); // x,y,width,height
  display.firstPage();
  do {
    display.fillScreen(bg);
    // Display first line
    //u8g2Fonts.setCursor(0, 0);
    display.fillScreen(GxEPD_WHITE);
    display.fillRect(0, 0, display.width(), display.height(), GxEPD_BLACK);
  } while (display.nextPage());
}
void TestPartialUpdate2() {
  display.setRotation(1);  // 0--> No rotation ,  1--> rotate 90 deg
  uint16_t bg = GxEPD_WHITE;
  uint16_t fg = GxEPD_BLACK;
  u8g2Fonts.setFontMode(1);                    // use u8g2 transparent mode (this is default)
  u8g2Fonts.setFontDirection(0);               // left to right (this is default)
  u8g2Fonts.setForegroundColor(fg);            // apply Adafruit GFX color
  u8g2Fonts.setBackgroundColor(bg);            // apply Adafruit GFX color
  u8g2Fonts.setFont(u8g2_font_logisoso32_tr);  //u8g2_font_logisoso32_tn--->numbers only to save memory ; u8g2_font_logisoso32_tr , u8g2_font_logisoso32_tf -->numbers&letters

  //display.setPartialWindow(0, 0, display.width(), display.height()); //this sets a window for the partial update, so the values can update without refreshing the entire screen.
  byte largeur = 50;
  display.setPartialWindow(display.width()/2-largeur/2, 0, largeur, display.height() / 2);
  //https://forum.arduino.cc/t/e-paper-partial-update-i-dont-know-how/527555/8
  //display.setPartialWindow(0, 90, 128, 25); // x,y,width,height
  display.firstPage();
  do {
    display.fillScreen(bg);
    // Display first line
    //u8g2Fonts.setCursor(0, 0);
    display.fillScreen(GxEPD_WHITE);
    display.fillRect(0, 0, display.width(), display.height(), GxEPD_BLACK);
  } while (display.nextPage());
}
void drawCornerTest()  //test fonction to be deleted
{
  display.setRotation(1);  // 0--> No rotation ,  1--> rotate 90 deg
  uint16_t bg = GxEPD_WHITE;
  uint16_t fg = GxEPD_BLACK;
  u8g2Fonts.setFontMode(1);          // use u8g2 transparent mode (this is default)
  u8g2Fonts.setFontDirection(0);     // left to right (this is default)
  u8g2Fonts.setForegroundColor(fg);  // apply Adafruit GFX color
  u8g2Fonts.setBackgroundColor(bg);  // apply Adafruit GFX color
  u8g2Fonts.setFont(u8g2_font_logisoso32_tr);
  display.firstPage();
  do {
    display.fillScreen(bg);
    display.fillRect(0, 0, 8, 8, GxEPD_BLACK);
    display.fillRect(display.width() - 8, 0, 8, 8, GxEPD_BLACK);
    display.fillRect(display.width() - 8, display.height() - 8, 8, 8, GxEPD_BLACK);
    display.fillRect(0, display.height() - 8, 8, 8, GxEPD_BLACK);

  } while (display.nextPage());
  delay(1000);
}