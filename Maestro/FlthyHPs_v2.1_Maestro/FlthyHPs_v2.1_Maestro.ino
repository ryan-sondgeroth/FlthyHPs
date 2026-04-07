/////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///                                                                                                           ///
///                                    Flthy Holoprojectors v2.1 - Maestro Edition                            ///
///                                       by Ryan Sondgeroth                                                  ///
///                                         aka FlthyMcNsty                                                   ///
///                                           01-23-2026                                                      ///
///                                                                                                           ///
///      Combines the movement and display functions of each Holoprojector into a single easy                 ///
///     to control sketch.  It utilizes 7 LED NeoPixel boards inside each HP to produce a more                ///
///        life-like representation of a hologram projection. Servo control is handled by a                   ///
///      Pololu 6-Channel Micro Maestro servo controller connected via SoftwareSerial,                        ///
///     giving a simple and reliable UART interface for all 6 servo channels (2 per HP).                      ///
///                                                                                                           ///
///                        Rhyno45 is a Wanker, in case Ya'll didn't know!                                    ///
///                                                                                                           ///
///                                      Special thanks to...                                                 ///
///   LostRebel and Knightshade for significant input on both the general functions of the system             ///
///          and this code!  It helped me make vast improvements in operation and effeciency.                 ///
///                                                                                                           ///
///                  Huge shout out to Nobser for the major improvements in v2.0!                             ///
///                                                                                                           ///
///                                                                                                           ///
///   Version History:                                                                                        ///
///     Maestro Edition - Replaced Adafruit PCA9685 / SlowServoPCA9685 servo driver with                     ///
///            Pololu 6-Channel Maestro via SoftwareSerial (TX only, pin 5).                                  ///
///            Removed software servo easing - replaced with Maestro hardware speed                           ///
///            and acceleration control (smoother, zero CPU cost).                                            ///
///            Added ServoMoveTimer per HP to estimate move completion since                                  ///
///            getMovingState() has a known bug on the 6-channel Maestro.                                     ///
///            Moved all servo position data out of sketch into Maestro script                                ///
///            (FlthyHPs_Maestro_Script.mscr). positionHP() now calls                                        ///
///            restartScriptAtSubroutine() - no sketch changes needed to calibrate                            ///
///            for a new installation, just update and reload the Maestro script.                             ///
///            Removed ENABLEBASICHP mode and RC control - positioning is now                                 ///
///            fully script-driven with Maestro hardware min/max limits as safety.                            ///
///     v2.1 - Added smooth servo acceleration/deceleration (easing):                                        ///
///            - Implemented ease-in-out, ease-in, and ease-out movement curves                              ///
///            - Configurable easing duration and enable/disable toggle                                      ///
///            - Integrated seamlessly into existing movement functions                                      ///
///            - Makes all servo movements look more natural and less robotic                                ///
///     v2.0 - Refactored for major improvements in efficiency and readability:                               ///
///            - Big Ups to Nobser for this amazing refactor!                                                 ///
///            - Added duration parsing for timed commands (pipe syntax)                                      ///
///            - Added readable error messages for better debugging                                           ///
///            - Moved all magic numbers to constants                                                         ///
///            - Replaced String class with char arrays to prevent heap fragmentation                         ///
///            - Added robust command parser with error handling                                              ///
///            - Implemented timer classes for better timing management                                       ///
///            - Consolidated all constants in namespaces                                                     ///
///            - Removed duplicate code in main loop                                                          ///
///            - Added bounds checking for all array accesses                                                 ///
///            - Improved memory usage with better data structures                                            ///
///            - Added a 'help' command to display command usage in the Serial Monitor.                       ///
///            - Optimized SRAM usage by moving large color arrays to PROGMEM.                                ///
///            - Replaced floating-point math with faster integer math for a performance boost.               ///
///            - Reduced code duplication by consolidating repeated logic into new helper functions.          ///
///            - Improved readability by replacing numeric command codes with named constants.                ///
///     v1.8 - Added the ability to set custom "Off Colors" per IOIIOOO's (Jason Cross) request               ///
///     v1.7 - Bug fix identified by skelmir. Thanks, for the input, my friend.                               ///
///     v1.6 - Added random LED sequences, DEBUG feedback, compiler directives, PROGMEM usage.                ///
///     v1.5 - Updated mapPulselength function to be more accurate.                                           ///
///     v1.4 - Updated volatile variable handling.                                                            ///
///     v1.3 - Added sequence runtime via command string, added X,Y,Z designators.                            ///
///     v1.2 - Major overhaul with bug fixes, streamlined functions, more presets, new features.              ///
///     v1.1 - Added One Time HP Twitch Function                                                              ///
///                                                                                                           ///
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///                                                                                                           ///
///     Commands and Structure                                                                                ///
///                                                                                                           ///
///     DT##C or DT##CS or DT##R or DT##P                                                                     ///
///                                                                                                           ///
///     D - the HP designator                                                                                 ///
///          F - Front HP                                                                                     ///
///          R - Rear HP                                                                                      ///
///          T - Top HP                                                                                       ///
///          A - All 3 HPs                                                                                    ///
///          X - Front & Rear HPs                                                                             ///
///          Y - Front & Top HPs                                                                              ///
///          Z - Rear & Top HPs                                                                               ///
///          S - Sequences (See Below)                                                                        ///
///                                                                                                           ///
///     T - the Sequence Type is either 0-Led Fuctions and 1-Servo Functions                                  ///
///                                                                                                           ///
///    ## - the Sequence Value including leading zero if necessary, ie sequence 3 is 03                       ///
///                                                                                                           ///
///     C - (Optional), the Color integer value from list below:                                              ///
///        Basic Color Integer Values                                                                         ///
///           1 = Red                                                                                         ///
///           2 = Yellow                                                                                      ///
///           3 = Green                                                                                       ///
///           4 = Cyan (Aqua)                                                                                 ///
///           5 = Blue                                                                                        ///
///           6 = Magenta                                                                                     ///
///           7 = Orange                                                                                      ///
///           8 = Purple                                                                                      ///
///           9 = White                                                                                       ///
///           0 = Random                                                                                      ///
///                                                                                                           ///
///     S - (Optional), Speed setting integer for the Dim Pulse LED function below (0-9)                      ///
///                                                                                                           ///
///     R - (Optional), Random State for clearing LED displays                                                ///
///        Random State Integer Values                                                                        ///
///           1 = Use Default Sequences                                                                       ///
///           2 = Use Random Sequences                                                                        ///
///                                                                                                           ///
///     P - (Optional), the Position integer value from list below:                                           ///
///        Preset Position Integer Values                                                                     ///
///           0 = Down                                                                                        ///
///           1 = Center                                                                                      ///
///           2 = Up                                                                                          ///
///           3 = Left                                                                                        ///
///           4 = Upper Left                                                                                  ///
///           5 = Lower Left                                                                                  ///
///           6 = Right                                                                                       ///
///           7 = Upper Right                                                                                 ///
///           8 = Lower Right                                                                                 ///
///                                                                                                           ///
///     D001    - Leia Sequence, Random shades of blue to mimic Leia Hologram                                 ///
///     D002C   - Color Projector Sequence, Like Leia above but using color command value                     ///
///     D003CS  - Dim Pulse Sequence, Color slowly pulses on and off                                          ///
///     D004C   - Cycle Sequence, using color command value                                                   ///
///     D005C   - Toggles Color, Simply sets LEDs tp solid color value.                                       ///
///     D006    - Rainbow Sequence                                                                            ///
///     D007C   - Short Circuit, Led flashes on and off with interval slowing over time                       ///
///     D096    - Clears LED, Disables Auto LED Sequence & "Off Color"                                        ///
///     D0971   - Clears LED, Enables Auto LED Sequence,Enables Default Sequences, Disables "Off Color"       ///
///     D0972   - Clears LED, Enables Auto LED Sequence,Enables Random Sequences, Disables "Off Color" .      ///
///     D098    - Clears LED, Disables Auto LED Sequence, enables "Off Color"                                 ///
///     D0991   - Clears LED, Enables Auto LED Sequence,Enables Default Sequences, Enables "Off Color"        ///
///     D0992   - Clears LED, Enables Auto LED Sequence,Enables Random Sequences, Enables "Off Color"         ///
///                                                                                                           ///
///     D101P   - Sends HP to a Preset Position (P = position index 0-8)                                      ///
///     D104    - Sends HP to a Random Position                                                               ///
///     D105    - Wags HP Left/Right 5 times                                                                  ///
///     D106    - Wags HP Up/Down 5 times                                                                     ///
///     D198    - Disables Auto HP Twitch                                                                     ///
///     D199    - Enables Auto HP Twitch                                                                      ///
///                                                                                                           ///
///       S1    - Leia Mode (Front HP in Down Position, Leia LED Sequence, all other HPs disabled)            ///
///       S4    - Clear all LEDs, Disable Auto HP Twitch, Disable Auto LED Sequence, Disables Off Color       ///
///       S5    - Clear all LEDs, Enable Auto HP Twitch, Enable Auto LED Sequence (w/ default seqs.),         ///
///               Disables Off Color                                                                          ///
///       S9    - Clear all LEDs, Enable Auto HP Twitch, Enable Auto LED Sequence (w/ random seqs.),          ///
///               Disables Off Color                                                                          ///
///       S7    - Clear all LEDs, Disable Auto HP Twitch, Disable Auto LED Sequence, Enables Off Color        ///
///       S8    - Clear all LEDs, Enable Auto HP Twitch, Enable Auto LED Sequence (w/ default seqs.),         ///
///               Enables Off Color                                                                           ///
///       S9    - Clear all LEDs, Enable Auto HP Twitch, Enable Auto LED Sequence (w/ random seqs.),          ///
///               Enables Off Color                                                                           ///
///                                                                                                           ///
///    Runtime values can be added to any command string by appending a pipe (|) followed by a               ///
///     numeric value indicating the desired time in seconds you wish the sequence to run.                    ///
///                                                                                                           ///
///       ie.  A007|25 would run the Rainbow Sequence on all 3 HPs for 25 seconds then clear each             ///
///            one, returning to the system's last known auto twitch mode.                                    ///
///                                                                                                           ///
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////////////////////////////
///                                                                                                  ///
///                                  Required Libraries                                              ///
///                                                                                                  ///
////////////////////////////////////////////////////////////////////////////////////////////////////////
#include <Adafruit_NeoPixel.h>
#include <Wire.h>
#include <SoftwareSerial.h>
#include <PololuMaestro.h>
#include <avr/pgmspace.h>

