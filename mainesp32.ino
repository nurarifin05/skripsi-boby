#include "DHT.h" //library sensor suhu
#include <WiFi.h> //library untuk terhubung ke jaringan wifi
#include <WiFiClient.h> 
#include <PubSubClient.h> //library untuk MQTT

const char* ssid = "M2fleet";	//sesuaikan dengan nama jaringan wifi, pastikan besar kecilnya sesuai
const char* password = "logika9119";	//nama password jaringan wifi

const char* mqtt_server = "202.154.58.25"; //inisialisasi server dari broker MQTT
const char* mqtt_user = "boby";	//username untuk mengakses MQTT
const char* mqtt_password = "skripsi"; //password untuk mengakses MQTT


WiFiClient espClient;
PubSubClient client(espClient); //perintah default dari library MQTT

#define DHTPIN 2 //pin untuk sensor DHT22 terletak pada pin D2(esp32)
#define DHTTYPE DHT22 //inisialisasi jenis sensor yang digunakan
DHT dht (DHTPIN, DHTTYPE);
const char ledMerah = 5; //pin D5 untuk tegangan positif dari LED

#define LED_TOPIC     "boby/on"	//topic MQTT yang digunakan untuk pengiriman data
#define LED_TOPIC2    "boby/off"

long lastMsg = 0;
char msg[20];

void receivedCallback(char* topic, byte* payload, unsigned int length) {
  Serial.print("Message received: ");
  Serial.println(topic);
 // Serial.print("]");
  String payloadtemp;

  Serial.print("payload: ");
  for (int i = 0; i < length; i++) {
    Serial.print((char)payload[i]);
  }/*for (int u =0; u < length; u--){
    Serial.print((char)payload[u]);
  }*/
  Serial.println();

  if((char)payload[0] =='o'){		
    digitalWrite(ledMerah, HIGH);	
  }if((char)payload[0] == 'f'){
    digitalWrite(ledMerah, LOW);
  }	//digunakan untuk mengirimkan perintah dari node-red menuju esp32


}


void mqttconnect() {

  while (!client.connected()) {
    Serial.print("MQTT connecting ...");

    String clientId = "ESP32Client";

    if (client.connect(clientId.c_str(), mqtt_user, mqtt_password)) {
      Serial.println("connected");

      client.subscribe(LED_TOPIC);
      client.subscribe(LED_TOPIC2);
    } else{
      Serial.print("failed, status code =");
      Serial.print(client.state());
      Serial.println("try again in 5 seconds");

      delay(5000);
    }
  }

}

void setup() {
  Serial.begin(115200);
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);

  
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }


    dht.begin();
  pinMode (5,OUTPUT);

  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());


  client.setServer(mqtt_server, 1883);

  client.setCallback(receivedCallback);
}
void loop() {

  if (!client.connected()) {
    mqttconnect();
  }

  client.loop();

  long t = dht.readTemperature();
  long h = dht.readHumidity();
  Serial.print("Temperature: ");
  Serial.println(t);
  delay(1000);
  Serial.print("Kelembapan: ");
  Serial.println(h);
  delay(1000);

  client.publish("boby/temperature", String(t).c_str(), true);	//perintah yang digunakan untuk mengirim data suhu dari esp32 menuju nodered dengan MQTT
  client.publish("boby/humidity", String(h).c_str(), true);	////perintah yang digunakan untuk mengirim data kelembapan dari esp32 menuju nodered dengan MQTT

  long now = millis();
  if (now - lastMsg > 3000) {
    lastMsg = now;
 
  }
}
