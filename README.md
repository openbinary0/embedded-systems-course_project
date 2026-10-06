# Specification
MIDI projects for generating and reading MIDI files and then play those signals as audio.


### Components
* [MIDI to USB adapter](https://www.electrokit.com/din-hona-5-pol-chassi-180).
* 2x [USB to MIDI](https://www.kjell.com/se/produkter/ljud-bild/kablar-adaptrar/din/plexgear-usb-midi-interface-p23954) (both way: send and receive MIDI events)
	* Alt. https://www.amazon.com/usb-midi/s?k=usb+to+midi
* 2x [MIDI port](https://www.mouser.se/en/ProductDetail/Adafruit/1134?qs=GURawfaeGuCcC0%252BrNLxslQ%3D%3D) (DIN female 5-pin) (that can connect to breadboard)*
* Input device (keypad)*
* Optocoupler*


## Block Diagram

```
             MIDI FILE
			    │
				▼
		┌─────────────────┐
		│ MIDI SEQUENCER  │
		│                 │
		│ • parse .mid    │
		│ • tracks        │
		│ • tempo         │
		│ • timing        │
		│ • event queue   │
		└────────┬────────┘
		         │
				MIDI events
			     │
				 ▼
		┌─────────────────┐
		│ MIDI SYNTHESIZER│
		│                 │
		│ • instruments   │
		│ • voices        │
		│ • samples       │
		│ • envelopes     │
		│ • mixing        │
		│ • effects       │
		└────────┬────────┘
		         │
				PCM audio
			     │
				 ▼
				DAC / I²S
			     │
				speakers
```

### MIDI Sequencer
- **Answers:** "What should happen and when?"
- Read MIDI file from SD card
- Produces "note data" essentially

### MIDI Synthesizer
- **Answers:** "What should that event sound like?"
- Takes MIDI events and produces (synthesizes) audio:
    - Type of note
	- Note ON/OFF
	- Velocity

#### Types of Synthesizers

| Synthesizer                | Needs SF2? | How it makes sound                                              |
| -------------------------- | ---------- | --------------------------------------------------------------- |
| **Oscillator synth**       | ❌          | Generates waveforms mathematically                              |
| **Wavetable/sample synth** | ❌/optional | Uses samples/wavetables built into firmware or external storage |
| **SoundFont synth**        | ✅          | Loads an `.sf2` containing samples + instrument definitions     |

So the simplest STM32 MIDI synth could be a sine wave generator. For multiple voices ("polyphony") you can add up the sine waves.

```
MIDI → oscillator → envelope → mixer → DAC
```

#### Polyphony
Polyphony is the maximum number of individual tones or notes an electronic instrument can produce at the exact same time. When you exceed the limit, the oldest sounding notes drop out or cut off.

#### Envelope
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

#### Mixer
Suppose you're playing:

```
C4 ──► sine wave ──► envelope ──┐
                                │
E4 ──► sine wave ──► envelope ──┼──► MIXER ──► audio
                                │
G4 ──► sine wave ──► envelope ──┘
```

The mixer simply **combines the audio signals**.
