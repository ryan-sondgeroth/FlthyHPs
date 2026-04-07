# FlthyHPs Maestro Setup Guide
### Pololu 6-Channel Micro Maestro — New Installation Calibration

---

## Overview

All servo position data for the FlthyHPs system lives in **FlthyHPs_Maestro_Script.mscr**, not in the Arduino sketch. This means calibrating a new installation is entirely done in **Maestro Control Center** — no code changes required.

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

**Home position:** Leave unset for now. After calibration you can optionally set this to your center position value so `goHome()` on startup moves all HPs to center.

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
  return
```

The comment showing the µs value is just for your reference — update it too so it stays accurate.

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

If the Script tab is awkward or read-only on your Windows machine, use **UscCmd** instead. This preserves the channel settings you already calibrated.

1. Connect the Maestro over USB
2. Open a Windows Command Prompt
3. Export the current Maestro settings:

```bat
UscCmd --getconf "%USERPROFILE%\Desktop\FlthyHPs_Maestro_Settings.txt"
```

4. Open `FlthyHPs_Maestro_Settings.txt` in a text editor
5. Find the `<Script>` section and replace everything between `<Script>` and `</Script>` with the contents of `FlthyHPs_Maestro_Script.mscr`
6. Save the edited settings file
7. Load the updated settings back onto the Maestro:

```bat
UscCmd --configure "%USERPROFILE%\Desktop\FlthyHPs_Maestro_Settings.txt"
```

8. In Maestro Control Center, verify **Run script on startup** is enabled if you want the script to start automatically after power-up

Notes:
- `UscCmd` is installed with the Pololu Maestro Windows software
- This method copies the script into your current Maestro configuration without forcing you to paste into the Script tab
- Keep the exported settings file as a backup; it includes your script source and channel settings

---

## Phase 5 — Verify

With the Arduino sketch running and the Maestro connected:

1. Power everything up — the sketch calls `goHome()` on startup which triggers subroutine 0 (center) for each HP. All three HPs should move to center.
2. Send a twitch command (`A104`) and watch the HPs move through random positions. Verify no binding or stalling.
3. Send a wag command (`A105` for left/right, `A106` for up/down) and verify the motion looks correct.
4. Send preset position commands (`F101P` where P = 0–8) to individually verify each named position on the Front HP.

If any position looks wrong or causes binding:
- Go back to the Status tab, find the correct values
- Update the script
- Reload via Script tab
- No sketch upload needed

---

## Adding More Positions

The script has room for additional positions well within the Maestro's script memory limit. To add a position:

1. Add a new subroutine at the end of the relevant HP block in the script
2. Increment `Config::HP_POSITIONS` in the sketch from `9` to the new total
3. Upload the updated sketch once
4. All new positions are immediately available to `positionHP()`, `twitchHP()`, and random movement

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
