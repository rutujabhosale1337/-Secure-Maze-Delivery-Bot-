/*
  Real-Time IoT Based Secure Autonomous Delivery Robot
  ESP32 + L298N + 2 IR line sensors + HC-SR04 + servo lock
  IMPORTANT: Adjust GPIOs to your actual board/wiring before powering.
  Demo web dashboard uses HTTP endpoints /api/status, /api/command, /api/otp.
  This is a starter firmware, not a safety-certified autonomous controller.
*/
#include <WiFi.h>
#include <WebServer.h>
#include <ESP32Servo.h>

const char* WIFI_SSID = "YOUR_WIFI_NAME";
const char* WIFI_PASSWORD = "YOUR_WIFI_PASSWORD";
const char* OTP_CODE = "9434"; // Change before demonstration

// ESP32 GPIO assignments (avoid boot-strapping pins where possible)
const int IR_LEFT = 34;       // analog/input-only GPIOs are OK for digital input
const int IR_RIGHT = 35;
const int TRIG_PIN = 5;
const int ECHO_PIN = 18;      // HC-SR04 echo is 5V: use a voltage divider to ESP32!
const int ENA = 25;           // L298N enable A; remove jumper if using PWM
const int IN1 = 26, IN2 = 27;
const int ENB = 14;           // L298N enable B; remove jumper if using PWM
const int IN3 = 32, IN4 = 33;
const int SERVO_PIN = 13;

const int MOTOR_SPEED = 150;  // 0..255
const int OBSTACLE_CM = 18;
const int LINE_BLACK = LOW;   // Change to HIGH if your sensor outputs opposite logic
const int SERVO_LOCKED = 10;
const int SERVO_UNLOCKED = 95;

WebServer server(80);
Servo lockServo;
String robotState = "IDLE";
bool locked = true;
unsigned long lastOtpUnlock = 0;
const unsigned long UNLOCK_MS = 8000;

