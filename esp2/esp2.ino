// esp 2 white
#include <Wire.h>
#include <ESP8266WiFi.h>
#include <espnow.h>
#include <math.h>

// IMU addresses for ESP2
const int IMU3 = 0x69;
const int IMU4 = 0x68;

// Variables for storing data
struct IMUData {
  float pitch, roll, yaw;
  float accelX, accelY, accelZ, accelMagnitude;
  float gyroX, gyroY, gyroZ;
};
IMUData imu3Data, imu4Data;

// ESP1 MAC address 
uint8_t esp1Mac[] = {0x48, 0x3F, 0xDA, 0x7E, 0x0F, 0xEF};

void setup() {
  Wire.begin(4, 5); // SDA on GPIO4, SCL on GPIO5
  Serial.begin(115200);

  // Initialize WiFi in station mode for ESP-NOW
  WiFi.mode(WIFI_STA);
  if (esp_now_init() != 0) {
    Serial.println("ESP-NOW Initialization Failed");
    return;
  }
  esp_now_set_self_role(ESP_NOW_ROLE_COMBO);
  esp_now_add_peer(esp1Mac, ESP_NOW_ROLE_COMBO, 0, NULL, 0);

  // Initialize IMUs
  InitializeMPU(IMU3);
  InitializeMPU(IMU4);
}

void loop() {
  // Read data from IMU3
  imu3Data = ReadIMU(IMU3);

  // Read data from IMU4
  imu4Data = ReadIMU(IMU4);

  // Send data to ESP1
  uint8_t data[sizeof(IMUData) * 2];
  memcpy(data, &imu3Data, sizeof(IMUData));
  memcpy(data + sizeof(IMUData), &imu4Data, sizeof(IMUData));
  esp_now_send(esp1Mac, data, sizeof(data));

  delay(1000);
}

void InitializeMPU(int address) {
  Wire.beginTransmission(address);
  Wire.write(0x6B); // Power management register
  Wire.write(0x00); // Wake up the MPU6050
  Wire.endTransmission();
}

IMUData ReadIMU(int address) {
  IMUData data;

  Wire.beginTransmission(address);
  Wire.write(0x3B); // Starting register for accelerometer data
  Wire.endTransmission();
  Wire.requestFrom(address, 14); // Request 14 bytes of data

  // Read accelerometer data
  long accelX = Wire.read() << 8 | Wire.read();
  long accelY = Wire.read() << 8 | Wire.read();
  long accelZ = Wire.read() << 8 | Wire.read();
  Wire.read(); // Skip temperature data
  // long gyroX = Wire.read() << 8 | Wire.read();
  // long gyroY = Wire.read() << 8 | Wire.read();
  // long gyroZ = Wire.read() << 8 | Wire.read();

  // Convert to physical values
  data.accelX = accelX / 16384.0;
  data.accelY = accelY / 16384.0;
  data.accelZ = accelZ / 16384.0;
  // data.gyroX = gyroX / 131.0;
  // data.gyroY = gyroY / 131.0;
  // data.gyroZ = gyroZ / 131.0;

  // Calculate pitch, roll, yaw, and magnitude
  data.pitch = atan2(data.accelY, sqrt(data.accelX * data.accelX + data.accelZ * data.accelZ)) * 180 / M_PI;
    // data.roll = atan2(-data.accelX, sqrt(data.accelY * data.accelY + data.accelZ * data.accelZ)) * 180 / M_PI;
    // data.yaw += data.gyroZ * 0.01; // Assuming 10ms loop time
    // data.accelMagnitude = sqrt(data.accelX * data.accelX + data.accelY * data.accelY + data.accelZ * data.accelZ);

  Serial.print("data will be sent to 1ZA");

  return data;
}

// #include <Wire.h>

// void setup() {
//   Wire.begin(4, 5); // SDA on GPIO4, SCL on GPIO5 (default pins for ESP8266)
//   Serial.begin(115200);
//   Serial.println("Scanning I2C devices...");
// }

// void loop() {
//   byte error, address;
//   int devicesFound = 0;

//   // Scan all I2C addresses from 1 to 127
//   for (address = 1; address < 127; address++) {
//     Wire.beginTransmission(address);
//     error = Wire.endTransmission();

//     if (error == 0) {
//       Serial.print("I2C device found at address 0x");
//       if (address < 16) Serial.print("0");
//       Serial.println(address, HEX);
//       devicesFound++;
//     } else if (error == 4) {
//       Serial.print("Unknown error at address 0x");
//       if (address < 16) Serial.print("0");
//       Serial.println(address, HEX);
//     }
//   }

