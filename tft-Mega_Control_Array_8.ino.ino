// Started with the paint example and went from there.

#include <Adafruit_GFX.h>    // Core graphics library
#include <Adafruit_TFTLCD.h> // Hardware-specific library
#include <TouchScreen.h>

#if defined(__SAM3X8E__)
    #undef __FlashStringHelper::F(string_literal)
    #define F(string_literal) string_literal
#endif

// For the Arduino Mega, use digital pins 22 through 29
// (on the 2-row header at the end of the board).
//   D0 connects to digital pin 22 (XP)
//   D1 connects to digital pin 23 (YM)
//   D2 connects to digital pin 24
//   D3 connects to digital pin 25
//   D4 connects to digital pin 26
//   D5 connects to digital pin 27
//   D6 connects to digital pin 28
//   D7 connects to digital pin 29

#define YP A3  // must be an analog pin, use "An" notation!
#define XM A2  // must be an analog pin, use "An" notation!
#define YM 23  // Shared with portA D1
#define XP 22  // Shared with portA D0

// converted to variables for future auto calibrate function
 int TS_MINX = 180;
 int TS_MINY = 160;
 int TS_MAXX = 900;
 int TS_MAXY = 920;

 //#define TS_MINX 100
 //#define TS_MINY 110
 //#define TS_MAXX 840
 //#define TS_MAXY 860

// For better pressure precision, we need to know the resistance
// between X+ and X- Use any multimeter to read it
// For the one we're using, its 300 ohms across the X plate
TouchScreen ts = TouchScreen(XP, YP, XM, YM, 410); // 410 measured

#define LCD_CS A3
#define LCD_CD A2
#define LCD_WR A1
#define LCD_RD A0
// optional
#define LCD_RESET A4 //This pin must be terminated high on LCD.

// Assign human-readable names to some common 16-bit color values:
#define	BLACK   0x0000
#define	BLUE    0x001F
#define	RED     0xF800
#define	GREEN   0x07E0
#define CYAN    0x07FF
#define MAGENTA 0xF81F
#define YELLOW  0xFFE0
#define WHITE   0xFFFF


//load the array with pretty colors
int COLOUR[8][6] = {BLUE,RED,GREEN,CYAN,MAGENTA,YELLOW,
                    WHITE,BLUE,RED,GREEN,CYAN,MAGENTA,
                    YELLOW,WHITE,BLUE,RED,GREEN,CYAN,
                    MAGENTA,YELLOW,WHITE,BLUE,RED,GREEN,
                    CYAN,MAGENTA,YELLOW,WHITE,BLUE,RED,
                    GREEN,CYAN,MAGENTA,YELLOW,WHITE,BLUE,
                    RED,GREEN,CYAN,MAGENTA,YELLOW,WHITE,
                    BLUE,RED,GREEN,CYAN,MAGENTA,YELLOW};

//keypress assignment values
String kpress_1[8][6] = {"01","02","03","04","05","06",
                       "07","08","09","10","11","12",
                       "13","14","15","16","17","18",
                       "19","20","21","22","23","24",
                       "25","26","27","28","29","30",
                       "31","32","33","34","35","36",
                       "37","38","39","40","40","42",
                       "43","44","45","46","47","48"};
/
/ pushbutton state for array of 48 buttons.
// quad state function (push on-push off).
int lighted[8][6] =   {0,0,0,0,0,0,
                      0,0,0,0,0,0,
                      0,0,0,0,0,0,
                      0,0,0,0,0,0,
                      0,0,0,0,0,0,
                      0,0,0,0,0,0,
                      0,0,0,0,0,0,
                      0,0,0,0,0,0};

//Global Variables
  int samplecount = 0;
  int x2,y2,z2;  //Not using z2?

Adafruit_TFTLCD tft(LCD_CS, LCD_CD, LCD_WR, LCD_RD, LCD_RESET);

#define BOXSIZE 38
// #define PENRADIUS 1
// int oldcolor, currentcolor;

