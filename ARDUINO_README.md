# Arduino Uno R3 Motor Controller

## Overview
This Arduino sketch controls a sand dune searcher robot with 2 sets of 2 wheels (4 wheels total) through a single H-bridge motor controller. The system provides smooth speed ramping and directional control via digital inputs.

## Features
- ✅ Digital input control (4 command inputs)
- ✅ Bi-directional motor control (forward/backward)
- ✅ Differential steering (left/right turns)
- ✅ Smooth speed ramping (acceleration and deceleration)
- ✅ PWM speed control (0-255)
- ✅ Serial debug output
- ✅ Emergency stop capability

## Quick Start

### 1. Hardware Setup
- Connect motors and H-bridge as per [WIRING_GUIDE.md](WIRING_GUIDE.md)
- Ensure adequate power supply for motors (6-12V recommended)
- Connect digital input switches to pins 3, 4, 6, 7

### 2. Software Setup
1. Open `motor_controller.ino` in Arduino IDE
2. Connect Arduino Uno R3 via USB
3. Select **Board**: "Arduino Uno" (Tools → Board)
4. Select correct **Port** (Tools → Port)
5. Click **Upload** (→ button)

### 3. Testing
1. Open Serial Monitor (Tools → Serial Monitor)
2. Set baud rate to **9600**
3. Press input switches to test each direction
4. Observe motor response and serial output

## Control Interface

### Digital Inputs (Active LOW)
Connect switches between Arduino pin and GND:

| Pin | Function | Action |
|-----|----------|--------|
| 4   | Forward  | Move robot forward |
| 3   | Backward | Move robot backward |
| 6   | Left     | Turn left (left wheels slower/reverse) |
| 7   | Right    | Turn right (right wheels slower/reverse) |

**Note**: Inputs use internal pull-up resistors, so they are active when connected to ground.

## Configuration Parameters

### Speed Settings
```cpp
const int MAX_SPEED = 255;        // Maximum PWM (0-255)
const int RAMP_RATE = 5;          // Speed change per cycle
const int RAMP_DELAY = 20;        // Delay between updates (ms)
```

### Tuning Tips
- **Increase RAMP_RATE**: Faster acceleration/deceleration
- **Decrease RAMP_RATE**: Smoother, slower speed changes
- **Increase RAMP_DELAY**: Slower response, smoother ramping
- **Decrease RAMP_DELAY**: Faster response, may be jerky
- **Adjust MAX_SPEED**: Reduce for slower max speed, increase for faster

## Operation Modes

### Forward
- Both motor sets run forward at full speed
- Ramps up smoothly from stop

### Backward  
- Both motor sets run backward at full speed
- Ramps up smoothly from stop

### Left Turn
- Right motors: Forward at full speed
- Left motors: Backward at half speed
- Creates pivot turn to the left

### Right Turn
- Left motors: Forward at full speed  
- Right motors: Backward at half speed
- Creates pivot turn to the right

### Stop
- All motors ramp down to zero speed
- Motors are braked (both pins LOW)

## Pin Assignment Reference

### Motor Control Pins
```
Left Motors (2 wheels):
  - Direction 1: Pin 8 (IN1)
  - Direction 2: Pin 9 (IN2)
  - Speed PWM:   Pin 10 (ENA)

Right Motors (2 wheels):
  - Direction 1: Pin 11 (IN3)
  - Direction 2: Pin 12 (IN4)
  - Speed PWM:   Pin 5 (ENB)
```

### Input Control Pins
```
Pin 4: Forward command
Pin 3: Backward command  
Pin 6: Left turn command
Pin 7: Right turn command
```

## Serial Monitor Output

The code outputs debug information to help troubleshoot:

```
Motor Controller Initialized
Ready for commands
Direction: FORWARD
Direction: STOP
Direction: LEFT
...
```

## Customization

### Change Turn Behavior
Modify the `LEFT_TURN` or `RIGHT_TURN` cases in `setDirection()`:

```cpp
case LEFT_TURN:
  // Current: Left backward, right forward (pivot turn)
  setMotorDirection(true, false, true, true);
  targetLeftSpeed = MAX_SPEED / 2;
  targetRightSpeed = MAX_SPEED;
  
  // Alternative 1: Left stop, right forward (swing turn)
  // setMotorDirection(false, false, true, true);
  // targetLeftSpeed = 0;
  // targetRightSpeed = MAX_SPEED;
  
  // Alternative 2: Left slow, right fast (gentle turn)
  // setMotorDirection(true, true, true, true);
  // targetLeftSpeed = MAX_SPEED / 3;
  // targetRightSpeed = MAX_SPEED;
  break;
```

### Add Remote Control
Replace digital inputs with RC receiver signals:
```cpp
// In setup():
pinMode(INPUT_FORWARD, INPUT);  // Remove PULLUP for RC

// In loop():
int forwardSignal = pulseIn(INPUT_FORWARD, HIGH, 25000);
bool forwardPressed = (forwardSignal > 1500);  // Typical RC threshold
```

### Add Speed Control Input
Use analog input for variable speed:
```cpp
const int SPEED_POT_PIN = A0;

void loop() {
  int speedSetting = analogRead(SPEED_POT_PIN);
  int maxSpeed = map(speedSetting, 0, 1023, 50, 255);
  // Use maxSpeed instead of MAX_SPEED in setDirection()
}
```

## Troubleshooting

### Issue: Motors don't respond
- Check Serial Monitor for "Ready for commands" message
- Verify power supply is connected and adequate
- Check all wiring connections
- Measure voltage at motor terminals

### Issue: Motors run but don't ramp
- Increase `RAMP_RATE` value
- Decrease `RAMP_DELAY` value  
- Check if PWM pins are correct (must be pins 3, 5, 6, 9, 10, 11)

### Issue: Inputs not detected
- Check Serial Monitor for direction changes
- Verify switches are connected to ground when pressed
- Test with multimeter: pins should read 5V (open) or 0V (pressed)

### Issue: Motors overheat
- Reduce `MAX_SPEED` constant
- Check motor current rating vs H-bridge rating
- Ensure adequate heat sinking on H-bridge
- Verify motors aren't stalled or overloaded

## Technical Specifications

- **Platform**: Arduino Uno R3 (ATmega328P)
- **Operating Voltage**: 5V logic, 6-12V motor power
- **PWM Frequency**: ~490 Hz (pins 5, 6) / ~980 Hz (pins 9, 10, 11)
- **PWM Resolution**: 8-bit (0-255)
- **Speed Ramp Rate**: Configurable (default: 5 steps per 20ms)
- **Maximum Speed**: Configurable (default: 255/255 = 100%)
- **Input Type**: Active LOW with internal pull-ups
- **Serial Baud Rate**: 9600 bps

## License
This code is provided as-is for the BEC Sand Dune Searcher project.

## Support
For wiring details, see [WIRING_GUIDE.md](WIRING_GUIDE.md)
