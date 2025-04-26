# TSSE - Trabajo práctico número 3

## Consideraciones

De la implementación original se quito el uso de `esp_log.h`.

Como se usaban macros `STAILQ` para manejar la cola se armaron funciones que manejan una lista enlazada para las suscripciones y callbacks. Por lo que he leido `cmock` no se lleva bien con las macros.

Se ha dejado que `ceedling` genere directamente los mocks. Para ello solo se incluye en `test_mqtt.c` el `#include "mock_mqtt_client.h"`.
