# ESP8266 e ESP32 - Exemplo de _endpoint_ em JSON

### Aplicações para ESP8266 e ESP32

Simulação de endpoints devolvendo JSON de temperatura e humidade

<details>  
  <summary>ESP8266 - LLMNR_Web_Server</summary>
- Web server para ESP8266
Link para configurar o NodeMCU ESP8266 no Arduino studio: https://arduino.esp8266.com/stable/package_esp8266com_index.json
</details>
<details>
  <summary>ESP32 - AdvancedWebServer</summary>
- Web server para ESP32
</details>

## Tutorial  
> - O JSON de retorno está no endpoint /status

1. Configure a rede WIFI em que seu ESP estará conectado  
1.1 Caso a rede não seja encontrada o ESP entrará em mode Access Point (AP)  
2. Compile e descarregue o programa para o ESP 
3. Abra o terminal serial do Arduino Studio e verifique o IP do ESP
4. Para acessar o endpoint, abra o browser de sua preferência, coloque o IP do ESP seguido de /status  
4.1 Exemplo: http://10.10.0.1/status
  > O retorno deverá ser o JSON com os dados randômicos de temperatura, millis e humidade.  
> ```json
> { "temperatura":44, "millis":2129286, "umidade":71}
> ```    
> - Utilize esse endpoint no NODE-RED para pegar os dados do ESP e enviar para alguma plataforma MQTT.

## Página inicial do ESP
A página do Dashboard do ESP é armazenado na variável "index_html", o arquivo original pode ser editado com qualquer editor de código (texto), recomenda-se editar o arquivo **_index_html_esp.html_**.
> **OBS:** Por incopatibilidade o arquivo editado precisa ser convertido para uma String em C++.  
> - Utilize este site para converter o [HTML em String C++](https://tomeko.net/online_tools/cpp_text_escape.php?lang=en).  


> Atribua a string convertida na variável **_index_html_** [código fonte](https://github.com/DenadaiSenai/esp8266_test/blob/a484e46ac1f8d950f663807b382c63a9ee94cd7f/LLMNR_Web_Server/LLMNR_Web_Server.ino#L82).  
> No arquivo **LLMNR_Web_Server.ino** https://github.com/DenadaiSenai/esp8266_test/blob/a484e46ac1f8d950f663807b382c63a9ee94cd7f/LLMNR_Web_Server/LLMNR_Web_Server.ino#L82
> No arquivo **AdvancedWebServer.ino** https://github.com/DenadaiSenai/esp8266_test/blob/71f702e39ee9db26c08ed3f28bb79f286a4f31b4/AdvancedWebServer/AdvancedWebServer.ino#L77

## Opcional
Caso queira diminuar o tamanho da String do index_html, utilize antes um conversor online para minimizar o HTML.
- Site para [minimizar o HTML](https://codebeautify.org/minify-html) antes de converter em string C++.
