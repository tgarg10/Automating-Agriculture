#include "secrets.h"
#include <WiFiClientSecure.h>
#include <MQTT.h>
#include "WiFi.h"
#include "esp_camera.h"

//Brownout Error
#include "soc/soc.h"
#include "soc/rtc_cntl_reg.h"

#include <ImgurUploader.h>

#define IMGUR_CLIENT_ID "54cdaf3befe6f96"

#define R_S 3
#define L_S 4

#define in1 9
#define in2 8
#define in3 7
#define in4 6


#define inA 13
#define inB 2


#define pump 10
#define sensor_pin A0
ImgurUploader imgurUploader(IMGUR_CLIENT_ID);

#define PWDN_GPIO_NUM     32
#define RESET_GPIO_NUM    -1
#define XCLK_GPIO_NUM      0
#define SIOD_GPIO_NUM     26
#define SIOC_GPIO_NUM     27
#define Y9_GPIO_NUM       35
#define Y8_GPIO_NUM       34
#define Y7_GPIO_NUM       39
#define Y6_GPIO_NUM       36
#define Y5_GPIO_NUM       21
#define Y4_GPIO_NUM       19
#define Y3_GPIO_NUM       18
#define Y2_GPIO_NUM        5
#define VSYNC_GPIO_NUM    25
#define HREF_GPIO_NUM     23
#define PCLK_GPIO_NUM     22

#define ESP32CAM_PUBLISH_TOPIC   "esp32/cam_2"
#define ESP32CAM_PUBLISH_TOPIC2   "image_log/cam" 
#define ESP32CAM_SUBSCRIBE_TOPIC   "cam3_controls" 
#define ESP32CAM_PUBLISH_TOPIC3 "moisture_log/cam"
#define BUFFER_LEN 256

char msg[BUFFER_LEN];
const int bufferSize = 1024 * 23; // 23552 bytes

WiFiClientSecure net = WiFiClientSecure();
MQTTClient client = MQTTClient(bufferSize);

void backward(){
  Serial.println("backward");
}

void Irrigatorreading(float Reading){
  snprintf(msg, BUFFER_LEN, "{\"name\":\"irrigator\", \"data\":%f}", Reading);
  client.publish(ESP32CAM_PUBLISH_TOPIC3, msg);
  Serial.println("Logged Reading!");
  Serial.setTimeout(10000);
}

void Moisturereading(float Reading){
  snprintf(msg, BUFFER_LEN, "{\"name\":\"moisture\", \"data\":%f}", Reading);
  client.publish(ESP32CAM_PUBLISH_TOPIC3, msg);
  Serial.println("Logged Reading!");
  Serial.setTimeout(10000);
}

void logimage(){
  camera_fb_t * fb = esp_camera_fb_get();
  if(fb != NULL && fb->format == PIXFORMAT_JPEG && fb->len < bufferSize){
    int imgurImage = imgurUploader.uploadBytes(fb->buf, fb->len, "log.jpg", "image/jpeg");
    if(imgurImage > 0){
      snprintf(msg, BUFFER_LEN, "{\"camera\":\"Irrigator/Moisture Observer\", \"link\":\"%s\"}", imgurUploader.getURL());
      client.publish(ESP32CAM_PUBLISH_TOPIC2, msg);
      Serial.setTimeout(10000);
    }
  }
  esp_camera_fb_return(fb);
}

void* getCommands(String& topic, String& payload){
  if (payload == "\"forward\""){
    forward();
  } else if (payload == "\"left\""){
    left();
  } else if (payload == "\"stop\""){
    stopmovement();
  } else if (payload == "\"right\""){
     right();
  } else if (payload == "\"backward\""){
    backward();
  } else if (payload == "\"log\""){
    logimage();
  } 
}

void connectAWS()
{
  WiFi.mode(WIFI_STA);
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);

  Serial.println("\n\n=====================");
  Serial.println("Connecting to Wi-Fi");
  Serial.println("=====================\n\n");
  int n = 0;
  while (WiFi.status() != WL_CONNECTED){
    delay(500);
    Serial.print(".");
    n++;
    if (n>20){
      ESP.restart();
    }
  }

  net.setCACert(AWS_CERT_CA);
  net.setCertificate(AWS_CERT_CRT);
  net.setPrivateKey(AWS_CERT_PRIVATE);

  client.begin(AWS_IOT_ENDPOINT, 8883, net);
  client.setCleanSession(true);

  Serial.println("\n\n=====================");
  Serial.println("Connecting to AWS IOT");
  Serial.println("=====================\n\n");
  while (!client.connect(THINGNAME)) {
    Serial.print(".");
    delay(100);
  }

  if(!client.connected()){
    Serial.println("AWS IoT Timeout!");
    ESP.restart();
    return;
  }

  client.onMessage(getCommands);
  client.subscribe(ESP32CAM_SUBSCRIBE_TOPIC);
  
  Serial.println("\n\n=====================");
  Serial.println("AWS IoT Connected!");
  Serial.println("=====================\n\n");
}

