#include <math.h>
#include <raylib.h>
#include <stdbool.h>
#include <stdint.h>

#include "song.h"

static struct {
  float incr;
  float idx;
  bool playing;
} sinState;

void updateSinChannel(void *data, unsigned int frames) {
  uint8_t *buffer = (uint8_t *)data;

  if (!sinState.playing) return;

  for (unsigned int i = 0; i < frames; i++) {
    buffer[i] = (uint8_t)(256 * sinf(2 * PI * sinState.idx));
    sinState.idx += sinState.incr;
    if (sinState.idx > 1) sinState.idx = 0;
  }
}

static struct {
  float incr;
  float idx;
  bool playing;
} pulseState;

void updatePulseChannel(void *data, unsigned int frames) {
  uint8_t *buffer = (uint8_t *)data;

  if (!pulseState.playing) return;

  for (unsigned int i = 0; i < frames; i++) {
    buffer[i] = (uint8_t)(255 * roundf(pulseState.idx));
    pulseState.idx += pulseState.incr;
    if (pulseState.idx > 1) pulseState.idx = 0;
  }
}

void toggleChannels(Channels channels, bool play) {
  if (channels & CHANNEL_SIN) sinState.playing = play;
  if (channels & CHANNEL_PULSE) pulseState.playing = play;
}

void setChannelsFrequency(Channels channels, float frequency) {
  float incr = frequency / SAMPLE_RATE;
  if (channels & CHANNEL_SIN) sinState.incr = incr;
  if (channels & CHANNEL_PULSE) pulseState.incr = incr;
}
