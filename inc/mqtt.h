#ifndef MQTT_H
#define MQTT_H

#include "mqtt_client.h"

// Inicializa y conecta al broker MQTT
int mqtt_conectar(const char *host, int puerto);

// Suscribe a un tópico con un callback asociado
int mqtt_suscribir(const char *topico, callback_t cb);

// Publica un mensaje en un tópico
int mqtt_publicar(const char *topico, const char *mensaje);

// Desuscribe de un tópico
int mqtt_desuscribir(const char *topico);

// Desconecta y libera el cliente MQTT
int mqtt_desconectar();

#endif