void setup(void) {
  Serial.begin(9600);
  Serial.println(F("Touchscreen Input Output devices"));
  tft.reset();
  
  uint16_t identifier = tft.readID();

  if(identifier == 0x9325) {
    Serial.println(F("Found ILI9325 LCD driver"));
  } else if(identifier == 0x9328) {
    Serial.println(F("Found ILI9328 LCD driver"));
  } else if(identifier == 0x7575) {
    Serial.println(F("Found HX8347G LCD driver"));
  } else if(identifier == 0x9341) {
    Serial.println(F("Found ILI9341 LCD driver"));
  } else if(identifier == 0x8357) {
    Serial.println(F("Found HX8357D LCD driver"));
  } else {
    Serial.print(F("Unknown LCD driver chip: "));
    Serial.println(identifier, HEX);
    Serial.println(F("If using the Adafruit 2.8\" TFT Arduino shield, the line:"));
    Serial.println(F("  #define USE_ADAFRUIT_SHIELD_PINOUT"));
    Serial.println(F("should appear in the library header (Adafruit_TFT.h)."));
    Serial.println(F("If using the breakout board, it should NOT be #defined!"));
    Serial.println(F("Also if using the breakout, double-check that all wiring"));
    Serial.println(F("matches the tutorial."));
    return;
  }

  tft.begin(identifier);

 
  tft.fillScreen(BLACK);
//int cols;
//int rows;
// Draw our active display according to the COLOUR array
  for (int rows=0; rows<8; rows++){
  for (int cols=0; cols<6; cols++){
  tft.fillRect((BOXSIZE+2)*cols, (BOXSIZE+2)*rows, BOXSIZE, BOXSIZE, COLOUR[rows][cols]);
//number the blocks, Set text to a binary NOT of the box color
  tft.setCursor((BOXSIZE+2)*cols+8, (BOXSIZE+2)*rows+12);
  tft.setTextColor(~COLOUR[rows][cols],COLOUR[rows][cols]);
  tft.setTextSize(2);
  tft.println(kpress_1[rows][cols]);

    } 
  }
}

#define MINPRESSURE 5
#define MAXPRESSURE 1000

void loop()
{
  int x1,y1,z1; //localized vars
  TSPoint p = ts.getPoint();
   // if sharing pins, you'll need to fix the directions of the touchscreen pins
  //pinMode(XP, OUTPUT);
  pinMode(XM, OUTPUT);
  pinMode(YP, OUTPUT);
  //pinMode(YM, OUTPUT);
  z1 = p.z; //Make a copy of the pressure A-D values
//  z2 = z1;

  // we have some minimum pressure we consider 'valid'
  // pressure of 0 means no pressing!
    if (p.z > MINPRESSURE && p.z < MAXPRESSURE) {    
      samplecount = 0;
      // scale from 0->1023 to tft.width
      p.x = map(p.x, TS_MINX, TS_MAXX, 0, tft.width());
      p.y = map(p.y, TS_MINY, TS_MAXY, 0, tft.height());

    x1 = p.x/40;  // Now
    y1 = p.y/40;
    x2 = x1;      // and Later (during pressure release)
    y2 = y1;

// turn box highlight on if it's off for the box touched
    if (lighted[x1][y1] == 0) { //first press
      tft.drawRect(x1*40, y1*40, BOXSIZE, BOXSIZE, WHITE);
//    dividing by blocksize in integer math, then multiplying by 40
//    guarantees the location of the pen is within the box.
      tft.drawRect(x1*40-1, y1*40-1, BOXSIZE+2, BOXSIZE+2, WHITE);
      tft.drawRect(x1*40-2, y1*40-2, BOXSIZE+4, BOXSIZE+4, WHITE);

//    Save which box we activated.
      lighted[x1][y1] = 1;
      samplecount = 0;  // reset zero debounce counter.
//      Serial.print(F("Set box\t")); //State zero debug
//      Serial.println(kpress_1[y2][x2]); //ditto
    }

   if (lighted[x1][y1] == 2) { //second press

//      x2 = x1;
//      y2 = y1;

// Unhighlight when pressed a second time.
      tft.drawRect(x1*40, y1*40, BOXSIZE, BOXSIZE, BLACK);
      tft.drawRect(x1*40-1, y1*40-1, BOXSIZE+2, BOXSIZE+2, BLACK);
      tft.drawRect(x1*40-2, y1*40-2, BOXSIZE+4, BOXSIZE+4, BLACK);
 
      lighted[x1][y1] = 3;
      samplecount = 0;
//      Serial.print(F("Unset box\t")); //State two debug
//      Serial.println(kpress_1[y2][x2]);
    }
  } 
   else
// Everything below here deals with the stylus being lifted from the touchscreen   
   samplecount++;
   // My Touchscreen occasionally spits out zero pressure glitches
   // even while stylus is on touchscreen
   // So I treat them like bounce by looking for 10 zeroes in a row
   // before assuming stylus is lifted.
   if (samplecount > 10) {

//    Switch lit = 1 so stylus is removed while lit.
      if (lighted[x2][y2] == 1 ) { //first release
      lighted[x2][y2] = 2;  //flag state 2 of (0-3)
      samplecount = 0; //reset the bouncing zero count.
//      Serial.println("First Release"); //State one debug
      }
   
      else
      if (lighted[x2][y2] == 3) { //second release
      lighted[x2][y2] = 0;
      samplecount = 0;
//      Serial.println("Second Release"); //State 3 debug
    }
  }
}


