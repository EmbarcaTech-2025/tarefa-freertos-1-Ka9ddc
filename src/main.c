#include "FreeRTOS.h"
#include "task.h"
#include <stdio.h>
#include "pico/stdlib.h"

#include "hardware/i2c.h"

// Definição dos pinos da bitdoglab
const uint LED_G_PIN = 11;
const uint LED_B_PIN = 12;
const uint LED_R_PIN = 13;
const uint BUZZER_PIN = 10;
const uint BTN_A_PIN = 5;
const uint BTN_B_PIN = 6;

// Definição dos handles para as tasks

TaskHandle_t led_task_handle = NULL;
TaskHandle_t buzzer_task_handle = NULL;

// Função com a task para controlar o led rgb, alterna as cores do led entre vermelho, verde e azul a cada 500ms.
void led_task(void *pvParameters) {
    // Configurando os pinos dos leds como saída
    gpio_init(LED_R_PIN);
    gpio_init(LED_G_PIN);
    gpio_init(LED_B_PIN);
    gpio_set_dir(LED_R_PIN, GPIO_OUT);
    gpio_set_dir(LED_G_PIN, GPIO_OUT);
    gpio_set_dir(LED_B_PIN, GPIO_OUT);

    while (1) {
        // Aciona o LED Vermelho
        gpio_put(LED_R_PIN, 1);
        gpio_put(LED_G_PIN, 0);
        gpio_put(LED_B_PIN, 0);
        vTaskDelay(pdMS_TO_TICKS(500)); // Delay de 500ms

        // Aciona o LED Verde
        gpio_put(LED_R_PIN, 0);
        gpio_put(LED_G_PIN, 1);
        gpio_put(LED_B_PIN, 0);
        vTaskDelay(pdMS_TO_TICKS(500)); // Delay de 500ms

        // Aciona o LED Azul
        gpio_put(LED_R_PIN, 0);
        gpio_put(LED_G_PIN, 0);
        gpio_put(LED_B_PIN, 1);
        vTaskDelay(pdMS_TO_TICKS(500)); // Delay de 500ms
    }
}

// Função com a task para controlar o buzzer, emitindo um bipe curto a cada 1 segundo.
void buzzer_task(void *pvParameters) {
    // Configura o pino do buzzer como saída
    gpio_init(BUZZER_PIN);
    gpio_set_dir(BUZZER_PIN, GPIO_OUT);

    while (1) {
        // Emite um bipe de 100ms
        gpio_put(BUZZER_PIN, 1);
        vTaskDelay(pdMS_TO_TICKS(100));
        gpio_put(BUZZER_PIN, 0);

        // Aguarda o restante do período de 1 segundo
        vTaskDelay(pdMS_TO_TICKS(900));
    }
}

// Função da task para monitorar os botões, ela faz um polling dos botões a cada 100ms.
// Caso o botão A esteja pressionado, suspende a tarefa do led, caso o botão B esteja pressionado, suspende a tarefa do buzzer.
void button_task(void *pvParameters) {
    // Configura os pinos dos botões como entrada com pull-up interno
    gpio_init(BTN_A_PIN);
    gpio_set_dir(BTN_A_PIN, GPIO_IN);
    gpio_pull_up(BTN_A_PIN);

    gpio_init(BTN_B_PIN);
    gpio_set_dir(BTN_B_PIN, GPIO_IN);
    gpio_pull_up(BTN_B_PIN);

    // Variáveis para guardar o estado anterior e evitar chamadas repetidas
    bool botao_a_pressionado_anteriormente = false;
    bool botao_b_pressionado_anteriormente = false;

    while (1) {
        // Botão A pressionado
        bool botao_a_pressionado = !gpio_get(BTN_A_PIN);

        if (botao_a_pressionado && !botao_a_pressionado_anteriormente) {
            printf("Botão A pressionado: Suspendendo tarefa do LED.\n");
            vTaskSuspend(led_task_handle);
        } else if (!botao_a_pressionado && botao_a_pressionado_anteriormente) {
            printf("Botão A solto: Retomando tarefa do LED.\n");
            vTaskResume(led_task_handle);
        }
        botao_a_pressionado_anteriormente = botao_a_pressionado;

        // Botão B pressionado
        bool botao_b_pressionado = !gpio_get(BTN_B_PIN);

        if (botao_b_pressionado && !botao_b_pressionado_anteriormente) {
            printf("Botão B pressionado: Suspendendo tarefa do Buzzer.\n");
            vTaskSuspend(buzzer_task_handle);
        } else if (!botao_b_pressionado && botao_b_pressionado_anteriormente) {
            printf("Botão B solto: Retomando tarefa do Buzzer.\n");
            vTaskResume(buzzer_task_handle);
        }
        botao_b_pressionado_anteriormente = botao_b_pressionado;

        // Polling de 100ms antes da próxima verificação.
        vTaskDelay(pdMS_TO_TICKS(100));
    }
}


int main() {
    stdio_init_all();
    printf("Iniciando sistema multitarefa com FreeRTOS...\n");

    // Cria a tarefa do led
    xTaskCreate(led_task, "LED_Task", 128, NULL, 1, &led_task_handle);

    // Cria a tarefa do buzzer
    xTaskCreate(buzzer_task, "Buzzer_Task", 128, NULL, 1, &buzzer_task_handle);

    // Cria a tarefa dos botões com maior prioridade para garantir a resposta rápida
    xTaskCreate(button_task, "Button_Task", 256, NULL, 2, NULL);

    // Inicia o agendador de tarefas do FreeRTOS
    vTaskStartScheduler();

    // Loop infinito
    while (1) {
    };
}