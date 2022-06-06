# Hand-Tracked Theremin

A stand-alone theremin application made in C++ using Ultraleap (Leap Motion) hand-tracking and the JUCE audio framework.

## Usage

Two hands must be visible by the hand-tracking hardware at all times for sound to be emitted. The left hand height
controls the volume of the note, and the right hand position (forwards/backwards) controls the pitch.

The frequency and volume sliders are read-only and are continually updated with the parameters of the currently emitting
wave.

There are a number of other sliders and option boxes. Some sliders control the size of the interaction zone, and the
bounds on the output parameters.

There are 4 frequency-scaling algorithms available:

- **Linear**: The frequency is proportional to distance
- **String**: The frequency changes as if a string were being shortened
- **Keyboard**: Distance between semitones is a constant
- **Theremin**: The frequency changes according to the resonant frequency of an RLC circuit, where the capacitance is
  inversely proportional to distance

There are several snapping modes available. These can be used to snap to scales (based on A440):

- **None**: No snapping - all frequencies can be played
- **Chromatic**: Snap to notes in the chromatic scale
- **Major**: Snap to notes in the A major scale
- **Pentatonic**: Snap to notes in the A major pentatonic scale
- **Chromatic Envelope**: Do not perform any snapping, but trigger the volume envelope whenever
  a note would change in the chromatic scale

There are also 17-tone versions of the Major, Pentatonic and Chromatic scales, as well as
11 and 14-tone versions of the Pentatonic scale.

There are also parameters to change an ADSR envelope. These parameters affect the volume of the note. If snapping is
enabled, the attack stage of the envelope is re-triggered when changing notes.

## Requirements

This currently only works on Windows, due to Ultraleap hand-tracking only being available on Windows.

The hand-tracking service must be installed and running in Desktop mode in order for the theremin to work correctly. An
Ultraleap hardware camera must also be plugged in.

## Building

CMake is required to build.

The hand-tracking SDK must be available in order to build, and is assumed by CMake to be installed in the default
location.

The other dependencies (`JUCE`, `moodycamel/readerwriterqueue`) are fetched by CMake when building.
