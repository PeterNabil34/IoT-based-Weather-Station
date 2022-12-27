#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>
#include <Adafruit_BMP085.h>
#include <DHT.h>

#define DHTPIN 0  //D3
#define DHTTYPE DHT11
#define HTML_Size 1900
#define HTML_Size_Smoke 300

DHT dht(DHTPIN, DHTTYPE);
Adafruit_BMP085 bmp;

const char ssid[] = "";
const char password[] = "";

const char host[] = "maker.ifttt.com";

const char private_key_1[] = "";
const char private_key_2[] = "";

const char event_Temperature[] = "Temperature_Alert";
const char event_Humidity[] = "Humidity_Alert";
const char event_Pressure[] = "Pressure_Alert";
const char event_Altitude[] = "Altitude_Alert";

ESP8266WebServer server(80);
ESP8266WebServer server_Smoke(80);

char temp[HTML_Size];
char htmlpage[HTML_Size] = "\
<!DOCTYPE html>\
<html>\
<head>\
<title>IoT Project</title>\
<meta charset=\"utf-8\">\
<meta name=\"viewport\" content=\"width=device-width, initial-scale=1\">\
<link rel=\"stylesheet\" href=\"https://maxcdn.bootstrapcdn.com/bootstrap/3.4.1/css/bootstrap.min.css\">\
<script src=\"https://ajax.googleapis.com/ajax/libs/jquery/3.6.1/jquery.min.js\"></script>\
<script src=\"https://maxcdn.bootstrapcdn.com/bootstrap/3.4.1/js/bootstrap.min.js\"></script>\
</head>\
<body>\
<div class=\"jumbotron text-center\">\
<h1>Weather Station</h1>\
</div>\
<div class=\"container\">\
<h3>Temperature in Celsius</h3>\
<div class=\"progress\">\
<div class=\"progress-bar progress-bar-info progress-bar-striped active\" role=\"progressbar\" style=\"width:%d%%\">\
%d°C\
</div>\
</div>\
<h3>Temperature in Fahrenheit</h3>\
<div class=\"progress\">\
<div class=\"progress-bar progress-bar-danger progress-bar-striped active\" role=\"progressbar\" style=\"width:%d%%\">\
%d°F\
</div>\
</div>\
<h3>Humidity</h3>\
<div class=\"progress\">\
<div class=\"progress-bar progress-bar-success progress-bar-striped active\" role=\"progressbar\" style=\"width:%d%%\">\
%d%%\
</div>\
</div>\
<h3>Pressure</h3>\
<div class=\"progress\">\
<div class=\"progress-bar progress-bar-warning progress-bar-striped active\" role=\"progressbar\" style=\"width:%d%%\">\
%d hPa\
</div>\
</div>\
<h3>Altitude</h3>\
<div class=\"progress\">\
<div class=\"progress-bar progress-bar-danger progress-bar-striped active\" role=\"progressbar\" style=\"width:%d%%\">\
%d meters\
</div>\
</div>\
</div>\
</body>\
</html>\
";

char temp_Smoke[HTML_Size_Smoke];
char htmlpage_Smoke[HTML_Size_Smoke] ="\
<!DOCTYPE html>\
<html>\
<body>\
<h1 class=\"active\" style=\"background-color:Tomato;\"><center>Warning</center></h1>\
<h3>There is a gas or smoke detected in the weather station.</h3>\
</body>\
</html>\
";


void setup() 
{
  pinMode (A0, INPUT);
  
  Serial.begin(9600);
  dht.begin();

  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED )
  {
    delay(500);
    Serial.print(".");
  }
  
  Serial.println("");
  Serial.println("WiFi Connected");
  Serial.println(WiFi.localIP());

  if (!bmp.begin()) 
  {
    Serial.println("Could not find a valid BMP180 sensor, check wiring!");
    while(1) 
    {}
  }
  
  server.on("/", handleRoot);
  server.onNotFound(handleNotFound);
  
  server_Smoke.on("/", handleRoot_Smoke);
  server_Smoke.onNotFound(handleNotFound_Smoke);

  server.begin();
  server_Smoke.begin();
}

