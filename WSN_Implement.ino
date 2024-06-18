#include <WiFi.h>                     //library provide connection to WIFI and communicate
#include <WebServer.h>                //library create and handles web services
#include <Wire.h>                     //librariy allows I2C communication 
#include <Adafruit_Sensor.h>          //library provide to access the sensors in LSM303
#include <Adafruit_LSM303_Accel.h>    //library provide function to read LSM303 accelerometer

//Set the SSID name and password
const char* ssid = "ESP32-Access1";
const char* password = "123456789";

//Assign the digital & analog pins to variable
const int trigPin = 15;
const int echoPin = 2;
const int tempPin = 32;

//Define I2C communication
Adafruit_LSM303_Accel_Unified accel = Adafruit_LSM303_Accel_Unified(54321);
//54321 is just an identifier for accel sensor in the constructor

//From wifi library, webserver class created a server variable with port 80, allows to communicate through http request
WebServer server(80);

//Response to the handleClient Request
void handleRoot() {
  //Creating html page 
  String html = "<!DOCTYPE html>";
  html += "<html><head><title>ESP32 Web Server</title>";
  //dynamically update the webserver every second
  html += "<meta http-equiv=\"refresh\" content=\"1\"></head>";
  //In html body, creating a table with 2 rows.
  html += "<body><h1 style=\"color:blue;font-size:72px;text-align:center\">ESP32 Web Server</h1></body></html>";
  html += "<table style='margin: 0 auto; width: 100%; font-size: 36px;' border='3'>";
  html += "<tr style=\"text-align:center;\"><th style=\"color:red;font-size:48px;\">Measurement</th><th style=\"color:red;font-size:48px;\">Value</th></tr>";
  
  //SRF05 Ultra sonic sensor
  //Sets the trigPin on LOW state for 10 micro seconds
  digitalWrite(trigPin, LOW);
  delayMicroseconds(10);
  //Sets the trigPin on HIGH state for 10 micro seconds
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  //Again sets the trigger pin back to LOW state
  digitalWrite(trigPin, LOW);
  //Reads the echoPin, returns the sound wave travel time in microseconds
  long duration = pulseIn(echoPin, HIGH);
  //Calculate distance, using speed x time
  float distance = duration * 0.034 / 2;
  //Display the distnace in Webserver html table
  html += "<tr><td>Distance</td><td>" + String(distance) + " cm</td></tr>";

  //TMP 36
  //Reading from TMP 36 in analogue format
  float tempReading = analogRead(tempPin);
  //Convert it into volatge(digital format), using voltage resolution
  float voltage = tempReading * (5.0 / 4096.0);
  //Convert the voltage into temperature(C)using appropriate scaling factors
  float temperature = (voltage - 0.5) * 50;
  //Display the temperature in Webserver html table
  html += "<tr><td>Ambient Temperature</td><td>" + String(temperature) + "&deg; C</td></tr>";

  //Accelerometer
  //A structure stores the sensor event
  sensors_event_t event;
  //Get the readings from accelerometer sensor readings
  accel.getEvent(&event);
  //Assign the X,Y & Z value to variables
  float accelX = event.acceleration.x;
  float accelY = event.acceleration.y;
  float accelZ = event.acceleration.z;
  //Display them in the Webserver html
  html += "<tr><td>Acceleration in X axis</td><td>" + String(accelX) + " &nbsp;m/s<sup>2</sup></td></tr>";
  html += "<tr><td>Acceleration in Y axis</td><td>" + String(accelY) + " &nbsp;m/s<sup>2</sup></td></tr>";
  html += "<tr><td>Acceleration in Z axis</td><td>" + String(accelZ) + " &nbsp;m/s<sup>2</sup></td></tr>";
  
  //Close the html page by end tag
  html += "</table></body></html>";
  //Sending http resposne to the client (Status,content type,response body)
  server.send(200, "text/html", html);
}


void setup() {
  pinMode(trigPin, OUTPUT);   //Initialising SRF05 pins
  pinMode(echoPin, INPUT);
  
  Wire.begin();               //Intialising I2C communication
  accel.begin();              //Intialising Accelerometer
  Serial.begin(115200);       //Initailising the serial monitor

  //Setting the ESP32 as an Access Point
  WiFi.softAP(ssid, password);
  Serial.print("Access Point \"");
  Serial.print(ssid);
  Serial.println("\" started");

  //Getting the access point IP address
  IPAddress IP = WiFi.softAPIP();
  Serial.print("AP IP address: ");
  Serial.println(IP);

  //Letting know the user web server started
  server.on("/", handleRoot);
  server.begin();
  Serial.println("Web server started");
}

void loop() {
  server.handleClient();    //handling incoming client requests such as GET and POST
}
