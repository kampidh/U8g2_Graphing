/*
  Simple graphing function for U8g2 display library.

  A demonstration to cycle through every possible features.
  This sketch works on Uno with limited memory space using U8g2 page buffer.
*/

#include <U8g2lib.h>
#include <Wire.h>
#include <U8g2Graphing.h>

//This example is using SSD1306 128x64 I2C monochrome OLED display

//U8G2_SSD1306_128X64_NONAME_F_HW_I2C u8g2(U8G2_R0, U8X8_PIN_NONE); //full buffer
U8G2_SSD1306_128X64_NONAME_1_HW_I2C u8g2(U8G2_R0, U8X8_PIN_NONE); //page buffer

//Constructor, takes a reference to the current instance of U8g2 display.
U8g2Graphing graph(&u8g2);

float result = 0;
float _ndx = 0;
int state = 0;
int analogIn = 0;

uint32_t preMil = 0;
uint32_t preMil2 = 0;
uint32_t preMil3 = 0;

void setup() {

  pinMode(A0, INPUT);

  u8g2.begin();
  u8g2.setFont(u8g2_font_tom_thumb_4x6_tf);
  u8g2.setFontMode(1);
  u8g2.setDrawColor(2);

  //There are 2 method to initiate the graph, .beginInt and .begin
  //.beginInt is used for integer data and lower RAM allocation.
  //.begin is used for floating point data but higher RAM allocation.
  //Using .begin on Uno will cause glitch and crash as it'll be over the RAM limit.
  //The arguments for the function is (from x, from y, to x, to y) in pixels.
  graph.beginInt(0, 9, 127, 63);

  //Display type init, explained below.
  graph.displaySet(true, false);
}

void loop() {
  //There are some examples on this sketch.
  //Cycling every 10s.
  realtimeGraph();
  preloadGraph();
  intervalGraph();
  pausableGraph();
}

//==================================================================================================

//In this example, the graph will draw a fourier series of a square wave continously.
//By realtime I mean the data will be inputted per screen refresh. creating a rolling graph effect.

void realtimeGraph() {
  //Initialize setting
  graph.displaySet(true, false);
  graph.pointerSet(false);
  graph.rangeSet(false);

  preMil2 = millis();
  //10 second loop
  while (millis() - preMil2 <= 10000) {

    for (float f = 0.0; f < 2.0; f += 0.02) {
      _ndx = (sin(f * PI) * 250) + 250;
      result = squareFourier(f * PI, 1) * 100;

      if (millis() - preMil >= 1000) {
        preMil = millis();

        (state >= 3) ? state = 0 : state++;

        //Cycling through different type of display using displaySet function.
        //The arguments are 2 boolean values.
        //First value determines the X axis visibility (true = visible, false = hidden)
        //Second value determines the sytle of how the graph is displayed (true = line, false = dotted)
        switch (state) {
          case 0:
            graph.displaySet(true, false);
            break;

          case 1:
            graph.displaySet(true, true);
            break;

          case 2:
            graph.displaySet(false, false);
            break;

          case 3:
            graph.displaySet(false, true);
            break;
        }
      }

      //Set the pointer to be displayed or not
      //the _ndx is an arbitary number from 0-500
      //being 0 is the rightmost/recent point and 500 is leftmost point.
      //If you don't want the arbitrary number, use .pointerSetI(bool, int) instead with 0 to max data point _ndx,
      //you can get the maximum number of data point value by using .getDataLen() function.
      graph.pointerSet(true, (int)_ndx);

      //Input the data to graph.
      graph.inputValue(result);

      u8g2.firstPage();
      do {
        u8g2.setCursor(4, 7);
        u8g2.setFont(u8g2_font_6x10_tr);
        u8g2.print("Cycling features");

        //Display the graph
        graph.displayGraph();
      } while (u8g2.nextPage());
    }
  }
  //Clear data points to prepare for the next function
  graph.clearData();

  u8g2.firstPage();
  do {
  } while (u8g2.nextPage());

  preMil2 = millis();
}


//In this example, we first sample all the data from the ADC to the graph points,
//and then display the graph. Kind of like normal mode on the oscilloscope.

void preloadGraph() {
  //Initialize setting
  graph.displaySet(true, false);
  graph.pointerSet(false);
  graph.rangeSet(false);

  preMil2 = millis();
  //10 second loop
  while (millis() - preMil2 <= 10000) {

    //This function disables the autorange function of the graph,
    //and set the min max range manually.
    graph.rangeSet(true, 0, 1024);

    //For loop to input the data to the graph as many as the total data points available
    //using .getDataLen() function. Add 2 to avoid glitch on the leftmost graph.
    for (int i = 0; i <= graph.getDataLen() + 2; i++) {
      graph.inputValue(analogRead(A0));
    }

    u8g2.firstPage();
    do {
      u8g2.setCursor(4, 7);
      u8g2.setFont(u8g2_font_6x10_tr);
      u8g2.print("Preload val");

      graph.displayGraph();
    } while (u8g2.nextPage());
  }
  graph.clearData();

  u8g2.firstPage();
  do {
  } while (u8g2.nextPage());
  
  preMil2 = millis();
}


//In this example, we sample the data once every 1 second
//without blocking the loop and display refreshes.

void intervalGraph() {
  static uint8_t roll = 0;
  //Initialize setting
  graph.displaySet(true, false);
  graph.pointerSet(false);
  graph.rangeSet(false);

  preMil2 = millis();
  //10 second loop
  while (millis() - preMil2 <= 10000) {
    roll++;

    //Set the sampling interval to 1 second (1000 ms)
    graph.intervalSet(1000);

    graph.inputValue(analogRead(A0));

    u8g2.firstPage();
    do {
      u8g2.setCursor(4, 7);
      u8g2.setFont(u8g2_font_6x10_tr);
      u8g2.print("1s intvl ");
      u8g2.print(roll);

      graph.displayGraph();
    } while (u8g2.nextPage());
  }
  graph.intervalSet(0);
  graph.clearData();

  u8g2.firstPage();
  do {
  } while (u8g2.nextPage());
  
  preMil2 = millis();
}


//In this example, the graph is paused every one second.

void pausableGraph() {
  static bool setActive = true;
  //Initialize setting
  graph.displaySet(true, false);
  graph.pointerSet(false);
  graph.rangeSet(false);

  preMil2 = millis();
  preMil3 = millis();
  //10 second loop
  while (millis() - preMil2 <= 10000) {

    if(millis() - preMil3 > 1000){
      preMil3 = millis();
      setActive = !setActive;
    }

    //Set the graph to run or pause.
    //Take note that this will mess with X axis label(time) accuracy.
    graph.startSampling(setActive);

    graph.inputValue(analogRead(A0));

    u8g2.firstPage();
    do {
      u8g2.setCursor(4, 7);
      u8g2.setFont(u8g2_font_6x10_tr);
      if (setActive){
        u8g2.print("Running ");
      } else {
        u8g2.print("Paused ");
      }
      u8g2.print(random(0, 100));

      graph.displayGraph();
    } while (u8g2.nextPage());
  }
  graph.intervalSet(0);
  graph.startSampling(true);
  graph.clearData();

  u8g2.firstPage();
  do {
  } while (u8g2.nextPage());
  
  preMil2 = millis();
}


float squareFourier(float degree, int harmonics) {
  float result = 0;
  for (int i = 1; i <= (harmonics * 2) + 1; i += 2) {
    result = result + ((float)1 / i) * sin(i * degree);
  }
  result = ((float)4 / PI) * result;
  return result;
}
