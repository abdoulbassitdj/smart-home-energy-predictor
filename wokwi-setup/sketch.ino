#include <WiFi.h>
#include <HTTPClient.h>
#include "DHT.h"

#define DHTPIN 4
#define DHTTYPE DHT22

DHT dht(DHTPIN, DHTTYPE);

// Informations de connexion pour le Wi-Fi virtuel de Wokwi
const char* ssid = "Wokwi-GUEST";
const char* password = "";

// ⚠️ REMPLACEZ CETTE URL par celle générée par localtunnel (gardez bien /update à la fin)
const char* serverUrl = "https://tidy-dingos-knock.loca.lt/update"; 

void setup() {
  Serial.begin(115200);
  dht.begin();

  // Connexion au réseau Wi-Fi virtuel
  WiFi.begin(ssid, password);
  Serial.print("Connexion au Wi-Fi");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nWi-Fi connecté !");
}

void loop() {
  // Attendre 5 secondes entre chaque envoi
  delay(5000); 

  float humidite = dht.readHumidity();
  float temperature = dht.readTemperature();

  // Vérification de la validité des lectures du capteur
  if (isnan(humidite) || isnan(temperature)) {
    Serial.println("Échec de la lecture du capteur DHT");
    return;
  }

  // Envoi de la requête HTTP si le Wi-Fi est connecté
  if (WiFi.status() == WL_CONNECTED) {
    HTTPClient http;
    
    // Initialisation de la requête HTTP sur l'URL de localtunnel
    http.begin(serverUrl); 
    http.addHeader("Content-Type", "application/json");
    
    // En-tête indispensable pour contourner la page de rappel/sécurité de localtunnel
    http.addHeader("Bypass-Tunnel-Reminder", "true");

    // Construction du payload JSON attendu par FastAPI
    String jsonPayload = "{\"temperature\":" + String(temperature) + 
                         ",\"humidite\":" + String(humidite) + "}";

    // Envoi effectif de la requête POST
    int httpResponseCode = http.POST(jsonPayload);

    if (httpResponseCode > 0) {
      Serial.print("Code de réponse HTTP : ");
      Serial.println(httpResponseCode);
      String response = http.getString();
      Serial.println("Réponse du serveur : " + response);
    } else {
      Serial.print("Erreur d'envoi HTTP : ");
      Serial.println(httpResponseCode);
    }
    
    http.end(); // Libération des ressources réseau
  } else {
    Serial.println("Erreur : Wi-Fi déconnecté");
  }
}
