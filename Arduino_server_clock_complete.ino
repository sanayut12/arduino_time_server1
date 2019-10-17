
#include <WiFiManager.h>     //https://github.com/tzapu/WiFiManager
#include <PubSubClient.h>


const char* ssid = "BaanNak_F2"; //ใส่ชื่อ SSID Wifi
const char* password = "27494332"; //ใส่รหัสผ่าน

int timezone = 7 * 3600; //ตั้งค่า TimeZone ตามเวลาประเทศไทย
int dst = 0; //กำหนดค่า Date Swing Time

// ส่วนของการตั้งค่า MQTT Server
#define mqtt_server "soldier.cloudmqtt.com" // Server MQTT
#define mqtt_port   10081 // Port MQTT
#define mqtt_user "gcznloay" // Username
#define mqtt_password "oV_PkJecvSU3" // Password
WiFiClient espClient;
PubSubClient client(espClient);
int timeSinceLastRead = 0;

void callback(char* topic, byte* payload, unsigned int length) {
  Serial.print("Message arrived [");
  Serial.print(topic);
  Serial.print("] ");
  String msg = "";
  }

void setup()
{
  /*Serial.begin(115200);
  Serial.setDebugOutput(true);*/
//ส่วนของการเชื่อมต่อ wifi
  WiFi.mode(WIFI_STA); //เชื่อมต่อ Wifi
  WiFi.begin(ssid, password);
  

  configTime(timezone, dst, "pool.ntp.org", "time.nist.gov"); //ดึงเวลาจาก Server
 //ส่วนของการเชื่อมต่อกับ cloud mqtt
  client.setServer(mqtt_server, mqtt_port);
 
}

void loop()
{
  //เช็คการเชื่อมต่อ ระหว่าง cloud mqtt
  if (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    if (client.connect("ESP8266Client", mqtt_user, mqtt_password)) {
      Serial.println("connected");
      client.subscribe("/ESP/TEMP");
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      delay(5000);
      return;
    }
  }
  client.loop();

//ส่วนของการดึงข้อมูลเวลาจาก server
//configTime(timezone, dst, "pool.ntp.org", "time.nist.gov"); //แสดงเวลาปัจจุบัน
time_t now = time(nullptr);
struct tm* p_tm = localtime(&now);
/*Serial.print(p_tm->tm_mday);
Serial.print(":");
Serial.print(p_tm->tm_mon);
Serial.print(":2");
Serial.print(p_tm->tm_year);
Serial.print(":");
Serial.print(p_tm->tm_wday);
Serial.print("..");
Serial.print(p_tm->tm_hour);
Serial.print(":");
Serial.print(p_tm->tm_min);
Serial.print(":");
Serial.print(p_tm->tm_sec);
Serial.println("");*/
//จัดเรียง ระเบียบ ของเวลา และทำให้เป็นแค่ 1ตัวแปร ที่เป็นแบบ string
char x[20] = "";
sprintf(x,"%d : %d : %d",p_tm->tm_hour,p_tm->tm_min,p_tm->tm_sec);
//ส่วนของการส่งข้อมูลไปยัง cloud mqtt
client.publish("/ESP/TEMP",x);

//หน่วงการทำงาน 1 วินาที
delay(1000);
}
