/*
	U8g2Graphing - a simple graphing function for U8g2 library
	Kampidh 2020
	
	Licensed under MIT license
	
*/

#include "U8g2Graphing.h"

//Constructor, takes pointer to existing U8g2 instance.
//========================================================================
U8g2Graphing::U8g2Graphing(U8G2 *u8g) : u8g2(u8g), graphstart(true) {}

//Deconstructor, delete all data.
//========================================================================
U8g2Graphing::~U8g2Graphing() {
	delete [] graph;
	delete [] dataset;
	delete [] graphInt;
	delete [] datasetInt;
}

//Floating point buffer initializer, define the position and size of the graph,
//then set the buffers to heap memory.
//========================================================================
void U8g2Graphing::begin(uint16_t fromx, uint16_t fromy, uint16_t tox, uint16_t toy) {
	this->fromx = fromx;
	this->fromy = fromy;
	this->tox = tox;
	this->toy = toy;
	grwidth = tox - fromx - 20;
	if (graphstart) {
		graph = new uint16_t*[grwidth];
		for (uint16_t i = 0; i < grwidth; i++) {
			graph[i] = new uint16_t[2];
		}
		dataset = new float[grwidth];
		for (uint16_t i = 0; i < grwidth; i++) {
			dataset[i] = 0;
		}
		for (uint16_t i = 0; i < grwidth; i++) {
			graph[i][0] = fromx;
			graph[i][1] = toy;
		}
		graphstart = false;
	}
	activate = true;
	xaxis = true;
	pointer = false;
	dotted = false;
	autorange = true;
	isFloat = true;
	curmil = 0;
	ppt = 0;
	_pointndx = 0;
	intvl = 0;
	spd = 1;
}

//Integer buffer initializer, define the position and size of the graph,
//then set the buffers to heap memory.
//========================================================================
void U8g2Graphing::beginInt(uint16_t fromx, uint16_t fromy, uint16_t tox, uint16_t toy) {
	this->fromx = fromx;
	this->fromy = fromy;
	this->tox = tox;
	this->toy = toy;
	grwidth = tox - fromx - 20;
	if (graphstart) {

		graphInt = new uint8_t*[grwidth];
		for (uint16_t i = 0; i < grwidth; i++) {
			graphInt[i] = new uint8_t[2];
		}
		datasetInt = new int[grwidth];

		for (uint16_t i = 0; i < grwidth; i++) {
			datasetInt[i] = 0;
		}
		for (uint16_t i = 0; i < grwidth; i++) {
			graphInt[i][0] = fromx;
			graphInt[i][1] = toy;
		}
		graphstart = false;
	}
	activate = true;
	xaxis = true;
	pointer = false;
	dotted = false;
	autorange = true;
	isFloat = false;
	curmil = 0;
	ppt = 0;
	_pointndx = 0;
	intvl = 0;
	spd = 1;
}

//Start the graph sampling using function.
//========================================================================
void U8g2Graphing::start() {
	activate = true;
}

//Stop the graph sampling using function.
//========================================================================
void U8g2Graphing::stop() {
	activate = false;
}

//Control the start-stop of graph sampling using external variable
//========================================================================
void U8g2Graphing::startSampling(bool sample) {
	activate = sample;
}

//Set the sampling interval,
//useful if you don't want to use delay function.
//========================================================================
void U8g2Graphing::intervalSet(uint32_t intvl) {
	this->intvl = intvl;
}

//Set the display style, with or without x axis and dotted or line style.
//========================================================================
void U8g2Graphing::displaySet(bool xaxis, bool dotted) {
	this->xaxis = xaxis;
	this->dotted = dotted;
}

//Set the pointer display, and set the pointer index,
//the index is mapped from 0 (rightmost data) to 500 (leftmost data).
//========================================================================
void U8g2Graphing::pointerSet(bool pointer, uint16_t pointndx) {
	this->pointer = pointer;
	_pointndx = map(pointndx, 0, 500, 0, grwidth - 2);
}

