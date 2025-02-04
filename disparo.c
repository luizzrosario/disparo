#include "pico/stdlib.h"
#include "hardware/gpio.h"
#include "hardware/timer.h"
#include "hardware/sync.h"

// Definições dos pinos dos LEDs e do botão
#define LED_VERDE 11  // Pino do LED verde
#define LED_AZUL 12   // Pino do LED azul
#define LED_VERMELHO 13 // Pino do LED vermelho
#define BUTTON 5      // Pino do botão

// Definições de tempo
#define DEBOUNCE_TIME 200 // Tempo de debounce em milissegundos
#define DELAY_TIME 3000   // Tempo entre mudanças de estado dos LEDs em milissegundos

// Variáveis globais
volatile bool leds_active = false; // Indica se os LEDs estão ativos
int led_index = 0;                // Índice para controle dos LEDs
alarm_id_t alarm_id = -1;         // ID do alarme agendado

// Callback para desligar LEDs em ordem crescente
int64_t controlar_leds(alarm_id_t id, void *user_data) {
    // Desliga os LEDs sequencialmente com base no índice atual
    if (led_index == 0) {
        gpio_put(LED_VERDE, 0); // Desliga o LED verde
    } else if (led_index == 1) {
        gpio_put(LED_AZUL, 0); // Desliga o LED azul
    } else if (led_index == 2) {
        gpio_put(LED_VERMELHO, 0); // Desliga o LED vermelho

        // Reseta o estado após desligar todos os LEDs
        uint32_t status = save_and_disable_interrupts(); // Entra em seção crítica
        leds_active = false; // Permite novo acionamento dos LEDs
        led_index = 0;       // Reseta o índice para a próxima ativação
        alarm_id = -1;       // Reseta o ID do alarme
        restore_interrupts(status); // Sai da seção crítica
    }

    // Incrementa o índice e agenda o próximo desligamento, se necessário
    led_index++;
    if (led_index < 3) {
        alarm_id = add_alarm_in_ms(DELAY_TIME, controlar_leds, NULL, false);
    }

    return 0; // Retorno padrão para callbacks de alarme
}

// Callback do botão com debounce
void botao_callback(uint gpio, uint32_t events) {
    static uint32_t ultimo_tempo = 0; // Armazena o tempo do último acionamento
    uint32_t tempo_atual = to_ms_since_boot(get_absolute_time()); // Obtém o tempo atual

    // Verifica o tempo de debounce para evitar acionamentos múltiplos
    if ((tempo_atual - ultimo_tempo) > DEBOUNCE_TIME) {
        ultimo_tempo = tempo_atual; // Atualiza o tempo do último acionamento

        // Verifica se os LEDs já estão ativos
        uint32_t status = save_and_disable_interrupts(); // Entra em seção crítica
        if (!leds_active) {
            leds_active = true; // Marca os LEDs como ativos
            led_index = 0;      // Reseta o índice para o início

            // Acende todos os LEDs
            gpio_put(LED_VERDE, 1);
            gpio_put(LED_AZUL, 1);
            gpio_put(LED_VERMELHO, 1);

            // Cancela qualquer alarme existente antes de agendar um novo
            if (alarm_id != -1) {
                cancel_alarm(alarm_id);
            }

            // Agenda o primeiro desligamento
            alarm_id = add_alarm_in_ms(DELAY_TIME, controlar_leds, NULL, false);
        }
        restore_interrupts(status); // Sai da seção crítica
    }
}

int main() {
    stdio_init_all(); // Inicializa a comunicação serial (para debug, se necessário)

    // Configuração dos LEDs
    gpio_init(LED_VERDE);
    gpio_set_dir(LED_VERDE, GPIO_OUT);
    gpio_put(LED_VERDE, 0); // Garante que o LED verde está desligado no início

    gpio_init(LED_AZUL);
    gpio_set_dir(LED_AZUL, GPIO_OUT);
    gpio_put(LED_AZUL, 0); // Garante que o LED azul está desligado no início

    gpio_init(LED_VERMELHO);
    gpio_set_dir(LED_VERMELHO, GPIO_OUT);
    gpio_put(LED_VERMELHO, 0); // Garante que o LED vermelho está desligado no início

    // Configuração do botão
    gpio_init(BUTTON);
    gpio_set_dir(BUTTON, GPIO_IN);
    gpio_pull_up(BUTTON); // Habilita o resistor de pull-up interno
    gpio_set_irq_enabled_with_callback(BUTTON, GPIO_IRQ_EDGE_FALL, true, &botao_callback);

    // Loop principal
    while (1) {
        tight_loop_contents(); // Mantém o CPU ocupado com uma operação leve
    }
}