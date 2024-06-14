#ifndef SONG_H_
#define SONG_H_

#include <stdbool.h>

#include "music_theory.h"

/*
 * Channel definitions.
 */

#define SAMPLE_RATE 44100
#define BUFFER_SIZE 4096

typedef enum {
  CHANNEL_SIN = 1 << 0,
  CHANNEL_PULSE = 1 << 1,
} Channels;

void updateSinChannel(void *data, unsigned int frames);
void updatePulseChannel(void *data, unsigned int frames);

void toggleChannels(Channels channels, bool play);
void setChannelsFrequency(Channels channels, float frequency);

#endif  // SONG_H_