//Set the pointer display, and set the pointer index,
//the index is an actual graph index, not mapped.
//========================================================================
void U8g2Graphing::pointerSetI(bool pointer, uint16_t pointndx) {
	this->pointer = pointer;
	uint16_t maxndx = getDataLen();
	_pointndx = constrain(pointndx, 0, maxndx);
}

//Set the range and disabling the autorange function.
//========================================================================
void U8g2Graphing::rangeSet(bool setrange, float vmin, float vmax) {
	this->autorange = !setrange;
	this->vmin = vmin;
	this->vmax = vmax;
}

//Input value converter.
//========================================================================
void U8g2Graphing::inputValue(float var) {
	if(isFloat){
		inValue(var);
	} else {
		inValue((int)var);
	}
}

void U8g2Graphing::inputValue(int var) {
	if(isFloat){
		inValue((float)var);
	} else {
		inValue(var);
	}
}

//Input the data into the buffer and displaying the graph later (floating point),
//works with both realtime and sampling mode, also works with U8g2 page buffer.
//========================================================================
void U8g2Graphing::inValue(float var) {
	if (millis() - curmil >= intvl && activate == true) {
		if (millis() - curmil < 5) {
			ppt = micros() - curmcr;
		} else {
			ppt = millis() - curmil;
		}
		curmcr = micros();
		curmil = millis();

		if (ndx >= grwidth - 1) {
			ndx = 0;
		} else {
			ndx++;
		}

		dataset[ndx] = var;
		graph[ndx][0] = tox + 1;

		for (uint16_t i = 0; i < grwidth; i++) {
			graph[i][0] -= spd;
		}

		float minvalget = 1e+6;
		for (uint16_t i = 0; i < grwidth; i++) {
			if (minvalget > dataset[i]) {
				minvalget = dataset[i];
			}
		}

		float maxvalget = -1e+6;
		for (uint16_t i = 0; i < grwidth; i++) {
			if (maxvalget < dataset[i]) {
				maxvalget = dataset[i];
			}
		}

		minval = minvalget;
		maxval = maxvalget;
		
		mindata = minvalget;
		maxdata = maxvalget;

	}

	if (!autorange) {
		minval = vmin;
		maxval = vmax;
	}

	if (minval == maxval) {
		maxval += 0.01;
		minval -= 0.01;
	}

	for (uint16_t i = 0; i < grwidth; i++) {
		uint16_t posy = fmap(dataset[i], minval, maxval, (!xaxis) ? toy : toy - 9, fromy);
		graph[i][1] = constrain(posy, fromy, (!xaxis) ? toy : toy - 9);
	}
}

//Input the data into the buffer and displaying the graph later (floating point),
//works with both realtime and sampling mode, also works with U8g2 page buffer.
//========================================================================
void U8g2Graphing::inValue(int var) {
	if (millis() - curmil >= intvl && activate == true) {
		if (millis() - curmil < 5) {
			ppt = micros() - curmcr;
		} else {
			ppt = millis() - curmil;
		}
		curmcr = micros();
		curmil = millis();

		if (ndx >= grwidth - 1) {
			ndx = 0;
		} else {
			ndx++;
		}

		datasetInt[ndx] = var;
		graphInt[ndx][0] = tox + 1;

		for (uint16_t i = 0; i < grwidth; i++) {
			graphInt[i][0] -= spd;
		}

		int minvalget = 32000;
		for (uint16_t i = 0; i < grwidth; i++) {
			if (minvalget > datasetInt[i]) {
				minvalget = datasetInt[i];
			}
		}

		int maxvalget = -32000;
		for (uint16_t i = 0; i < grwidth; i++) {
			if (maxvalget < datasetInt[i]) {
				maxvalget = datasetInt[i];
			}
		}

		minval = minvalget;
		maxval = maxvalget;
		
		mindata = minvalget;
		maxdata = maxvalget;

	}

	if (!autorange) {
		minval = vmin;
		maxval = vmax;
	}

	if ((int)minval == (int)maxval) {
		maxval += 1;
		minval -= 1;
	}

	for (uint16_t i = 0; i < grwidth; i++) {
		uint16_t posy = map(datasetInt[i], minval, maxval, (!xaxis) ? toy : toy - 9, fromy);
		graphInt[i][1] = constrain(posy, fromy, (!xaxis) ? toy : toy - 9);
	}
}

