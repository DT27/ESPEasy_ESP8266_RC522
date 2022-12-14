#ifndef GLOBALS_MQTT_H
#define GLOBALS_MQTT_H

#include "../../ESPEasy_common.h"


#if FEATURE_MQTT

# include <WiFiClient.h>
# include <PubSubClient.h>

#include "../Helpers/LongTermTimer.h"

// MQTT client
extern WiFiClient   mqtt;
extern PubSubClient MQTTclient;
extern bool MQTTclient_should_reconnect;
extern bool MQTTclient_must_send_LWT_connected;
extern bool MQTTclient_connected;
extern int  mqtt_reconnect_count;
extern LongTermTimer MQTTclient_next_connect_attempt;
#endif // if FEATURE_MQTT

#ifdef USES_P037

// mqtt import status
extern bool P037_MQTTImport_connected;
#endif // ifdef USES_P037


#endif // GLOBALS_MQTT_H
