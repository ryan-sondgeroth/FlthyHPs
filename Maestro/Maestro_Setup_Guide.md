# FlthyHPs Maestro Setup Guide
### Pololu 6-Channel Micro Maestro — New Installation Calibration

---

## Overview

All base servo position data for the FlthyHPs system lives in **FlthyHPs_Maestro_Script.mscr**, not in the Arduino sketch. This means calibrating a new installation is done in **Maestro Control Center** — no sketch changes are needed for position tuning. Custom routines can be added later starting at subroutine `27`.

The process has three phases:
1. Configure channel settings and safety limits
2. Find and record positions for each HP
3. Update the script and reload it onto the Maestro

---

## What You Need

- Pololu Maestro Control Center (Windows or Linux via USB)
- USB cable connected to the Maestro
- The droid powered up with servos connected
- A copy of `FlthyHPs_Maestro_Script.mscr`

---

## Channel Assignments

| Channel | Name | HP | Axis |
|---------|------|------------|------|
| 0 | Front 1 | Front HP | Pan (left/right) |
| 1 | Front 2 | Front HP | Tilt (up/down) |
| 2 | Rear 1 | Rear HP | Pan (left/right) |
| 3 | Rear 2 | Rear HP | Tilt (up/down) |
| 4 | Top 1 | Top HP | Pan (left/right) |
| 5 | Top 2 | Top HP | Tilt (up/down) |

> Note: Which axis is "pan" vs "tilt" depends on your physical installation. The labels above assume a typical R2 HP mounting. If your axes are swapped, simply swap the channel numbers in the script.

---

## Phase 1 — Channel Configuration

Open Maestro Control Center and connect to the Maestro. Go to the **Channel Settings** tab.

### For each channel (0–5):

**Mode:** Set to `Servo`

**Min / Max pulse width:**
These are your hardware safety limits. The Maestro will clamp any command to within these values regardless of what the script sends — this prevents servos from stalling against mechanical stops.

- Start with a conservative range: **Min = 1200 µs, Max = 1800 µs**
- You will tighten these after finding your actual position values in Phase 2
- Final min/max should be set to just outside your outermost calibrated positions — roughly 50 µs of headroom on each end

**Speed:** Leave at `0` (unlimited) during calibration so sliders respond immediately. The sketch controls speed at runtime.

**Acceleration:** Leave at `0` during calibration. Again, the sketch sets this at runtime.

**Home position:** Leave unset for now. The sketch explicitly centers each HP on startup, so you do not need to rely on Maestro home positions.

Click **Apply Settings** before moving on. If you want a backup on your computer, use **File > Save settings file...**

---

## Phase 2 — Finding Positions

Go to the **Status** tab. You will see a slider for each channel.

### Work one HP at a time

For each HP, move both sliders simultaneously to find each position. The two channels in a pair work together — moving one axis affects the perceived direction of the HP, and because the axes may not be aligned with physical up/down/left/right, you will often need to adjust both channels to reach a clean position.

### Recommended order

Work through positions in this order for each HP:

1. **Center (position 0)** — Start here. Find the resting position where the HP is pointing straight forward (or straight down for the Top HP). This is your reference point for everything else. Note both channel values.

2. **Up (position 1)** — Move the HP to point clearly upward. Note both values.

3. **Down (position 2)** — Move to point clearly downward. Note both values.

4. **Left (position 3)** — Move to point clearly to the left. Note both values.

5. **Right (position 6)** — Move to point clearly to the right. Note both values.

6. **Upper-Left (position 4)** — Diagonal, roughly halfway between Up and Left. Note both values.

7. **Lower-Left (position 5)** — Diagonal, roughly halfway between Down and Left. Note both values.

8. **Upper-Right (position 7)** — Diagonal, roughly halfway between Up and Right. Note both values.

9. **Lower-Right (position 8)** — Diagonal, roughly halfway between Down and Right. Note both values.