// Macro to suppress unused parameter warnings
#define UNUSED(x) ((void)(x))

////////////////////////////////////////////////////////////////////////////////////////////////////////
///                                  Maestro Serial Interface                                        ///
///                                                                                                  ///
///  The Pololu Maestro uses a simple one-way serial protocol (Pololu compact protocol).             ///
///  We only need to transmit (TX), so pin 6 is a dummy RX (leave unconnected).                     ///
///  Wire Arduino pin 5 (TX) to the Maestro RX pin only.                                            ///
///                                                                                                  ///
///  Maestro compact protocol commands used:                                                         ///
///    Set Target:      0x84, channel, target_low, target_high                                       ///
///      target is in units of quarter-microseconds (e.g. 1500us = 6000)                            ///
///    Set Speed:       0x87, channel, speed_low, speed_high                                         ///
///      speed in units of (quarter-us / 10ms). 0 = unlimited.                                      ///
///    Set Accel:       0x89, channel, accel_low, accel_high                                         ///
///      accel in units of (quarter-us / 10ms) per 80ms. 0 = unlimited.                             ///
///                                                                                                  ///
////////////////////////////////////////////////////////////////////////////////////////////////////////

constexpr uint8_t MAESTRO_RX_PIN = 6;  // Dummy - leave unconnected
constexpr uint8_t MAESTRO_TX_PIN = 5;  // Wire this to Maestro RX pin
constexpr uint16_t MAESTRO_BAUD = 19200;

SoftwareSerial maestroSerial(MAESTRO_RX_PIN, MAESTRO_TX_PIN);
MiniMaestro maestro(maestroSerial);

////////////////////////////////////////////////////////////////////////////////////////////////////////
///                                  Configuration Namespaces                                        ///
////////////////////////////////////////////////////////////////////////////////////////////////////////
namespace Config {
   // Version
   constexpr uint8_t VERSION = 1;

   // Hardware
   constexpr uint8_t HP_COUNT = 3;
   constexpr uint8_t LEDS_PER_HP = 7;
   constexpr uint8_t SERVOS_PER_HP = 2;
   constexpr uint8_t HP_POSITIONS = 9;
   constexpr uint8_t HP_CENTER_POSITION = 0;

   // I2C Address (for incoming command reception only - no servo I2C needed)
   constexpr uint8_t I2C_ADDRESS = 0x19;

   // Timing Constants (ms)
   constexpr unsigned long LED_STARTUP_BASE = 3800;
   constexpr unsigned long HP_STARTUP_DELAY = 4000;
   constexpr unsigned long STATUS_LED_DURATION = 200;
   constexpr unsigned long CYCLE_INTERVAL = 75;
   constexpr unsigned long RAINBOW_INTERVAL = 10;
   constexpr unsigned long WAG_INTERVAL = 400;
   constexpr unsigned long COLOR_PROJECTOR_MIN = 50;
   constexpr unsigned long COLOR_PROJECTOR_MAX = 150;

   // Animation Limits
   constexpr uint8_t WAG_CYCLES = 10;
   constexpr uint8_t SHORT_CIRCUIT_MAX_LOOPS = 20;

   // Command Buffer
   constexpr uint8_t MAX_COMMAND_LENGTH = 12;
   constexpr uint8_t INPUT_BUFFER_LEN = 20;

   // Maestro Hardware Speed & Acceleration
   // Speed units: quarter-us per 10ms. Lower = slower.
   //   Example: 30 quarter-us/10ms means ~200ms to travel 600us of range.
   //   0 = unlimited (instant jump - not recommended).
   // Acceleration units: (quarter-us/10ms) per 80ms. Lower = gentler ramp.
   //   0 = unlimited (instant speed change).
   //   Range 1-255. ~5 gives a smooth but not sluggish ramp.
   constexpr uint16_t MAESTRO_MOVE_SPEED = 30;    // Default speed for all channels
   constexpr uint16_t MAESTRO_ACCELERATION = 5;   // Smooth acceleration ramp

   // Move completion timing
   // Since we can't use getMovingState() reliably on the 6-channel Maestro,
   // we estimate move duration from the speed setting and pulse range.
   // This is used to know when an HP has "arrived" before triggering follow-on actions.
   // Formula: (max_range_quarter_us / speed) * 10ms + margin
   // At speed=30, 600us range = 2400 quarter-us / 30 * 10ms = 800ms + 200ms margin
   constexpr unsigned long MOVE_DURATION_MS = 1000;  // Estimated max move time in ms
}

////////////////////////////////////////////////////////////////////////////////////////////////////////
///                                  User Adjustable Settings                                        ///
////////////////////////////////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////////////
///*****                       Enable Debug Mode                          *****///
///*****                                                                  *****///
///*****    Uncomment the following line to enable debugging mode where   *****///
///*****  additional sketch information is printed to the Serial Monitor. *****///
///*****  This is strictly for debugging and shouldn't be enabled during  *****///
///*****                    normal droid operation.                       *****///
///*****                                                                  *****///
//////////////////////////////////////////////////////////////////////////////////

// Debug Mode
//#define DEBUG

//////////////////////////////////////////////////////////////////////////////////
///*****                    Adafruit Jewel Board Version                  *****///
///*****                                                                  *****///
///*****       The Adafruit Jewels come in RGB and RGBW versions.         *****///
///*****    Uncomment NEO_JEWEL_RGBW if you have the RGBW version.        *****///
///*****                                                                  *****///
//////////////////////////////////////////////////////////////////////////////////

// NeoPixel Type
//#define NEO_JEWEL_RGBW

