# Senspecimenprogramaro de Ondformo Generatori

Esperanto for "Sample-less program of waveform generation". A sample-less
synthetizer.

## Build

`make`

Needs a system installation of [Raylib](https://github.com/raysan5/raylib).
Generates the binary `build/song`.

## Run

Run the program with a song file as argument, e.g.: `build/song cdef.song`.

## File format

The file format was hacked together. Very simple but works. Empty lines are
allowed anywhere. I'll probably change it a lot in the future.

The first line is the song name.

The second line contains information in the form of space-separated numeric
ASCII values:

- The tuning, i.e., the frequency of the A4 note in hertz. Floating-point.  
- The key. This should be a value between 0 (for C) and 11 (for B). The value
  represents how much semitones to diverge from C of course. Integer.  
- The "alignment". This is a floating-point value in seconds representing the
  length of a single note. Think of it as the length of your *semibreve*,
  although thinking in usual musical compass notation does not make sense here.

The third line contains first the number of notes in the music scale, and them
a sequence of values representing the amount of semitones to diverge from the
key for each note. The first value will be usually 0, otherwise your first note
wouldn't be the key itself. All values in this line are integers.

Example, the example music in the repo (named "cdef", tuned to 440hz, in C,
aligned to half a second, with the major scale):

```
cdef
440.0 0 0.5
7 0 2 4 5 7 9 11
```

All the remaining lines represents sequence of notes themselves, or special
values to switch channels.

The first value in a line is the length of each note in the sequence. It's
multiplied by the alignment. Each value remaining is a note, 1-indexed in the
scale. A 0 means a pause note. Example line from the "cdef" song:

```
1 1 2 3
```

This sequence means full-sized notes (in this case, half a second). The first
one is a C (first of the major C scale), the second a D and the third an E.

A line with the words "sin" or "pulse" switches the parsing channel. Note
that the channels are played simultaneous. After the channel name, there should
be two values: the first (integer) sets the octave to be used for the sequence
(where 4 is the base which contains the tune-reference A). The second one is a
floating-point number between 0 and 1 to set the volume for the sequence.
Example: the line `sin 3 0.5` switches to the sine wave channel in the third
octave and half the volume.
