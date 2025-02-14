// ESP 1 BLACK
#include <Wire.h>
#include <ESP8266WiFi.h>
#include <FirebaseESP8266.h>
#include <espnow.h>
#include <math.h>

// WiFi credentials
#define WIFI_SSID "ayat"
#define WIFI_PASSWORD "ayat1234"

// Firebase credentials
FirebaseConfig config;
FirebaseAuth auth;
FirebaseData firebaseData;

// IMU addresses for ESP1
const int IMU1 = 0x69;
const int IMU2 = 0x68;

// Motor control
const int motorPin = 16; // GPIO16 (D0) to transistor base
const int motorDuration = 1000; // Motor vibration duration in milliseconds
int bad_shoulder=0;
int bad_shoulder_count=0;
int bad_shoulder_timer=0;

int bad_back=0;
int bad_back_count=0;
int bad_back_timer=0;



// Variables for storing data
struct IMUData {
    float pitch, roll, yaw;
    float accelX, accelY, accelZ, accelMagnitude;
    float gyroX, gyroY, gyroZ;
};
IMUData imu3Data, imu4Data; // Received data from ESP2

// IMU differences thresholds with configurable tolerance
float pitchDiff12 = fabs(9.64 - 44.99);
float pitchDiff34 = fabs(76.56 - 83.33);
float rollDiff12 = fabs(7.9 - 43.33);
float tolerance = 10.0; // ± tolerance range in degrees

// Timer variables
unsigned long outOfRangeStartTime12 = 0;
unsigned long outOfRangeStartTime34 = 0;
unsigned long timerThreshold = 5000; // 5 seconds

// Counters for neck and shoulder
int neckCounter = 0;
int shoulderCounter = 0;

// IMU readings
float pitch = 0.0, roll = 0.0, yaw = 0.0;
float accelX = 0.0, accelY = 0.0, accelZ = 0.0;
float gForceX = 0.0, gForceY = 0.0, gForceZ = 0.0;

// Mode and timing variables
enum Mode { RECEIVING, SENDING };
Mode currentMode = RECEIVING;

unsigned long modeStartTime = 0;
unsigned long receivingDuration = 10000; // 10 seconds for receiving
unsigned long sendingDuration = 2000;    // 2 seconds for sending

// Callback when data is received via ESP-NOW
void onDataRecv(uint8_t* mac, uint8_t* incomingData, uint8_t len) {
    if (currentMode == RECEIVING && len == sizeof(IMUData) * 2) {
        memcpy(&imu3Data, incomingData, sizeof(IMUData));
        memcpy(&imu4Data, incomingData + sizeof(IMUData), sizeof(IMUData));
    }
}

void setup() {
    Wire.begin(4, 5); // SDA on GPIO4, SCL on GPIO5
    Serial.begin(115200);

    pinMode(motorPin, OUTPUT); // Set motorPin as output
    digitalWrite(motorPin, LOW); // Ensure motor is off initially

    // Initialize WiFi
    WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
    Serial.print("Connecting to WiFi");
    while (WiFi.status() != WL_CONNECTED) {
        Serial.print(".");
        delay(500);
    }
    Serial.println("\nConnected to WiFi!");

    // Configure Firebase
    config.host = "https://assisstive-f3b65-default-rtdb.firebaseio.com/";
    config.signer.tokens.legacy_token = "axJ7iyKwEALkklbo6l5ckdWGLt0k7UeJkfw5lHZO";
    Firebase.begin(&config, &auth);

    // Initialize ESP-NOW
    WiFi.mode(WIFI_STA);
    if (esp_now_init() != 0) {
        Serial.println("ESP-NOW Initialization Failed");
        return;
    }
    esp_now_set_self_role(ESP_NOW_ROLE_COMBO);
    esp_now_register_recv_cb(onDataRecv);

    // Initialize IMUs
    InitializeMPU(IMU1);
    InitializeMPU(IMU2);

    // Set initial mode
    modeStartTime = millis();
}

