# IoT-based-Weather-Station
The system is responsible for monitoring temperature, humidity, pressure, altitude and detecting for smoke or gas, then shows the readings on a web page in the form of progress bars, and sends an alert if any value exceeds a certain threshold.
There are two ways for sending an alert:<br />
1- Sending a message on the smartphone based on the SIM card number entered.<br />
2- Sending a notification on the mobile application (IFTTT).<br />

Hardware components used:<br />
• NodeMCU ESP8266.<br />
• DHT11 Temperature and Humidity sensor.<br />
• BMP180 Barometric Pressure Sensor.<br />
• MQ2 Smoke sensor.<br />

Libraries used:<br />
• ESP8266WiFi<br />
• WiFiClient<br />
• ESP8266WebServer<br />
• Adafruit_BMP085<br />
• DHT_sensor<br />
