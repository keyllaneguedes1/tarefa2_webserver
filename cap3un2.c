#include "pico/stdlib.h"
#include "hardware/adc.h"
#include "pico/cyw43_arch.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "lwip/pbuf.h"
#include "lwip/tcp.h"
#include "lwip/netif.h"

// Configurações
#define WIFI_SSID "Uai Fai_2.4G"
#define WIFI_PASSWORD "2003@2008"
#define BUTTON_PIN 5
#define JOYSTICK_X_PIN 26
#define JOYSTICK_Y_PIN 27

// Mapeamento do joystick
const char* map_joystick_to_direction(int x, int y) {
    const int low = 1500, high = 2500;
    if (x < low && y > high) return "Noroeste";
    if (x > high && y > high) return "Nordeste";
    if (x < low && y < low) return "Sudoeste";
    if (x > high && y < low) return "Sudeste";
    if (x < low) return "Oeste";
    if (x > high) return "Leste";
    if (y > high) return "Norte";
    if (y < low) return "Sul";
    return "Centro";
}

// Gera os dados em formato JSON
static char* generate_sensor_data() {
    bool button_state = !gpio_get(BUTTON_PIN);
    
    adc_select_input(4);
    float temperature = 27.0f - ((adc_read() * 3.3f / (1 << 12)) - 0.706f) / 0.001721f;
    
    adc_select_input(0);
    int joy_y = adc_read();
    adc_select_input(1);
    int joy_x = adc_read();
    const char* joy_direction = map_joystick_to_direction(joy_x, joy_y);
    
    char* json = (char*)malloc(256);
    snprintf(json, 256,
        "{\"button\":%d,\"temp\":%.2f,\"joy_x\":%d,\"joy_y\":%d,\"direction\":\"%s\"}",
        button_state, temperature, joy_x, joy_y, joy_direction);
    
    return json;
}

// Callback para requisições HTTP
static err_t tcp_server_recv(void *arg, struct tcp_pcb *tpcb, struct pbuf *p, err_t err) {
    if (!p) {
        tcp_close(tpcb);
        tcp_recv(tpcb, NULL);
        return ERR_OK;
    }

    char request[1024];
    strncpy(request, (char*)p->payload, p->len);
    request[p->len] = '\0';

    // Verifica se é uma requisição AJAX
    bool is_ajax = strstr(request, "X-Requested-With: XMLHttpRequest") != NULL;

    if (is_ajax || strstr(request, "/data")) {
        // Resposta JSON para AJAX
        char* json_data = generate_sensor_data();
        
        char response[512];
        snprintf(response, sizeof(response),
            "HTTP/1.1 200 OK\r\n"
            "Content-Type: application/json\r\n"
            "Access-Control-Allow-Origin: *\r\n"
            "Cache-Control: no-cache, no-store, must-revalidate\r\n"
            "Pragma: no-cache\r\n"
            "Expires: 0\r\n"
            "Content-Length: %d\r\n\r\n%s",
            strlen(json_data), json_data);
        
        tcp_write(tpcb, response, strlen(response), TCP_WRITE_FLAG_COPY);
        free(json_data);
    } else {
        // Página HTML completa
        const char* html = 
            "HTTP/1.1 200 OK\r\n"
            "Content-Type: text/html\r\n"
            "Cache-Control: no-cache, no-store, must-revalidate\r\n"
            "Pragma: no-cache\r\n"
            "Expires: 0\r\n\r\n"
            "<!DOCTYPE html><html><head><meta charset='utf-8'><title>Monitor Pico W</title>"
            "<style>"
            "body {font-family: Arial, sans-serif; text-align: center; margin-top: 50px;}"
            ".card {border: 1px solid #ddd; border-radius: 8px; padding: 20px; margin: 10px auto; width: 300px;}"
            ".pressed {background-color: #ffdddd;}"
            "</style>"
            "<script>"
            "function updateData() {"
            "  fetch('/data')"
            "    .then(response => response.json())"
            "    .then(data => {"
            "      document.getElementById('botao').innerHTML = data.button ? 'PRESSIONADO' : 'SOLTO';"
            "      document.getElementById('botao').parentElement.className = data.button ? 'card pressed' : 'card';"
            "      document.getElementById('temp').innerHTML = data.temp.toFixed(2) + ' °C';"
            "      document.getElementById('joy_x').innerHTML = data.joy_x;"
            "      document.getElementById('joy_y').innerHTML = data.joy_y;"
            "      document.getElementById('direction').innerHTML = data.direction;"
            "    });"
            "}"
            "setInterval(updateData, 1000);"
            "window.onload = updateData;"
            "</script></head><body>"
            "<h1>Monitor Pico W</h1>"
            "<div class='card'><h2>Botão</h2><p id='botao'>-</p></div>"
            "<div class='card'><h2>Temperatura</h2><p id='temp'>-</p></div>"
            "<div class='card'><h2>Joystick</h2>"
            "<p>X: <span id='joy_x'>-</span>, Y: <span id='joy_y'>-</span></p>"
            "<p>Direção: <strong id='direction'>-</strong></p></div></body></html>";
        
        tcp_write(tpcb, html, strlen(html), TCP_WRITE_FLAG_COPY);
    }

    tcp_output(tpcb);
    pbuf_free(p);
    return ERR_OK;
}


static err_t tcp_server_accept(void *arg, struct tcp_pcb *newpcb, err_t err) {
    tcp_recv(newpcb, tcp_server_recv);
    return ERR_OK;
}

int main() {
    stdio_init_all();

    // Configuração do hardware
    gpio_init(BUTTON_PIN);
    gpio_set_dir(BUTTON_PIN, GPIO_IN);
    gpio_pull_up(BUTTON_PIN);

    adc_init();
    adc_gpio_init(JOYSTICK_X_PIN);
    adc_gpio_init(JOYSTICK_Y_PIN);
    adc_set_temp_sensor_enabled(true);

    // Conexão Wi-Fi
    if (cyw43_arch_init()) {
        printf("Falha ao inicializar Wi-Fi\n");
        return -1;
    }

    cyw43_arch_enable_sta_mode();
    printf("Conectando ao Wi-Fi...\n");
    if (cyw43_arch_wifi_connect_timeout_ms(WIFI_SSID, WIFI_PASSWORD, CYW43_AUTH_WPA2_AES_PSK, 20000)) {
        printf("Falha ao conectar\n");
        return -1;
    }
    printf("Conectado!\n");

    // Servidor TCP
    struct tcp_pcb *server = tcp_new();
    tcp_bind(server, IP_ADDR_ANY, 80);
    server = tcp_listen(server);
    tcp_accept(server, tcp_server_accept);

    printf("Servidor iniciado. IP: %s\n", ip4addr_ntoa(netif_ip4_addr(netif_default)));

    while (true) {
        cyw43_arch_poll();
        sleep_ms(1);
    }

    cyw43_arch_deinit();
    return 0;
}