/*
	U8g2Graphing - a simple graphing function for U8g2 library
	Kampidh 2020
	
	Licensed under MIT license
	
*/

#ifndef U8G2GRAPH_H
#define U8G2GRAPH_H

#include <Arduino.h>
#include <U8g2lib.h>

class U8g2Graphing
{
  public:
    U8g2Graphing(U8G2 *u8g);
    ~U8g2Graphing();
    void begin(uint16_t fromx, uint16_t fromy, uint16_t tox, uint16_t toy);
    void beginInt(uint16_t fromx, uint16_t fromy, uint16_t tox, uint16_t toy);
    void start();
    void stop();
    void startSampling(bool sample);
    void intervalSet(uint32_t intvl);
    void displaySet(bool xaxis, bool dotted = false);
    void pointerSet(bool pointer, uint16_t pointndx = 0);
    void pointerSetI(bool pointer, uint16_t pointndx = 0);
    void rangeSet(bool setrange, float vmin = 0, float vmax = 0);
    void inputValue(float var);
	void inputValue(int var);
	void clearData();
    uint16_t getDataLen();
    float getMin();
    float getMax();
	float getDataMin();
    float getDataMax();
    void displayGraph();
  private:
	void inValue(float var);
    void inValue(int var);
    float fmap(float x, float in_min, float in_max, float out_min, float out_max);
    uint16_t **graph;
    float *dataset;
    uint8_t **graphInt;
    int *datasetInt;
    uint16_t grwidth, fromx, fromy, tox, toy, ndx, _pointndx, spd;
    float minval, maxval, mindata, maxdata, vmin, vmax;
    uint32_t curmil, curmcr, ppt, intvl;
    bool graphstart, activate, xaxis, autorange, pointer, dotted, isFloat;
    U8G2 *u8g2;
};

#endif
