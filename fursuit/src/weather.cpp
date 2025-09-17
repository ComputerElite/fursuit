#include "weather.hpp"
#include "main.h"
#include "DHT.h"
#include "passwords.h"
#include <ArduinoJson.h>
#include <HTTPClient.h>
#include <Wire.h>
#include <SPI.h>
#include <Adafruit_BME280.h>
#include "imu.h"

float humidity = 0;
float temperature = 0;
float pressure = 0;
float altitude = 0;
long lastRead = 0;

//DHT dht(DHTPIN, DHTTYPE);

Adafruit_BME280 bme; // use I2C interface


void sendPost()
{   
    //Serial.printf("POSTING\n");
    if(UPLOAD_URL == "https://weather.example.com/") return; // return if default value
    HTTPClient http;
    http.begin(UPLOAD_URL); // Replace with your URL
    http.addHeader("Content-Type", "application/json");
    http.addHeader("Accept", "*/*");
    http.addHeader("Content-Length", "no-cache");
    http.addHeader("Authentication", UPLOAD_PASSWORD);
    http.setUserAgent("fursuit-weather/0.1");
    http.setTimeout(5000);

    StaticJsonDocument<512> doc;
    doc["t"] = temperature;
    doc["h"] = humidity;
    doc["p"] = pressure;
    doc["a"] = altitude;
    doc["iW"] = imuWorking;
    doc["steps"] = stepsSinceLastSend;
    int stepsSent = stepsSinceLastSend;
    String payload;
    serializeJson(doc, payload);

    int httpResponseCode = http.POST(payload);

    if (httpResponseCode > 0)
    {
        //Serial.printf("POST Success: %d\n", httpResponseCode);
        String response = http.getString();
        if(httpResponseCode == 200)
        {
            stepsSinceLastSend -= stepsSent; // reset steps since last send
        }
        Serial.println(response);
    }
    else
    {
        //Serial.printf("POST Failed: %s\n", http.errorToString(httpResponseCode).c_str());
    }

    http.end();
}

// FreeRTOS task
void httpTask(void* pvParameters) {
  while (true) {
    sendPost();  // Make HTTP request
    vTaskDelay(1000 / portTICK_PERIOD_MS);  // Wait 5 seconds
  }
}
bool weatherAvailable = false;

void SetupWeather()
{
    //dht.begin();
    if(!bme.begin(0x76)) {
        Serial.println("Could not find a valid BME280 sensor, check wiring!");
        // If other data than from the BME should be sent, remove this return
        return;
    }
    weatherAvailable = true;

    xTaskCreatePinnedToCore(
        httpTask,       // Function to run
        "HttpTask",     // Name
        8192,           // Stack size in bytes
        NULL,           // Parameters
        1,              // Priority
        NULL,           // Task handle
        0               // Core 0
    );

}

void UpdateWeather()
{
    if (lastLoop - lastRead < 1000 || !weatherAvailable)
        return; // we can only read every second
    lastRead = lastLoop;
    
    // Old Sensor
    //temperature = dht.readTemperature();
    //humidity = dht.readHumidity();
    
    //BME280
    temperature = bme.readTemperature();
    pressure = bme.readPressure() / 100.0F; // Convert to hPa
    altitude = bme.readAltitude(1013.25); // Adjust to your sea level pressure
    humidity = bme.readHumidity();
}