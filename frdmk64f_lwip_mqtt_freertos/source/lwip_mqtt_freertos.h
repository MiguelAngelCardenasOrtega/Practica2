/*
 * lwip_mqtt_freertos.h
 *
 *  Created on: Feb 28, 2024
 *      Author: mcardenas
 */

#ifndef LWIP_MQTT_FREERTOS_H_
#define LWIP_MQTT_FREERTOS_H_

#include "lwip/apps/mqtt.h"

void connect_to_mqtt(void *ctx);

mqtt_connection_status_t get_mqtt_status();
void publish_message(void *ctx);

#endif /* LWIP_MQTT_FREERTOS_H_ */
