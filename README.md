# speedy_gonzales
A small unique robot plattform with AVR microcontroller programmed in C.

1	Vorwort
Der Roboter „Speedy-bot“ ist ein kleiner frei in C programmierbarer Lern Roboter. Er soll in der Lage sein, verschiedene Aufgaben selbstständig zu erledigen, dazu gehören das Lösen von Linienlabyrinthen. Dazu ist er mit einer Reihe von 5 analogen IR-Reflektions Sensoren an der Unterseite ausgestattet, sowie zwei optische Motorencoder. Zur Kommunikation wird ein HC-06 Bluetooth Modul verwendet. Als Antrieb dienen zwei DC-Getriebemotoren.
Die Elektronik ist komplett in SMD aufgebaut und als Steuercomputer werkelt ein Atmega644PA. 

2	Aufgaben/ Ziele
Hier eine kleine  Liste mit möglichen Aufgaben die Spirit bewältigen soll:
•	Einfaches Linienfolgen PID Regler mit max. Geschwindigkeit
•	Linienfolgen mit Unterbrechungen, Kreuzungen, Hindernissen, etc…
•	Joghurt Sumo
•	Fernsteuerung über PC/ Smartphone
3	Mechanik
Der Roboter hat die Abmessungen von 100x75 mm und hat zwei Antriebsräder mit zwei Stützrädern vorn und hinten. Sein Gesamtgewicht mit Akkus beläuft sich auf 85g.