### Tips

- **Go slow.** Move sliders gradually and watch for any resistance or binding sound from the servo. Stop before that point.
- **Check the whole envelope.** After finding all 9 positions, manually drag sliders through the full range between them to confirm no intermediate position causes binding.
- **The diagonal positions should feel natural.** If upper-left requires extreme values on one axis to look diagonal, the axis mapping may be swapped — check your wiring against the channel assignment table above.
- **Record µs values, not quarter-µs.** The Status tab displays in microseconds. The script needs quarter-microseconds (multiply by 4). It is easier to record µs now and convert when editing the script.
- **Symmetry is a good sanity check.** Left and Right should be roughly equal distance from Center on channel 0. Up and Down should be roughly equal on channel 1. If they are very asymmetric, verify the HP is mechanically centered at rest.

### After finding all positions — tighten your Min/Max limits

Go back to **Channel Settings** and update Min/Max for each channel:

- Find the lowest µs value you recorded for that channel across all 9 positions
- Set **Min** to that value minus 50 µs
- Find the highest µs value recorded
- Set **Max** to that value plus 50 µs

This gives a small buffer while ensuring the Maestro will always clamp any out-of-range command before it reaches the servo.

Click **Apply Settings**. If you want a backup on your computer, use **File > Save settings file...**

---

## Phase 3 — Update the Script

Open `FlthyHPs_Maestro_Script.mscr` in a text editor.

For each position in each HP block, update the two values with your recorded µs values **multiplied by 4**.

Example — if you recorded channel 0 = 1480 µs and channel 1 = 1390 µs for hp1_up:

```
sub hp1_up            # subroutine 1
  5920 0 servo        # ch0 = 1480us  (1480 x 4 = 5920)
  5560 1 servo        # ch1 = 1390us  (1390 x 4 = 5560)
  quit
```

The comment showing the µs value is just for your reference — update it too so it stays accurate.

Important: these position subroutines are launched from the Arduino sketch with Maestro `restartScript(subroutine)`, so each one must end with `quit`. Using `return` here causes a subroutine call stack overflow/underflow error on the Maestro.

Custom Maestro routines start at subroutine `27`. Use `M27`, `M28`, and so on from the sketch to run them. Keep the base position routines `0-26` intact, and put any reusable pose helper subroutines at the end of the file so the base table stays easy to scan.

### Subroutine reference

| Subroutine | HP | Servo 1 (Pan) | Servo 2 (Tilt) | Position |
|---|---|---|---|---|
| 0 | Front | Ch 0 — Front 1 | Ch 1 — Front 2 | center |
| 1 | Front | Ch 0 — Front 1 | Ch 1 — Front 2 | up |
| 2 | Front | Ch 0 — Front 1 | Ch 1 — Front 2 | down |
| 3 | Front | Ch 0 — Front 1 | Ch 1 — Front 2 | left |
| 4 | Front | Ch 0 — Front 1 | Ch 1 — Front 2 | upper-left |
| 5 | Front | Ch 0 — Front 1 | Ch 1 — Front 2 | lower-left |
| 6 | Front | Ch 0 — Front 1 | Ch 1 — Front 2 | right |
| 7 | Front | Ch 0 — Front 1 | Ch 1 — Front 2 | upper-right |
| 8 | Front | Ch 0 — Front 1 | Ch 1 — Front 2 | lower-right |
| 9 | Rear | Ch 2 — Rear 1 | Ch 3 — Rear 2 | center |
| 10 | Rear | Ch 2 — Rear 1 | Ch 3 — Rear 2 | up |
| 11 | Rear | Ch 2 — Rear 1 | Ch 3 — Rear 2 | down |
| 12 | Rear | Ch 2 — Rear 1 | Ch 3 — Rear 2 | left |
| 13 | Rear | Ch 2 — Rear 1 | Ch 3 — Rear 2 | upper-left |
| 14 | Rear | Ch 2 — Rear 1 | Ch 3 — Rear 2 | lower-left |
| 15 | Rear | Ch 2 — Rear 1 | Ch 3 — Rear 2 | right |
| 16 | Rear | Ch 2 — Rear 1 | Ch 3 — Rear 2 | upper-right |
| 17 | Rear | Ch 2 — Rear 1 | Ch 3 — Rear 2 | lower-right |
| 18 | Top | Ch 4 — Top 1 | Ch 5 — Top 2 | center |
| 19 | Top | Ch 4 — Top 1 | Ch 5 — Top 2 | up |
| 20 | Top | Ch 4 — Top 1 | Ch 5 — Top 2 | down |
| 21 | Top | Ch 4 — Top 1 | Ch 5 — Top 2 | left |
| 22 | Top | Ch 4 — Top 1 | Ch 5 — Top 2 | upper-left |
| 23 | Top | Ch 4 — Top 1 | Ch 5 — Top 2 | lower-left |
| 24 | Top | Ch 4 — Top 1 | Ch 5 — Top 2 | right |
| 25 | Top | Ch 4 — Top 1 | Ch 5 — Top 2 | upper-right |
| 26 | Top | Ch 4 — Top 1 | Ch 5 — Top 2 | lower-right |

