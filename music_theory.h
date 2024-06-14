#ifndef SONG_MUSIC_THEORY_H_
#define SONG_MUSIC_THEORY_H_

typedef struct {
  int size;
  int *semitoneIndexes;
} Scale;

typedef enum {
  C,
  Cs,
  D,
  Ds,
  E,
  F,
  Fs,
  G,
  Gs,
  A,
  As,
  B,
} Note;

#define Bb As
#define Db Cs
#define Eb Ds
#define Gb Fs
#define Ab Gs

/*
 * The fourth octave is considered the base, so tune should be A4 (usually
 * 440hz).
 *
 * Example: return the frequency of the C's major scale first, tuned to 440:
 * ```
 * num2frequency(440, C, 4, 1, majorScale);
 * ```
 */
float num2frequency(float tune, Note key, int octave, int num, Scale scale);

#endif  // SONG_MUSIC_THEORY_H_
