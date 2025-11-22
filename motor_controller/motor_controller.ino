
// =========================================================
// INTEGRATED CODE: SENSORS + MOTOR CONTROLLER + FSM
// =========================================================
// ================== MOTOR CONTROL SECTION ==================
//
// Based on your partner's motor controller, adapted to:
// - Remove button inputs
// - Expose percent-based left/right speed control
// - Keep ramping and direction control
//
// Motor control pins
// Left motor set (2 wheels)
const int LEFT_MOTOR_PIN1   = 8;   // IN1 - Left motor direction control 1
const int LEFT_MOTOR_PIN2   = 9;   // IN2 - Left motor direction control 2
const int LEFT_MOTOR_ENABLE = 10;  // ENA - Left motor PWM speed control
// Right motor set (2 wheels)
const int RIGHT_MOTOR_PIN1   = 11; // IN3 - Right motor direction control 1
const int RIGHT_MOTOR_PIN2   = 12; // IN4 - Right motor direction control 2
const int RIGHT_MOTOR_ENABLE = 5;  // ENB - Right motor PWM speed control
// Speed control parameters
int currentLeftSpeed  = 0;   // Current PWM value for left motors (0-255)
int currentRightSpeed = 0;   // Current PWM value for right motors (0-255)
int targetLeftSpeed   = 0;   // Target PWM value for left motors
int targetRightSpeed  = 0;   // Target PWM value for right motors
const int MAX_SPEED     = 255;  // Maximum PWM value (full speed)
const int RAMP_RATE     = 5;    // Speed change per ramp step
const int RAMP_DELAY_MS = 20;   // Time between ramp steps (ms)
unsigned long lastRampUpdate = 0;
// Direction states (we mostly care about FORWARD/STOP, but keep enum)
enum Direction {
  STOP,
  FORWARD,
  BACKWARD,
  LEFT_TURN,
  RIGHT_TURN
};
Direction currentDirection = STOP;
// Forward declaration
void stopMotors();
// Set motor direction (both sides)
void setMotorDirection(bool leftEnabled, bool leftForward,
                       bool rightEnabled, bool rightForward) {
  // Left motor
  if (leftEnabled) {
    if (leftForward) {
      digitalWrite(LEFT_MOTOR_PIN1, HIGH);
      digitalWrite(LEFT_MOTOR_PIN2, LOW);
    } else {
      digitalWrite(LEFT_MOTOR_PIN1, LOW);
      digitalWrite(LEFT_MOTOR_PIN2, HIGH);
    }
  } else {
    digitalWrite(LEFT_MOTOR_PIN1, LOW);
    digitalWrite(LEFT_MOTOR_PIN2, LOW);
  }
  // Right motor
  if (rightEnabled) {
    if (rightForward) {
      digitalWrite(RIGHT_MOTOR_PIN1, HIGH);
      digitalWrite(RIGHT_MOTOR_PIN2, LOW);
    } else {
      digitalWrite(RIGHT_MOTOR_PIN1, LOW);
      digitalWrite(RIGHT_MOTOR_PIN2, HIGH);
    }
  } else {
    digitalWrite(RIGHT_MOTOR_PIN1, LOW);
    digitalWrite(RIGHT_MOTOR_PIN2, LOW);
  }
}
// Core ramping logic – called periodically from loop()
void rampSpeed() {
  // Gradually adjust left motor speed
  if (currentLeftSpeed < targetLeftSpeed) {
    currentLeftSpeed = min(currentLeftSpeed + RAMP_RATE, targetLeftSpeed);
  } else if (currentLeftSpeed > targetLeftSpeed) {
    currentLeftSpeed = max(currentLeftSpeed - RAMP_RATE, targetLeftSpeed);
  }
  // Gradually adjust right motor speed
  if (currentRightSpeed < targetRightSpeed) {
    currentRightSpeed = min(currentRightSpeed + RAMP_RATE, targetRightSpeed);
  } else if (currentRightSpeed > targetRightSpeed) {
    currentRightSpeed = max(currentRightSpeed - RAMP_RATE, targetRightSpeed);
  }
  // Apply speed via PWM
  analogWrite(LEFT_MOTOR_ENABLE,  currentLeftSpeed);
  analogWrite(RIGHT_MOTOR_ENABLE, currentRightSpeed);
}
// Hard stop
void stopMotors() {
  // Brake both sides
  digitalWrite(LEFT_MOTOR_PIN1, LOW);
  digitalWrite(LEFT_MOTOR_PIN2, LOW);
  digitalWrite(RIGHT_MOTOR_PIN1, LOW);
  digitalWrite(RIGHT_MOTOR_PIN2, LOW);
  analogWrite(LEFT_MOTOR_ENABLE,  0);
  analogWrite(RIGHT_MOTOR_ENABLE, 0);
  currentLeftSpeed  = 0;
  currentRightSpeed = 0;
  targetLeftSpeed   = 0;
  targetRightSpeed  = 0;
  currentDirection = STOP;
}
// High-level: set targets as percentages (0–100) for each side.
// We keep both sides going FORWARD and use differential speeds for turns.
void setMotorTargetsPct(int leftPct, int rightPct) {
  // Clamp 0–100
  leftPct  = constrain(leftPct,  0, 100);
  rightPct = constrain(rightPct, 0, 100);
  // Convert percentage to PWM 0–255
  targetLeftSpeed  = map(leftPct,  0, 100, 0, MAX_SPEED);
  targetRightSpeed = map(rightPct, 0, 100, 0, MAX_SPEED);
  if (leftPct == 0 && rightPct == 0) {
    // Full stop
    stopMotors();
  } else {
    // Forward on both sides, enabled
    setMotorDirection(true, true, true, true);
    currentDirection = FORWARD;
  }
}
// ================== ULTRASONIC SENSOR SECTION ==================
// Pin assignments
const int TRIG_LEFT   = A0;
const int ECHO_LEFT   = A1;
const int TRIG_RIGHT  = A2;
const int ECHO_RIGHT  = A3;
const int TRIG_FRONT  = A4;
const int ECHO_FRONT  = A5;
// Global distance variables (cm)
double leftDistCM  = 999.0;
double rightDistCM = 999.0;
double frontDistCM = 999.0;
// Timing control
const unsigned long PING_INTERVAL = 100; // 100 ms = 10 Hz
unsigned long lastLeftPing  = 0;
unsigned long lastRightPing = 0;
unsigned long lastFrontPing = 0;
// One sensor read
double readUltrasonicCM(int trigPin, int echoPin) {
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);
  long dur = pulseIn(echoPin, HIGH, 30000); // up to ~5 m
  if (dur == 0) {
    return 999.0; // no echo
  }
  double distanceCM = (double)dur * 0.0343 / 2.0;
  return distanceCM;
}
void initUltrasonicSensors() {
  pinMode(TRIG_LEFT,  OUTPUT);
  pinMode(ECHO_LEFT,  INPUT);
  pinMode(TRIG_RIGHT, OUTPUT);
  pinMode(ECHO_RIGHT, INPUT);
  pinMode(TRIG_FRONT, OUTPUT);
  pinMode(ECHO_FRONT, INPUT);
  digitalWrite(TRIG_LEFT,  LOW);
  digitalWrite(TRIG_RIGHT, LOW);
  digitalWrite(TRIG_FRONT, LOW);
}
void updateDistanceReadings() {
  unsigned long now = millis();
  if (now - lastLeftPing >= PING_INTERVAL) {
    lastLeftPing = now;
    leftDistCM = readUltrasonicCM(TRIG_LEFT, ECHO_LEFT);
  }
  if (now - lastRightPing >= PING_INTERVAL) {
    lastRightPing = now;
    rightDistCM = readUltrasonicCM(TRIG_RIGHT, ECHO_RIGHT);
  }
  if (now - lastFrontPing >= PING_INTERVAL) {
    lastFrontPing = now;
    frontDistCM = readUltrasonicCM(TRIG_FRONT, ECHO_FRONT);
  }
}
// ================== FSM + GO SWITCH SECTION ==================
// GO/STOP toggle button reuses forward switch
const int PIN_GO_SWITCH = 3;  // NO to this pin, COM to GND
// ================== FSM SPEED CALIBRATION ==================
// Max drive speed as a percentage of full motor power (0–100)
int DRIVE_MAX_PCT  = 100;   // tune this for "overall speed"
// Inner wheel speed during turns as a percentage of DRIVE_MAX_PCT (0–100)
int TURN_SLOW_PCT  = 50;   // 50 = inner wheel at 50% of straight speed
// Calibration constants
struct Calibration {
  double L_detect_min;
  double L_detect_max;
  double R_inner_close;
  double R_outer_far_min;
  double R_outer_far_max;
  double R_outer_turn_min;
  double R_outer_turn_max;
  double F_final_detect;
  double F_stop_dist;
};
Calibration cal;
// States
enum State {
  STATE_IDLE = 0,
  STATE_PHASE1,
  STATE_PHASE2,
  STATE_PHASE3,
  STATE_PHASE4,
  STATE_PHASE5,
  STATE_PHASE6,
  STATE_DONE
};
State state = STATE_IDLE;
bool running = false;
// Button debounce
int lastRawButton       = HIGH;
int stableButtonState   = HIGH;
unsigned long lastChangeTime = 0;
const unsigned long debounceMs = 50;
void updateGoSwitch() {
  int raw = digitalRead(PIN_GO_SWITCH);
  unsigned long now = millis();
  if (raw != lastRawButton) {
    lastChangeTime = now;
    lastRawButton = raw;
  }
  if ((now - lastChangeTime) > debounceMs && raw != stableButtonState) {
    stableButtonState = raw;
    if (stableButtonState == LOW) { // button press
      running = !running;
      if (running) {
        state = STATE_PHASE1;
      } else {
        state = STATE_IDLE;
        stopMotors();
      }
    }
  }
}
bool inRange(double value, double minVal, double maxVal) {
  return (value >= minVal && value <= maxVal);
}
// FSM motor wrappers using percent-based targets
void driveStraight() {
  // Both sides at configured max drive speed
  setMotorTargetsPct(DRIVE_MAX_PCT, DRIVE_MAX_PCT);
}
void driveLeftArc() {
  // Inner (left) side slower, outer (right) at full drive speed
  int inner = (DRIVE_MAX_PCT * TURN_SLOW_PCT) / 100;
  setMotorTargetsPct(inner, DRIVE_MAX_PCT);
}
void driveRightArc() {
  // Inner (right) side slower, outer (left) at full drive speed
  int inner = (DRIVE_MAX_PCT * TURN_SLOW_PCT) / 100;
  setMotorTargetsPct(DRIVE_MAX_PCT, inner);
}
// ================== SETUP ==================
void setup() {
  Serial.begin(9600);
  // Motor pins
  pinMode(LEFT_MOTOR_PIN1,   OUTPUT);
  pinMode(LEFT_MOTOR_PIN2,   OUTPUT);
  pinMode(LEFT_MOTOR_ENABLE, OUTPUT);
  pinMode(RIGHT_MOTOR_PIN1,   OUTPUT);
  pinMode(RIGHT_MOTOR_PIN2,   OUTPUT);
  pinMode(RIGHT_MOTOR_ENABLE, OUTPUT);
  // GO switch
  pinMode(PIN_GO_SWITCH, INPUT_PULLUP);
  // Sensors
  initUltrasonicSensors();
  // Calibration defaults
  cal.L_detect_min     = 13.0;
  cal.L_detect_max     = 15.0;
  cal.R_inner_close    = 12.0;
  cal.R_outer_far_min  = 20.0;
  cal.R_outer_far_max  = 30.0;
  cal.R_outer_turn_min = 12.0;
  cal.R_outer_turn_max = 14.0;
  cal.F_final_detect   = 20.0;
  cal.F_stop_dist      = 7.0;
  stopMotors();
  state   = STATE_IDLE;
  running = false;
}
// ================== FULL PATH FSM ==================
void loop_full() {
  updateGoSwitch();
  if (!running) {
    stopMotors();
    state = STATE_IDLE;
    return;
  }
  // Use latest sensor readings (already updated)
  double L = leftDistCM;
  double R = rightDistCM;
  double F = frontDistCM;
  switch (state) {
    case STATE_IDLE:
      stopMotors();
      break;
    case STATE_PHASE1:
      // Straight until left in [L_detect_min, L_detect_max]
      driveStraight();
      if (inRange(L, cal.L_detect_min, cal.L_detect_max)) {
        state = STATE_PHASE2;
      }
      break;
    case STATE_PHASE2:
      // Left arc until inner object on right (R <= R_inner_close)
      driveLeftArc();
      if (R <= cal.R_inner_close) {
        state = STATE_PHASE3;
      }
      break;
    case STATE_PHASE3:
      // Straight_2: until right in outer far band
      driveStraight();
      if (inRange(R, cal.R_outer_far_min, cal.R_outer_far_max)) {
        state = STATE_PHASE4;
      }
      break;
    case STATE_PHASE4:
      // Approach: straight until right in turning band
      driveStraight();
      if (inRange(R, cal.R_outer_turn_min, cal.R_outer_turn_max)) {
        state = STATE_PHASE5;
      }
      break;
    case STATE_PHASE5:
      // Right arc until front sees final obstacle
      driveRightArc();
      if (F <= cal.F_final_detect) {
        state = STATE_PHASE6;
      }
      break;
    case STATE_PHASE6:
      // Final approach: straight to stop distance
      driveStraight();
      if (F <= cal.F_stop_dist) {
        stopMotors();
        state = STATE_DONE;
      }
      break;
    case STATE_DONE:
      stopMotors();
      break;
    default:
      stopMotors();
      state = STATE_IDLE;
      break;
  }
}
// ================== LEFT-ONLY TEST FSM ==================
void loop_testLeftOnly() {
  updateGoSwitch();
  if (!running) {
    stopMotors();
    state = STATE_IDLE;
    return;
  }
  double L = leftDistCM;
  double R = rightDistCM;
  switch (state) {
    case STATE_IDLE:
      stopMotors();
      break;
    case STATE_PHASE1:
      // Straight until left sees your foot in band
      driveStraight();
      if (inRange(L, cal.L_detect_min, cal.L_detect_max)) {
        state = STATE_PHASE2;
      }
      break;
    case STATE_PHASE2:
      // Left arc; go back to straight if you "pass" the object
      driveLeftArc();
      if (L > cal.L_detect_max) {
        state = STATE_PHASE1;
      }
      if (R <= cal.R_inner_close) {
        state = STATE_PHASE1;
      }
      break;
    default:
      state = STATE_PHASE1;
      break;
  }
}
// ================== MODE SELECT + MAIN LOOP ==================
//
// 1 = full course logic
// 2 = left-only test (circle around your foot)
#define MODE 2
void loop() {
  // 1) Update sensors
  updateDistanceReadings();
  // 2) Update motor speeds with ramping at ~RAMP_DELAY_MS
  unsigned long now = millis();
  if (now - lastRampUpdate >= RAMP_DELAY_MS) {
    lastRampUpdate = now;
    rampSpeed();
  }
  // 3) Run the selected FSM
#if MODE == 1
  loop_full();
#else
  loop_testLeftOnly();
#endif
  // Optional debug prints if needed:
  /*
  Serial.print("L: "); Serial.print(leftDistCM, 1);
  Serial.print("  F: "); Serial.print(frontDistCM, 1);
  Serial.print("  R: "); Serial.print(rightDistCM, 1);
  Serial.print("  State: "); Serial.println(state);
  */
}
