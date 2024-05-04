#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "pico/stdio.h"
#include "pico/cyw43_arch.h"
#include "hardware/adc.h"
#include "hardware/gpio.h"

/* Incluir as bibliotecas MQTT */
#include "pico/mutex.h"
#include "pico/sem.h"
#include "pico/stdlib.h"
#include "pico/util/queue.h"
#include "pico/util/heap.h"
#include "pico/util/mutex.h"
#include "hardware/irq.h"
#include "hardware/sync.h"
#include "hardware/uart.h"

/* Definir configurações MQTT */
#define MQTT_BROKER getenv("MQTT_BROKER")
#define MQTT_PORT 1883
#define MQTT_TOPIC "temperatura/cpu"

/* Definir informações de rede */
#define WIFI_SSID getenv("WIFI_SSID")
#define WIFI_PASSWORD getenv("WIFI_PASSWORD")

/* Definir pinos de botão e LED */
#define BUTTON_PIN 15
#define LED_PIN 25

/* Tempo de espera para a conexão MQTT (em milissegundos) */
#define MQTT_CONNECT_TIMEOUT_MS 5000

float read_onboard_temperature(const char unit) {
    /* 12-bit conversion, assume max value == ADC_VREF == 3.3 V */
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

void send_temperature_via_mqtt(float temperature) {
    /* Implemente a lógica para enviar a temperatura via MQTT */
    printf("Temperatura enviada via MQTT: %.2f C\n", temperature);
}

bool repeating_timer_callback(struct repeating_timer *t) {
    printf("Temperatura na CPU: %.2f C\n", read_onboard_temperature('C'));
    return true;
}

void button_pressed_handler() {
    float temperature = read_onboard_temperature('C');
    send_temperature_via_mqtt(temperature);
}

int main() {
    stdio_init_all();
    cyw43_arch_init();

    adc_init();
    adc_set_temp_sensor_enabled(true);
    adc_select_input(4);

    /* Configurar pino do botão como entrada e pino do LED como saída */
    gpio_init(BUTTON_PIN);
    gpio_set_dir(BUTTON_PIN, GPIO_IN);
    gpio_pull_up(BUTTON_PIN);
    gpio_init(LED_PIN);
    gpio_set_dir(LED_PIN, GPIO_OUT);

    /* Definir interrupção para detectar pressionamento do botão */
    gpio_set_irq_enabled_with_callback(BUTTON_PIN, GPIO_IRQ_EDGE_FALL, true, &button_pressed_handler);

    struct repeating_timer timer;

    /* Adicionar temporizador para leitura periódica da temperatura */
    add_repeating_timer_ms(MEASUREMENT_TIMER_MS, repeating_timer_callback, NULL, &timer);

    /* Loop principal */
    while (true) {
        tight_loop_contents();
    }

    return 0;
}
