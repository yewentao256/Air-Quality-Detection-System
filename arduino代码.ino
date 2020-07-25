#include <SoftwareSerial.h>//串口通信库

//uno上的rx,tx
SoftwareSerial pms5Serial(6, 7);
//用于存储PM2.5读数
unsigned int pm2_5;
//将传感器通过主动式传输协议一次对外输出的数据存储到数组pms5_data中
int pms5_data[40];
//用于存储甲醛（mg/m3）、温度（摄氏度）、湿度（百分比）读数
float hcho , temp , humidity ;
void setup() {
  //串口监视器初始化
  Serial.begin(115200);
  //pmg7传感器串口初始化
  pms5Serial.begin(9600);
  pinMode(7, OUTPUT);
  //用于存储传感器通过主动式传输协议一次对外输出的数据
}
void loop() {
  int w1 = pms5Serial.read(), w2;
  Serial.println(w1);
  if (w1 == 0x42) {
    w2 = pms5Serial.read();
    Serial.println(w2);
    if (w2 == 0x4d) {
      for (int data_num = 2; pms5Serial.available() && data_num < 40; data_num++) {
        pms5_data[data_num] = pms5Serial.read();
      }
      //读取完一组数据后，进行校验；如果校验成功，获取PM2.5、甲醛、温度、湿度的读数
      int confirm = 0x42 + 0x4d;

      for (int i = 2; i < 38; i++) {
        confirm += pms5_data[i];
      }
      if (pms5_data[38] * 256 + pms5_data[39] == confirm) {
        //大气PM2.5是下标12 13
        //甲醛 28 29（真实浓度mg/m3 = 该数据/1000）
        //温度 30 31 摄氏度
        //湿度 32 33 单位%
        //温度&湿度的真实值都为 数据/10
        //校验位 38 39
        pm2_5 = pms5_data[12] * 256 + pms5_data[13];
        hcho = (float) (pms5_data[28] * 256 + pms5_data[29]) / 1000.0;
        temp = (float) (pms5_data[30] * 256 + pms5_data[31]) / 10.0;
        humidity = (float) (pms5_data[32] * 256 + pms5_data[33]) / 10.0;
        /* 其他操作 */
        Serial.print("PM2.5: ");
        Serial.println(pm2_5);
        Serial.print("HCHO: ");
        Serial.println(hcho);
        Serial.print("Temperature: ");
        Serial.println(temp);
        Serial.print("Humidity: ");
        Serial.println(humidity);
        //Serial.println(confirm);
        delay(1000);
      }
      else {
        //校验失败
        Serial.println("error");
        //Serial.println(pms5_data[38] * 256 + pms5_data[39]);
        //Serial.println(confirm);
        delay(1000);
      }
    }
  }
}