//////////////////////////////////////////////////////////////////////////////////
///*****               Arduino Pin Assignments for LEDs / Status          *****///
///*****                                                                  *****///
///*****  LED_PINS maps {Front, Rear, Top} to Arduino digital pins.       *****///
///*****  HP_PINS maps each HP to its 2 Maestro servo channels (0-5).     *****///
///*****  STATUS_LED_PIN flashes on Serial/I2C receive.                   *****///
///*****                                                                  *****///
///*****  NOTE: No OUTPUT_ENABLED_PIN needed - the Maestro handles        *****///
///*****        servo power directly.                                     *****///
///*****                                                                  *****///
///*****  MAESTRO_RESET_PIN wires to Maestro RST. Pulled low briefly on  *****///
///*****  startup to reset the Maestro each time the Arduino boots.       *****///
///*****                                                                  *****///
//////////////////////////////////////////////////////////////////////////////////

// Hardware Pins
const uint8_t LED_PINS[Config::HP_COUNT] PROGMEM = {2, 3, 4};  // {Front, Rear, Top}

// Maestro channel assignments: each HP uses 2 channels
// HP_PINS[hp][0] = pan/horizontal servo channel
// HP_PINS[hp][1] = tilt/vertical servo channel
const uint8_t HP_PINS[Config::HP_COUNT][Config::SERVOS_PER_HP] PROGMEM = {{0,1}, {2,3}, {4,5}};

constexpr uint8_t STATUS_LED_PIN = 13;
constexpr uint8_t MAESTRO_RESET_PIN = 7;  // Wire to Maestro RST pin

//////////////////////////////////////////////////////////////////////////////////
///*****                          LED Brightness                          *****///
///*****                                                                  *****///
///*****                 Adjust LED Brightness Level (0-255)              *****///
//////////////////////////////////////////////////////////////////////////////////

// LED Settings
constexpr uint8_t BRIGHTNESS = 150;

//////////////////////////////////////////////////////////////////////////////////
///*****               Dim Pulse Speed Range and Default Speed            *****///
///*****                                                                  *****///
///*****  DIM_PULSE_SPEED is the default speed if no value is supplied.   *****///
///*****  dimPulseSpeedRange maps option values 0-9 into a ms delay range. *****///
///*****  Lower delay values = faster pulse.                              *****///
///*****                                                                  *****///
//////////////////////////////////////////////////////////////////////////////////

constexpr uint8_t DIM_PULSE_SPEED = 5;
const uint8_t DIM_PULSE_SPEED_RANGE[2] PROGMEM = {5, 75};

//////////////////////////////////////////////////////////////////////////////////
///*****         Enable/Disable LED Auto Twitch and Random Sequences      *****///
///*****                                                                  *****///
///*****                           Auto Disable = 0                       *****///
///*****                 Auto Enabled w/ Default Sequence = 1             *****///
///*****                 Auto Enabled w/ Random Sequences = 2             *****///
///*****                                                                  *****///
//////////////////////////////////////////////////////////////////////////////////

// Auto Twitch Settings
uint8_t enableTwitchLED[Config::HP_COUNT] = {1, 1, 1};  // 0=Off, 1=Default, 2=Random
bool enableTwitchHP[Config::HP_COUNT] = {true, true, true};
const uint8_t startEnableTwitchLED[Config::HP_COUNT] = {1, 1, 1};

//////////////////////////////////////////////////////////////////////////////////
///*****                    Default LED Auto Twitch Commands              *****///
///*****                                                                  *****///
///*****  DEFAULT_LED_TWITCH_CMD per HP: {Sequence, Color, Speed}         *****///
///*****  Used when enableTwitchLED[hp] == 1 (default sequences).         *****///
///*****                                                                  *****///
//////////////////////////////////////////////////////////////////////////////////

// Default Commands
const uint8_t DEFAULT_LED_TWITCH_CMD[Config::HP_COUNT][3] PROGMEM = {{1,5,0}, {1,5,0}, {1,5,0}};

///////////////////////////////////////////////////////////////////////////////////
///*****               LED & Servo Auto Twitch Interval Ranges             *****///
///*****                                                                   *****///
///*****  These min/max ranges (seconds) randomize how often twitches run. *****///
///*****  LED_TWITCH_RUN_INTERVAL randomizes how long the LED effect runs. *****///
///*****  SERVO_SPEED range controls move speeds (ms). Lower=faster.       *****///
///*****                                                                   *****///
///////////////////////////////////////////////////////////////////////////////////

// Timing Intervals
const uint16_t LED_TWITCH_INTERVAL[Config::HP_COUNT][2] PROGMEM = {{30,90}, {30,90}, {30,90}};
const uint16_t HP_TWITCH_INTERVAL[Config::HP_COUNT][2] PROGMEM = {{45,120}, {60,180}, {60,180}};
const uint16_t LED_TWITCH_RUN_INTERVAL[Config::HP_COUNT][2] PROGMEM = {{5,25}, {5,25}, {5,25}};
const uint16_t SERVO_SPEED[2] PROGMEM = {150, 400};

///////////////////////////////////////////////////////////////////////////////////
///*****                      Default Color Settings                       *****///
///*****                                                                   *****///
///*****  DEFAULT_COLOR: hue used by default LED sequences per HP.         *****///
///*****  SHORT_COLOR: hue used by short-circuit sequence per HP.          *****///
///*****                                                                   *****///
///////////////////////////////////////////////////////////////////////////////////

// Color Settings
const uint8_t DEFAULT_COLOR[Config::HP_COUNT] PROGMEM = {5, 5, 5};
const uint8_t SHORT_COLOR[Config::HP_COUNT] PROGMEM = {7, 7, 7};

///////////////////////////////////////////////////////////////////////////////////
///*****                    Optional Default "Off" Color                   *****///
///*****                                                                   *****///
///*****  OFF_COLOR sets the LED idle/off state per HP.                    *****///
///*****  Use -1 for true OFF (black). Any 0-9 uses that palette color.    *****///
///*****                                                                   *****///
///////////////////////////////////////////////////////////////////////////////////

const int8_t OFF_COLOR[Config::HP_COUNT] PROGMEM = {-1, -1, -1};

