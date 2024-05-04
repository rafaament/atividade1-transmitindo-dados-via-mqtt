#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "pico/stdio.h"
#include "pico/cyw43_arch.h"
#include "hardware/adc.h"
#include "hardware/gpio.h"
#include "pico/mutex.h"
#include "pico/sem.h"
#include "pico/stdlib.h"
#include "pico/util/queue.h"
#include "pico/util/heap.h"
#include "pico/util/mutex.h"
#include "hardware/irq.h"
#include "hardware/sync.h"
#include "hardware/uart.h"
#include "pico/time.h"

#include "paho_mqtt.h"

#define MQTT_BROKER getenv("MQTT_BROKER")
#define MQTT_PORT 1883
#define MQTT_TOPIC "temperatura/cpu"

#define WIFI_SSID getenv("WIFI_SSID")
#define WIFI_PASSWORD getenv("WIFI_PASSWORD")

#define BUTTON_PIN 15
#define LED_PIN 25

#define MQTT_CONNECT_TIMEOUT_MS 5000
#define MEASUREMENT_TIMER_MS 5000

float read_onboard_temperature(const char unit) {
    const float conversionFactor = 3.3f / (1 << 12);
    float adc = (float)adc_read() * conversionFactor;
    float tempC = 27.0f - (adc - 0.706f) / 0.001721f;
    if (unit == 'C') {
        return tempC;
    } else if (unit == 'F') {
        return tempC * 9 / 5 + 32;
    }
    return -1.0f;
}

void send_temperature_via_mqtt(float temperature, struct mqtt_client *client) {
    char payload[20];
    snprintf(payload, sizeof(payload), "%.2f", temperature);
    struct mqtt_message msg = {
        .qos = MQTT_QOS_AT_LEAST_ONCE,
        .retained = false,
        .dup = false,
        .payload = payload,
        .payload_len = strlen(payload),
        .topic = MQTT_TOPIC,
    };
    mqtt_publish(client, &msg);
}

bool repeating_timer_callback(struct repeating_timer *t) {
    float temperature = read_onboard_temperature('C');
    printf("Temperatura na CPU: %.2f C\n", temperature);
    struct mqtt_client *client = get_mqtt_client();
    if (client && mqtt_client_is_connected(client)) {
        send_temperature_via_mqtt(temperature, client);
    }
    return true;
}

void button_pressed_handler() {
    float temperature = read_onboard_temperature('C');
    printf("Temperatura no botão pressionado: %.2f C\n", temperature);
    struct mqtt_client *client = get_mqtt_client();
    if (client && mqtt_client_is_connected(client)) {
        send_temperature_via_mqtt(temperature, client);
    }
}

int main() {
    stdio_init_all();
    cyw43_arch_init();

    adc_init();
    adc_set_temp_sensor_enabled(true);
    adc_select_input(4);

    gpio_init(BUTTON_PIN);
    gpio_set_dir(BUTTON_PIN, GPIO_IN);
    gpio_pull_up(BUTTON_PIN);
    gpio_init(LED_PIN);
    gpio_set_dir(LED_PIN, GPIO_OUT);

    gpio_set_irq_enabled_with_callback(BUTTON_PIN, GPIO_IRQ_EDGE_FALL, true, &button_pressed_handler);

    struct repeating_timer timer;
    add_repeating_timer_ms(MEASUREMENT_TIMER_MS, repeating_timer_callback, NULL, &timer);

    mqtt_connect(MQTT_BROKER, MQTT_PORT, MQTT_CONNECT_TIMEOUT_MS, WIFI_SSID, WIFI_PASSWORD);

    while (true) {
        tight_loop_contents();
    }

    return 0;
}
