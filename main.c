#include <raylib.h>
#include <stdbool.h>
#include <stdio.h>
#include <errno.h>

#include "song.h"
#include "music_theory.h"
#include "player.h"

int main(int argc, char *argv[]) {
  if (argc < 2) {
    printf("Provide a file via args:\n  $ %s <file>\n", argv[0]);
    return 1;
  }

  Player player;
  if (initPlayer(&player, argv[1])) {
    perror("Error reading file: ");
    return errno;
  }

  InitWindow(1200, 800, "Senspecimenprogramaro de Ondformo Generatori");
  InitAudioDevice();
  // SetTargetFPS(60);

  AudioStream sinChannel = LoadAudioStream(SAMPLE_RATE, 8, 1);
  AudioStream pulseChannel = LoadAudioStream(SAMPLE_RATE, 8, 1);

  toggleChannels(CHANNEL_PULSE, false);
  toggleChannels(CHANNEL_SIN, false);
  setChannelsFrequency(CHANNEL_PULSE | CHANNEL_SIN, 440);

  SetAudioStreamCallback(sinChannel, updateSinChannel);
  SetAudioStreamCallback(pulseChannel, updatePulseChannel);
  PlayAudioStream(sinChannel);
  PlayAudioStream(pulseChannel);

  while (!WindowShouldClose()) {
    if (!update(&player)) break;
  }

  UnloadAudioStream(sinChannel);
  UnloadAudioStream(pulseChannel);
  CloseAudioDevice();
  CloseWindow();

  deletePlayer(&player);

  return 0;
}