void setMotor(int leftSpeed, int rightSpeed) {
  // Signed speeds: positive = forward, negative = reverse
  digitalWrite(IN1, leftSpeed >= 0 ? HIGH : LOW);
  digitalWrite(IN2, leftSpeed >= 0 ? LOW : HIGH);
  digitalWrite(IN3, rightSpeed >= 0 ? HIGH : LOW);
  digitalWrite(IN4, rightSpeed >= 0 ? LOW : HIGH);
  ledcWrite(ENA, constrain(abs(leftSpeed), 0, 255));
  ledcWrite(ENB, constrain(abs(rightSpeed), 0, 255));
}
void stopMotors() {
  digitalWrite(IN1, LOW); digitalWrite(IN2, LOW);
  digitalWrite(IN3, LOW); digitalWrite(IN4, LOW);
  ledcWrite(ENA, 0); ledcWrite(ENB, 0);
}
float distanceCm() {
  digitalWrite(TRIG_PIN, LOW); delayMicroseconds(3);
  digitalWrite(TRIG_PIN, HIGH); delayMicroseconds(10); digitalWrite(TRIG_PIN, LOW);
  long duration = pulseIn(ECHO_PIN, HIGH, 25000);
  if (duration == 0) return 999;
  return duration * 0.0343f / 2.0f;
}
void lockBox(bool unlock) {
  locked = !unlock;
  lockServo.write(unlock ? SERVO_UNLOCKED : SERVO_LOCKED);
  if (unlock) lastOtpUnlock = millis();
}
void sendStatus() {
  String json = "{\"state\":\"" + robotState + "\",\"locked\":" + String(locked ? "true":"false") +
                ",\"battery\":98,\"distance_cm\":" + String(distanceCm(),1) + "}";
  server.sendHeader("Access-Control-Allow-Origin", "*");
  server.send(200, "application/json", json);
}
void handleOptions() {
  server.sendHeader("Access-Control-Allow-Origin", "*");
  server.sendHeader("Access-Control-Allow-Methods", "GET, POST, OPTIONS");
  server.sendHeader("Access-Control-Allow-Headers", "Content-Type");
  server.send(204);
}
void handleCommand() {
  String body = server.arg("plain");
  server.sendHeader("Access-Control-Allow-Origin", "*");
  if (body.indexOf("\"start\"") >= 0) {
    robotState = "EN ROUTE";
    server.send(200, "application/json", "{\"ok\":true,\"state\":\"EN ROUTE\"}");
  } else if (body.indexOf("\"stop\"") >= 0) {
    robotState = "IDLE"; stopMotors();
    server.send(200, "application/json", "{\"ok\":true,\"state\":\"IDLE\"}");
  } else if (body.indexOf("\"lock\"") >= 0) {
    lockBox(false);
    server.send(200, "application/json", "{\"ok\":true,\"locked\":true}");
  } else {
    server.send(400, "application/json", "{\"ok\":false,\"error\":\"unknown command\"}");
  }
}
void handleOtp() {
  String body = server.arg("plain");
  server.sendHeader("Access-Control-Allow-Origin", "*");
  if (body.indexOf(String("\"otp\":\"") + OTP_CODE + "\"") >= 0) {
    lockBox(true);
    server.send(200, "application/json", "{\"ok\":true,\"unlocked\":true}");
  } else {
    lockBox(false);
    server.send(403, "application/json", "{\"ok\":false,\"unlocked\":false}");
  }
}
void setup() {
  Serial.begin(115200);
  pinMode(IR_LEFT, INPUT); pinMode(IR_RIGHT, INPUT);
  pinMode(TRIG_PIN, OUTPUT); pinMode(ECHO_PIN, INPUT);
  pinMode(IN1, OUTPUT); pinMode(IN2, OUTPUT); pinMode(IN3, OUTPUT); pinMode(IN4, OUTPUT);
  // Arduino-ESP32 core 3.x LEDC API: attach pin directly. For core 2.x, use ledcSetup/ledcAttachPin.
  ledcAttach(ENA, 1000, 8); ledcAttach(ENB, 1000, 8);
  lockServo.setPeriodHertz(50); lockServo.attach(SERVO_PIN, 500, 2400); lockBox(false);
  WiFi.mode(WIFI_STA); WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  Serial.print("Connecting to WiFi");
  unsigned long start = millis();
  while (WiFi.status() != WL_CONNECTED && millis() - start < 20000) { delay(400); Serial.print("."); }
  if (WiFi.status() == WL_CONNECTED) Serial.printf("\nDashboard API: http://%s\n", WiFi.localIP().toString().c_str());
  else Serial.println("\nWiFi not connected; check credentials.");
  server.on("/api/status", HTTP_GET, sendStatus);
  server.on("/api/status", HTTP_OPTIONS, handleOptions);
  server.on("/api/command", HTTP_POST, handleCommand);
  server.on("/api/command", HTTP_OPTIONS, handleOptions);
  server.on("/api/otp", HTTP_POST, handleOtp);
  server.on("/api/otp", HTTP_OPTIONS, handleOptions);
  server.begin();
}
void loop() {
  server.handleClient();
  if (locked == false && millis() - lastOtpUnlock > UNLOCK_MS) lockBox(false);

  if (robotState != "EN ROUTE") { stopMotors(); delay(10); return; }
  float d = distanceCm();
  if (d < OBSTACLE_CM) { stopMotors(); delay(30); return; } // stop safely; operator must clear obstacle
  bool leftOnLine = digitalRead(IR_LEFT) == LINE_BLACK;
  bool rightOnLine = digitalRead(IR_RIGHT) == LINE_BLACK;
  // Basic 2-sensor line following; tune speed/turn behavior for your chassis.
  if (leftOnLine && rightOnLine) setMotor(MOTOR_SPEED, MOTOR_SPEED);
  else if (leftOnLine && !rightOnLine) setMotor(70, MOTOR_SPEED);
  else if (!leftOnLine && rightOnLine) setMotor(MOTOR_SPEED, 70);
  else stopMotors(); // lost line: stop rather than guessing a route
  delay(10);
}