---

## Phase 4 — Load the Script onto the Maestro

1. In Maestro Control Center, go to the **Script** tab
2. Paste the contents of `FlthyHPs_Maestro_Script.mscr` into the script editor
3. If you want the script to start automatically after power-up, enable **Run script on startup**
4. Click **Apply Settings**
5. Use **File > Save settings file...** to save a portable backup after the script is loaded

Important:
- Do not try to build a new script-bearing settings file by hand from `UscCmd --getconf` output
- Pololu stores the script source code on the PC and in settings files saved by **Maestro Control Center**, not on the Maestro itself
- `UscCmd --getconf` is useful for exporting device settings, but it is not a reliable starting point for hand-authoring a `<Script>` block
- Once you have a valid settings file saved from Maestro Control Center, you can use `UscCmd --configure FILE` later to load that complete settings file onto another Maestro
- If you need to re-load only the custom routine edits, remember that the sketch uses `M##` for direct Maestro subroutines and `DT...` for the normal HP commands

---

## Phase 5 — Verify

With the Arduino sketch running and the Maestro connected:

1. Power everything up — the sketch centers each HP on startup by sending the base center position. All three HPs should move to center.
2. Send a twitch command (`A104`) and watch the HPs move through random positions. Verify no binding or stalling.
3. Send a wag command (`A105` for left/right, `A106` for up/down) and verify the motion looks correct. You can append `S`, `M`, or `F` for speed, for example `A105|20S`.
4. Send preset position commands (`F101P` where P = 0–8) to individually verify each named position on the Front HP.
5. Send `M27` to verify the custom Maestro clock routine runs after the base positions are loaded.

If any position looks wrong or causes binding:
- Go back to the Status tab, find the correct values
- Update the script
- Reload via Script tab
- No sketch upload needed

---

## Adding More Positions

The script has room for additional positions well within the Maestro's script memory limit. To add a position:

1. Leave subroutines `0-26` alone unless you are changing the base position table.
2. Add new custom subroutines starting at `27` and above.
3. Keep any reusable pose helper subroutines at the end of the file so the base table stays readable.
4. If you add new base positions, increment `Config::HP_POSITIONS` in the sketch from `9` to the new total and update both the sketch and script together.

---

## Quick Reference — Conversion

| µs | Quarter-µs |
|---|---|
| 1000 | 4000 |
| 1100 | 4400 |
| 1200 | 4800 |
| 1300 | 5200 |
| 1400 | 5600 |
| 1500 | 6000 (center) |
| 1600 | 6400 |
| 1700 | 6800 |
| 1800 | 7200 |
| 1900 | 7600 |
| 2000 | 8000 |
