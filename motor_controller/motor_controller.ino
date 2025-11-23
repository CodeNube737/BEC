// ==== MOTOR CONTROL PINS ====
const int LEFT_MOTOR_PIN1   = 8;   // IN1
const int LEFT_MOTOR_PIN2   = 9;   // IN2
const int LEFT_MOTOR_ENABLE = 10;  // ENA (PWM)

const int RIGHT_MOTOR_PIN1   = 11; // IN3
const int RIGHT_MOTOR_PIN2   = 12; // IN4
const int RIGHT_MOTOR_ENABLE = 5;  // ENB (PWM)

// ==== SPEED CONTROL PARAMETERS ====
int currentLeftSpeed  = 0;
int currentRightSpeed = 0;
int targetLeftSpeed   = 0;
int targetRightSpeed  = 0;
const int MAX_SPEED     = 255;
const int RAMP_RATE     = 10;
const int RAMP_DELAY_MS = 20;
unsigned long lastRampUpdate = 0;

// ==== DIRECTION ENUM ====
enum Direction {
  STOP,
  FORWARD,
  BACKWARD,
  LEFT_TURN,
  RIGHT_TURN
};
Direction currentDirection = STOP;

// ==== MOTOR DIRECTION FUNCTION ====
void setMotorDirection(bool leftForward, bool rightForward) {
  // Left motor direction
  digitalWrite(LEFT_MOTOR_PIN1, leftForward ? HIGH : LOW);
  digitalWrite(LEFT_MOTOR_PIN2, leftForward ? LOW : HIGH);
  // Right motor direction
  digitalWrite(RIGHT_MOTOR_PIN1, rightForward ? HIGH : LOW);
  digitalWrite(RIGHT_MOTOR_PIN2, rightForward ? LOW : HIGH);
}

// ==== HARD STOP FUNCTION ====
void stopMotors() {
  digitalWrite(LEFT_MOTOR_PIN1, LOW);
  digitalWrite(LEFT_MOTOR_PIN2, LOW);
  digitalWrite(RIGHT_MOTOR_PIN1, LOW);
  digitalWrite(RIGHT_MOTOR_PIN2, LOW);
  analogWrite(LEFT_MOTOR_ENABLE, 0);
  analogWrite(RIGHT_MOTOR_ENABLE, 0);
  currentLeftSpeed = 0;
  currentRightSpeed = 0;
  targetLeftSpeed = 0;
  targetRightSpeed = 0;
  currentDirection = STOP;
}

// ==== COMMAND HANDLER ====
void handleCommand(char cmd) {
  switch (cmd) {
    case 'w': // Forward
      setMotorDirection(true, true); // Both forward
      targetLeftSpeed = MAX_SPEED;
      targetRightSpeed = MAX_SPEED;
      currentDirection = FORWARD;
      Serial.println("Direction: FORWARD");
      break;
    case 'z': // Backward
      setMotorDirection(false, false); // Both backward
      targetLeftSpeed = MAX_SPEED;
      targetRightSpeed = MAX_SPEED;
      currentDirection = BACKWARD;
      Serial.println("Direction: BACKWARD");
      break;
    case 'a': // Pivot left: left reverse, right forward
      setMotorDirection(false, true);
      targetLeftSpeed = MAX_SPEED;
      targetRightSpeed = MAX_SPEED;
      currentDirection = LEFT_TURN;
      Serial.println("Direction: LEFT");
      break;
    case 'd': // Pivot right: left forward, right reverse
      setMotorDirection(true, false);
      targetLeftSpeed = MAX_SPEED;
      targetRightSpeed = MAX_SPEED;
      currentDirection = RIGHT_TURN;
      Serial.println("Direction: RIGHT");
      break;
    case 's': // Stop
      stopMotors();
      Serial.println("Direction: STOP");
      break;
    default:
      break; // Ignore others
  }
}

// ==== SMOOTH RAMPS ====
void rampSpeed() {
  // Left
  if (currentLeftSpeed < targetLeftSpeed)
    currentLeftSpeed = min(currentLeftSpeed + RAMP_RATE, targetLeftSpeed);
  else if (currentLeftSpeed > targetLeftSpeed)
    currentLeftSpeed = max(currentLeftSpeed - RAMP_RATE, targetLeftSpeed);
  // Right
  if (currentRightSpeed < targetRightSpeed)
    currentRightSpeed = min(currentRightSpeed + RAMP_RATE, targetRightSpeed);
  else if (currentRightSpeed > targetRightSpeed)
    currentRightSpeed = max(currentRightSpeed - RAMP_RATE, targetRightSpeed);
  // Apply
  analogWrite(LEFT_MOTOR_ENABLE, currentLeftSpeed);
  analogWrite(RIGHT_MOTOR_ENABLE, currentRightSpeed);
}

// ==== SETUP ====
void setup() {
  Serial.begin(9600);
  pinMode(LEFT_MOTOR_PIN1, OUTPUT);
  pinMode(LEFT_MOTOR_PIN2, OUTPUT);
  pinMode(LEFT_MOTOR_ENABLE, OUTPUT);
  pinMode(RIGHT_MOTOR_PIN1, OUTPUT);
  pinMode(RIGHT_MOTOR_PIN2, OUTPUT);
  pinMode(RIGHT_MOTOR_ENABLE, OUTPUT);
  stopMotors();
  Serial.println("Motor Controller Initialized");
  Serial.println("Motors stopped");
  Serial.println("Ready for commands");
  Serial.println("Controls: 'w' = forward, 'z' = backward, 'a' = left pivot, 'd' = right pivot, 's' = stop");
}

// ==== MAIN LOOP ====
void loop() {
  // Read serial commands
  if (Serial.available() > 0) {
    char ch = Serial.read();
    handleCommand(ch);
  }

  // Update motor speed with ramping
  unsigned long now = millis();
  if (now - lastRampUpdate >= RAMP_DELAY_MS) {
    lastRampUpdate = now;
    rampSpeed();
  }
}