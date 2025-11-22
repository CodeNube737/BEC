/*
 * Arduino Uno R3 Motor Controller for Sand Dune Searcher (BEC)
 * Controls 2 sets of 2 wheels through a single H-bridge motor controller
 * 
 * Features:
 * - Digital input control
 * - Direction control (forward/backward/left/right/stop)
 * - Speed ramping (smooth acceleration and deceleration)
 * - PWM speed control
 * 
 * Hardware Connections:
 * - Digital Input Pins: Forward (4), Backward (3), Left (6), Right (7)
 * - H-bridge Motor Driver (e.g., L298N or similar):
 *   - Left Motors: IN1 (pin 8), IN2 (pin 9), Enable A/PWM (pin 10)
 *   - Right Motors: IN3 (pin 11), IN4 (pin 12), Enable B/PWM (pin 5)
 */

// Motor control pins
// Left motor set (2 wheels)
const int LEFT_MOTOR_PIN1 = 8;    // IN1 - Left motor direction control 1
const int LEFT_MOTOR_PIN2 = 9;    // IN2 - Left motor direction control 2
const int LEFT_MOTOR_ENABLE = 10; // ENA - Left motor PWM speed control

// Right motor set (2 wheels)
const int RIGHT_MOTOR_PIN1 = 11;  // IN3 - Right motor direction control 1
const int RIGHT_MOTOR_PIN2 = 12;  // IN4 - Right motor direction control 2
const int RIGHT_MOTOR_ENABLE = 5; // ENB - Right motor PWM speed control

// Digital input pins for control commands
const int INPUT_FORWARD = 4;      // Digital input for forward command
const int INPUT_BACKWARD = 3;     // Digital input for backward command
const int INPUT_LEFT = 6;         // Digital input for left turn command
const int INPUT_RIGHT = 7;        // Digital input for right turn command

// Speed control parameters
int currentLeftSpeed = 0;         // Current PWM value for left motors (0-255)
int currentRightSpeed = 0;        // Current PWM value for right motors (0-255)
int targetLeftSpeed = 0;          // Target PWM value for left motors
int targetRightSpeed = 0;         // Target PWM value for right motors

const int MAX_SPEED = 255;        // Maximum PWM value (full speed)
const int TURN_SPEED_RATIO = 2;   // Speed reduction for turning (divider: 2 = 50%, 3 = 33%, etc.)
const int RAMP_RATE = 5;          // Speed change per iteration (lower = smoother)
const int RAMP_DELAY = 20;        // Delay between speed updates (ms)

// Direction states
enum Direction {
  STOP,
  FORWARD,
  BACKWARD,
  LEFT_TURN,
  RIGHT_TURN
};

Direction currentDirection = STOP;

void setup() {
  // Initialize serial communication for debugging
  Serial.begin(9600);
  Serial.println("Motor Controller Initialized");
  
  // Configure motor control pins as outputs
  pinMode(LEFT_MOTOR_PIN1, OUTPUT);
  pinMode(LEFT_MOTOR_PIN2, OUTPUT);
  pinMode(LEFT_MOTOR_ENABLE, OUTPUT);
  pinMode(RIGHT_MOTOR_PIN1, OUTPUT);
  pinMode(RIGHT_MOTOR_PIN2, OUTPUT);
  pinMode(RIGHT_MOTOR_ENABLE, OUTPUT);
  
  // Configure input pins
  pinMode(INPUT_FORWARD, INPUT_PULLUP);   // Using internal pull-up resistor
  pinMode(INPUT_BACKWARD, INPUT_PULLUP);  // Active LOW (connect to GND to activate)
  pinMode(INPUT_LEFT, INPUT_PULLUP);
  pinMode(INPUT_RIGHT, INPUT_PULLUP);
  
  // Initialize motors to stop
  stopMotors();
  
  Serial.println("Ready for commands");
}

// Main control loop
void loop() {
  // Read digital inputs (active LOW due to pull-up resistors)
  bool forwardPressed = (digitalRead(INPUT_FORWARD) == LOW);
  bool backwardPressed = (digitalRead(INPUT_BACKWARD) == LOW);
  bool leftPressed = (digitalRead(INPUT_LEFT) == LOW);
  bool rightPressed = (digitalRead(INPUT_RIGHT) == LOW);
  
  // Determine desired direction based on inputs
  // Priority: Forward/Backward takes precedence over turning for safety
  // (prevents diagonal movement which could be unpredictable)
  Direction desiredDirection = STOP;
  
  if (forwardPressed && !backwardPressed) {
    desiredDirection = FORWARD;
  } else if (backwardPressed && !forwardPressed) {
    desiredDirection = BACKWARD;
  } else if (leftPressed && !rightPressed) {
    desiredDirection = LEFT_TURN;
  } else if (rightPressed && !leftPressed) {
    desiredDirection = RIGHT_TURN;
  }
  
  // Update direction if changed
  if (desiredDirection != currentDirection) {
    setDirection(desiredDirection);
    currentDirection = desiredDirection;
  }
  
  // Perform speed ramping
  rampSpeed();
  
  // Small delay for stability
  delay(RAMP_DELAY);
}

void setDirection(Direction dir) {
  Serial.print("Direction: ");
  
  switch (dir) {
    case FORWARD:
      Serial.println("FORWARD");
      setMotorDirection(true, true, true, true);  // Both motors forward
      targetLeftSpeed = MAX_SPEED;
      targetRightSpeed = MAX_SPEED;
      break;
      
    case BACKWARD:
      Serial.println("BACKWARD");
      setMotorDirection(true, false, true, false); // Both motors backward
      targetLeftSpeed = MAX_SPEED;
      targetRightSpeed = MAX_SPEED;
      break;
      
    case LEFT_TURN:
      Serial.println("LEFT");
      setMotorDirection(true, false, true, true);  // Left backward, right forward
      targetLeftSpeed = MAX_SPEED / TURN_SPEED_RATIO;  // Adjust turn speed
      targetRightSpeed = MAX_SPEED;
      break;
      
    case RIGHT_TURN:
      Serial.println("RIGHT");
      setMotorDirection(true, true, true, false);  // Left forward, right backward
      targetLeftSpeed = MAX_SPEED;
      targetRightSpeed = MAX_SPEED / TURN_SPEED_RATIO;  // Adjust turn speed
      break;
      
    case STOP:
    default:
      Serial.println("STOP");
      targetLeftSpeed = 0;
      targetRightSpeed = 0;
      break;
  }
}

void setMotorDirection(bool leftEnabled, bool leftForward, bool rightEnabled, bool rightForward) {
  // Set left motor direction
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
  
  // Set right motor direction
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
  
  // Apply speed to motors via PWM
  analogWrite(LEFT_MOTOR_ENABLE, currentLeftSpeed);
  analogWrite(RIGHT_MOTOR_ENABLE, currentRightSpeed);
}

void stopMotors() {
  // Set all motor control pins to LOW (brake)
  digitalWrite(LEFT_MOTOR_PIN1, LOW);
  digitalWrite(LEFT_MOTOR_PIN2, LOW);
  digitalWrite(RIGHT_MOTOR_PIN1, LOW);
  digitalWrite(RIGHT_MOTOR_PIN2, LOW);
  
  // Set speed to zero
  analogWrite(LEFT_MOTOR_ENABLE, 0);
  analogWrite(RIGHT_MOTOR_ENABLE, 0);
  
  currentLeftSpeed = 0;
  currentRightSpeed = 0;
  targetLeftSpeed = 0;
  targetRightSpeed = 0;
  
  Serial.println("Motors stopped");
}
