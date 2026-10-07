# Specification

## What is MIDI?
**MIDI:** Musical Instrument Digital Interface, is a digital
set of instructions that describes how to play a song.
MIDI events (instructions) consist of information such as:

- Note pitch
- Duration (when a note turns ON/OFF)
- Volume
- Instrument
- ...

You can think of it as digital sheet music. It describes what
notes to play rather than containing the actual audio data.

MIDI data is stored digitally in files with the `.mid` extension.

## What is this Project?
This project implements a full MIDI pipeline consisting of:

1. MIDI sequencer that reads data from `.mid` files
   and outputs MIDI events on a MIDI port.
2. MIDI synthesizer that reads MIDI events from a MIDI port
   and produces an audio signal.

You can think of the two projects above as being subprojects.

When 1 and 2 are connected in series with a MIDI cable,
we get a full system that can read `.mid` files from e.g.
an SD card and produce an audio signal.
In other words, a **MIDI player**.
Along with a DAC, this gives us an analog audio signal.

## Division
To divide things up, each subproject
will have its own STM32L432KC and breadboard.

## Block Diagram

```
               MIDI FILE
			    │
				▼
		┌───────────────────┐
		│ 1. MIDI SEQUENCER │
		│                   │
		│ • parse .mid      │
		│ • tracks          │
		│ • tempo           │
		│ • timing          │
		│ • event queue     │
		└────────┬──────────┘
		         │
				MIDI events
				 │
				 ▼
		┌─────────────────────┐
		│ 2. MIDI SYNTHESIZER │
		│                     │
		│ • instruments       │
		│ • voices            │
		│ • samples           │
		│ • envelopes         │
		│ • mixing            │
		│ • effects           │
		└────────┬────────────┘
		         │
				PCM audio
				 │
				 ▼
				DAC
				 │
				Analog audio
```

## MIDI Pipeline (from .mid to audio signal)

### 1. MIDI Sequencer
A MIDI sequencer reads a `.mid` file and sends its MIDI events
to the synthesizer at the correct times.

A MIDI file contains time-stamped MIDI events, with instructions
such as **Note On**, **Note Off**, **instrument changes**,
and **controller changes** (for e.g. note volume). It also
contains timing information such as tempo and tick resolution.

So the sequencer:

- Reads and parses the `.mid` file.
- Determines when each MIDI event should occur.
- Converts the MIDI timing information into real time.
- Sends each event to the synthesizer
  when its scheduled time is reached.

For example:

```
Time       Event
────────────────────────
0.000 s    Note On  C4
0.500 s    Note Off C4
0.500 s    Note On  E4
1.000 s    Note Off E4
```

Please note that the sequencer does not generate audio.
It simply controls what the synthesizer should play and when.

### 2. MIDI Synthesizer
A synthesizer is a system that generates (synthesizes) an
audio signal using things such as oscillators, filters,
and amplifiers.

In this project, synthesis will be implemented
digitally using DSP (Digital Signal Processing).

#### MIDI events -> Audio
The synthesizer receives MIDI events from the sequencer
and converts them into audio. The MIDI events will have
to be parsed/decoded into structured instructions that
the code can understand.

To produce an audio signal representing notes we'll need:
1. Base waves/samples (representing e.g. instruments)
2. Then we apply the following operations on the
   base wave/samples to get the proper note samples:
   - Pitch shifting
   - Volume/velocity
   - Envelopes
3. In case of a polyphonic song (song with multiple voices/notes
   playing at the same time): Mix (combine) the voices.

#### Base Signal
There are a couple ways of implementing the base signal (waves/samples):

1. **Digital oscillators** that compute the waveform in real time.
2. **Wavetables** where waveform samples are stored in memory.
3. **Reading sample data from `.sf2` files**.

**Alternative 1:** Digital oscillators are simple and
useful for early testing, but generating more
complex waveforms can be CPU intensive.

**Alternative 2:** <u>Best alternative</u> considering
how samples stored in ROM require no computations,
along with the large amount of ROM the MCU has (256 kB).
The synthesizer still performs computations such as:
pitch generation, interpolation, envelopes (attack/decay, etc),
and mixing.

**Alternative 3:** An .sf2 file contains samples together with
information describing how they are used to construct instruments.
Reading from `.sf2` files is not that feasible due to how little
RAM the STM32L432KC has, only having 64 kB. Considering how
`.sf2` files can be multiple megabytes in size, RAM expansion
would be needed at a minimum.

##### Offline Reading of `.sf2` files
Instead, `.sf2` files can be used offline as the source for
extracting/preparing the samples or wavetables that will
be embedded in the STM32's ROM. There are some useful tools
for this such as:

