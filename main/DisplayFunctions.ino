void displayChoice(char Choix1[], char Choix2[]) {
  //Serial.println("Setting Screen");
  byte PartialWindowX = 0;
  byte PartialWindowY = display.height() / 2 + 10;
  byte PartialWindowWidth = display.width();
  byte PartialWindowHeight = display.height() / 2 - 10;

  u8g2Fonts.setFont(u8g2_font_logisoso18_tf);  
  display.setPartialWindow(PartialWindowX, PartialWindowY, PartialWindowWidth, PartialWindowHeight);  // x,y,width,height
  //this sets a window for the partial update, so the values can update without refreshing the entire screen.

  display.firstPage();
  do {
    display.fillScreen(bg);
    display.drawRect(PartialWindowX, PartialWindowY, PartialWindowWidth, PartialWindowHeight, GxEPD_BLACK);
    u8g2Fonts.setCursor(0, display.height() - 8);  // x , y
    u8g2Fonts.print(Choix1);

    u8g2Fonts.setCursor(display.width() / 2 + 30, display.height() - 8);  // x , y
    u8g2Fonts.print(Choix2);

  } while (display.nextPage());
}

void updateTimeDisplay(byte Hour = 100, byte Minute = 0) {
  //Serial.println("Setting Screen");
  byte PartialWindowX = 0;
  byte PartialWindowY = 0;
  byte PartialWindowWidth = 90;
  byte PartialWindowHeight = display.height() / 2 + 10;
  
  if (Hour == 100) {
    Hour = Clock.getHour(h12, PM);
    Minute = Clock.getMinute();
  }
  
  u8g2Fonts.setFont(u8g2_font_logisoso32_tn);  
  display.setPartialWindow(PartialWindowX, PartialWindowY, PartialWindowWidth, PartialWindowHeight);  // x,y,width,height
  
  display.firstPage();
  do {
    display.fillScreen(bg);
    //display.drawRect(PartialWindowX, PartialWindowY, PartialWindowWidth, PartialWindowHeight, GxEPD_BLACK);
    u8g2Fonts.setCursor(0, 50);           // x , y
    if (Hour < 10) u8g2Fonts.print("0");  // Formatting to always display 2 digits hours
    u8g2Fonts.print(Hour);
    u8g2Fonts.print(":");
    if (Minute < 10) u8g2Fonts.print("0");  // Formatting to always display 2 digits minutes
    u8g2Fonts.print(Minute);
  } while (display.nextPage());
}

// void displayStopHeatingTOTD(byte StopHeatingTOTD) {
//   Serial.println("Setting StopHeatingTOTD Screen");
//   byte PartialWindowX = 90;
//   byte PartialWindowY = 0;
//   byte PartialWindowWidth = display.width() - 90;
//   byte PartialWindowHeight =  display.height() / 2 ;

//   u8g2Fonts.setFont(u8g2_font_logisoso16_tr);  //u8g2_font_logisoso32_tn--->numbers only to save memory ; u8g2_font_logisoso32_tr , u8g2_font_logisoso32_tf -->numbers&letters

//   display.setPartialWindow(PartialWindowX, PartialWindowY, PartialWindowWidth, PartialWindowHeight);  // x,y,width,height
//   //this sets a window for the partial update, so the values can update without refreshing the entire screen.
//   display.firstPage();
//   do {
//     display.fillScreen(bg);
//     //display.drawRect(PartialWindowX, PartialWindowY, PartialWindowWidth, PartialWindowHeight, GxEPD_BLACK);
//     if (StopHeatingTOTD != 0) {
//       display.drawBitmap(PartialWindowX + 65, PartialWindowY, logo, 48, 48, GxEPD_BLACK);
//       u8g2Fonts.setCursor(PartialWindowX + 10, PartialWindowY + 30);  // x , y
//       u8g2Fonts.print("Reste");
//       u8g2Fonts.setCursor(PartialWindowX + 10, PartialWindowY + 50);  // x , y
//       u8g2Fonts.print(StopHeatingTOTD);
//       u8g2Fonts.print("min.");
//     }
//   } while (display.nextPage());
// }

// void displayStartHeatingTOTD(byte StartHeatingTOTD) {
//   //Serial.println("Setting  Screen");
//   byte PartialWindowX = 142;
//   byte PartialWindowY = 0;
//   byte PartialWindowWidth = display.width() / 2;
//   byte PartialWindowHeight = display.height() / 2 + 10;

//   //Used to set icon above button 1 & 2 according to the variables
//   u8g2Fonts.setFont(u8g2_font_logisoso18_tr);  //u8g2_font_logisoso32_tn--->numbers only to save memory ; u8g2_font_logisoso32_tr , u8g2_font_logisoso32_tf -->numbers&letters

//   display.setPartialWindow(PartialWindowX, PartialWindowY, PartialWindowWidth, PartialWindowHeight);  // x,y,width,height
//   //this sets a window for the partial update, so the values can update without refreshing the entire screen.
//   display.firstPage();
//   do {
//     display.fillScreen(bg);
//     //display.drawRect(PartialWindowX, PartialWindowY, PartialWindowWidth, PartialWindowHeight, GxEPD_BLACK);
//     if (StartHeatingTOTD != 0) {
//       u8g2Fonts.setCursor(PartialWindowX + 5, PartialWindowY + 25);  // x , y
//       u8g2Fonts.println("Progr.");
//       u8g2Fonts.setCursor(PartialWindowX + 5, PartialWindowY + 50);  // x , y
//       u8g2Fonts.print(" ");
//       u8g2Fonts.print(StartHeatingTOTD);
//       u8g2Fonts.print("h");
//     }
//   } while (display.nextPage());
// }

void displayprog(byte mode, unsigned int StartORStopHeatingTOTD)  {
  //Serial.println("Setting StopHeatingTOTD Screen");
  byte PartialWindowX = 100;
  byte PartialWindowY = 0;
  byte PartialWindowWidth = display.width() - 90;
  byte PartialWindowHeight =  display.height() / 2 ;
  u8g2Fonts.setFont(u8g2_font_logisoso16_tr);

  display.setPartialWindow(PartialWindowX, PartialWindowY, PartialWindowWidth, PartialWindowHeight);  // x,y,width,height
  //this sets a window for the partial update, so the values can update without refreshing the entire screen.
  display.firstPage();
  do {
    display.fillScreen(bg);
    //display.drawRect(PartialWindowX, PartialWindowY, PartialWindowWidth, PartialWindowHeight, GxEPD_BLACK);
      if (mode == 1){
        u8g2Fonts.setCursor(PartialWindowX , PartialWindowY + 30);  // x , y
        u8g2Fonts.print("Progr.");
        u8g2Fonts.print((StartORStopHeatingTOTD-StartORStopHeatingTOTD%60)/60);
        u8g2Fonts.print("h");
        u8g2Fonts.print(StartORStopHeatingTOTD%60);
      }
      else if (mode == 2) {
        display.drawBitmap(PartialWindowX + 55, PartialWindowY, logo, 48, 48, GxEPD_BLACK); // x , y, bitmap, width, height, color

        u8g2Fonts.setCursor(PartialWindowX , PartialWindowY + 30);  // x , y
        u8g2Fonts.print("Reste");

        u8g2Fonts.setCursor(PartialWindowX , PartialWindowY + 50);  // x , y
        u8g2Fonts.print(StartORStopHeatingTOTD);
        u8g2Fonts.print("min.");
      }
  } while (display.nextPage());
}
