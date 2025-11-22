# Quick Start Guide - 1.5 Hour Build

This guide helps you get the BEC Sand Dune Searcher running in 1.5 hours or less.

## Pre-Flight Checklist (5 minutes)
- [ ] Arduino Uno R3
- [ ] H-bridge motor controller (L298N recommended)
- [ ] 4 DC motors (2 left, 2 right)
- [ ] Motor power supply (6-12V, adequate amperage)
- [ ] 4 input switches/buttons
- [ ] Jumper wires
- [ ] USB cable for Arduino programming
- [ ] Computer with Arduino IDE installed

## Step 1: Arduino Setup (10 minutes)
1. Download Arduino IDE from arduino.cc if not installed
2. Connect Arduino Uno R3 to computer via USB
3. Open Arduino IDE
4. Go to **Tools → Board** → Select "Arduino Uno"
5. Go to **Tools → Port** → Select the appropriate COM/USB port
6. Open `motor_controller.ino` from this repository
7. Click **Upload** button (→)
8. Wait for "Done uploading" message

## Step 2: Test Arduino Code (5 minutes)
1. Open **Tools → Serial Monitor**
2. Set baud rate to **9600** (bottom right dropdown)
3. You should see:
   ```
   Motor Controller Initialized
   Ready for commands
   ```
4. If you see this message, the Arduino is programmed correctly!

## Step 3: Wire the Control Inputs (10 minutes)
Connect 4 switches/buttons for control:
- Pin 4 to switch to GND (Forward)
- Pin 3 to switch to GND (Backward)
- Pin 6 to switch to GND (Left)
- Pin 7 to switch to GND (Right)

**Quick Tip**: Use a breadboard to organize the switches.

## Step 4: Test Inputs (5 minutes)
1. Keep Serial Monitor open
2. Press each switch one at a time
3. Verify Serial Monitor shows:
   - "Direction: FORWARD" when pin 4 pressed
   - "Direction: BACKWARD" when pin 3 pressed
   - "Direction: LEFT" when pin 6 pressed
   - "Direction: RIGHT" when pin 7 pressed
   - "Direction: STOP" when released

If all inputs work correctly, proceed to motor wiring.

## Step 5: Wire the Motors (20 minutes)

### A. Connect motors in parallel pairs:
**Left side (2 motors):**
- Connect positive terminals together → H-bridge OUT1
- Connect negative terminals together → H-bridge OUT2

**Right side (2 motors):**
- Connect positive terminals together → H-bridge OUT3
- Connect negative terminals together → H-bridge OUT4

### B. Connect H-bridge control signals:
```
Arduino → H-bridge
Pin 8   → IN1
Pin 9   → IN2
Pin 10  → ENA
Pin 11  → IN3
Pin 12  → IN4
Pin 5   → ENB
```

### C. Connect grounds:
```
Arduino GND → H-bridge Logic GND
Motor Power GND → H-bridge Motor GND
(All grounds must be common!)
```

## Step 6: Power Connections (10 minutes)

### Motor Power Supply:
1. Connect motor power supply (+) to H-bridge motor power input (VCC/VM)
2. Connect motor power supply (-) to H-bridge motor ground
3. **CRITICAL**: Connect motor ground to Arduino ground (common ground)

### Arduino Power:
**Option A**: Keep USB connected (testing phase)
**Option B**: Use external 7-12V to Arduino VIN pin
**Option C**: Use 5V from H-bridge regulator (if available) to Arduino 5V pin

⚠️ **Safety Check**:
- [ ] All grounds connected together?
- [ ] Motor power supply voltage correct (6-12V)?
- [ ] No shorts between power and ground?
- [ ] All connections secure?

## Step 7: First Motor Test (15 minutes)

### SAFETY FIRST:
- Keep robot on blocks (wheels off ground)
- Be ready to disconnect power
- Start with low voltage if adjustable supply

### Test Procedure:
1. Power on motor supply
2. Keep Serial Monitor open
3. Press Forward switch
   - Check: Both wheel sets rotate forward
   - Check: Speed ramps up smoothly
4. Release switch
   - Check: Motors ramp down and stop
5. Test Backward, Left, Right similarly

### If motors don't move:
- Check power supply voltage with multimeter
- Verify H-bridge power LED (if present)
- Check Serial Monitor for direction changes
- Measure voltage at motor terminals

### If motors run wrong direction:
- Swap the two wires for that motor pair
- OR modify code direction logic

## Step 8: Calibration and Tuning (10 minutes)

### Adjust Speed:
Edit in `motor_controller.ino`:
```cpp
const int MAX_SPEED = 255;    // Lower for slower max speed (e.g., 180)
```

### Adjust Ramping:
```cpp
const int RAMP_RATE = 5;      // Increase for faster accel (e.g., 10)
const int RAMP_DELAY = 20;    // Decrease for faster response (e.g., 10)
```

### Adjust Turn Behavior:
In `setDirection()` function, modify turn speed ratios:
```cpp
case LEFT_TURN:
  targetLeftSpeed = MAX_SPEED / 2;   // Change ratio for turn rate
  targetRightSpeed = MAX_SPEED;
```

### Upload Changes:
1. Click Upload in Arduino IDE
2. Wait for completion
3. Test new behavior

## Step 9: Final Testing (10 minutes)

1. **Controlled Surface Test**:
   - Place robot on smooth, flat surface
   - Test all four directions
   - Verify smooth acceleration/deceleration

2. **Load Test**:
   - Let robot run for 1-2 minutes in each direction
   - Check for overheating (motors or H-bridge)
   - Monitor power supply current draw

3. **Terrain Test** (if time permits):
   - Test on intended surface (sand dune, rough terrain)
   - Adjust MAX_SPEED if needed for torque

## Troubleshooting Quick Reference

| Problem | Quick Fix |
|---------|-----------|
| Motors don't move | Check power supply, verify voltage at motor terminals |
| Erratic behavior | Add capacitors to motors, check all ground connections |
| Wrong direction | Swap motor wires or modify code |
| No response to inputs | Check Serial Monitor, verify switch connections |
| Motors overheat | Reduce MAX_SPEED, check for stall conditions |
| H-bridge hot | Ensure adequate heat sinking, reduce current draw |

## Time Saved = Success!

If you've reached this point in under 1.5 hours, congratulations! Your Sand Dune Searcher is operational.

## Next Steps (Optional)

- Add remote control (RC receiver instead of switches)
- Add sensors (ultrasonic, IR, etc.)
- Implement autonomous behavior
- Add speed potentiometer for variable speed
- Install battery pack for mobile operation
- Add emergency stop button

## Support

For detailed information:
- **Wiring**: See [WIRING_GUIDE.md](WIRING_GUIDE.md)
- **Code**: See [ARDUINO_README.md](ARDUINO_README.md)
- **Schematic**: See [SCHEMATIC.txt](SCHEMATIC.txt)

## Safety Reminders

⚠️ Always:
- Disconnect power when wiring
- Use appropriate wire gauge for current
- Ensure adequate ventilation for H-bridge
- Keep fingers away from moving wheels during testing
- Have emergency stop readily accessible
- Use proper fuses/circuit protection

---
**Estimated Total Time**: 90 minutes (1.5 hours)
**Difficulty**: Intermediate
**Required Skills**: Basic electronics, Arduino programming
