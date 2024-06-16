#include <math.h>
#include <raylib.h>
#include <stdbool.h>
#include <stdint.h>

#include "song.h"

typedef struct {
  float incr;
  float idx;
  bool playing;
  AudioStream stream;
} ChannelState;

static ChannelState sinState;
static ChannelState pulseState;

void updateSinChannel(void *data, unsigned int frames) {
  uint8_t *buffer = (uint8_t *)data;

  if (!sinState.playing) return;

  for (unsigned int i = 0; i < frames; i++) {
    buffer[i] = (uint8_t)(256 * sinf(2 * PI * sinState.idx));
    sinState.idx += sinState.incr;
    if (sinState.idx > 1) sinState.idx = 0;
  }
}

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

void setChannelsVolume(Channels channels, float volume) {
  if (channels & CHANNEL_SIN) SetAudioStreamVolume(sinState.stream, volume);
  if (channels & CHANNEL_PULSE) SetAudioStreamVolume(pulseState.stream, volume);
}

void initChannels(void) {
  sinState.stream = LoadAudioStream(SAMPLE_RATE, 8, 1);
  pulseState.stream = LoadAudioStream(SAMPLE_RATE, 8, 1);

  toggleChannels(CHANNEL_PULSE, false);
  toggleChannels(CHANNEL_SIN, false);
  setChannelsFrequency(CHANNEL_PULSE | CHANNEL_SIN, 440);

  SetAudioStreamCallback(sinState.stream, updateSinChannel);
  SetAudioStreamCallback(pulseState.stream, updatePulseChannel);
  PlayAudioStream(sinState.stream);
  PlayAudioStream(pulseState.stream);
}

void deleteChannels() {
  UnloadAudioStream(sinState.stream);
  UnloadAudioStream(pulseState.stream);
}
