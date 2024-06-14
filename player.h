#ifndef SONG_PLAYER_H_
#define SONG_PLAYER_H_

#include <stdbool.h>

#include "music_theory.h"

typedef struct {
  float time;
  /* 0 means pause. */
  int note;
} SongUnit;

typedef struct {
  SongUnit *units;
  int size;
  int capacity;
  /* Please don't change the array when you begin to use this. And set it to 0
   * after. */
  int iterator;
} SongUnitArray;

#define SONG_ARRAY_SIZE (4096 / sizeof(SongUnitArray))

typedef struct {
  float tune;
  Note key;
  int octave;
  float align;
	Scale scale;
  SongUnitArray sinNotes;
  SongUnitArray pulseNotes;
  char *songName;
} Player;

/*
 * Returns and sets errno in failure, 0 otherwise.
 */
int initPlayer(Player *player, const char *file);
void deletePlayer(Player *player);
/*
 * Returns false when done.
 */
bool update(Player *player);

#endif  // SONG_PLAYER_H_
