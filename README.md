# esp8266_test

Aplicações para ESP8266 e ESP32

Simulação de endpoints devolvendo JSON de temperatura e humidade

## LLMNR_Web_Server
Web server para ESP8266

## AdvancedWebServer
Web server para ESP32

O JSON de retorno está no endpoint /status

1. Configura a rede WIFI que seu ESP estará conectado 
2. Compile o Programa para o ESP 
3. Abra o terminal serial do Arduino Studio e verifique o IP do ESP
4. Para acessar o endpoint, abra o browser de sua preferência, coloque o IP do ESP seguido de /status
4.1 Exemplo: http://192.168.0.100/status
  O retorne deverá ser o JSON com os dados randômicos de temperatura, millis e humidade.
