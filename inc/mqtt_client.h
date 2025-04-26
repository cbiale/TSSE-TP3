// mqtt_client.h - Archivo de cabecera para mockear

#ifndef MQTT_CLIENT_H
#define MQTT_CLIENT_H

#include <stdint.h>

// Definiciones para ESP-IDF
typedef void* esp_mqtt_client_handle_t;
typedef void* esp_event_base_t;

typedef struct esp_mqtt_event_t *esp_mqtt_event_handle_t;

typedef enum {
    MQTT_EVENT_ERROR = 0,
    MQTT_EVENT_CONNECTED,
    MQTT_EVENT_DISCONNECTED,
    MQTT_EVENT_SUBSCRIBED,
    MQTT_EVENT_UNSUBSCRIBED,
    MQTT_EVENT_PUBLISHED,
    MQTT_EVENT_DATA,
    MQTT_EVENT_ANY
} mqtt_event_id_t;


typedef struct esp_mqtt_event_t {
    esp_mqtt_client_handle_t client;   // Cliente MQTT que recibió el evento
    mqtt_event_id_t event_id;           // Tipo de evento
    void *user_context;                 // Contexto del usuario
    void *event_data;                   // Datos asociados al evento
    int data_len;                       // Longitud del payload
    int total_data_len;                 // Longitud total si es fragmentado
    int current_data_offset;            // Offset actual si es fragmentado
    const char *topic;                  // Tópico del mensaje
    int topic_len;                      // Longitud del tópico
    const char *data;                   // Payload recibido
    int msg_id;                         // ID del mensaje (por ejemplo de un publish)
    // Otros campos...
} esp_mqtt_event_t;


typedef enum {
    ESP_OK = 0,
    ESP_FAIL = -1
} esp_err_t;

// Estructura de configuración del cliente MQTT
typedef struct {
    struct {
        struct {
            char *uri;
        } address;
    } broker;
} esp_mqtt_client_config_t;

// Define el tipo de callback usado por el módulo
typedef void (*callback_t)(const char *topico, const char *mensaje);

// Funciones del cliente MQTT para mockear
esp_mqtt_client_handle_t esp_mqtt_client_init(const esp_mqtt_client_config_t *config);
esp_err_t esp_mqtt_client_start(esp_mqtt_client_handle_t client);
esp_err_t esp_mqtt_client_stop(esp_mqtt_client_handle_t client);
esp_err_t esp_mqtt_client_destroy(esp_mqtt_client_handle_t client);
esp_err_t esp_mqtt_client_subscribe(esp_mqtt_client_handle_t client, const char *topic, int qos);
esp_err_t esp_mqtt_client_unsubscribe(esp_mqtt_client_handle_t client, const char *topic);
esp_err_t esp_mqtt_client_publish(esp_mqtt_client_handle_t client, const char *topic, const char *data, int len, int qos, int retain);
esp_err_t esp_mqtt_client_register_event(esp_mqtt_client_handle_t client, int32_t event_id, void* handler, void* handler_args);
esp_err_t esp_mqtt_client_stop(esp_mqtt_client_handle_t client);
esp_err_t esp_mqtt_client_destroy(esp_mqtt_client_handle_t client);


// Pseudo definiciones para ESP_EVENT_ANY_ID
#define ESP_EVENT_ANY_ID -1

#endif // MQTT_CLIENT_H