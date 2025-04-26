#include "unity.h"
#include "mqtt.h"
#include "mock_mqtt_client.h"

// Variables y estructuras necesarias para las pruebas
extern esp_mqtt_client_handle_t cliente;

void setUp(void) {
    cliente = NULL;
}

void tearDown(void) {
}

// Tests de mqtt_conectar

// Test de conexión exitosa
void test_mqtt_conectar_exito(void) {
    esp_mqtt_client_init_IgnoreAndReturn((void*)0x1234);
    esp_mqtt_client_start_IgnoreAndReturn(ESP_OK);
    esp_mqtt_client_register_event_IgnoreAndReturn(ESP_OK);

    int valor = mqtt_conectar("test.mosquitto.org", 1883);
    // Verifica que la conexión fue exitosa
    TEST_ASSERT_EQUAL(0, valor);
}

// Test de conexión fallida
void test_mqtt_conectar_falla_init(void) {
    esp_mqtt_client_init_IgnoreAndReturn(NULL);

    int valor = mqtt_conectar("test.mosquitto.org", 1883);
    // Verifica que la conexión falló
    TEST_ASSERT_EQUAL(1, valor);
}

// Test de conexión fallida al iniciar el cliente
void test_mqtt_conectar_falla_start(void) {
    esp_mqtt_client_init_IgnoreAndReturn((void*)0x1234);
    esp_mqtt_client_start_IgnoreAndReturn(ESP_FAIL);

    int valor = mqtt_conectar("test.mosquitto.org", 1883);
    // Verifica que la conexión falló
    TEST_ASSERT_EQUAL(1, valor);
}

// Tests de mqtt_suscribir

// Test de suscripción exitosa
void test_mqtt_suscribir_exito(void) {
    esp_mqtt_client_init_IgnoreAndReturn((void*)0x1234);
    esp_mqtt_client_start_IgnoreAndReturn(ESP_OK);
    esp_mqtt_client_register_event_IgnoreAndReturn(ESP_OK);
    int resultado = mqtt_conectar("test.org", 1883);
    TEST_ASSERT_EQUAL(0, resultado);

    esp_mqtt_client_subscribe_IgnoreAndReturn(ESP_OK);
    int valor = mqtt_suscribir("sensor/temperatura", (callback_t)0x1234);
    TEST_ASSERT_EQUAL(0, valor);
}

// Test de suscripción fallida
void test_mqtt_suscribir_falla_subscribe(void) {
    esp_mqtt_client_init_IgnoreAndReturn((void*)0x1234);
    esp_mqtt_client_start_IgnoreAndReturn(ESP_OK);
    esp_mqtt_client_register_event_IgnoreAndReturn(ESP_OK);
    int resultado = mqtt_conectar("test.org", 1883);
    TEST_ASSERT_EQUAL(0, resultado);

    esp_mqtt_client_subscribe_IgnoreAndReturn(ESP_FAIL);
    int valor = mqtt_suscribir("sensor/humedad", (callback_t)0x1234);
    TEST_ASSERT_EQUAL(1, valor);
}

// Tests de mqtt_publicar

// Test de publicación exitosa
void test_mqtt_publicar_exito(void) {
    esp_mqtt_client_init_IgnoreAndReturn((void*)0x1234);
    esp_mqtt_client_start_IgnoreAndReturn(ESP_OK);
    esp_mqtt_client_register_event_IgnoreAndReturn(ESP_OK);
    int resultado = mqtt_conectar("test.org", 1883);
    TEST_ASSERT_EQUAL(0, resultado);

    esp_mqtt_client_publish_IgnoreAndReturn(ESP_OK);
    int valor = mqtt_publicar("sensor/temperatura", "25");
    TEST_ASSERT_EQUAL(0, valor);
}

// Test de publicación fallida
void test_mqtt_publicar_falla_publish(void) {
    esp_mqtt_client_init_IgnoreAndReturn((void*)0x1234);
    esp_mqtt_client_start_IgnoreAndReturn(ESP_OK);
    esp_mqtt_client_register_event_IgnoreAndReturn(ESP_OK);
    int resultado = mqtt_conectar("test.org", 1883);
    TEST_ASSERT_EQUAL(0, resultado);

    esp_mqtt_client_publish_IgnoreAndReturn(ESP_FAIL);
    int valor = mqtt_publicar("sensor/humedad", "70");
    TEST_ASSERT_EQUAL(1, valor);
}

// Tests de mqtt_desuscribir

// Test de desuscripción exitosa
void test_mqtt_desuscribir_exito(void) {
    esp_mqtt_client_init_IgnoreAndReturn((void*)0x1234);
    esp_mqtt_client_start_IgnoreAndReturn(ESP_OK);
    esp_mqtt_client_register_event_IgnoreAndReturn(ESP_OK);
    int resultado = mqtt_conectar("test.org", 1883);
    TEST_ASSERT_EQUAL(0, resultado);

    esp_mqtt_client_unsubscribe_IgnoreAndReturn(ESP_OK);
    int valor = mqtt_desuscribir("sensor/temperatura");
    TEST_ASSERT_EQUAL(0, valor);
}

// Test de desuscripción fallida
void test_mqtt_desuscribir_falla_unsubscribe(void) {
    esp_mqtt_client_init_IgnoreAndReturn((void*)0x1234);
    esp_mqtt_client_start_IgnoreAndReturn(ESP_OK);
    esp_mqtt_client_register_event_IgnoreAndReturn(ESP_OK);
    int resultado = mqtt_conectar("test.org", 1883);
    TEST_ASSERT_EQUAL(0, resultado);

    esp_mqtt_client_unsubscribe_IgnoreAndReturn(ESP_FAIL);
    int valor = mqtt_desuscribir("sensor/humedad");
    TEST_ASSERT_EQUAL(1, valor);
}

// Tests de mqtt_desconectar

// Test de desconexión exitosa
void test_mqtt_desconectar_exito(void) {
    esp_mqtt_client_init_IgnoreAndReturn((void*)0x1234);
    esp_mqtt_client_start_IgnoreAndReturn(ESP_OK);
    esp_mqtt_client_register_event_IgnoreAndReturn(ESP_OK);
    int resultado = mqtt_conectar("test.org", 1883);
    TEST_ASSERT_EQUAL(0, resultado);

    esp_mqtt_client_stop_IgnoreAndReturn(ESP_OK);
    esp_mqtt_client_destroy_IgnoreAndReturn(ESP_OK);
    int valor = mqtt_desconectar();
    TEST_ASSERT_EQUAL(0, valor);
}


// Test de desconexión sin cliente
void test_mqtt_desconectar_sin_cliente(void) {
    int valor = mqtt_desconectar();
    // Verifica que la desconexión fue exitosa
    TEST_ASSERT_EQUAL(1, valor);
}
