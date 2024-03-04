/*
 * glucometer.c
 *
 *  Created on: Feb 28, 2024
 *      Author: mcardenas
 */

#include "lwip/opt.h"
#include "lwip/api.h"
#include "lwip/apps/mqtt.h"
#include "lwip/dhcp.h"
#include "lwip/netdb.h"
#include "lwip/netifapi.h"
#include "lwip/prot/dhcp.h"
#include "lwip/tcpip.h"
#include "lwip/timeouts.h"
#include "netif/ethernet.h"
#include "enet_ethernetif.h"
#include "lwip_mqtt_id.h"
#include "stdio.h"
#include "glucometer.h"
#include "lwip_mqtt_freertos.h"
#include "timers.h"
#include "semphr.h"

uint8_t state_names[][7] = { "INIT",
							 "OFF",
							 "ON",
							 "CONFIG"};

uint8_t glucometer_state = GLUCOMETER_INIT;
uint32_t measurement_period = 1; //This variable is in seconds
uint16_t glucose_level = 0; //This is in mg per dL
uint8_t oxigen_saturation = 0; //This is in percentage
uint8_t heart_rate = 0; //This is in bpm
uint8_t battery_level = 0; //This is in percentage

void measurement_timer_cb( TimerHandle_t xTimer )
{
	err_t err = tcpip_callback(publish_message, NULL);
	if (err != ERR_OK)
	{
		PRINTF("Failed to invoke publishing of a message on the tcpip_thread: %d.\r\n", err);
	}
}

void set_glucometer_state(uint8_t state)
{
	PRINTF("Setting Glucometer to %s state\r\n\r\n", state_names[state]);
	glucometer_state = state;
}

void set_glucometer_period(uint32_t period)
{
	measurement_period = period;
}

void mqtt_init(void)
{
	/* Start connecting to MQTT broker from tcpip_thread */
	err_t err = tcpip_callback(connect_to_mqtt, NULL);
	if (err != ERR_OK)
	{
		PRINTF("Failed to invoke broker connection on the tcpip_thread: %d.\r\n", err);
	}

	while (get_mqtt_status() != MQTT_CONNECT_ACCEPTED)
	{
		vTaskDelay(pdMS_TO_TICKS(50));
	}
}

/*!
 * @brief Glucometer APP task.
 *
 * @param arg unused
 */
void glucometer_task(void *arg)
{
	TimerHandle_t measurement_timer;
	SemaphoreHandle_t measurement_semaphore;

	measurement_timer = xTimerCreate("Measurement Timer", pdMS_TO_TICKS(measurement_period*1000), pdTRUE, ( void * ) 0, measurement_timer_cb);
	measurement_semaphore = xSemaphoreCreateBinary(); //Semaphore is created empty

	while (true)
	{
		switch (glucometer_state)
		{
			case GLUCOMETER_INIT:
				mqtt_init();
				glucometer_state = GLUCOMETER_OFF;
				break;

			case GLUCOMETER_OFF:

				if (xTimerIsTimerActive(measurement_timer) != pdFALSE)
				{
					xTimerStop(measurement_timer, portMAX_DELAY);
				}
				else
				{
					vTaskDelay(pdMS_TO_TICKS(10));
				}
				break;

			case GLUCOMETER_ON:

				if (xTimerIsTimerActive(measurement_timer) == pdFALSE)
				{
					xTimerStart(measurement_timer, portMAX_DELAY);
				}
				else
				{
					vTaskDelay(pdMS_TO_TICKS(10));
				}
				break;

			case GLUCOMETER_CHANGE_CONFIG:
				if (xTimerIsTimerActive(measurement_timer) != pdFALSE)
				{
					xTimerChangePeriod(measurement_timer, measurement_period * 1000, portMAX_DELAY);
					glucometer_state = GLUCOMETER_ON;
				}
				else
				{
					xTimerChangePeriod(measurement_timer, measurement_period * 1000, portMAX_DELAY);
					xTimerStop(measurement_timer, portMAX_DELAY);
					glucometer_state = GLUCOMETER_OFF;
				}
				break;

			default:
				PRINTF("ERROR INVALID STATE!\r\n\r\n");
				break;
		}
	}
}


