

#ifndef LEDRGB_H_
#define LEDRGB_H_

#define LED_BLUE 	21u
#define LED_RED 	22u
#define LED_GREEN 	26u

typedef enum{
	red,
	green,
	blue,
	purple,
	white,
	yellow,
};


#endif /* LEDRGB_H_ */


void LedBegin();
void LedOff();
void LedColor(int color);
