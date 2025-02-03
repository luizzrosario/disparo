#include "pico/stdlib.h"
#include "hardware/gpio.h"
#include "hardware/timer.h"

#define BUTTON_PIN 5
#define LED_GREEN 11
#define LED_BLUE 12
#define LED_RED 13

volatile bool button_pressed = false;
volatile bool sequence_running = false;

// Função de callback para o botão (com debounce)
bool debounce_button() {
    static uint32_t last_press_time = 0;
    uint32_t current_time = to_ms_since_boot(get_absolute_time());
    if (current_time - last_press_time > 200) { // 200ms de debounce
        last_press_time = current_time;
        return true;
    }
    return false;
}

void button_callback(uint gpio, uint32_t events) {
    if (gpio == BUTTON_PIN && events == GPIO_IRQ_EDGE_FALL) {
        if (!sequence_running && debounce_button()) {
            button_pressed = true;
        }
    }
}

// Função de callback para o timer
int64_t led_timer_callback(alarm_id_t id, void *user_data) {
    static int led_state = 0;
    switch (led_state) {
        case 0:
            gpio_put(LED_GREEN, 1);  // Liga LED GREEN
            gpio_put(LED_BLUE, 1);   // Liga LED BLUE
            gpio_put(LED_RED, 1);    // Liga LED RED
            break;
        case 1:
            gpio_put(LED_RED, 0);    // Desliga LED RED
            break;
        case 2:
            gpio_put(LED_BLUE, 0);   // Desliga LED BLUE
            break;
        case 3:
            gpio_put(LED_GREEN, 0);  // Desliga LED GREEN
            sequence_running = false; // Finaliza a sequência
            break;
    }
    led_state++;
    if (led_state < 4) {
        add_alarm_in_ms(3000, led_timer_callback, NULL, false); // Próximo estado após 3 segundos
    }
    return 0;
}

int main() {
    stdio_init_all();
    
    gpio_init(BUTTON_PIN);
    gpio_set_dir(BUTTON_PIN, GPIO_IN);
    gpio_pull_up(BUTTON_PIN);
    gpio_set_irq_enabled_with_callback(BUTTON_PIN, GPIO_IRQ_EDGE_FALL, true, &button_callback);
    
    gpio_init(LED_GREEN);
    gpio_set_dir(LED_GREEN, GPIO_OUT);
    gpio_put(LED_GREEN, 0);
    
    gpio_init(LED_BLUE);
    gpio_set_dir(LED_BLUE, GPIO_OUT);
    gpio_put(LED_BLUE, 0);
    
    gpio_init(LED_RED);
    gpio_set_dir(LED_RED, GPIO_OUT);
    gpio_put(LED_RED, 0);
    
    while (1) {
        if (button_pressed && !sequence_running) {
            button_pressed = false;     // Reseta a variável de controle do botão
            sequence_running = true;    // Marca que a sequência começou
            add_alarm_in_ms(3000, led_timer_callback, NULL, false); // Inicia a sequência de LEDs
        }
        sleep_ms(10);  // Aguarda um curto período de tempo antes de verificar novamente
    }
}