void loop() 
{ 
  while (analogRead(A0) > 200)
  {
    Serial.println("");
    Serial.println("Smoke detected");
    Serial.println("");
    
    server_Smoke.handleClient();
    
    delay(500);
  }
  
  server.handleClient()
   
  delay(4000);
  float Temperature_C = dht.readTemperature();
  float Temperature_F = dht.readTemperature(true);
  float Humidity = dht.readHumidity();
  float Pressure = bmp.readPressure() / 100;  //hPa
  float SealevelPressure = bmp.readSealevelPressure() / 100;  //hPa
  float Altitude = bmp.readAltitude();
  float Real_Altitude = bmp.readAltitude(101900);  //Assume that the Sealevel Pressure = 101900 Pascals
  
  Serial.print("Temperature: ");
  Serial.print(Temperature_C);
  Serial.print("°C = ");
  Serial.print(Temperature_F);
  Serial.print("°F / ");
  Serial.print("Humidity: ");
  Serial.print(Humidity);
  Serial.print("% / ");
  Serial.print("Pressure: ");
  Serial.print(Pressure);
  Serial.print(" hPa / ");
  Serial.print("Sealevel Pressure = ");
  Serial.print(SealevelPressure);
  Serial.print(" hPa / ");
  Serial.print("Altitude: ");
  Serial.print(Altitude);
  Serial.print(" meters and Real Altitude: ");
  Serial.print(Real_Altitude);
  Serial.println(" meters.");
  Serial.println("");
  
  if (Temperature_C >= 21)
  {
    Serial.println("");
    Serial.println("Temperature(in Celsius) Event occured");
    
    WiFiClient client;
    if (!client.connect(host, 80))
    {
      Serial.println("");
      Serial.println("Failed Connection");
      delay(2000);
      return;      
    }
    
    String url = "/trigger/";
    url = url + event_Temperature + "/with/key/" + private_key_1;

    String http = "GET ";
    http = http + url + " HTTP/1.1\r\n";
    http = http + "Host: " + host + "\r\n";
    http = http + "Connection: close\r\n\r\n";
    
    client.print(http);
    client.stop();

    Serial.println("");
    Serial.println("Temperature(in Celsius) SMS has Sent");
  }

  if (Humidity >= 70)
  {
    Serial.println("");
    Serial.println("Humidity Event occured");
    
    WiFiClient client;
    if (!client.connect(host, 80))
    {
      Serial.println("");
      Serial.println("Failed Connection");
      delay(2000);
      return;      
    }
    
    String url = "/trigger/";
    url = url + event_Humidity + "/with/key/" + private_key_1;

    String http = "GET ";
    http = http + url + " HTTP/1.1\r\n";
    http = http + "Host: " + host + "\r\n";
    http = http + "Connection: close\r\n\r\n";
    
    client.print(http);
    client.stop();

    Serial.println("");
    Serial.println("Humidity SMS has Sent");
  }
  
  if (Pressure >= 1015)
  {
    Serial.println("");
    Serial.println("Pressure Event occured");
    
    WiFiClient client;
    if (!client.connect(host, 80))
    {
      Serial.println("");
      Serial.println("Failed Connection");
      delay(2000);
      return;      
    }
    
    String url = "/trigger/";
    url = url + event_Pressure + "/with/key/" + private_key_2;

    String http = "GET ";
    http = http + url + " HTTP/1.1\r\n";
    http = http + "Host: " + host + "\r\n";
    http = http + "Connection: close\r\n\r\n";
    
    client.print(http);
    client.stop();

    Serial.println("");
    Serial.println("Pressure SMS has Sent");
  }
  
  if (Real_Altitude >= 28)
  {
    Serial.println("");
    Serial.println("Altitude Event occured");
    
    WiFiClient client;
    if (!client.connect(host, 80))
    {
      Serial.println("");
      Serial.println("Failed Connection");
      delay(2000);
      return;      
    }
    
    String url = "/trigger/";
    url = url + event_Altitude + "/with/key/" + private_key_2;

    String http = "GET ";
    http = http + url + " HTTP/1.1\r\n";
    http = http + "Host: " + host + "\r\n";
    http = http + "Connection: close\r\n\r\n";
    
    client.print(http);
    client.stop();

    Serial.println("");
    Serial.println("Altitude SMS has Sent");
  }
}

void handleRoot()
{
  int Temperature_C = dht.readTemperature();
  int Temperature_F = dht.readTemperature(true);
  int Humidity = dht.readHumidity();
  int Pressure = bmp.readPressure() / 100; //hPa
  int Pressure_width = Pressure * 100 / 1100;  //to get a percentage value
  int Altitude = bmp.readAltitude(101900);  //Assume that the Sealevel Pressure = 101900 Pascals
  
  snprintf (temp, HTML_Size, htmlpage, Temperature_C, Temperature_C, Temperature_F, Temperature_F, Humidity, Humidity , Pressure_width, Pressure, Altitude, Altitude);
  server.send(200, "text/html", temp);
}

void handleNotFound()
{
  server.send(404, "text/html", "<h1>Sorry, Page Not Found</h1>");
}

void handleRoot_Smoke()
{
  server_Smoke.send(200, "text/html", htmlpage_Smoke);
}

void handleNotFound_Smoke()
{
  server_Smoke.send(404, "text/html", "<h1>Sorry, Smoke Page Not Found</h1>");
}


