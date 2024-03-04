/*
 * glucometer.h
 *
 *  Created on: Feb 28, 2024
 *      Author: mcardenas
 */

#ifndef GLUCOMETER_H_
#define GLUCOMETER_H_

#define START_STOP_TOPIC "miguel/practica2/start-stop"
#define CONFIG_TOPIC     "miguel/practica2/config"
#define OXIGEN_TOPIC	 "miguel/practica2/oxigen"
#define HEART_RATE_TOPIC "miguel/practica2/heart-rate"
#define GLUCOSE_TOPIC	 "miguel/practica2/glucose"
#define BATTERY_TOPIC	 "miguel/practica2/battery"
#define START			 "Start"
#define STOP			 "Stop"

typedef enum {
    GLUCOMETER_INIT = 0,
	GLUCOMETER_OFF,
	GLUCOMETER_ON,
	GLUCOMETER_CHANGE_CONFIG,
	GLUCOMETER_MAX_STATE
} glucometer_state_enum;

void glucometer_task(void *arg);
void set_glucometer_state(uint8_t state);
void set_glucometer_period(uint32_t period);

#endif /* GLUCOMETER_H_ */
