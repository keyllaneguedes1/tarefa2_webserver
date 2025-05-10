# Raspberry Pi Pico W - Servidor Web Local com Sensores

Este projeto demonstra como transformar o **Raspberry Pi Pico W** em um **servidor web local**, que fornece dados de sensores embarcados (botão, joystick e temperatura) em formato **JSON**, acessíveis por navegador ou aplicação via **requisição HTTP/AJAX**.

## 🧰 Funcionalidades

- Leitura do estado de um botão (GPIO)
- Leitura de temperatura interna do RP2040 (ADC)
- Leitura de joystick analógico (eixos X e Y via ADC)
- Identificação da direção do joystick
- Geração e envio de dados em formato JSON
- Servidor HTTP local para visualização em rede Wi-Fi

## 📷 Demonstração

A placa cria um ponto de acesso Wi-Fi ou se conecta a um roteador, e fornece dados em tempo real no navegador ao acessar o IP local.

## 🧪 Componentes Utilizados

- Raspberry Pi Pico W
- Joystick analógico (ligado aos pinos ADC 26 e 27)
- Botão (ligado ao GPIO 5)
- Sensor de temperatura interno do RP2040
- Conexão Wi-Fi local

## 🧠 Como Funciona

1. A placa se conecta a uma rede Wi-Fi.
2. Um **servidor TCP/IP HTTP** é iniciado localmente.
3. Ao acessar `/data` no navegador, os dados são respondidos em formato JSON:
   ```json
   {
     "button": 1,
     "temp": 26.78,
     "joy_x": 2048,
     "joy_y": 3050,
     "direction": "Norte"
   }
4. A página cliente pode utilizar AJAX para atualizar os dados em tempo real.

📂 Estrutura do Código
main.c – Código principal em C (bare-metal) com:

- Inicialização de GPIO, ADC e Wi-Fi
- Leitura de sensores
- Montagem de resposta HTTP com JSON
- Callback TCP para servir os dados

🔧 Configuração

- SDK do Raspberry Pi Pico instalado
- LWIP habilitado
- Biblioteca CYW43 para conexão Wi-Fi
- Compilação com CMake

💡 Aplicações
- Monitoramento embarcado local
- Prototipagem de dispositivos IoT sem internet
- Integração com dashboards HTML/JS locais
- Interface com automação residencial