//   if (devicesFound == 0) {
//     Serial.println("No I2C devices found. Check connections!");
//   } else {
//     Serial.print("Total devices found: ");
//     Serial.println(devicesFound);
//   }

//   Serial.println("\nScan complete. Waiting 5 seconds before the next scan...");
//   delay(5000); // Wait 5 seconds before scanning again
// }





// #include <Wire.h>

// // IMU addresses
// const int IMU1 = 0x69;  // Address of the first IMU
// const int IMU2 = 0x68;  // Address of the second IMU

// // Variables to store IMU readings
// long accelX, accelY, accelZ;
// float gForceX, gForceY, gForceZ;
// float gyroX, gyroY, gyroZ;
// float pitch, roll, yaw;
// float accelMagnitude;

// void setup() {
//   Wire.begin();  // Initialize I2C communication
//   Serial.begin(115200);  // Start serial communication
//   while (!Serial);  // Wait for the serial port to connect
//   Serial.println("IMU Reader Initialized");

//   // Initialize the IMUs
//   InitializeMPU(IMU1);
//   InitializeMPU(IMU2);
// }

// void loop() {
//   // Read data from IMU1
//   Serial.println("IMU1 Readings:");
//   ReadIMU(IMU1);

//   // Read data from IMU2
//   Serial.println("IMU2 Readings:");
//   ReadIMU(IMU2);

//   delay(1000);  // Wait 1 second before reading again
// }

// // Function to initialize an IMU
// void InitializeMPU(int address) {
//   Wire.beginTransmission(address);
//   Wire.write(0x6B); // Power management register
//   Wire.write(0x00); // Wake up the MPU6050
//   Wire.endTransmission();
//   Serial.print("Initialized IMU at address 0x");
//   Serial.println(address, HEX);
// }

// // Function to read data from an IMU
// void ReadIMU(int address) {
//   Wire.beginTransmission(address);
//   Wire.write(0x3B); // Starting register for accelerometer data
//   Wire.endTransmission();
//   Wire.requestFrom(address, 14); // Request 14 bytes of data

//   // Read accelerometer data
//   accelX = Wire.read() << 8 | Wire.read();
//   accelY = Wire.read() << 8 | Wire.read();
//   accelZ = Wire.read() << 8 | Wire.read();
//   Wire.read(); // Skip temperature data
//   gyroX = Wire.read() << 8 | Wire.read();
//   gyroY = Wire.read() << 8 | Wire.read();
//   gyroZ = Wire.read() << 8 | Wire.read();

//   // Convert to physical values
//   gForceX = accelX / 16384.0;
//   gForceY = accelY / 16384.0;
//   gForceZ = accelZ / 16384.0;
//   float GyroX = gyroX / 131.0;
//   float GyroY = gyroY / 131.0;
//   float GyroZ = gyroZ / 131.0;

//   // Calculate pitch, roll, and yaw
//   pitch = atan2(gForceY, sqrt(gForceX * gForceX + gForceZ * gForceZ)) * 180 / M_PI;
//   roll = atan2(-gForceX, sqrt(gForceY * gForceY + gForceZ * gForceZ)) * 180 / M_PI;
//   yaw += GyroZ * 0.01; // Assuming 10ms loop time for integration

//   // Calculate acceleration magnitude
//   accelMagnitude = sqrt(gForceX * gForceX + gForceY * gForceY + gForceZ * gForceZ);

//   // Print data
//   Serial.print("Pitch: ");
//   Serial.print(pitch);
//   Serial.print("°, Roll: ");
//   Serial.print(roll);
//   Serial.print("°, Yaw: ");
//   Serial.print(yaw);
//   Serial.print("° | Accel X: ");
//   Serial.print(gForceX);
//   Serial.print("g, Accel Y: ");
//   Serial.print(gForceY);
//   Serial.print("g, Accel Z: ");
//   Serial.print(gForceZ);
//   Serial.print("g, Magnitude: ");
//   Serial.print(accelMagnitude);
//   Serial.print("g | GyroX: ");
//   Serial.print(GyroX);
//   Serial.print("°/s, GyroY: ");
//   Serial.print(GyroY);
//   Serial.print("°/s, GyroZ: ");
//   Serial.println(GyroZ);
// }