// PROGMEM access helpers
static inline uint8_t readLedPin(uint8_t hp) {
   return pgm_read_byte(&LED_PINS[hp]);
}
static inline uint8_t readHpPin(uint8_t hp, uint8_t idx) {
   return pgm_read_byte(&HP_PINS[hp][idx]);
}
static inline uint8_t readDimPulseSpeedRange(uint8_t idx) {
   return pgm_read_byte(&DIM_PULSE_SPEED_RANGE[idx]);
}
static inline uint8_t readDefaultLedTwitchCmd(uint8_t hp, uint8_t idx) {
   return pgm_read_byte(&DEFAULT_LED_TWITCH_CMD[hp][idx]);
}
static inline uint16_t readLedTwitchInterval(uint8_t hp, uint8_t idx) {
   return pgm_read_word(&LED_TWITCH_INTERVAL[hp][idx]);
}
static inline uint16_t readHpTwitchInterval(uint8_t hp, uint8_t idx) {
   return pgm_read_word(&HP_TWITCH_INTERVAL[hp][idx]);
}
static inline uint16_t readLedTwitchRunInterval(uint8_t hp, uint8_t idx) {
   return pgm_read_word(&LED_TWITCH_RUN_INTERVAL[hp][idx]);
}
static inline uint16_t readServoSpeed(uint8_t idx) {
   return pgm_read_word(&SERVO_SPEED[idx]);
}
static inline uint8_t readDefaultColor(uint8_t hp) {
   return pgm_read_byte(&DEFAULT_COLOR[hp]);
}
static inline uint8_t readShortColor(uint8_t hp) {
   return pgm_read_byte(&SHORT_COLOR[hp]);
}
static inline int8_t readOffColor(uint8_t hp) {
   return (int8_t)pgm_read_byte(&OFF_COLOR[hp]);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////
///                                  Enumerations                                                    ///
////////////////////////////////////////////////////////////////////////////////////////////////////////

enum class LedFunction : uint8_t {
   DO_NOTHING = 0,
   LEIA = 1,
   COLOR_PROJECTOR = 2,
   DIM_PULSE = 3,
   CYCLE = 4,
   SOLID_COLOR = 5,
   RAINBOW = 6,
   SHORT_CIRCUIT = 7,
   CLEAR_NO_AUTO_OVERRIDE_ON = 96,
   CLEAR_AUTO_OVERRIDE_ON = 97,
   CLEAR_NO_AUTO_OVERRIDE_OFF = 98,
   CLEAR_AUTO_OVERRIDE_OFF = 99,
   SET_OFF_COLOR = 100
};

enum class HpFunction : uint8_t {
   DO_NOTHING = 0,
   GOTO_PRESET = 1,
   RANDOM_POS = 4,
   WAG_LEFT_RIGHT = 5,
   WAG_UP_DOWN = 6,
   AUTO_TWITCH_OFF = 98,
   AUTO_TWITCH_ON = 99
};

enum class Sequence : uint8_t {
   LEIA_MODE = 1,
   ALL_OFF = 4,
   ALL_ON_DEFAULT = 5,
   ALL_ON_RANDOM = 6,
   ALL_OFF_WITH_OFFCOLOR = 7,
   ALL_ON_DEFAULT_WITH_OFFCOLOR = 8,
   ALL_ON_RANDOM_WITH_OFFCOLOR = 9
};

enum class CommandError : uint8_t {
   OK = 0,
   INVALID_FORMAT,
   INVALID_DEVICE,
   INVALID_TYPE,
   INVALID_FUNCTION,
   INVALID_COLOR,
   OUT_OF_BOUNDS,
   BUFFER_OVERFLOW
};

////////////////////////////////////////////////////////////////////////////////////////////////////////
///                                  Data Structures                                                 ///
////////////////////////////////////////////////////////////////////////////////////////////////////////

struct Command {
   char device = 0;
   uint8_t type = 0;
   uint8_t function = 0;
   int8_t option1 = -1;
   int8_t option2 = -1;
   int16_t duration = -1;

   bool isValid() const {
       static const char VALID_DEVICES[] = "FRTAXYZS";
       return strchr(VALID_DEVICES, device) != nullptr;
   }
};

struct LedCommand {
   LedFunction function = LedFunction::DO_NOTHING;
   uint8_t option1 = 0;
   uint8_t option2 = 0;
   int16_t halt = -1;
};

struct HpCommand {
   HpFunction function = HpFunction::DO_NOTHING;
   uint8_t option1 = 0;
   int16_t halt = -1;
};

////////////////////////////////////////////////////////////////////////////////////////////////////////
///                                  Servo Move Timer                                                ///
///                                                                                                  ///
///  Tracks whether the Maestro has finished moving an HP's servos.                                 ///
///  Since getMovingState() has a known bug on the 6-channel Maestro, we estimate                   ///
///  completion by timing from when the move command was sent.                                       ///
////////////////////////////////////////////////////////////////////////////////////////////////////////

struct ServoMoveTimer {
   bool moving = false;
   unsigned long startTime = 0;

   void start() {
       moving = true;
       startTime = millis();
   }

   bool isComplete() {
       if (!moving) return true;
       if (millis() - startTime >= Config::MOVE_DURATION_MS) {
           moving = false;
           return true;
       }
       return false;
   }
};

////////////////////////////////////////////////////////////////////////////////////////////////////////
///                                  Timer Class                                                     ///
////////////////////////////////////////////////////////////////////////////////////////////////////////

class SimpleTimer {
private:
   unsigned long previousMillis = 0;
   unsigned long interval;

public:
   SimpleTimer(unsigned long _interval = 100) : interval(_interval) {}

   bool check() {
       unsigned long currentMillis = millis();
       if (currentMillis - previousMillis >= interval) {
           previousMillis = currentMillis;
           return true;
       }
       return false;
   }

   void reset() { previousMillis = millis(); }
   void setInterval(unsigned long _interval) { interval = _interval; }
   unsigned long getElapsed() const { return millis() - previousMillis; }
};

////////////////////////////////////////////////////////////////////////////////////////////////////////
///                                  Command Buffer Class                                            ///
////////////////////////////////////////////////////////////////////////////////////////////////////////

class CommandBuffer {
private:
   char buffer[Config::INPUT_BUFFER_LEN];
   uint8_t index = 0;

public:
   void addChar(char c) {
       if (index < Config::INPUT_BUFFER_LEN - 1) {
           buffer[index++] = c;
       }
   }

   void clear() {
       index = 0;
       buffer[0] = '\0';
   }

   bool isComplete(char c) const {
       return (c == '\r' || c == '\n') && index > 0;
   }

   const char* get() {
       buffer[index] = '\0';
       return buffer;
   }

   uint8_t length() const { return index; }
};

////////////////////////////////////////////////////////////////////////////////////////////////////////
///                                  Global Variables                                                ///
////////////////////////////////////////////////////////////////////////////////////////////////////////

constexpr uint32_t C_OFF = 0x000000;

// Color Table (stored in PROGMEM)
const PROGMEM uint32_t BASIC_COLORS[10][10] = {
   {0xFF0000, 0xFFFF00, 0x00FF00, 0x00FFFF, 0x0000FF, 0xFF00FF, 0x800080, 0xFFFFFF, 0x000000, 0x000000},
   {0xFF0000, 0xFF0000, 0xFF0000, 0xFFFFFF, 0xFFA0A0, 0xFD5555, 0xFFD3D3, 0x000000, 0x000000, 0x000000},
   {0xFFFF00, 0xFFFF00, 0xFFFF00, 0xFFFFFF, 0xFDFD43, 0xFFFF82, 0xFFFFBA, 0x000000, 0x000000, 0x000000},
   {0x00FF00, 0x00FF00, 0x00FF00, 0xFFFFFF, 0x57FC57, 0x80FC80, 0xBDFFB1, 0x000000, 0x000000, 0x000000},
   {0x00FFFF, 0x00FFFF, 0x00FFFF, 0xFFFFFF, 0x38FFFF, 0x71FDFD, 0xA4FDFD, 0x000000, 0x000000, 0x000000},
   {0x0000FF, 0x0000FF, 0x0000FF, 0xFFFFFF, 0xACACFF, 0x7676FF, 0x5A5AFF, 0x000000, 0x000000, 0x000000},
   {0xFF00FF, 0xFF00FF, 0xFF00FF, 0xFFFFFF, 0xFB3BFB, 0xFD75FD, 0xFD9EFD, 0x000000, 0x000000, 0x000000},
   {0xFF8000, 0xFF8000, 0xFF8000, 0xFFFFFF, 0xFB9B3A, 0xFFBE7D, 0xFCD2A7, 0x000000, 0x000000, 0x000000},
   {0x800080, 0x800080, 0x800080, 0xFFFFFF, 0xA131A1, 0x9B449B, 0xBD5FBD, 0x000000, 0x000000, 0x000000},
   {0xFFFFFF, 0xFFFFFF, 0xFFFFFF, 0xFFFFFF, 0xB7B6B6, 0x858484, 0xA09F9F, 0x000000, 0x000000, 0x000000}
};

// System State
CommandBuffer commandBuffer;
volatile bool stringComplete = false;
LedCommand ledCommands[Config::HP_COUNT];
HpCommand hpCommands[Config::HP_COUNT];
bool offColorOverride[Config::HP_COUNT] = {false, false, false};

// Timers
SimpleTimer ledTimers[Config::HP_COUNT];
SimpleTimer hpTwitchTimers[Config::HP_COUNT];
SimpleTimer ledTwitchTimers[Config::HP_COUNT];
SimpleTimer statusLedTimer(Config::STATUS_LED_DURATION);
SimpleTimer wagTimers[Config::HP_COUNT];

// Servo Move Timers (one per HP - tracks estimated Maestro move completion)
ServoMoveTimer servoMoveTimers[Config::HP_COUNT];

// Animation State
uint8_t animationFrame[Config::HP_COUNT] = {0, 0, 0};
uint8_t shortCircuitLoop[Config::HP_COUNT] = {0, 0, 0};
bool shortCircuitFlag[Config::HP_COUNT] = {false, false, false};
int8_t wagCount[Config::HP_COUNT] = {-1, -1, -1};
unsigned long ledHaltTime[Config::HP_COUNT];
unsigned long hpHaltTime[Config::HP_COUNT];
unsigned long twitchLEDRunTime[Config::HP_COUNT];

// Hardware Objects
Adafruit_NeoPixel neoStrips[Config::HP_COUNT];

#ifdef NEO_JEWEL_RGBW
   #define HP_NEO_TYPE (NEO_GRBW + NEO_KHZ800)
#else
   #define HP_NEO_TYPE (NEO_GRB + NEO_KHZ800)
#endif

////////////////////////////////////////////////////////////////////////////////////////////////////////
///                                  Maestro Serial Functions                                        ///
///                                                                                                  ///
///  All Maestro communication is handled by the PololuMaestro library (MiniMaestro class).          ///
///  Key methods used:                                                                               ///
///    maestro.setTarget(channel, quarterUs)   - move servo (target in quarter-microseconds)         ///
///    maestro.setSpeed(channel, speed)        - speed limit in quarter-us per 10ms (0=unlimited)    ///
///    maestro.setAcceleration(channel, accel) - accel limit 0-255 (0=unlimited)                    ///
///                                                                                                  ///
////////////////////////////////////////////////////////////////////////////////////////////////////////
///                                  Function Declarations                                           ///
////////////////////////////////////////////////////////////////////////////////////////////////////////

// Command Processing
Command parseCommand(const char* cmdStr);
CommandError validateCommand(const Command& cmd);
void processCommand(const Command& cmd);
void executeCommand(uint8_t hp, const Command& cmd);
const __FlashStringHelper* errorToString(CommandError err);

// LED Functions
void ledOff(uint8_t hp);
void ledColor(uint8_t hp, uint8_t color);
void colorProjector(uint8_t hp, uint8_t color);
void dimPulse(uint8_t hp, uint8_t color, uint8_t speed);
void cycle(uint8_t hp, uint8_t color);
void rainbow(uint8_t hp);
void shortCircuit(uint8_t hp, uint8_t color);
uint32_t getColor(uint8_t colorIndex, uint8_t variant);
uint32_t wheel(uint8_t pos);
uint32_t dimColorValue(uint8_t color, uint8_t brightness);

// HP Servo Functions
void positionHP(uint8_t hp, uint8_t pos, uint16_t speed);
void twitchHP(uint8_t hp);
void wagHP(uint8_t hp, uint8_t type);

// System Functions
void resetAnimation(uint8_t hp);
void setAutoMode(bool hpTwitch, uint8_t ledTwitchMode, bool offColorOverride);
void flushCommandArray(uint8_t hp, uint8_t type);
void statusLedOn();
void printHelp();
void resetLEDTwitch(uint8_t hp);
void resetHPTwitch(uint8_t hp);
void initializeStartupState();
bool isLedIdle(uint8_t hp);
bool isHpIdle(uint8_t hp);


#ifdef DEBUG
void printDebug(const char* msg);
void printHpName(uint8_t hp);
int freeRam();
#endif

////////////////////////////////////////////////////////////////////////////////////////////////////////
///                                  Setup                                                           ///
////////////////////////////////////////////////////////////////////////////////////////////////////////

void setup() {
   Serial.begin(9600); // NOLINT  // Hardware serial for incoming commands
   Wire.begin(Config::I2C_ADDRESS);
   Wire.onReceive(i2cEvent);
   randomSeed(analogRead(3));

   // Status LED
   pinMode(STATUS_LED_PIN, OUTPUT);

   // Reset Maestro on startup so it always boots into a clean state.
   // Wire MAESTRO_RESET_PIN to the Maestro RST pin.
   pinMode(MAESTRO_RESET_PIN, OUTPUT);
   digitalWrite(MAESTRO_RESET_PIN, LOW);
   delay(10);
   digitalWrite(MAESTRO_RESET_PIN, HIGH);
   delay(100);  // Give Maestro time to boot before sending serial commands

   // Start Maestro serial interface
   maestroSerial.begin(MAESTRO_BAUD);

   // Apply speed/accel limits to all Maestro channels via PololuMaestro library
   for (uint8_t ch = 0; ch < Config::HP_COUNT * Config::SERVOS_PER_HP; ch++) {
       maestro.setSpeed(ch, Config::MAESTRO_MOVE_SPEED);
       maestro.setAcceleration(ch, Config::MAESTRO_ACCELERATION);
   }

   // Initialize LEDs and Timers
   for (uint8_t i = 0; i < Config::HP_COUNT; i++) {
       neoStrips[i].updateType(HP_NEO_TYPE);
       neoStrips[i].updateLength(Config::LEDS_PER_HP);
       neoStrips[i].setPin(readLedPin(i));
       neoStrips[i].begin();
       neoStrips[i].setBrightness(BRIGHTNESS);
       neoStrips[i].show();

       twitchLEDRunTime[i] = random(readLedTwitchRunInterval(i, 0), readLedTwitchRunInterval(i, 1));

       ledTwitchTimers[i].setInterval(random(readLedTwitchInterval(i, 0), readLedTwitchInterval(i, 1)) * 1000UL);
       hpTwitchTimers[i].setInterval(random(readHpTwitchInterval(i, 0), readHpTwitchInterval(i, 1)) * 1000UL);

       wagTimers[i].setInterval(Config::WAG_INTERVAL);
   }

   initializeStartupState();

   Serial.print(F("\nFlthyHPs v2."));
   Serial.println(Config::VERSION);
   Serial.println(F("Maestro Edition"));
   Serial.println(F("Type 'help' for commands"));

   #ifdef DEBUG
   Serial.print(F("Free RAM: "));
   Serial.print(freeRam());
   Serial.println(F(" bytes"));
   #endif
}

////////////////////////////////////////////////////////////////////////////////////////////////////////
///                                  Main Loop                                                       ///
////////////////////////////////////////////////////////////////////////////////////////////////////////

void loop() {
   // Check status LED
   if (statusLedTimer.check()) {
       digitalWrite(STATUS_LED_PIN, LOW);
   }

   // Process complete command
   if (stringComplete) {
       Command cmd = parseCommand(commandBuffer.get());

       if (cmd.device == 'H' && strcasecmp(commandBuffer.get(), "help") == 0) {
           printHelp();
       } else {
           CommandError error = validateCommand(cmd);
           if (error == CommandError::OK) {
               processCommand(cmd);
               #ifdef DEBUG
               Serial.print(F("Command processed: "));
               Serial.println(commandBuffer.get());
               #endif
           } else {
               Serial.print(F("Error: "));
               Serial.println(errorToString(error));
           }
       }

       commandBuffer.clear();
       stringComplete = false;
   }

   // Process LED commands
   for (uint8_t i = 0; i < Config::HP_COUNT; i++) {
       if (ledCommands[i].halt > 0 && (millis() - ledHaltTime[i] >= ledCommands[i].halt * 1000UL)) {
           flushCommandArray(i, 0);
           ledOff(i);
           resetLEDTwitch(i);
       }

       switch (ledCommands[i].function) {
           case LedFunction::LEIA:
               colorProjector(i, 5);
               break;
           case LedFunction::COLOR_PROJECTOR:
               colorProjector(i, ledCommands[i].option1);
               break;
           case LedFunction::DIM_PULSE:
               dimPulse(i, ledCommands[i].option1, ledCommands[i].option2);
               break;
           case LedFunction::CYCLE:
               cycle(i, ledCommands[i].option1);
               break;
           case LedFunction::SOLID_COLOR:
           case LedFunction::SET_OFF_COLOR:
               ledColor(i, ledCommands[i].option1);
               break;
           case LedFunction::RAINBOW:
               rainbow(i);
               break;
           case LedFunction::SHORT_CIRCUIT:
               shortCircuit(i, ledCommands[i].option1);
               break;
           case LedFunction::CLEAR_NO_AUTO_OVERRIDE_ON:
               enableTwitchLED[i] = 0;
               resetLEDTwitch(i);
               offColorOverride[i] = true;
               flushCommandArray(i, 0);
               break;
           case LedFunction::CLEAR_AUTO_OVERRIDE_ON:
               enableTwitchLED[i] = (ledCommands[i].option1 == 1 || ledCommands[i].option1 == 2) ?
                                    ledCommands[i].option1 : startEnableTwitchLED[i];
               resetLEDTwitch(i);
               offColorOverride[i] = true;
               flushCommandArray(i, 0);
               break;
           case LedFunction::CLEAR_NO_AUTO_OVERRIDE_OFF:
               enableTwitchLED[i] = 0;
               resetLEDTwitch(i);
               offColorOverride[i] = false;
               flushCommandArray(i, 0);
               break;
           case LedFunction::CLEAR_AUTO_OVERRIDE_OFF:
               enableTwitchLED[i] = (ledCommands[i].option1 == 1 || ledCommands[i].option1 == 2) ?
                                    ledCommands[i].option1 : startEnableTwitchLED[i];
               resetLEDTwitch(i);
               offColorOverride[i] = false;
               flushCommandArray(i, 0);
               break;
           default:
               break;
       }

       // Auto LED twitch
       if (enableTwitchLED[i] > 0 && ledTwitchTimers[i].check() && isLedIdle(i)) {

           twitchLEDRunTime[i] = random(readLedTwitchRunInterval(i, 0), readLedTwitchRunInterval(i, 1));
           ledHaltTime[i] = millis();
           resetAnimation(i);

           if (enableTwitchLED[i] == 2) {
               ledCommands[i].function = (LedFunction)random(2, 8);
               ledCommands[i].option1 = random(0, 10);
               ledCommands[i].option2 = random(0, 10);
           } else {
               ledCommands[i].function = (LedFunction)readDefaultLedTwitchCmd(i, 0);
               ledCommands[i].option1 = readDefaultLedTwitchCmd(i, 1);
               ledCommands[i].option2 = readDefaultLedTwitchCmd(i, 2);
           }
           ledCommands[i].halt = twitchLEDRunTime[i];

           #ifdef DEBUG
           Serial.print(F("Auto LED Twitch: "));
           printHpName(i);
           Serial.print(F(" Function="));
           Serial.print((uint8_t)ledCommands[i].function);
           Serial.print(F(" Duration="));
           Serial.println(twitchLEDRunTime[i]);
           #endif

           ledTwitchTimers[i].setInterval(random(readLedTwitchInterval(i, 0), readLedTwitchInterval(i, 1)) * 1000UL);
       }
   }

   // Process HP commands
   for (uint8_t i = 0; i < Config::HP_COUNT; i++) {
       if (hpCommands[i].halt > 0 && (millis() - hpHaltTime[i] >= hpCommands[i].halt * 1000UL)) {
           flushCommandArray(i, 1);
           wagCount[i] = -1;
       }

       switch (hpCommands[i].function) {
           case HpFunction::GOTO_PRESET:
               positionHP(i, hpCommands[i].option1, readServoSpeed(0));
               flushCommandArray(i, 1);
               break;
           case HpFunction::RANDOM_POS:
               twitchHP(i);
               flushCommandArray(i, 1);
               break;
           case HpFunction::WAG_LEFT_RIGHT:
               wagHP(i, 1);
               break;
           case HpFunction::WAG_UP_DOWN:
               wagHP(i, 2);
               break;
           case HpFunction::AUTO_TWITCH_OFF:
               enableTwitchHP[i] = false;
               flushCommandArray(i, 1);
               break;
           case HpFunction::AUTO_TWITCH_ON:
               enableTwitchHP[i] = true;
               flushCommandArray(i, 1);
               break;
           default:
               break;
       }

       // Auto HP twitch
       if (enableTwitchHP[i] && hpTwitchTimers[i].check() && isHpIdle(i)) {
           twitchHP(i);
           resetHPTwitch(i);

           #ifdef DEBUG
           Serial.print(F("Auto HP Twitch: "));
           printHpName(i);
           Serial.println();
           #endif
       }
   }
}

////////////////////////////////////////////////////////////////////////////////////////////////////////
///                                  Command Processing Functions                                    ///
////////////////////////////////////////////////////////////////////////////////////////////////////////

Command parseCommand(const char* cmdStr) {
   Command cmd;

   if (!cmdStr || strlen(cmdStr) < 2) {
       return cmd;
   }

   char buffer[Config::INPUT_BUFFER_LEN];
   strncpy(buffer, cmdStr, Config::INPUT_BUFFER_LEN - 1);
   buffer[Config::INPUT_BUFFER_LEN - 1] = '\0';

   char* pipePos = strchr(buffer, '|');
   if (pipePos) {
       *pipePos = '\0';
       cmd.duration = atoi(pipePos + 1);
   }

   if (strcasecmp(buffer, "help") == 0 || strcmp(buffer, "?") == 0) {
       cmd.device = 'H';
       return cmd;
   }

   cmd.device = toupper(buffer[0]);

   if (strlen(buffer) > 1) {
       cmd.type = buffer[1] - '0';
   }

   if (strlen(buffer) > 3) {
       cmd.function = (buffer[2] - '0') * 10 + (buffer[3] - '0');
   }

   if (strlen(buffer) > 4) {
       cmd.option1 = buffer[4] - '0';
   }

   if (strlen(buffer) > 5) {
       cmd.option2 = buffer[5] - '0';
   }

   return cmd;
}

CommandError validateCommand(const Command& cmd) {
   if (!cmd.isValid()) {
       return CommandError::INVALID_DEVICE;
   }

   if (cmd.device != 'S' && cmd.device != 'H' && cmd.type > 1) {
       return CommandError::INVALID_TYPE;
   }

   if (cmd.option1 > 9 || cmd.option2 > 9) {
       return CommandError::OUT_OF_BOUNDS;
   }

   return CommandError::OK;
}

const __FlashStringHelper* errorToString(CommandError err) {
   switch(err) {
       case CommandError::OK:               return F("No error");
       case CommandError::INVALID_FORMAT:   return F("Invalid command format");
       case CommandError::INVALID_DEVICE:   return F("Invalid device (use F,R,T,A,X,Y,Z,S)");
       case CommandError::INVALID_TYPE:     return F("Invalid type (use 0 for LED, 1 for Servo)");
       case CommandError::INVALID_FUNCTION: return F("Invalid function number");
       case CommandError::INVALID_COLOR:    return F("Invalid color (use 0-9)");
       case CommandError::OUT_OF_BOUNDS:    return F("Value out of bounds");
       case CommandError::BUFFER_OVERFLOW:  return F("Command too long");
       default:                             return F("Unknown error");
   }
}

void processCommand(const Command& cmd) {
   if (cmd.device == 'S') {
       switch ((Sequence)cmd.function) {
           case Sequence::LEIA_MODE:
               for (uint8_t i = 0; i < Config::HP_COUNT; i++) {
                   resetAnimation(i);
                   flushCommandArray(i, 0);
                   flushCommandArray(i, 1);
               }
               positionHP(0, 0, readServoSpeed(0));
               ledCommands[0].function = LedFunction::LEIA;
               if (cmd.duration > 0) {
                   for (uint8_t i = 0; i < Config::HP_COUNT; i++) {
                       ledCommands[i].halt = cmd.duration;
                       ledHaltTime[i] = millis();
                       hpCommands[i].halt = cmd.duration;
                       hpHaltTime[i] = millis();
                   }
               }
               break;
           case Sequence::ALL_OFF:
               setAutoMode(false, 0, true);
               break;
           case Sequence::ALL_ON_DEFAULT:
               setAutoMode(true, 1, true);
               break;
           case Sequence::ALL_ON_RANDOM:
               setAutoMode(true, 2, true);
               break;
           case Sequence::ALL_OFF_WITH_OFFCOLOR:
               setAutoMode(false, 0, false);
               break;
           case Sequence::ALL_ON_DEFAULT_WITH_OFFCOLOR:
               setAutoMode(true, 1, false);
               break;
           case Sequence::ALL_ON_RANDOM_WITH_OFFCOLOR:
               setAutoMode(true, 2, false);
               break;
       }
       return;
   }

   uint8_t hpMask = 0;
   switch (cmd.device) {
       case 'F': hpMask = 0b001; break;
       case 'R': hpMask = 0b010; break;
       case 'T': hpMask = 0b100; break;
       case 'A': hpMask = 0b111; break;
       case 'X': hpMask = 0b011; break;
       case 'Y': hpMask = 0b101; break;
       case 'Z': hpMask = 0b110; break;
   }

   for (uint8_t i = 0; i < Config::HP_COUNT; i++) {
       if (hpMask & (1 << i)) {
           executeCommand(i, cmd);
       }
   }
}

void executeCommand(uint8_t hp, const Command& cmd) {
   if (cmd.type == 0) {
       flushCommandArray(hp, 0);

       if (cmd.function >= 96 && cmd.function <= 99) {
           ledCommands[hp].function = (LedFunction)cmd.function;
           ledCommands[hp].option1 = cmd.option1;
           return;
       }

       ledCommands[hp].function = (LedFunction)cmd.function;

       if (cmd.option1 >= 0) {
           ledCommands[hp].option1 = (cmd.option1 == 0) ? random(1, 10) : cmd.option1;
       } else {
           ledCommands[hp].option1 = (cmd.function == 7) ? readShortColor(hp) : readDefaultColor(hp);
       }

       if (cmd.function == 3) {
           ledCommands[hp].option2 = (cmd.option2 >= 0) ? cmd.option2 : DIM_PULSE_SPEED;
       }

       if (cmd.duration > 0) {
           ledCommands[hp].halt = cmd.duration;
           ledHaltTime[hp] = millis();
       }

       resetAnimation(hp);
   }
   else if (cmd.type == 1) {
       flushCommandArray(hp, 1);

       hpCommands[hp].function = (HpFunction)cmd.function;
       hpCommands[hp].option1 = (cmd.option1 >= 0) ? cmd.option1 : 1;

       if (cmd.duration > 0) {
           hpCommands[hp].halt = cmd.duration;
           hpHaltTime[hp] = millis();
       }

       wagCount[hp] = -1;
   }
}

////////////////////////////////////////////////////////////////////////////////////////////////////////
///                                  LED Functions                                                   ///
////////////////////////////////////////////////////////////////////////////////////////////////////////

void ledOff(uint8_t hp) {
   neoStrips[hp].clear();
   neoStrips[hp].show();
}

void ledColor(uint8_t hp, uint8_t color) {
   uint32_t c = getColor(color, 0);
   for (uint8_t i = 0; i < Config::LEDS_PER_HP; i++) {
       neoStrips[hp].setPixelColor(i, c);
   }
   neoStrips[hp].show();
}

void colorProjector(uint8_t hp, uint8_t color) {
   if (ledTimers[hp].check()) {
       for (uint8_t i = 0; i < Config::LEDS_PER_HP; i++) {
           neoStrips[hp].setPixelColor(i, getColor(color, random(0, 10)));
       }
       neoStrips[hp].show();
       ledTimers[hp].setInterval(random(Config::COLOR_PROJECTOR_MIN, Config::COLOR_PROJECTOR_MAX));
   }
}

void dimPulse(uint8_t hp, uint8_t color, uint8_t speed) {
   uint16_t interval = map(speed, 0, 9, readDimPulseSpeedRange(1), readDimPulseSpeedRange(0));
   unsigned long elapsed = ledTimers[hp].getElapsed();
   uint8_t frames = (elapsed / interval) % 64;

   if (frames > 32) frames = 64 - frames;

   uint32_t c = dimColorValue(color, frames * 8);
   for (uint8_t i = 0; i < Config::LEDS_PER_HP; i++) {
       neoStrips[hp].setPixelColor(i, c);
   }
   neoStrips[hp].show();
}

void cycle(uint8_t hp, uint8_t color) {
   if (ledTimers[hp].check()) {
       ledTimers[hp].setInterval(Config::CYCLE_INTERVAL);

       neoStrips[hp].clear();
       if (animationFrame[hp] > 0 && animationFrame[hp] < Config::LEDS_PER_HP) {
           neoStrips[hp].setPixelColor(animationFrame[hp], getColor(color, 0));
       }
       neoStrips[hp].show();

       if (++animationFrame[hp] >= Config::LEDS_PER_HP) {
           animationFrame[hp] = 1;
       }
   }
}

void rainbow(uint8_t hp) {
   unsigned long elapsed = ledTimers[hp].getElapsed();
   uint16_t frames = (elapsed / Config::RAINBOW_INTERVAL) % (256 * 5);

   for (uint8_t i = 0; i < Config::LEDS_PER_HP; i++) {
       neoStrips[hp].setPixelColor(i, wheel(((i * 256 / Config::LEDS_PER_HP) + frames) & 255));
   }
   neoStrips[hp].show();
}

void shortCircuit(uint8_t hp, uint8_t color) {
   if (shortCircuitLoop[hp] > Config::SHORT_CIRCUIT_MAX_LOOPS) return;

   uint16_t interval = 10 + (shortCircuitLoop[hp] * random(15, 25));

   if (ledTimers[hp].getElapsed() >= interval) {
       ledTimers[hp].reset();

       if (!shortCircuitFlag[hp]) {
           ledOff(hp);
           shortCircuitFlag[hp] = true;
       } else {
           for (uint8_t i = 0; i < Config::LEDS_PER_HP; i++) {
               neoStrips[hp].setPixelColor(i, getColor(color, random(0, 10)));
           }
           neoStrips[hp].show();
           shortCircuitFlag[hp] = false;
           shortCircuitLoop[hp]++;
       }
   }
}

uint32_t getColor(uint8_t colorIndex, uint8_t variant) {
   if (colorIndex > 9) colorIndex = 0;
   if (variant > 9) variant = 0;
   return pgm_read_dword(&BASIC_COLORS[colorIndex][variant]);
}

uint32_t wheel(uint8_t pos) {
   pos = 255 - pos;
   if (pos < 85) {
       return neoStrips[0].Color(255 - pos * 3, 0, pos * 3);
   }
   if (pos < 170) {
       pos -= 85;
       return neoStrips[0].Color(0, pos * 3, 255 - pos * 3);
   }
   pos -= 170;
   return neoStrips[0].Color(pos * 3, 255 - pos * 3, 0);
}

uint32_t dimColorValue(uint8_t color, uint8_t brightness) {
   if (brightness == 0) return C_OFF;
   uint32_t c = getColor(color, 0);
   uint8_t r = (uint8_t)(((uint16_t)((c >> 16) & 0xFF) * brightness) / 255);
   uint8_t g = (uint8_t)(((uint16_t)((c >> 8)  & 0xFF) * brightness) / 255);
   uint8_t b = (uint8_t)(((uint16_t)(c & 0xFF)  * brightness) / 255);
   return neoStrips[0].Color(r, g, b);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////
///                                  HP Servo Functions                                              ///
///                                                                                                  ///
///  Speed and acceleration are handled in hardware by the Maestro.                                  ///
///  We set speed/accel per channel then simply send the target position.                            ///
///  The Maestro smoothly ramps to the target automatically.                                         ///
///  The speed parameter here maps to a Maestro speed value (quarter-us/10ms).                      ///
///  A ServoMoveTimer tracks estimated completion time per HP.                                       ///
////////////////////////////////////////////////////////////////////////////////////////////////////////

void positionHP(uint8_t hp, uint8_t pos, uint16_t speed) {
   if (hp >= Config::HP_COUNT) return;
   if (pos >= Config::HP_POSITIONS) pos = 0;

   // Apply speed to both channels for this HP before triggering the subroutine.
   // Speed of 0 means use the default from Config.
   uint16_t maestroSpeed = (speed > 0) ? speed : Config::MAESTRO_MOVE_SPEED;
   maestro.setSpeed(readHpPin(hp, 0), maestroSpeed);
   maestro.setSpeed(readHpPin(hp, 1), maestroSpeed);

   // Subroutine index = (hp * HP_POSITIONS) + position.
   // Positions are defined in FlthyHPs_Maestro_Script.mscr loaded onto the Maestro.
   uint8_t subroutine = (hp * Config::HP_POSITIONS) + pos;
   maestro.restartScript(subroutine);

   servoMoveTimers[hp].start();
}

void twitchHP(uint8_t hp) {
   // Pick a random speed within the configured range, mapped to Maestro units.
   // Lower speedMs = faster = higher Maestro speed value.
   // Maestro speed 30 ≈ 800ms, speed 80 ≈ 300ms for 600us range.
   uint16_t speedMs = random(readServoSpeed(0), readServoSpeed(1));
   uint16_t maestroSpeed = map(speedMs, readServoSpeed(0), readServoSpeed(1), 80, 30);

   // Pick a random safe position — all positions are defined in the Maestro script.
   positionHP(hp, random(0, Config::HP_POSITIONS), maestroSpeed);
}

void wagHP(uint8_t hp, uint8_t type) {
   if (wagCount[hp] < 0) {
       wagCount[hp] = 0;
       wagTimers[hp].reset();
   }

   if (wagTimers[hp].check()) {
       wagCount[hp]++;

       if (wagCount[hp] % 2) {
           positionHP(hp, (type == 1) ? 3 : 1, 250);  // left or up
       } else {
           positionHP(hp, (type == 1) ? 6 : 2, 250);  // right or down
       }

       if (wagCount[hp] >= Config::WAG_CYCLES) {
           flushCommandArray(hp, 1);
           wagCount[hp] = -1;
       }
   }
}


////////////////////////////////////////////////////////////////////////////////////////////////////////
///                                  System Functions                                                ///
////////////////////////////////////////////////////////////////////////////////////////////////////////

void resetAnimation(uint8_t hp) {
   animationFrame[hp] = 0;
   shortCircuitFlag[hp] = false;
   shortCircuitLoop[hp] = 0;
   ledTimers[hp].reset();
   ledTimers[hp].setInterval(100);
   ledOff(hp);
}

void setAutoMode(bool hpTwitch, uint8_t ledTwitchMode, bool offColorOverrideMode) {
   for (uint8_t i = 0; i < Config::HP_COUNT; i++) {
       enableTwitchHP[i] = hpTwitch;
       enableTwitchLED[i] = ledTwitchMode;
       offColorOverride[i] = offColorOverrideMode;
       flushCommandArray(i, 0);
       flushCommandArray(i, 1);
       ledOff(i);
   }
}

void initializeStartupState() {
   for (uint8_t i = 0; i < Config::HP_COUNT; i++) {
       resetAnimation(i);

       // Start with the configured default LED pattern on every HP so the
       // system provides an immediate power-on visual check.
       ledCommands[i].function = (LedFunction)readDefaultLedTwitchCmd(i, 0);
       ledCommands[i].option1 = readDefaultLedTwitchCmd(i, 1);
       ledCommands[i].option2 = readDefaultLedTwitchCmd(i, 2);
       ledCommands[i].halt = (Config::LED_STARTUP_BASE + 999UL) / 1000UL;
       ledHaltTime[i] = millis();

       // Always center the HPs on startup, independent of later auto-twitch state.
       positionHP(i, Config::HP_CENTER_POSITION, Config::MAESTRO_MOVE_SPEED);

       // Start fresh auto-twitch timers from boot so startup actions happen first.
       resetLEDTwitch(i);
       resetHPTwitch(i);
   }
}

bool isLedIdle(uint8_t hp) {
   return ledCommands[hp].function == LedFunction::DO_NOTHING ||
          ledCommands[hp].function == LedFunction::SET_OFF_COLOR;
}

bool isHpIdle(uint8_t hp) {
   return hpCommands[hp].function == HpFunction::DO_NOTHING;
}

void flushCommandArray(uint8_t hp, uint8_t type) {
   if (type == 0) {
       if (readOffColor(hp) >= 0 && !offColorOverride[hp]) {
           ledCommands[hp].function = LedFunction::SET_OFF_COLOR;
           ledCommands[hp].option1 = readOffColor(hp);
       } else {
           ledCommands[hp].function = LedFunction::DO_NOTHING;
           ledCommands[hp].option1 = 0;
       }
       ledCommands[hp].option2 = 0;
       ledCommands[hp].halt = -1;
   } else {
       hpCommands[hp].function = HpFunction::DO_NOTHING;
       hpCommands[hp].option1 = 0;
       hpCommands[hp].halt = -1;
       if (enableTwitchHP[hp]) {
           resetHPTwitch(hp);
       }
   }
}

void resetLEDTwitch(uint8_t hp) {
   ledOff(hp);
   ledTwitchTimers[hp].setInterval(random(readLedTwitchInterval(hp, 0), readLedTwitchInterval(hp, 1)) * 1000UL);
   ledTwitchTimers[hp].reset();
}

void resetHPTwitch(uint8_t hp) {
   hpTwitchTimers[hp].setInterval(random(readHpTwitchInterval(hp, 0), readHpTwitchInterval(hp, 1)) * 1000UL);
   hpTwitchTimers[hp].reset();
}

void statusLedOn() {
   digitalWrite(STATUS_LED_PIN, HIGH);
   statusLedTimer.reset();
}

void printHelp() {
   Serial.println(F("\n=== FlthyHPs v2.1 Maestro Edition ==="));
   Serial.println(F("Format: DT##[C][S][|duration]"));
   Serial.println(F("D=Device, T=Type, ##=Function"));
   Serial.println(F("\nDEVICES (D):"));
   Serial.println(F("  F,R,T = Front,Rear,Top HP"));
   Serial.println(F("  A = All HPs"));
   Serial.println(F("  X = Front+Rear, Y = Front+Top"));
   Serial.println(F("  Z = Rear+Top, S = Sequence"));
   Serial.println(F("\nTYPE (T): 0=LED, 1=Servo"));
   Serial.println(F("\nLED FUNCTIONS (T=0):"));
   Serial.println(F("  01 = Leia (blue hologram)"));
   Serial.println(F("  02 = Color projector"));
   Serial.println(F("  03 = Dim pulse"));
   Serial.println(F("  04 = Cycle"));
   Serial.println(F("  05 = Solid color"));
   Serial.println(F("  06 = Rainbow"));
   Serial.println(F("  07 = Short circuit"));
   Serial.println(F("  96-99 = Clear modes"));
   Serial.println(F("\nSERVO FUNCTIONS (T=1):"));
   Serial.println(F("  01 = Go to position"));
   Serial.println(F("  02/03 = RC control"));
   Serial.println(F("  04 = Random position"));
   Serial.println(F("  05/06 = Wag"));
   Serial.println(F("  98/99 = Auto twitch off/on"));
   Serial.println(F("\nOPTIONS:"));
   Serial.println(F("  C = Color (1-9, 0=random)"));
   Serial.println(F("  S = Speed (0-9 for dim pulse)"));
   Serial.println(F("  |duration = Run time in seconds"));
   Serial.println(F("\nEXAMPLES:"));
   Serial.println(F("  F0055    -> Front LED solid blue"));
   Serial.println(F("  A104     -> All servos random"));
   Serial.println(F("  T006|20  -> Top rainbow for 20s"));
   Serial.println(F("  S5       -> Enable all auto"));
   Serial.println(F("=====================================\n"));
}

////////////////////////////////////////////////////////////////////////////////////////////////////////
///                                  Communication Functions                                         ///
////////////////////////////////////////////////////////////////////////////////////////////////////////

void serialEvent() {
   while (Serial.available()) {
       char inChar = Serial.read();

       if (commandBuffer.isComplete(inChar)) {
           stringComplete = true;
       } else if (inChar != '\r' && inChar != '\n') {
           commandBuffer.addChar(inChar);
       }

       statusLedOn();
   }
}

void i2cEvent(int howMany) {
   UNUSED(howMany);

   commandBuffer.clear();

   while (Wire.available() && commandBuffer.length() < Config::MAX_COMMAND_LENGTH) {
       char inChar = Wire.read();
       commandBuffer.addChar(inChar);
   }

   while (Wire.available()) {
       Wire.read();
   }

   stringComplete = true;
   statusLedOn();
}

////////////////////////////////////////////////////////////////////////////////////////////////////////
///                                  Debug Functions                                                 ///
////////////////////////////////////////////////////////////////////////////////////////////////////////

#ifdef DEBUG
void printDebug(const char* msg) {
   Serial.print(F("[DEBUG] "));
   Serial.println(msg);
}

void printHpName(uint8_t hp) {
   switch(hp) {
       case 0: Serial.print(F("Front")); break;
       case 1: Serial.print(F("Rear"));  break;
       case 2: Serial.print(F("Top"));   break;
       default: Serial.print(F("Unknown")); break;
   }
}

int freeRam() {
   extern int __heap_start, *__brkval;
   int v;
   return (int) &v - (__brkval == 0 ? (int) &__heap_start : (int) __brkval);
}
#endif
