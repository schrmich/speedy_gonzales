/*
 * timer.h
 *
 * Created: 01.06.2019 16:14:14
 *  Author: mschr
 */ 


#ifndef TIMER_H_
#define TIMER_H_

#include <stdint.h>

/*
 * 16bit Stopwatches --> maximum value is 65535 or ~65.535 seconds!
 */

#define STOPWATCH1 1
#define STOPWATCH2 2
#define STOPWATCH3 4
#define STOPWATCH4 8
#define STOPWATCH5 16	// 
#define STOPWATCH6 32	// 
#define STOPWATCH7 64	// 
#define STOPWATCH8 128	// Reserviert für Motor Rotate Funktion Autodrive

typedef struct {
	uint8_t watches;
	uint16_t watch1;
	uint16_t watch2;
	uint16_t watch3;
	uint16_t watch4;
	uint16_t watch5;
	uint16_t watch6;
	uint16_t watch7;
	uint16_t watch8;
} stopwatches_t;

extern volatile stopwatches_t stopwatches;

void setStopwatch(uint16_t watch, uint16_t timer);
uint16_t getStopwatch(uint8_t watch);

#define stopStopwatch1() stopwatches.watches &= ~STOPWATCH1
#define stopStopwatch2() stopwatches.watches &= ~STOPWATCH2
#define stopStopwatch3() stopwatches.watches &= ~STOPWATCH3
#define stopStopwatch4() stopwatches.watches &= ~STOPWATCH4
#define stopStopwatch5() stopwatches.watches &= ~STOPWATCH5
#define stopStopwatch6() stopwatches.watches &= ~STOPWATCH6
#define stopStopwatch7() stopwatches.watches &= ~STOPWATCH7
#define stopStopwatch8() stopwatches.watches &= ~STOPWATCH8

#define startStopwatch1() stopwatches.watches |= STOPWATCH1
#define startStopwatch2() stopwatches.watches |= STOPWATCH2
#define startStopwatch3() stopwatches.watches |= STOPWATCH3
#define startStopwatch4() stopwatches.watches |= STOPWATCH4
#define startStopwatch5() stopwatches.watches |= STOPWATCH5
#define startStopwatch6() stopwatches.watches |= STOPWATCH6
#define startStopwatch7() stopwatches.watches |= STOPWATCH7
#define startStopwatch8() stopwatches.watches |= STOPWATCH8

#define isStopwatch1Running() (stopwatches.watches & STOPWATCH1)
#define isStopwatch2Running() (stopwatches.watches & STOPWATCH2)
#define isStopwatch3Running() (stopwatches.watches & STOPWATCH3)
#define isStopwatch4Running() (stopwatches.watches & STOPWATCH4)
#define isStopwatch5Running() (stopwatches.watches & STOPWATCH5)
#define isStopwatch6Running() (stopwatches.watches & STOPWATCH6)
#define isStopwatch7Running() (stopwatches.watches & STOPWATCH7)
#define isStopwatch8Running() (stopwatches.watches & STOPWATCH8)


#endif /* TIMER_H_ */