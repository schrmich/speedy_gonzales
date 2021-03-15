/**
* @mainpage Speedy Bot Library
*
* <imga src="/20180202_150233.jpg" alt="Speedy-bot">
*
* @version V2
* @section sec1 Einleitung
* Der Roboter „Spirit bot“ ist ein kleiner frei in C programmierbarer Lern Roboter. Er soll in der Lage sein,<br>
* verschiedene Aufgaben selbststaendig zu erledigen. Dazu gehoeren das Loesen von Linienlabyrinthen.<br>

* <br>
*
*
* @subsection sec2_1 Hardware
* Zur detektion von Linien ist er mit einer Reihe von 5 analogen IR-Reflektions Sensoren an der Unterseite ausgestattet.<br>
* Als Antrieb dienen zwei DC-Getriebemotoren.<br>
* Zwei optische Motorencoder dienen der Erfassung von zurück gelegter Entfernung.<br>
* Zur Kommunikation wird ein HC-06 Bluetooth Modul mit 57200 Baud verwendet.<br>
*
* @subsection sec2_2 Verwendete Perepherie
*
* @author Michael Schreiber
*/

/**
 * @file speedy.h
 * @brief Zugriff auf alle Funktionen von Speedy
 * 
 * Diese Datei muss uns Projekt als erstes ins Projekt includiert werden.<br>
 * In dieser Datei sind alle anderen Headerdateien der Library eingebunden.
 */ 


#ifndef SPEEDY_H_
#define SPEEDY_H_

#include "system.h"
#include "uart.h"
#include "timer.h"
#include "motor.h"
//#include "line_sensors.h"
//#include "encoder.h"
//#include "pid.h"
//#include "notes.h"
//#include "sound.h"
#include "scheduler51.h"
//#include "i2cmaster.h"
#include "globals.h"

#define LCD_SHIELD



#endif /* SPEEDY_H_ */