void cameraInit(){
  camera_config_t config;
  config.ledc_channel = LEDC_CHANNEL_0;
  config.ledc_timer = LEDC_TIMER_0;
  config.pin_d0 = Y2_GPIO_NUM;
  config.pin_d1 = Y3_GPIO_NUM;
  config.pin_d2 = Y4_GPIO_NUM;
  config.pin_d3 = Y5_GPIO_NUM;
  config.pin_d4 = Y6_GPIO_NUM;
  config.pin_d5 = Y7_GPIO_NUM;
  config.pin_d6 = Y8_GPIO_NUM;
  config.pin_d7 = Y9_GPIO_NUM;
  config.pin_xclk = XCLK_GPIO_NUM;
  config.pin_pclk = PCLK_GPIO_NUM;
  config.pin_vsync = VSYNC_GPIO_NUM;
  config.pin_href = HREF_GPIO_NUM;
  config.pin_sscb_sda = SIOD_GPIO_NUM;
  config.pin_sscb_scl = SIOC_GPIO_NUM;
  config.pin_pwdn = PWDN_GPIO_NUM;
  config.pin_reset = RESET_GPIO_NUM;
  config.xclk_freq_hz = 20000000;
  config.pixel_format = PIXFORMAT_JPEG;
  config.frame_size = FRAMESIZE_QVGA; // 640x480
  config.jpeg_quality = 6;
  config.fb_count = 2;

  esp_err_t err = esp_camera_init(&config);
  if (err != ESP_OK) {
    Serial.printf("Camera init error 0x%x", err);
    ESP.restart();
    return;
  }
}

void grabImage(){
  camera_fb_t * fb = esp_camera_fb_get();
  if(fb != NULL && fb->format == PIXFORMAT_JPEG && fb->len < bufferSize){
    Serial.print("Image Length: ");
    Serial.print(fb->len);
    Serial.print("\t Publish Image: ");
    bool result = client.publish(ESP32CAM_PUBLISH_TOPIC, (const char*)fb->buf, fb->len);
    Serial.println(result);
    if(!result){
      ESP.restart();
    }
  }
  esp_camera_fb_return(fb);
  delay(1);
}

bool executed = false;  
int output_value;

void setup() {
    Serial.begin(115200);
  WRITE_PERI_REG(RTC_CNTL_BROWN_OUT_REG, 0);
  cameraInit();
  connectAWS();
pinMode(in1, OUTPUT); 
pinMode(in2, OUTPUT);
pinMode(in3, OUTPUT);
pinMode(in4, OUTPUT);
pinMode(5, OUTPUT);
pinMode(11, OUTPUT);

pinMode(R_S, INPUT);
pinMode(L_S, INPUT);

pinMode(inA, OUTPUT);
pinMode(inB, OUTPUT);

pinMode(pump, OUTPUT);
pinMode(sensor_pin, INPUT);

delay(1000);
}
void loop() {

  client.loop();
  if(client.connected()) grabImage();
//analogWrite(5, 150);
//analogWrite(11, 150);

if((digitalRead(R_S) == 0)&&(digitalRead(L_S) == 0)){forward();}   //if Right Sensor and Left Sensor are at White color then it will call forword function

if((digitalRead(R_S) == 1)&&(digitalRead(L_S) == 0)){right();} //if Right Sensor is Black and Left Sensor is White then it will call turn Right function  

if((digitalRead(R_S) == 0)&&(digitalRead(L_S) == 1)){left();}  //if Right Sensor is White and Left Sensor is Black then it will call turn Left function

if((digitalRead(R_S) == 1)&&(digitalRead(L_S) == 1)){stopmovement();} //if Right Sensor and Left Sensor are at Black color then it will call Stop function
}

void forward(){  //forword
digitalWrite(in1, HIGH); //Right Motor forword Pin 
digitalWrite(in2, LOW);  //Right Motor backword Pin 
digitalWrite(in3, LOW);  //Left Motor backword Pin 
digitalWrite(in4, HIGH); //Left Motor forword Pin 
digitalWrite(pump, LOW);
digitalWrite(inA, LOW);
digitalWrite(inB, LOW);
}

void right(){ //turnRight
digitalWrite(in1, LOW);  //Right Motor forword Pin 
digitalWrite(in2, HIGH); //Right Motor backword Pin  
digitalWrite(in3, LOW);  //Left Motor backword Pin 
digitalWrite(in4, HIGH); //Left Motor forword Pin 
digitalWrite(pump, LOW);
digitalWrite(inA, LOW);
digitalWrite(inB, LOW);
}

void left(){ //turnLeft
digitalWrite(in1, HIGH); //Right Motor forword Pin 
digitalWrite(in2, LOW);  //Right Motor backword Pin 
digitalWrite(in3, HIGH); //Left Motor backword Pin 
digitalWrite(in4, LOW);  //Left Motor forword Pin 
digitalWrite(pump, LOW);
digitalWrite(inA, LOW);
digitalWrite(inB, LOW);
}

void stopmovement(){ //stop
digitalWrite(in1, LOW); //Right Motor forword Pin 
digitalWrite(in2, LOW); //Right Motor backword Pin 
digitalWrite(in3, LOW); //Left Motor backword Pin 
digitalWrite(in4, LOW); //Left Motor forword Pin 
delay(3000);
digitalWrite(inA, HIGH);
digitalWrite(inB, LOW);
delay(130);
digitalWrite(inA, LOW);
digitalWrite(inB, LOW);
delay(10000);
digitalWrite(inA, LOW);
digitalWrite(inB, HIGH);
delay(130);
digitalWrite(inA, LOW);
digitalWrite(inB, LOW);

output_value= analogRead(sensor_pin);
output_value = map(output_value,550,10,0,100);

if (output_value > 75);{
  digitalWrite(pump, HIGH);
  delay(1000);
  digitalWrite(pump, LOW);
}
if ((output_value > 50)&&(output_value < 75));{
  digitalWrite(pump, HIGH);
  delay(2000);
  digitalWrite(pump, LOW);  
}
if ((output_value > 25)&&(output_value < 50));{
  digitalWrite(pump, HIGH);
  delay(3000);
  digitalWrite(pump, LOW);
}
if(output_value < 25);{
  digitalWrite(pump, HIGH);
  delay(5000);
  digitalWrite(pump, LOW);
}
}