- https://teensyaudio.github.io/Wavetable-Synthesis/html/md_additional_pages_soundfontDecoder.html
- https://ultraabox.github.io/sample_extractor.html

## Product

### Minimum Viable Product (MVP) - Most Important Features
- Monophonic (one note/voice at a time) MIDI sequencer
  which reads MIDI data from ROM and sends out
  MIDI events over UART.
- Basic synthesizer which reads and parses MIDI events over
  UART and produces a single sine wave corresponding to
  the current note's frequency.

### Extra/Non-Essential Features
- Polyphonic (allows for multiple notes/voices at a time)
- **Sequencer:**
  - Read `.mid` files from SD-card
  - Display for selecting `.mid` file
  - Buttons for changing the file
- **Synthesizer:**
  - Uses wavetables to store the sample data for base signals
  - Mixing of the notes/voices
  - Handles Volume/Amplitude of note
  - Envelopes (ADSR: Attack/Decay/Sustain/Release of the note)

## Components
**Necessary:**

- **2x DIN Chassi Mount:** for testing MIDI basics.
  - https://www.electrokit.com/din-hona-5-pol-chassi-180
- **1x or 2x USB to MIDI converters** (both way: send and receive MIDI events):
  Needed to test either the sequencer or synthesizer. You can use a
  computer connected over MIDI to replace the sequencer or synthesizer:
  - Testing:
    - `sequencer -> computer (acts as synthesizer)`: Test the sequencer
	- `computer (acts as sequencer) -> synthesizer`: Test the synthesizer
  - Links:
    - https://www.kjell.com/se/produkter/ljud-bild/kablar-adaptrar/din/plexgear-usb-midi-interface-p23954
	- Alt: https://www.amazon.com/usb-midi/s?k=usb+to+midi

**Maybe (If not available at University):**
- **2x MIDI ports** (DIN female 5-pin) (that can connect to breadboard):
  Each part in the pipeline will need one MIDI port.
  The sequencer will have one for output, the synthesizer
  will have one for input.
  - https://www.mouser.se/en/ProductDetail/Adafruit/1134?qs=GURawfaeGuCcC0%252BrNLxslQ%3D%3D
- **Optocoupler:** For MIDI input. As MIDI uses currents to transmit
  signals, an optocoupler is needed to convert a current to a voltage.



# General MIDI Info (Mostly AI-Generated)

## MIDI Sequencer
- **Answers:** "What should happen and when?"
- Read MIDI file from SD card
- Produces "note data" essentially

## MIDI Synthesizer
- **Answers:** "What should that event sound like?"
- Takes MIDI events and produces (synthesizes) audio:
    - Type of note
	- Note ON/OFF
	- Velocity

### Types of Synthesizers

| Synthesizer                | Needs SF2? | How it makes sound                                              |
| -------------------------- | ---------- | --------------------------------------------------------------- |
| **Oscillator synth**       | ❌          | Generates waveforms mathematically                              |
| **Wavetable/sample synth** | ❌/optional | Uses samples/wavetables built into firmware or external storage |
| **SoundFont synth**        | ✅          | Loads an `.sf2` containing samples + instrument definitions     |

So the simplest STM32 MIDI synth could be a sine wave generator. For multiple voices ("polyphony") you can add up the sine waves.

```
MIDI → oscillator → envelope → mixer → DAC
```

### Polyphony
Polyphony is the maximum number of individual tones or notes an electronic instrument can produce at the exact same time. When you exceed the limit, the oldest sounding notes drop out or cut off.

### Envelope
If you just generate a sine wave it would start and stop abruptly, which sounds unnatural. An **envelope** applies a changing multiplier to the sine wave. The most common one is **ADSR**: Attack, Decay, Sustain, Release.

```
volume
  1.0 │       /\
      │      /  \
	  │     /    \────────────
	  │    /                  \
	  │   /                    \
  0.0 │──┘                      └──────
      └───────────────────────────────► time
	      A    D       S          R
```

**For example:**
- **Attack:** how quickly the note reaches full volume
- **Decay:** how quickly it drops from full volume
- **Sustain:** volume while you hold the note
- **Release:** how quickly it fades after releasing the note

### Mixer
Suppose you're playing:

```
C4 ──► sine wave ──► envelope ──┐
                                │
E4 ──► sine wave ──► envelope ──┼──► MIXER ──► audio
                                │
G4 ──► sine wave ──► envelope ──┘
```

The mixer simply **combines the audio signals**.
