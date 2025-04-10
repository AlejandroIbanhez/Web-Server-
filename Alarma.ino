/**
 * @file Alarma.ino
 * @brief Proyecto para controlar una alarma con dos LEDs y un buzzer mediante una interfaz web.
 * 
 * Este código crea un punto de acceso Wi-Fi desde el ESP32 y permite controlar
 * LEDs y un buzzer a través de botones ON y OFF en una página web.
 */

#include <WiFi.h>        ///< Librería para funciones Wi-Fi del ESP32
#include <WebServer.h>   ///< Librería para crear un servidor web en ESP32
#include <WiFiAP.h>      ///< Librería para configurar el ESP32 como Access Point

/// Nombre de la red Wi-Fi (SSID) del Access Point
const char *ssid = "Alarma";
/// Contraseña del Access Point
const char *password = "contrasenasegura";

/// Instancia del servidor web en el puerto 80
WebServer server(80);

/// Pin GPIO para el LED 1
const int led1 = 27;
/// Pin GPIO para el LED 2
const int led2 = 26;
/// Pin GPIO para el buzzer
const int buzzer = 25;

/**
 * @brief Muestra la página principal con botones ON y OFF
 * 
 * Esta función genera código HTML que se muestra en el navegador del usuario.
 * Contiene botones para encender o apagar la alarma.
 */
void handleRoot() {
  String html = "<!DOCTYPE html><html><head><meta charset='UTF-8'><title>Control</title></head>";
  html += "<body style='margin: 0; height: 100vh; display: flex; flex-direction: column; justify-content: center; align-items: center;";
  html += "background: linear-gradient(to right, #0b2e13, #1d6b2f); font-family: Arial, sans-serif;'>";

  html += "<h1 style='color: white; font-size: 60px; margin-bottom: 40px;'>Control de Alarma</h1>";
  html += "<p><a href='/on'><button style='padding: 20px 40px; font-size: 40px; border-radius: 30px; margin: 20px;'>ON</button></a></p>";
  html += "<p><a href='/off'><button style='padding: 20px 40px; font-size: 40px; border-radius: 30px; margin: 20px;'>OFF</button></a></p>";

  html += "</body></html>";

  server.send(200, "text/html", html); ///< Envío del HTML al navegador
}

/**
 * @brief Configura el hardware y el servidor web.
 * 
 * Inicializa los pines, configura el Access Point y define las rutas del servidor web.
 */
void setup(void) {
  pinMode(led1, OUTPUT);     ///< Configura led1 como salida
  pinMode(led2, OUTPUT);     ///< Configura led2 como salida
  pinMode(buzzer, OUTPUT);   ///< Configura el buzzer como salida

  Serial.begin(115200);      ///< Inicia comunicación serie

  // Intenta crear el Access Point con el SSID y contraseña definidos
  if (!WiFi.softAP(ssid, password)) {
    Serial.println("Error al crear el Access Point.");
    while (true); ///< Si falla, queda en bucle infinito
  }

  IPAddress myIP = WiFi.softAPIP(); ///< Obtiene la IP del Access Point
  Serial.print("Dirección IP del AP: ");
  Serial.println(myIP);

  server.on("/", handleRoot); ///< Ruta raíz para mostrar la página web

  /**
   * @brief Ruta para encender la alarma
   * 
   * Activa secuencialmente los LEDs y el buzzer para simular una señal de alarma
   */
  server.on("/on", []() {
    for (int i = 0; i < 3; i++) {
      digitalWrite(led1, HIGH);
      digitalWrite(led2, LOW);
      digitalWrite(buzzer, HIGH);
      delay(500);
  
      digitalWrite(led1,  LOW);
      digitalWrite(led2, HIGH);
      digitalWrite(buzzer, LOW);
      delay(500);

    }

    digitalWrite(led1, HIGH);
    digitalWrite(buzzer, HIGH);
    delay(500);

    server.sendHeader("Location", "/"); ///< Redirige a la página principal
    server.send(303);                   ///< Código 303: redirección
  });

  /**
   * @brief Ruta para apagar la alarma
   * 
   * Apaga todos los dispositivos (LEDs y buzzer)
   */
  server.on("/off", []() {
    digitalWrite(led1, LOW);
    digitalWrite(led2, LOW);
    digitalWrite(buzzer, LOW);

    server.sendHeader("Location", "/"); ///< Redirige a la página principal
    server.send(303);                   ///< Código 303: redirección
  });

  server.begin(); ///< Inicia el servidor web
  Serial.println("Servidor HTTP iniciado");
}

/**
 * @brief Función principal que atiende las peticiones web.
 * 
 * Este bucle mantiene activo el servidor y responde a los clientes.
 */
void loop(void) {
  server.handleClient(); ///< Atiende clientes conectados al servidor
  delay(2);              ///< Pequeña pausa para permitir multitarea
}
