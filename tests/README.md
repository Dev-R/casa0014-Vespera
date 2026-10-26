# Tests

Incremental test sketches for the Mood Dial project. Each builds on the previous.

## Button Tests
| Test | Purpose |
|------|---------|
| `button_v1_read` | Basic button read (1=pressed, 0=not) |
| `button_v2_count` | Counts presses with debounce |

## Dial Tests
| Test | Purpose |
|------|---------|
| `dial_v1_raw` | Raw potentiometer values (0-4095) |
| `dial_v2_mood` | Maps dial to 12 moods |
| `dial_v3_oled` | Displays mood on OLED screen |
| `dial_v4_hysteresis` | Prevents flickering at mood boundaries |

## Vespera Tests
| Test | Purpose |
|------|---------|
| `mqtt_washes` | Tests WiFi + MQTT with RGB colour cycles |
| `blade_finder` | Maps LED numbers (0-71) to blade positions |

## Wiring
- **Button**: D4 to GND
- **Potentiometer**: Outer legs to 3.3V/GND, middle to A1
- **OLED**: SDA to pin 11, SCL to pin 12
