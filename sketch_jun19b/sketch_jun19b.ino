#include <DHT.h>
#include <WiFi.h>
#include <HTTPClient.h>
#include <LiquidCrystal_I2C.h>

// Definisikan pin
#define DHTPIN 27
#define RELAYPIN 4

// Definisikan tipe DHT
#define DHTTYPE DHT22

DHT dht(DHTPIN, DHTTYPE); // Mengatur pin ESP32
LiquidCrystal_I2C lcd(0x27, 16, 2); // Alamat LCD, 16 Karakter, 2 Baris

// Deklarasikan kredensial WiFi
const char* ssid = "NAMA_WIFI"; // Ganti dengan nama WiFi Anda
const char* password = "PASS"; // Ganti dengan password WiFi Anda

// API Key ThingSpeak
const char* apiKey = "1K8OS7SCUKG2UXCE"; // Ganti dengan API Key ThingSpeak Anda
const char* server = "http://api.thingspeak.com";

void setup() {
  // Serial Monitor untuk memeriksa Suhu dan Kelembapan
  Serial.begin(115200);

  // Memulai sensor DHT
  dht.begin();

  // Memulai koneksi WiFi
  WiFi.begin(ssid, password);
  Serial.print("Connecting to WiFi");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("Connected to WiFi");

  // Memulai LCD
  lcd.init();
  lcd.clear();
  lcd.backlight(); // Menyalakan lampu latar LCD

  // Mengatur pin ESP 32 sebagai keluaran sinyal listrik
  pinMode(RELAYPIN, OUTPUT);

  // digitalWrite(RELAYPIN, HIGH);
}

void loop() {
  // Bisa langsung tanpa menyimpan ke dalam variabel terlebih dahulu
  float Temperature = dht.readTemperature();
  float Humidity = dht.readHumidity();

  if (isnan(Temperature)) { // Jika nilai yang diperoleh tidak valid (NaN), pesan kesalahan akan ditampilkan.
    Serial.println(F("Error reading temperature!"));
  }
  else {
    // Menampilkan suhu dalam celcius
    Serial.print(F("Temperature: "));
    Serial.print(Temperature);
    Serial.println(F("°C"));
  }

  if (isnan(Humidity)) { // Jika nilai yang diperoleh tidak valid (NaN), pesan kesalahan akan ditampilkan.
    Serial.println(F("Error reading humidity!"));
  }
  else {
    // Menampilkan persentase kelembapan
    Serial.print(F("Humidity: "));
    Serial.print(Humidity);
    Serial.println(F("%"));
  }

  // Mengirim data ke ThingSpeak
  if (WiFi.status() == WL_CONNECTED) {
    HTTPClient http;

    String url = String(server) + "/update?api_key=" + apiKey + "&field1=" + String(Humidity) + "&field2=" + String(Temperature);
    http.begin(url);
    int httpResponseCode = http.GET();

    if (httpResponseCode > 0) {
      Serial.print("HTTP Response code: ");
      Serial.println(httpResponseCode);
    }
    else {
      Serial.print("Error code: ");
      Serial.println(httpResponseCode);
    }
    // Tutup koneksi
    http.end();
  }
  else {
    Serial.println("WiFi Disconnected");
  }

  lcd.setCursor(0, 0); // Atur posisi karakter dimulai dari 0 baris ke-0
  lcd.print("Temp: ");
  lcd.print(Temperature);
  lcd.print((char)223); // Atau lcd.print(char(223));
  lcd.print("C");
  lcd.setCursor(0, 1); // Atur posisi karakter dimulai dari 0 baris ke-1
  lcd.print("Humidity: ");
  lcd.print(Humidity);
  lcd.print("%");

  // Algoritma pengontrolan sprayer
  if (Humidity < 90) {
    // Jika kelembapan < 40%, aktifkan sprayer
    digitalWrite(RELAYPIN, LOW);
  }
  else if (Humidity >= 90) {
    // Jika kelembapan >= 60%, matikan sprayer
    digitalWrite(RELAYPIN, HIGH);
  }
  else {
    // Jika kelembapan antara 40% hingga 60%
    if (Temperature > 25 && Humidity < 50) {
      // Jika suhu > 25°C dan kelembapan < 50%, aktifkan sprayer
      digitalWrite(RELAYPIN, LOW);
    }
    else if (Temperature < 20 && Humidity > 50) {
      // Jika suhu < 20°C dan kelembapan > 50%, matikan sprayer
      digitalWrite(RELAYPIN, HIGH);
    }
    else if (Temperature >= 20 && Temperature <= 25) {
      // Jika suhu antara 20°C dan 25°C, kelembapan 40-60% dianggap normal
      digitalWrite(RELAYPIN, HIGH);
    }
  }

  // Penundaan selama 2 detik sebelum pembacaan berikutnya
  delay(2000);
}