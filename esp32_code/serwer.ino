#include <WiFi.h>
#include <ESPAsyncWebServer.h>
#include <SPIFFS.h>

const char* ssid = "";
const char* password = "";

AsyncWebServer server(80);

void setup() {
    Serial.begin(115200);

    if (!SPIFFS.begin(true)) {
        Serial.println("Błąd inicjalizacji SPIFFS!");
        return;
    }

    WiFi.begin(ssid, password);
    Serial.print("Łączenie z Wi-Fi...");
    while (WiFi.status() != WL_CONNECTED) {
        delay(1000);
        Serial.print(".");
    }
    Serial.println("\nPołączono z Wi-Fi!");
    Serial.println("Adres IP: " + WiFi.localIP());

    server.serveStatic("/", SPIFFS, "/").setDefaultFile("index.html");

    server.on("/upload", HTTP_POST, [](AsyncWebServerRequest *request) {
        if (request->hasParam("image", true)) {
            String imageData = request->getParam("image", true)->value();
            imageData.replace("data:image/jpeg;base64,", ""); // Usunięcie nagłówka base64

            File file = SPIFFS.open("/przetworzone_zdjecie.jpg", FILE_WRITE);
            if (!file) {
                request->send(500, "text/plain", "Błąd otwarcia pliku!");
                return;
            }

            size_t decodedLength = base64_decode((unsigned char*)file.write, imageData.c_str(), imageData.length());
            file.close();

            request->send(200, "text/plain", "Zdjęcie zostało zapisane!");
        } else {
            request->send(400, "text/plain", "Brak danych zdjęcia!");
        }
    });

    server.begin();
}

void loop() {
    delay(10);
}

size_t base64_decode(unsigned char* output, const char* input, size_t inputLength) {
    const char* base64_chars = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";
    size_t outputLength = 0;
    int i = 0, j = 0;
    unsigned char char_array_4[4], char_array_3[3];

    while (inputLength-- && (input[i] != '=')) {
        char_array_4[j++] = input[i++];
        if (j == 4) {
            for (j = 0; j < 4; j++)
                char_array_4[j] = strchr(base64_chars, char_array_4[j]) - base64_chars;

            char_array_3[0] = (char_array_4[0] << 2) + ((char_array_4[1] & 0x30) >> 4);
            char_array_3[1] = ((char_array_4[1] & 0xf) << 4) + ((char_array_4[2] & 0x3c) >> 2);
            char_array_3[2] = ((char_array_4[2] & 0x3) << 6) + char_array_4[3];

            for (j = 0; j < 3; j++)
                output[outputLength++] = char_array_3[j];
            j = 0;
        }
    }

    if (j) {
        for (int k = j; k < 4; k++)
            char_array_4[k] = 0;

        for (int k = 0; k < 4; k++)
            char_array_4[k] = strchr(base64_chars, char_array_4[k]) - base64_chars;

        char_array_3[0] = (char_array_4[0] << 2) + ((char_array_4[1] & 0x30) >> 4);
        char_array_3[1] = ((char_array_4[1] & 0xf) << 4) + ((char_array_4[2] & 0x3c) >> 2);
        char_array_3[2] = ((char_array_4[2] & 0x3) << 6) + char_array_4[3];

        for (int k = 0; k < j - 1; k++)
            output[outputLength++] = char_array_3[k];
    }

    return outputLength;
}