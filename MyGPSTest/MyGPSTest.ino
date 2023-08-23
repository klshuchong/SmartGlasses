#include <TinyGPS++.h>
#include <AceTime.h>
#include <U8glib.h>
#include <String.h>

TinyGPSPlus gps;

float latitude;
float longitude;
const float latCorrection = 0.01286, lngCorrection = 0.007294;  //纬度和经度的修正
float speed;
int year, month, day, hour, minute, second;

using namespace ace_time;

static ExtendedZoneProcessor UTCProcessor, tzProcessor;
TimeZone UTCTz = TimeZone::forZoneInfo(&zonedbx::kZoneUTC, &UTCProcessor);             //标准时间
TimeZone localTz = TimeZone::forZoneInfo(&zonedbx::kZoneAsia_Shanghai, &tzProcessor);  //当前时区（上海时间，因为库里找不到北京所以用上海）
ZonedDateTime curTime;
acetime_t epochSeconds;

U8GLIB_SSD1306_128X32 u8g(U8G_I2C_OPT_NONE);  // I2C / TWI
int flag = 0;                                 //当前打印的页面类型，0-4表示时间页，5-9表示位置页

void setup() {
  Serial.begin(9600);  //set the baud rate of serial port to 9600;
  u8g.setFont(u8g_font_unifont);
}

void loop() {
  while (Serial.available() > 0) {
    gps.encode(Serial.read());  //The encode() method encodes the string in the encoding format specified by encoding.

    if (gps.location.isUpdated()) {
      //解析日期和时间
      year = gps.date.year();
      month = gps.date.month();
      day = gps.date.day();
      hour = gps.time.hour();
      minute = gps.time.minute();
      second = gps.time.second();

      //换算时区
      curTime = ZonedDateTime::forComponents(year, month, day, hour, minute, second, UTCTz);
      epochSeconds = curTime.toEpochSeconds();
      ZonedDateTime localTime = ZonedDateTime::forEpochSeconds(epochSeconds, localTz);
      year = localTime.year();
      month = localTime.month();
      day = localTime.day();
      hour = localTime.hour();
      minute = localTime.minute();
      second = localTime.second();

      //获取并打印经纬度和速率
      latitude = gps.location.lat() + latCorrection;   //gps.location.lat() can export latitude
      longitude = gps.location.lng() + lngCorrection;  //gps.location.lng() can export longitude
      speed = gps.speed.kmph();
      //Serial.print(latitude);
      //Serial.print(" ");
      //Serial.println(longitude);

      //打印时间和经纬度
      u8g.firstPage();
      if (flag < 5) {
        do {
          draw_time();
        } while (u8g.nextPage());
        flag++;
      } else if (flag < 10) {
        do {
          draw_pos();
        } while (u8g.nextPage());
        flag++;
      } else if (flag < 15) {
        do {
          draw_speed();
        } while (u8g.nextPage());
        flag++;
        if (flag == 15) flag = 0;
      }
    }
  }
}

void draw_time() {
  u8g.setPrintPos(32, 18);
  u8g.print(year);
  u8g.print("/");
  u8g.print(month);
  u8g.print("/");
  u8g.print(day);
  u8g.setPrintPos(32, 32);
  u8g.print(hour);
  u8g.print(":");
  if (minute < 10) u8g.print("0");
  u8g.print(minute);
  u8g.print(":");
  if (second < 10) u8g.print("0");
  u8g.print(second);
}

void draw_pos() {
  u8g.setPrintPos(20, 18);
  u8g.print(abs(latitude), 6);
  if (latitude > 0) u8g.print(" N");
  else u8g.print(" S");
  u8g.setPrintPos(20, 32);
  u8g.print(abs(longitude), 6);
  if (longitude > 0) u8g.print(" E");
  else u8g.print(" W");
  u8g.print(" E");
}

void draw_speed() {
  u8g.setPrintPos(20, 22);
  u8g.print(speed);
  u8g.print("km/h");
}