#include "pico/stdlib.h"
#include <stdio.h>
#include <math.h>
#include "hardware/pio.h"
#include "hardware/clocks.h"
#include "hardware/adc.h"
#include "hardware/timer.h"
#include "pico/bootrom.h"
#include "pio_matrix.pio.h"

// Define a luminosidade dos LEDs
#define L (0.05)

// Define o número de LEDs na matriz e o pino de dados conectado à matriz
#define NUM_LEDS 25
#define OUT_PIN 7

// Define os pinos para o LED vermelho e os botões A e B
#define RED 13
#define buton_A 5
#define buton_B 6

// Variáveis globais para contagem e controle de tempo
static volatile uint16_t contador = 0;
static volatile uint32_t last_time = 0;

// Protótipos das funções
void initialize_gpio();
void numeros(double *desenho, uint32_t valor_led, PIO pio, uint sm, double r, double g, double b);
void gpio_irq_handler(uint gpio, uint32_t events);

// Arrays que representam os desenhos dos números de 0 a 9 em uma matriz 5x5
double numero0[25] = {0.0, L, L, L, 0.0,
                      0.0, L, 0.0, L, 0.0,
                      0.0, L, 0.0, L, 0.0,
                      0.0, L, 0.0, L, 0.0,
                      0.0, L, L, L, 0.0};
double numero1[25] = {0.0, L, L, 0.0, 0.0,
                      0.0, 0.0, L, 0.0, 0.0,
                      0.0, 0.0, L, 0.0, 0.0,
                      0.0, 0.0, L, 0.0, 0.0,
                      0.0, 0.0, L, 0.0, 0.0};
double numero2[25] = {0.0, L, L, L, 0.0,
                      0.0, L, 0.0, 0.0, 0.0,
                      0.0, L, L, L, 0.0,
                      0.0, 0.0, 0.0, L, 0.0,
                      0.0, L, L, L, 0.0};
double numero3[25] = {0.0, L, L, L, 0.0,
                      0.0, L, 0.0, 0.0, 0.0,
                      0.0, L, L, L, 0.0,
                      0.0, L, 0.0, 0.0, 0.0,
                      0.0, L, L, L, 0.0};
double numero4[25] = {0.0, L, 0.0, L, 0.0,
                      0.0, L, 0.0, L, 0.0,
                      0.0, L, L, L, 0.0,
                      0.0, L, 0.0, 0.0, 0.0,
                      0.0, 0.0, 0.0, L, 0.0};
double numero5[25] = {0.0, L, L, L, 0.0,
                      0.0, 0.0, 0.0, L, 0.0,
                      0.0, L, L, L, 0.0,
                      0.0, L, 0.0, 0.0, 0.0,
                      0.0, L, L, L, 0.0};

double numero6[25] = {0.0, L, L, L, 0.0,
                      0.0, 0.0, 0.0, L, 0.0,
                      0.0, L, L, L, 0.0,
                      0.0, L, 0.0, L, 0.0,
                      0.0, L, L, L, 0.0};

double numero7[25] = {0.0, L, L, L, 0.0,
                      0.0, L, 0.0, 0.0, 0.0,
                      0.0, 0.0, L, 0.0, 0.0,
                      0.0, 0.0, L, 0.0, 0.0,
                      0.0, 0.0, L, 0.0, 0.0};

double numero8[25] = {0.0, L, L, L, 0.0,
                      0.0, L, 0.0, L, 0.0,
                      0.0, L, L, L, 0.0,
                      0.0, L, 0.0, L, 0.0,
                      0.0, L, L, L, 0.0};

double numero9[25] = {0.0, L, L, L, 0.0,
                      0.0, L, 0.0, L, 0.0,
                      0.0, L, L, L, 0.0,
                      0.0, L, 0.0, 0.0, 0.0,
                      0.0, L, L, L, 0.0};

// Função para converter valores de cor RGB em um valor de 32 bits para a matriz de LEDs
uint32_t matrix_rgb(float r, float g, float b)
{
    unsigned char R, G, B;
    R = r * 255;
    G = g * 255;
    B = b * 255;

    return (G << 24) | (R << 16) | (B << 8);
}

// Função principal
int main()
{
    PIO pio = pio0;
    bool ok;
    uint16_t i;
    uint32_t valor_led;
    double r = 0.0, b = 0.0, g = 0.0;

    // Configura o clock do sistema para 128 MHz
    ok = set_sys_clock_khz(128000, false);
    if (ok)
        printf("clock set ok");

    // Inicializa a stdio e os GPIOs
    stdio_init_all();
    initialize_gpio();

    // Configura a máquina de estados (state machine) do PIO
    uint sm = pio_claim_unused_sm(pio, true);
    uint offset = pio_add_program(pio, &pio_matrix_program);
    pio_matrix_program_init(pio, sm, offset, OUT_PIN);

    // Loop principal
    while(true){
        // Pisca o LED vermelho
        gpio_put(RED,1);
        sleep_ms(100);
        gpio_put(RED,0);
        sleep_ms(100);
        
        // Exibe o número correspondente ao valor do contador na matriz de LEDs
        if(contador==0){numeros(numero0, valor_led, pio, sm, r, g, b);};
        if(contador==1){numeros(numero1, valor_led, pio, sm, r, g, b);};
        if(contador==2){numeros(numero2, valor_led, pio, sm, r, g, b);};
        if(contador==3){numeros(numero3, valor_led, pio, sm, r, g, b);};
        if(contador==4){numeros(numero4, valor_led, pio, sm, r, g, b);};
        if(contador==5){numeros(numero5, valor_led, pio, sm, r, g, b);};
        if(contador==6){numeros(numero6, valor_led, pio, sm, r, g, b);};
        if(contador==7){numeros(numero7, valor_led, pio, sm, r, g, b);};
        if(contador==8){numeros(numero8, valor_led, pio, sm, r, g, b);};
        if(contador==9){numeros(numero9, valor_led, pio, sm, r, g, b);};
    }

    return 0;
}

// Função para inicializar os GPIOs
void initialize_gpio()
{
    gpio_init(RED);
    gpio_set_dir(RED, GPIO_OUT);
       
    gpio_init(buton_A);
    gpio_set_dir(buton_A, GPIO_IN);
    gpio_pull_up(buton_A);

    gpio_init(buton_B);
    gpio_set_dir(buton_B, GPIO_IN);
    gpio_pull_up(buton_B);

    // Configura as interrupções para os botões A e B
    gpio_set_irq_enabled_with_callback(buton_A, GPIO_IRQ_EDGE_FALL, true, &gpio_irq_handler);
    gpio_set_irq_enabled_with_callback(buton_B, GPIO_IRQ_EDGE_FALL, true, &gpio_irq_handler);
}

// Função para exibir um número na matriz de LEDs
void numeros(double *desenho, uint32_t valor_led, PIO pio, uint sm, double r, double g, double b)
{
    for (int16_t i = 0; i < NUM_LEDS; i++)
    {
        valor_led = matrix_rgb(r, desenho[24 - i], b);
        pio_sm_put_blocking(pio, sm, valor_led);
    }
}

// Função de tratamento de interrupção para os botões
void gpio_irq_handler(uint gpio, uint32_t events)
{
    uint32_t tempo_atual= to_us_since_boot(get_absolute_time());
    if(tempo_atual-last_time>200000){
        last_time=tempo_atual;
        if(gpio==5){
            contador++;
            printf("Botão A pressionado. \n");
        }
        if(gpio==6){
            contador--;
            printf("Botão B pressionado. \n");
        }
    }
}