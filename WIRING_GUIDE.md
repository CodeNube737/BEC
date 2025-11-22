# Arduino Motor Controller - Wiring Guide

## Components Required
- Arduino Uno R3
- H-bridge Motor Driver (e.g., L298N, L293D, or similar)
- 2 sets of 2 wheels with DC motors (4 motors total, connected in parallel pairs)
- Power supply (adequate for motors - typically 6-12V DC)
- 4 digital input switches/sensors
- Jumper wires

## Pin Connections

### Arduino to H-bridge Motor Driver

#### Left Motor Set (2 wheels wired in parallel)
- Arduino Pin 8  → H-bridge IN1 (Left motor direction control 1)
- Arduino Pin 9  → H-bridge IN2 (Left motor direction control 2)
- Arduino Pin 10 → H-bridge ENA (Left motor PWM speed control)

#### Right Motor Set (2 wheels wired in parallel)
- Arduino Pin 11 → H-bridge IN3 (Right motor direction control 1)
- Arduino Pin 12 → H-bridge IN4 (Right motor direction control 2)
- Arduino Pin 5  → H-bridge ENB (Right motor PWM speed control)

### Digital Input Controls
Connect switches/sensors between Arduino pins and GND (active LOW):
- Arduino Pin 4 → Forward command switch
- Arduino Pin 3 → Backward command switch
- Arduino Pin 6 → Left turn command switch
- Arduino Pin 7 → Right turn command switch

### Power Connections

#### H-bridge Power
- Motor power supply (+) → H-bridge VCC/Motor power input (6-12V)
- Motor power supply (-) → H-bridge GND
- Arduino GND → H-bridge Logic GND (common ground)
- Arduino 5V → H-bridge Logic power (if separate logic power input exists)

#### Motor Connections
- Left motors (2 wheels) → Connect in parallel to H-bridge Output A (OUT1 and OUT2)
- Right motors (2 wheels) → Connect in parallel to H-bridge Output B (OUT3 and OUT4)

## Wiring Diagram (ASCII)

```
                                    MOTOR POWER SUPPLY
                                         (6-12V)
                                            |
                                            |
                    +---------------------- + ----------------------+
                    |                                               |
                    |         H-BRIDGE MOTOR DRIVER                 |
                    |         (e.g., L298N)                         |
                    |                                               |
    Arduino         |   IN1  IN2  ENA  |  IN3  IN4  ENB           |
    Pin 8  ---------|----+              |                           |
    Pin 9  ---------|-------+           |                           |
    Pin 10 ---------|----------+        |                           |
    Pin 11 ---------|                +--|                           |
    Pin 12 ---------|                   +--                         |
    Pin 5  ---------|                      +-                       |
                    |                                               |
                    |  OUT1  OUT2         OUT3  OUT4               |
                    +----+-----+------------+-----+-----------------+
                         |     |            |     |
                         |     |            |     |
                    +----+     +----+  +----+     +----+
                    |               |  |               |
                LEFT MOTORS      RIGHT MOTORS
                (2 wheels)       (2 wheels)
                in parallel      in parallel

    
    INPUT SWITCHES (connect between Arduino pin and GND):
    
    Pin 4 ----[Switch]---- GND  (Forward)
    Pin 6 ----[Switch]---- GND  (Left)
    Pin 7 ----[Switch]---- GND  (Right)
    Pin 3 ----[Switch]---- GND  (Backward)
```

## Testing Procedure

1. **Safety First**: Ensure motors are NOT connected initially
2. **Upload Code**: Upload the motor_controller.ino to Arduino Uno R3
3. **Open Serial Monitor**: Set to 9600 baud to see debug messages
4. **Test Inputs**: Press each switch and verify Serial Monitor shows correct direction
5. **Connect Motors**: Once verified, connect motors to H-bridge
6. **Test Movement**: Test each direction with motors under no load
7. **Full Load Test**: Test with wheels on the ground

## Common H-bridge Motor Drivers

### L298N Module
- Dual H-bridge
- Can drive 2 DC motors (perfect for left/right wheel sets)
- Operating voltage: 5-35V
- Max current: 2A per channel
- Has built-in 5V regulator (can power Arduino if motor voltage is 7-12V)

### L293D IC
- Dual H-bridge IC
- Max voltage: 36V
- Max current: 600mA per channel (may be insufficient for larger motors)
- Requires external components

## Troubleshooting

### Motors don't move
- Check power supply is adequate (voltage and current)
- Verify all ground connections are common
- Check motor connections are secure
- Verify code uploaded successfully

### Motors move in wrong direction
- Swap the two wires going to the motor to reverse direction
- Or modify the code direction logic

### Speed ramping too slow/fast
- Adjust `RAMP_RATE` (increase for faster ramping)
- Adjust `RAMP_DELAY` (decrease for faster updates)

### Erratic behavior
- Add capacitors across motor terminals (100nF ceramic) to reduce electrical noise
- Ensure adequate power supply capacity
- Check for loose connections

## Safety Notes

⚠️ **Important Safety Considerations:**
1. Never hot-plug motors while powered
2. Ensure motor power supply can handle total current draw
3. Use appropriate wire gauge for motor currents
4. Add fuses to protect against short circuits
5. Ensure proper ventilation for H-bridge (can get hot under load)
6. Test in a safe area where the vehicle can move freely
