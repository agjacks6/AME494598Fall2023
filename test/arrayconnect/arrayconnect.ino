#define LILYGO_WATCH_2019_WITH_TOUCH
#include <WiFi.h>
#include <LilyGoWatch.h>
#include <SimpleDHT.h>

const char *ssid = "Babyjail";
const char *password = "12345678";
const char* serverName = "http://23.23.35.74:1234/setValue";

WiFiServer server(esp32Port);

String currentAnswer = "No answer yet.";

void setup() {
    Serial.begin(115200);

    // Connect to Wi-Fi
    WiFi.begin(ssid, password);
    while (WiFi.status() != WL_CONNECTED) {
        delay(1000);
        Serial.println("Connecting to WiFi...");
    }
    Serial.println("Connected to WiFi");

    // Set up the server
    server.begin();
}

void loop() {
    WiFiClient client = server.available();

    if (client) {
        while (client.connected()) {
            if (client.available()) {
                String request = client.readStringUntil('\r');
                client.flush();

                if (request.indexOf("/update") != -1) {
                    // Extract the answer from the request
                    int answerIndex = request.indexOf("?answer=");
                    if (answerIndex != -1) {
                        String answer = request.substring(answerIndex + 8);
                        answer.replace("%20", " "); // Replace spaces encoded as %20
                        currentAnswer = answer;
                        Serial.println("Updated answer: " + currentAnswer);
                    }

                    // Send a response
                    client.println("HTTP/1.1 200 OK");
                    client.println("Content-type:text/html");
                    client.println();
                    client.println("{\"status\":\"updated\"}");
                } else {
                    // Send the current answer
                    client.println("HTTP/1.1 200 OK");
                    client.println("Content-type:text/html");
                    client.println();
                    client.println(currentAnswer);
                }
            }
        }
        client.stop();
    }
}