void loop() {
    unsigned long currentMillis = millis();

    if(bad_shoulder){
      bad_shoulder_timer++;
      if(bad_shoulder_timer==5){
        shoulderCounter++;
        VibrateMotor(1);
        Serial.println("Motor vibrated");
      }
    }

    if(bad_shoulder==0){
        bad_shoulder_timer=0;
        Serial.println("Shoulder alligned ");
        Serial.println("shoulder count:");
        Serial.println(shoulderCounter);
      }

if(bad_back){
      bad_back_timer++;
      if(bad_back_timer==5){
        neckCounter++;
        VibrateMotor(1);
        Serial.println("Motor vibrated");
      }
    }

    if(bad_back==0){
        bad_back_timer=0;
        Serial.println("back alligned ");
        Serial.println("back count:");
        Serial.println(neckCounter);
      }

    // Switch between receiving and sending modes based on elapsed time
    if (currentMode == RECEIVING && currentMillis - modeStartTime >= receivingDuration) {
        currentMode = SENDING;
        modeStartTime = currentMillis;
        Serial.println("Switching to SENDING mode");
    } else if (currentMode == SENDING && currentMillis - modeStartTime >= sendingDuration) {
        currentMode = RECEIVING;
        modeStartTime = currentMillis;
        Serial.println("Switching to RECEIVING mode");
    }

    if (currentMode == RECEIVING) {
        // Perform receiving tasks
        ReadAndCheckIMUs();
    } else if (currentMode == SENDING) {
        // Perform sending tasks
        IncrementFirebaseCounter("/neckCounter", neckCounter);
        IncrementFirebaseCounter("/shoulderCounter", shoulderCounter);
    }
}

void InitializeMPU(int address) {
    Wire.beginTransmission(address);
    Wire.write(0x6B); // Power management register
    Wire.write(0x00); // Wake up the MPU6050
    Wire.endTransmission();
}

void ReadAndCheckIMUs() {
    ReadIMU(IMU1);
    float pitch1 = pitch;
    float roll1 = roll;

    ReadIMU(IMU2);
    float pitch2 = pitch;
    float roll2 = roll;

    float diffPitch12 = fabs(pitch1 - pitch2);
    float diffRoll12 = fabs(roll1 - roll2);

    Serial.print("IMU1 - Pitch: "); Serial.print(pitch1); Serial.print(", Roll: "); Serial.println(roll1);
    Serial.print("IMU2 - Pitch: "); Serial.print(pitch2); Serial.print(", Roll: "); Serial.println(roll2);

    float diffPitch34 = fabs(imu3Data.pitch - imu4Data.pitch);
    Serial.print("IMU3 - Pitch: "); Serial.println(imu3Data.pitch);
    Serial.print("IMU4 - Pitch: "); Serial.println(imu4Data.pitch);

    if (CheckOutOfRange(diffPitch12, pitchDiff12) || CheckOutOfRange(diffRoll12, rollDiff12)) {
        //HandleOutOfRange(outOfRangeStartTime12, shoulderCounter, 1);
        bad_shoulder=1;
    }
    else{
      bad_shoulder=0;
    }

    if (CheckOutOfRange(diffPitch34, pitchDiff34)) {
        // HandleOutOfRange(outOfRangeStartTime34, neckCounter, 2);
     bad_back=1;
    }
    else{
      bad_back=0;
    }
}

bool CheckOutOfRange(float value, float target) {
    return (value < target - tolerance || value > target + tolerance);
}

void HandleOutOfRange(unsigned long &startTime, int &counter, int vibrationCount) {
    if (startTime == 0) {
        startTime = millis();
    } else if (millis() - startTime > timerThreshold) {
        VibrateMotor(vibrationCount);
        counter++;
        startTime = 0; // Reset the timer after action
    }
}

void ReadIMU(int address) {
    Wire.beginTransmission(address);
    Wire.write(0x3B);
    Wire.endTransmission();
    Wire.requestFrom(address, 14);

    accelX = Wire.read() << 8 | Wire.read();
    accelY = Wire.read() << 8 | Wire.read();
    accelZ = Wire.read() << 8 | Wire.read();
    Wire.read(); // Skip temperature
    
    gForceX = accelX / 16384.0;
    gForceY = accelY / 16384.0;
    gForceZ = accelZ / 16384.0;

    pitch = atan2(gForceY, sqrt(gForceX * gForceX + gForceZ * gForceZ)) * 180 / M_PI;
    roll = atan2(gForceX, sqrt(gForceY * gForceY + gForceZ * gForceZ)) * 180 / M_PI;

    delay(1000); // Read once per second
}

void IncrementFirebaseCounter(const char* counterPath, int counterValue) {
    if (Firebase.setInt(firebaseData, counterPath, counterValue)) {
        Serial.print(counterPath);
        Serial.print(" updated to: ");
        Serial.println(counterValue);
    } else {
        Serial.print("Failed to update ");
        Serial.println(counterPath);
        Serial.println(firebaseData.errorReason());
    }
}

void VibrateMotor(int count) {
    for (int i = 0; i < count; i++) {
        digitalWrite(motorPin, HIGH);
        delay(motorDuration);
        digitalWrite(motorPin, LOW);
        delay(100); // Pause between vibrations
    }
    Serial.println("Motor vibrated");
}






