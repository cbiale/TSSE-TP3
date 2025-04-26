#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "mqtt.h"

// Cliente MQTT
esp_mqtt_client_handle_t cliente;

// tipo de dato para la lista de suscripciones
typedef struct mqtt_sub {
    char *topico;        // Tópico al que está suscrito
    callback_t callback; // Callback asociado al tópico
    struct mqtt_sub *next; // Puntero al siguiente nodo
} mqtt_sub_t;

static mqtt_sub_t *head = NULL; // La lista está inicialmente vacía

// agrega un callback a la lista de suscripciones
static int agregar_callback(const char *topico, callback_t callback) {
    // Verifica si el tópico ya existe en la lista
    mqtt_sub_t *actual = head;
    while (actual != NULL) {
        if (strcmp(actual->topico, topico) == 0) {
            // El tópico ya existe, devuelve 0
            // modifica el callback
            actual->callback = callback;
            return 0;
        }
        actual = actual->next;
    }
    // Si no se encuentra, crea un nuevo nodo
    mqtt_sub_t *nuevo_nodo = malloc(sizeof(mqtt_sub_t));
    if (nuevo_nodo == NULL) {
        // Manejo de error por falta de memoria
        return 1;
    }
    nuevo_nodo->topico = strdup(topico); // Copia el tópico
    if (nuevo_nodo->topico == NULL) {
        // Manejo de error por falta de memoria
        free(nuevo_nodo);
        return 1;
    }
    nuevo_nodo->callback = callback;
    nuevo_nodo->next = head; // Inserta al principio de la lista
    head = nuevo_nodo;
    return 0; // Indica que se agregó correctamente
}

// quita un callback de la lista de suscripciones
static int quitar_callback(const char *topico) {
    mqtt_sub_t *actual = head;
    mqtt_sub_t *anterior = NULL;
    // Recorre la lista para buscar el tópico
    while (actual != NULL) {
        if (strcmp(actual->topico, topico) == 0) {
            // Si el tópico se encuentra, elimina el nodo
            if (anterior == NULL) {
                // El nodo a eliminar es el primero
                head = actual->next;
            } else {
                // El nodo a eliminar está en el medio o al final
                anterior->next = actual->next;
            }
            // Libera la memoria del nodo eliminado
            free(actual->topico);
            free(actual);
            return 0; // Indica que se eliminó correctamente
        }
        // Avanza al siguiente nodo
        anterior = actual;
        actual = actual->next;
    }
    return 1; // Indica que el tópico no se encontró
}

// una funcion recibe un topico, recorre la lista de suscripciones
// si existe el topico devuelve el callback, sino NULL
static callback_t obtener_callback(const char *topico) {
    mqtt_sub_t *actual = head;
    // Recorre la lista para buscar el tópico
    while (actual != NULL) {
        if (strcmp(actual->topico, topico) == 0) {
            // Si el tópico se encuentra, devuelve el callback
            return actual->callback;
        }
        // Avanza al siguiente nodo
        actual = actual->next;
    }
    return NULL; // Indica que no se encontró el callback
}

// Dispatcher global para eventos MQTT
static void mqtt_event_handler(void *handler_args, esp_event_base_t base, int32_t event_id, void *event_data) {
    esp_mqtt_event_handle_t evento = event_data;

    if (evento->event_id == MQTT_EVENT_DATA) {
        char topico[evento->topic_len + 1];
        char mensaje[evento->data_len + 1];
        memcpy(topico, evento->topic, evento->topic_len);
        memcpy(mensaje, evento->data, evento->data_len);
        topico[evento->topic_len] = '\0';
        mensaje[evento->data_len] = '\0';

        // Verifica si el tópico está registrado
        // si esta registrado llama al callback asociado
        callback_t callback = obtener_callback(topico);
        if (callback != NULL) {
            callback(topico, mensaje);
            return;
        }
    }
}

// Conecta al broker MQTT 
// devuelve 0 si se conecta
// devuelve 1 en caso de error 
int mqtt_conectar(const char *host, int puerto) {
    // Verifica si el cliente ya está conectado
    if (cliente != NULL) {
        mqtt_desconectar();
    }

    // Armado de la URI
    char uri[512];
    snprintf(uri, sizeof(uri), "mqtt://%s:%d", host, puerto);

    // Datos de configuración
    esp_mqtt_client_config_t mqtt_cfg = {
        .broker.address.uri = uri,
    };

    // Crea un manejador del cliente MQTT
    cliente = esp_mqtt_client_init(&mqtt_cfg);
    if (cliente == NULL) {
        return 1;
    }

    // Inicia el cliente MQTT con el manejador creado
    esp_err_t err = esp_mqtt_client_start(cliente);
    if (err != ESP_OK) {
        return 1;
    }

    // Registra el manejador de eventos del cliente
    esp_mqtt_client_register_event(cliente, ESP_EVENT_ANY_ID, mqtt_event_handler, NULL);
    return 0;
}

// Suscribe a un tópico con un callback asociado
// devuelve 0 si es correcto, 1 en caso de error
int mqtt_suscribir(const char *topico, callback_t cb) {
    if (cliente == NULL) {
        return 1;
    }

    esp_err_t err = esp_mqtt_client_subscribe(cliente, topico, 0);
    if (err != ESP_OK) {
        return 1;
    }
    // agrega el callback 
    if (agregar_callback(topico, cb) != 0) {
        return 1;
    }

    return 0;
}

// Publica un mensaje en un tópico
// devuelve 0 si es correcto, 1 en caso de error
int mqtt_publicar(const char *topico, const char *mensaje) {
    // Verifica si el cliente está conectado
    if (cliente == NULL) {
        return 1;
    }

    // Publicar el mensaje en el tópico
    esp_err_t err = esp_mqtt_client_publish(cliente, topico, mensaje, 0, 1, 0);
    if (err != ESP_OK) {
        return 1;
    }
    return 0;
}

// Desuscribe de un tópico
// devuelve 0 si es correcto, 1 en caso de error
int mqtt_desuscribir(const char *topico) {
    // Verifica si el cliente está conectado
    if (cliente == NULL) {
        return 1;
    }

    // Desuscribirse del tópico
    esp_err_t err = esp_mqtt_client_unsubscribe(cliente, topico);
    if (err != ESP_OK) {
        return 1;
    }

    // Quita el callback asociado al tópico
    if (quitar_callback(topico) != 0) {
        return 1;
    }
    return 0;
}

// Desconecta y libera el cliente MQTT
// devuelve 0 si es correcto, 1 en caso de error
int mqtt_desconectar() {
    // Verifica si el cliente está conectado
    if (cliente != NULL) {
        esp_mqtt_client_stop(cliente);
        esp_mqtt_client_destroy(cliente);
        // libera la lista de suscripciones
        mqtt_sub_t *actual = head;
        while (actual != NULL) {
            mqtt_sub_t *temp = actual;
            actual = actual->next;
            free(temp->topico);
            free(temp);
        }
    }
    return 1;
}
