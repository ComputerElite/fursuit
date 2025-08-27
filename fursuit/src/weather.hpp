#define DHTPIN 19
#define DHTTYPE DHT11

extern float humidity;
extern float temperature;  
extern float pressure;
extern float altitude;

extern void SetupWeather();
extern void UpdateWeather();