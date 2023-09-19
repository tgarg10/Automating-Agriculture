#include "secrets.h"
#include <WiFiClientSecure.h>
#include <MQTT.h>
#include "WiFi.h"
#include "esp_camera.h"

//Brownout Error
#include "soc/soc.h"
#include "soc/rtc_cntl_reg.h"

//Time
#include <NTPtimeESP.h>

#include <ImgurUploader.h>

#define IMGUR_CLIENT_ID "54cdaf3befe6f96"

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

#define ESP32CAM_PUBLISH_TOPIC   "esp32/cam_1"
#define ESP32CAM_PUBLISH_TOPIC2   "image_log/cam" 
#define ESP32CAM_SUBSCRIBE_TOPIC   "cam2_controls" 
#define BUFFER_LEN 256

char msg[BUFFER_LEN];
const int bufferSize = 1024 * 23; // 23552 bytes

WiFiClientSecure net = WiFiClientSecure();
MQTTClient client = MQTTClient(bufferSize);

NTPtime NTPch("ch.pool.ntp.org");
strDateTime dateTime;

void logimage(){
  camera_fb_t * fb = esp_camera_fb_get();
  if(fb != NULL && fb->format == PIXFORMAT_JPEG && fb->len < bufferSize){
    int imgurImage = imgurUploader.uploadBytes(fb->buf, fb->len, "log.jpg", "image/jpeg");
    if(imgurImage > 0){
      snprintf(msg, BUFFER_LEN, "{\"camera\":\"Hub Camera 2\", \"link\":\"%s\"}", imgurUploader.getURL());
      client.publish(ESP32CAM_PUBLISH_TOPIC2, msg);
      Serial.setTimeout(10000);
    }
  }
  esp_camera_fb_return(fb);
}

void* getCommands(String& topic, String& payload){
  if (payload == "\"log\""){
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

void setup() {
  Serial.begin(115200);
  WRITE_PERI_REG(RTC_CNTL_BROWN_OUT_REG, 0);
  cameraInit();
  connectAWS();
}

byte _min = 1;
bool executed = false;  
void loop() {
  client.loop();
  if(client.connected()) grabImage();
  dateTime = NTPch.getNTPtime(1.0, 1);
  byte minute =  dateTime.minute;
  if (minute <= _min && !executed){
    executed = true;
    logimage();
  } else if (minute > _min && executed){
    executed = false;
  }
}
