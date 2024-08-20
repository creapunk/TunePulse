# Voltage Pattern Control Description

The ability to control various types of motors with different pin configurations is highly beneficial for various applications.

These functions are implemented through two sequential processing stages.

## selector_motor_type

The first stage, `selector_motor_type`, receives normalized voltage in alpha-beta coordinates as input and outputs the corresponding voltage for the ABCD channels, where:

- `A`, `B`, `C`, `D` represent the voltage values at the corresponding half-bridges.
- `AB` is used as the output for a `Single DC` motor, with channels `C` and `D` functioning as a `BRAKING CHOPPER`.
- `AB` and `CD` are two half-bridges for controlling a `Dual DC` or `STEPPER` motor.
- `ABC` are the three phases for a `BLDC` or `PMSM` motor in `UVW` configuration, with channel `D` serving as a `BRAKING CHOPPER`.

## selector_interconnect_pwm

The second stage, `selector_interconnect_pwm`, takes a 4-channel array input from the `selector_motor_type`. It maps the incoming channels `ABCD` to the output channels `1234` based on a predefined pattern. The combination is defined using a `uint8_t`, where each successive 2 bits correspond to a channel from `ABCD` assigned to a channel from `1234`. The least significant 2 bits correspond to `A`, and the most significant 2 bits to `D`. For example:

- Input value: `0b11100100`
- Split into a sequence of 2-bit values: `0b11` `0b10` `0b01` `0b00`
- This translates to: `3`, `2`, `1`, `0`
- The result will be the mapping: `A-1`, `B-2`, `C-3`, `D-4`

Presets are defined as follows:
``` C++
enum PatternPWM : uint8_t {
  ABCD = 0b11100100,  // Pattern 0: {0, 1, 2, 3}
  ACDB = 0b01111000,  // Pattern 1: {0, 2, 3, 1}
  ADBC = 0b10011100,  // Pattern 2: {0, 3, 1, 2}
  DCAB = 0b01001011   // Pattern 3: {3, 2, 0, 1}
};
```
A useful feature is the ability to set all channels to generate a single value if needed (although the specific use case for this is unclear).

## Data Types

To simplify the mathematics, speed up the code, and reduce memory usage, all blocks at this level work with normalized voltage rather than absolute voltage. Normalization is done according to the pattern `[0 ... MAX_VOLTAGE]` => `[0 ... INT16_MAX]`, where `MAX_VOLTAGE` is the maximum voltage the voltage sensor can handle.

Since the system is designed to work with DC voltages (or rectified AC), the current sensor provides only a positive range from `int16_t`, which offers `15-bit` resolution control over the voltage or `32768` values for the range.

This approach to normalization eliminates the need for range checks at each step and allows simple mathematical adjustments to the PWM duty cycle for the current supply voltage in each PWM cycle without unnecessary conversions to and from volts.

`INT16_MIN` is reserved for the off state of normalized voltage.

The negative voltage range for channels `A-D` and `1-4` is reserved for the logic of the `BRAKING` mode.