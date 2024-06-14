#include "music_theory.h"

#include <assert.h>
#include <stdbool.h>
#include <math.h>

float num2frequency(float tune, Note key, int octave, int num, Scale scale) {
  assert(scale.size >= num);

  // Convert tuning from A4 to the key.
  float fact = ((float) (((int) key) - ((int) A))) / 12;
  tune *= powf(2, fact);

  int note = scale.semitoneIndexes[num - 1];
  float base = tune * powf(2, octave - 4);
  return base * powf(2, ((float) note) / 12);
}
