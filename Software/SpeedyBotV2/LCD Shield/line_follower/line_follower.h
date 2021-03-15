/*
 * line_follower.h
 *
 * Created: 27.08.2020 18:43:53
 *  Author: mschr
 */ 


#ifndef LINE_FOLLOWER_H_
#define LINE_FOLLOWER_H_

#include <stdint.h>


extern int16_t baseSpeed;
extern float k_p, k_i, k_d;

void task_pid_line_follow_main(void);

#endif /* LINE_FOLLOWER_H_ */