//Clear graph data.
//========================================================================
void U8g2Graphing::clearData() {
	if(isFloat){
		for (uint16_t i = 0; i < grwidth; i++) {
			dataset[i] = 0;
		}
		for (uint16_t i = 0; i < grwidth; i++) {
			graph[i][0] = fromx;
			graph[i][1] = toy;
		}
	} else {
		for (uint16_t i = 0; i < grwidth; i++) {
			datasetInt[i] = 0;
		}
		for (uint16_t i = 0; i < grwidth; i++) {
			graphInt[i][0] = fromx;
			graphInt[i][1] = toy;
		}
	}
}

//Return the data length of the graph.
//========================================================================
uint16_t U8g2Graphing::getDataLen() {
	return grwidth - 2;
}

//Return the Min value of the graph, useful for multiple graph with same range.
//========================================================================
float U8g2Graphing::getMin() {
	return minval;
}

//Return the Max value of the graph, useful for multiple graph with same range.
//========================================================================
float U8g2Graphing::getMax() {
	return maxval;
}

//Return the Min value from the data set.
//========================================================================
float U8g2Graphing::getDataMin() {
	return mindata;
}

//Return the Max value from the data set.
//========================================================================
float U8g2Graphing::getDataMax() {
	return maxdata;
}

//Standalone function to display the graph.
//========================================================================
void U8g2Graphing::displayGraph() {
	if(isFloat){
		u8g2->setDrawColor(0);
		u8g2->drawBox(fromx, fromy, tox, toy);
		u8g2->setDrawColor(1);
		u8g2->setClipWindow(fromx + 21, fromy, tox, (!xaxis) ? toy + 1 : toy - 8);
		for (uint16_t i = 0; i < grwidth; i++) {
			if (dotted) {
				u8g2->drawPixel(graph[i][0], graph[i][1]);
			} else {
				if (i == 0) {
					u8g2->drawLine(graph[i][0], graph[i][1], graph[i][0] - spd, graph[grwidth - 1][1]);
				} else {
					u8g2->drawLine(graph[i][0], graph[i][1], graph[i][0] - spd, graph[i - 1][1]);
				}
			}
		}
		u8g2->setMaxClipWindow();

		u8g2->setFont(u8g2_font_tom_thumb_4x6_tn);
		u8g2->setFontMode(1);
		if (xaxis) {
			uint32_t xvalmid;
			uint32_t xvalfar;
			if (ppt >= 1000) {
				xvalfar = ((ppt * (tox - (fromx + 21))) / spd) / 1000;
				xvalmid = ((ppt * ((tox - (fromx + 21)) / 2)) / spd) / 1000;
			} else {
				xvalfar = (ppt * (tox - (fromx + 21))) / spd;
				xvalmid = (ppt * ((tox - (fromx + 21)) / 2)) / spd;
			}
			u8g2->setDrawColor(0);
			u8g2->drawBox(fromx, toy - 9, tox - fromx + 1, 9);
			u8g2->setDrawColor(1);
			u8g2->drawHLine(fromx + 21, toy - 9, tox - fromx - 20);
			u8g2->drawVLine(tox, toy - 9, 3);
			u8g2->setCursor(tox - 2, toy);
			u8g2->print("0");
			u8g2->drawVLine(fromx + 21, toy - 9, 3);
			u8g2->setCursor(fromx + ((xvalfar >= 0 && xvalfar < 10) ? 20 :
			(xvalfar >= 10 && xvalfar < 100) ? 18 :
			(xvalfar >= 100 && xvalfar < 1000) ? 16 :
			(xvalfar >= 1000 && xvalfar < 10000) ? 14 :
			(xvalfar >= 10000 && xvalfar < 100000) ? 12 :
			(xvalfar >= 100000) ? 10 : 8), toy);
			u8g2->print(xvalfar);
			if (tox - (fromx + 21) > 31) {
				u8g2->drawVLine(tox - ((tox - (fromx + 21)) / 2), toy - 9, 3);
				u8g2->setCursor((tox - ((tox - (fromx + 21)) / 2)) - ((xvalmid >= 0 && xvalmid < 10) ? 1 :
				(xvalmid >= 10 && xvalmid < 100) ? 3 :
				(xvalmid >= 100 && xvalmid < 1000) ? 5 :
				(xvalmid >= 1000 && xvalmid < 10000) ? 7 :
				(xvalmid >= 10000 && xvalmid < 100000) ? 9 :
				(xvalmid >= 100000) ? 11 : 13), toy);
				u8g2->print(xvalmid);
			}
		}

		u8g2->setDrawColor(0);
		u8g2->drawBox(fromx, fromy, 21, (!xaxis) ? toy - fromy : toy - fromy - 9);
		u8g2->setDrawColor(1);

		if (!xaxis) {
			u8g2->drawVLine(fromx + 21, fromy, toy - fromy + 1);
			u8g2->drawHLine(fromx + 15, fromy, 6);
			u8g2->drawHLine(fromx + 15, toy, 6);
			u8g2->drawHLine(fromx + 17, ((toy - fromy) / 2) + fromy, 4);
			u8g2->setCursor(fromx, fromy + 7);
			if (maxval >= 1000 || maxval <= -100) {
				u8g2->print(maxval, 0);
			} else {
				u8g2->print(maxval, 1);
			}
			u8g2->setCursor(fromx, toy - 2);
			if (minval >= 1000 || minval <= -100) {
				u8g2->print(minval, 0);
			} else {
				u8g2->print(minval, 1);
			}
		} else {
			u8g2->drawVLine(fromx + 21, fromy, toy - fromy - 8);
			u8g2->drawHLine(fromx + 15, fromy, 6);
			u8g2->drawHLine(fromx + 15, toy - 9, 6);
			u8g2->drawHLine(fromx + 17, ((toy - fromy - 8) / 2) + fromy, 4);
			u8g2->setCursor(fromx, fromy + 7);
			if (maxval >= 1000 || maxval <= -100) {
				u8g2->print(maxval, 0);
			} else {
				u8g2->print(maxval, 1);
			}
			u8g2->setCursor(fromx, toy - 11);
			if (minval >= 1000 || minval <= -100) {
				u8g2->print(minval, 0);
			} else {
				u8g2->print(minval, 1);
			}
		}

		if (pointer) {
			for (uint16_t i = 0; i < grwidth; i++) {
				if (graph[i][0] == tox - _pointndx) {
					if (graph[i][1] > toy - ((!xaxis) ? 16 : 25)) {
						u8g2->drawVLine(graph[i][0], graph[i][1] - 8, 8);
						if (graph[i][0] < fromx + 35) {
							u8g2->drawBox(fromx + 22, graph[i][1] - 15, 29, 7);
							u8g2->setCursor(fromx + 23, graph[i][1] - 9);
							u8g2->setDrawColor(0);
							u8g2->print(dataset[i], (dataset[i] >= 0 && dataset[i] < 10) ? 5 :
							(dataset[i] >= 10 && dataset[i] < 100 || dataset[i] > -10 && dataset[i] < 0) ? 4 :
							(dataset[i] >= 100 && dataset[i] < 1000 || dataset[i] > -100 && dataset[i] <= -10) ? 3 :
							(dataset[i] >= 1000 && dataset[i] < 10000 || dataset[i] > -1000 && dataset[i] <= -100) ? 2 :
							(dataset[i] >= 10000 || dataset[i] <= -1000) ? 1 : 0);
						} else if (graph[i][0] > tox - 16) {
							u8g2->drawBox(tox - 28, graph[i][1] - 15, 29, 7);
							u8g2->setCursor(tox - 27, graph[i][1] - 9);
							u8g2->setDrawColor(0);
							u8g2->print(dataset[i], (dataset[i] >= 0 && dataset[i] < 10) ? 5 :
							(dataset[i] >= 10 && dataset[i] < 100 || dataset[i] > -10 && dataset[i] < 0) ? 4 :
							(dataset[i] >= 100 && dataset[i] < 1000 || dataset[i] > -100 && dataset[i] <= -10) ? 3 :
							(dataset[i] >= 1000 && dataset[i] < 10000 || dataset[i] > -1000 && dataset[i] <= -100) ? 2 :
							(dataset[i] >= 10000 || dataset[i] <= -1000) ? 1 : 0);
						} else {
							u8g2->drawBox(graph[i][0] - 13, graph[i][1] - 15, 29, 7);
							u8g2->setCursor(graph[i][0] - 12, graph[i][1] - 9);
							u8g2->setDrawColor(0);
							u8g2->print(dataset[i], (dataset[i] >= 0 && dataset[i] < 10) ? 5 :
							(dataset[i] >= 10 && dataset[i] < 100 || dataset[i] > -10 && dataset[i] < 0) ? 4 :
							(dataset[i] >= 100 && dataset[i] < 1000 || dataset[i] > -100 && dataset[i] <= -10) ? 3 :
							(dataset[i] >= 1000 && dataset[i] < 10000 || dataset[i] > -1000 && dataset[i] <= -100) ? 2 :
							(dataset[i] >= 10000 || dataset[i] <= -1000) ? 1 : 0);
						}
					} else {
						u8g2->drawVLine(graph[i][0], graph[i][1], 8);
						if (graph[i][0] < fromx + 35) {
							u8g2->drawBox(fromx + 22, graph[i][1] + 8, 29, 7);
							u8g2->setCursor(fromx + 23, graph[i][1] + 14);
							u8g2->setDrawColor(0);
							u8g2->print(dataset[i], (dataset[i] >= 0 && dataset[i] < 10) ? 5 :
							(dataset[i] >= 10 && dataset[i] < 100 || dataset[i] > -10 && dataset[i] < 0) ? 4 :
							(dataset[i] >= 100 && dataset[i] < 1000 || dataset[i] > -100 && dataset[i] <= -10) ? 3 :
							(dataset[i] >= 1000 && dataset[i] < 10000 || dataset[i] > -1000 && dataset[i] <= -100) ? 2 :
							(dataset[i] >= 10000 || dataset[i] <= -1000) ? 1 : 0);
						} else if (graph[i][0] > tox - 16) {
							u8g2->drawBox(tox - 28, graph[i][1] + 8, 29, 7);
							u8g2->setCursor(tox - 27, graph[i][1] + 14);
							u8g2->setDrawColor(0);
							u8g2->print(dataset[i], (dataset[i] >= 0 && dataset[i] < 10) ? 5 :
							(dataset[i] >= 10 && dataset[i] < 100 || dataset[i] > -10 && dataset[i] < 0) ? 4 :
							(dataset[i] >= 100 && dataset[i] < 1000 || dataset[i] > -100 && dataset[i] <= -10) ? 3 :
							(dataset[i] >= 1000 && dataset[i] < 10000 || dataset[i] > -1000 && dataset[i] <= -100) ? 2 :
							(dataset[i] >= 10000 || dataset[i] <= -1000) ? 1 : 0);
						} else {
							u8g2->drawBox(graph[i][0] - 13, graph[i][1] + 8, 29, 7);
							u8g2->setCursor(graph[i][0] - 12, graph[i][1] + 14);
							u8g2->setDrawColor(0);
							u8g2->print(dataset[i], (dataset[i] >= 0 && dataset[i] < 10) ? 5 :
							(dataset[i] >= 10 && dataset[i] < 100 || dataset[i] > -10 && dataset[i] < 0) ? 4 :
							(dataset[i] >= 100 && dataset[i] < 1000 || dataset[i] > -100 && dataset[i] <= -10) ? 3 :
							(dataset[i] >= 1000 && dataset[i] < 10000 || dataset[i] > -1000 && dataset[i] <= -100) ? 2 :
							(dataset[i] >= 10000 || dataset[i] <= -1000) ? 1 : 0);
						}
					}
					u8g2->setDrawColor(1);
				}
			}
		}
	}
	else 
	{
		u8g2->setDrawColor(0);
		u8g2->drawBox(fromx, fromy, tox, toy);
		u8g2->setDrawColor(1);
		u8g2->setClipWindow(fromx + 21, fromy, tox, (!xaxis) ? toy + 1 : toy - 8);
		for (uint16_t i = 0; i < grwidth; i++) {
			if (dotted) {
				u8g2->drawPixel(graphInt[i][0], graphInt[i][1]);
			} else {
				if (i == 0) {
					u8g2->drawLine(graphInt[i][0], graphInt[i][1], graphInt[i][0] - spd, graphInt[grwidth - 1][1]);
				} else {
					u8g2->drawLine(graphInt[i][0], graphInt[i][1], graphInt[i][0] - spd, graphInt[i - 1][1]);
				}
			}
		}
		u8g2->setMaxClipWindow();

		u8g2->setFont(u8g2_font_tom_thumb_4x6_tn);
		u8g2->setFontMode(1);
		if (xaxis) {
			uint32_t xvalmid;
			uint32_t xvalfar;
			if (ppt >= 1000) {
				xvalfar = ((ppt * (tox - (fromx + 21))) / spd) / 1000;
				xvalmid = ((ppt * ((tox - (fromx + 21)) / 2)) / spd) / 1000;
			} else {
				xvalfar = (ppt * (tox - (fromx + 21))) / spd;
				xvalmid = (ppt * ((tox - (fromx + 21)) / 2)) / spd;
			}
			u8g2->setDrawColor(0);
			u8g2->drawBox(fromx, toy - 9, tox - fromx + 1, 9);
			u8g2->setDrawColor(1);
			u8g2->drawHLine(fromx + 21, toy - 9, tox - fromx - 20);
			u8g2->drawVLine(tox, toy - 9, 3);
			u8g2->setCursor(tox - 2, toy);
			u8g2->print("0");
			u8g2->drawVLine(fromx + 21, toy - 9, 3);
			u8g2->setCursor(fromx + ((xvalfar >= 0 && xvalfar < 10) ? 20 :
			(xvalfar >= 10 && xvalfar < 100) ? 18 :
			(xvalfar >= 100 && xvalfar < 1000) ? 16 :
			(xvalfar >= 1000 && xvalfar < 10000) ? 14 :
			(xvalfar >= 10000 && xvalfar < 100000) ? 12 :
			(xvalfar >= 100000) ? 10 : 8), toy);
			u8g2->print(xvalfar);
			if (tox - (fromx + 21) > 31) {
				u8g2->drawVLine(tox - ((tox - (fromx + 21)) / 2), toy - 9, 3);
				u8g2->setCursor((tox - ((tox - (fromx + 21)) / 2)) - ((xvalmid >= 0 && xvalmid < 10) ? 1 :
				(xvalmid >= 10 && xvalmid < 100) ? 3 :
				(xvalmid >= 100 && xvalmid < 1000) ? 5 :
				(xvalmid >= 1000 && xvalmid < 10000) ? 7 :
				(xvalmid >= 10000 && xvalmid < 100000) ? 9 :
				(xvalmid >= 100000) ? 11 : 13), toy);
				u8g2->print(xvalmid);
			}
		}

		u8g2->setDrawColor(0);
		u8g2->drawBox(fromx, fromy, 21, (!xaxis) ? toy - fromy : toy - fromy - 9);
		u8g2->setDrawColor(1);

		if (!xaxis) {
			u8g2->drawVLine(fromx + 21, fromy, toy - fromy + 1);
			u8g2->drawHLine(fromx + 15, fromy, 6);
			u8g2->drawHLine(fromx + 15, toy, 6);
			u8g2->drawHLine(fromx + 17, ((toy - fromy) / 2) + fromy, 4);
			u8g2->setCursor(fromx, fromy + 7);
			u8g2->print(maxval, 0);
			u8g2->setCursor(fromx, toy - 2);
			u8g2->print(minval, 0);
		} else {
			u8g2->drawVLine(fromx + 21, fromy, toy - fromy - 8);
			u8g2->drawHLine(fromx + 15, fromy, 6);
			u8g2->drawHLine(fromx + 15, toy - 9, 6);
			u8g2->drawHLine(fromx + 17, ((toy - fromy - 8) / 2) + fromy, 4);
			u8g2->setCursor(fromx, fromy + 7);
			u8g2->print(maxval, 0);
			u8g2->setCursor(fromx, toy - 11);
			u8g2->print(minval, 0);
		}

		if (pointer) {
			for (uint16_t i = 0; i < grwidth; i++) {
				if (graphInt[i][0] == tox - _pointndx) {
					char bfr[6];
					sprintf(bfr, "%i", datasetInt[i]);
					byte valength = u8g2->getStrWidth(bfr) + 2;
					if (graphInt[i][1] > toy - ((!xaxis) ? 16 : 25)) {
						u8g2->drawVLine(graphInt[i][0], graphInt[i][1] - 8, 8);
						if (graphInt[i][0] < fromx + (valength / 2) + 21) {
							u8g2->drawBox(fromx + 22, graphInt[i][1] - 15, valength, 7);
							u8g2->setCursor(fromx + 23, graphInt[i][1] - 9);
							u8g2->setDrawColor(0);
							u8g2->print(bfr);
						} else if (graphInt[i][0] > tox - (valength / 2)) {
							u8g2->drawBox(tox - valength, graphInt[i][1] - 15, valength, 7);
							u8g2->setCursor(tox - valength + 1, graphInt[i][1] - 9);
							u8g2->setDrawColor(0);
							u8g2->print(bfr);
						} else {
							u8g2->drawBox(graphInt[i][0] - (valength / 2), graphInt[i][1] - 15, valength, 7);
							u8g2->setCursor(graphInt[i][0] - ((valength / 2) - 1), graphInt[i][1] - 9);
							u8g2->setDrawColor(0);
							u8g2->print(bfr);
						}
					} else {
						u8g2->drawVLine(graphInt[i][0], graphInt[i][1], 8);
						if (graphInt[i][0] < fromx + (valength / 2) + 21) {
							u8g2->drawBox(fromx + 22, graphInt[i][1] + 8, valength, 7);
							u8g2->setCursor(fromx + 23, graphInt[i][1] + 14);
							u8g2->setDrawColor(0);
							u8g2->print(bfr);
						} else if (graphInt[i][0] > tox - (valength / 2)) {
							u8g2->drawBox(tox - valength, graphInt[i][1] + 8, valength, 7);
							u8g2->setCursor(tox - valength + 1, graphInt[i][1] + 14);
							u8g2->setDrawColor(0);
							u8g2->print(bfr);
						} else {
							u8g2->drawBox(graphInt[i][0] - (valength / 2), graphInt[i][1] + 8, valength, 7);
							u8g2->setCursor(graphInt[i][0] - ((valength / 2) - 1), graphInt[i][1] + 14);
							u8g2->setDrawColor(0);
							u8g2->print(bfr);
						}
					}
					u8g2->setDrawColor(1);
				}
			}
		}
	}
}

//Private floating point map function.
//========================================================================
float U8g2Graphing::fmap(float x, float in_min, float in_max, float out_min, float out_max) {
	return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}
