#include "player.h"

#include <assert.h>
#include <errno.h>
#include <raylib.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "music_theory.h"
#include "song.h"

void appendSongUnitArray(SongUnitArray *array, SongUnit unit) {
  if (array->capacity == array->size) {
    array->units =
        realloc(array->units, array->capacity * 2 * sizeof(SongUnit));
    assert(array->units != NULL && "Buy more RAM.");
    array->capacity *= 2;
  }

  array->units[array->size] = unit;
  array->size++;
}

void initSongUnitArray(SongUnitArray *array) {
  array->units = calloc(SONG_ARRAY_SIZE, sizeof(SongUnit));
  assert(array->units != NULL && "Buy more RAM.");
  array->capacity = SONG_ARRAY_SIZE;
  array->size = 0;
  array->iterator = 0;
}

void deleteSongUnitArray(SongUnitArray *array) { free(array->units); }

// Returns -1 in failure.
int parseSongName(Player *player, char **lines, int lineNum) {
  for (int i = 0; i < lineNum; i++) {
    int len = strlen(lines[i]);
    if (len > 0) {
      player->songName = calloc(len + 1, sizeof(char));
      strcpy(player->songName, lines[i]);
      return i + 1;
    }
  }
  return -1;
}

// Same.
int parseSongInfo(Player *player, char **lines, int lineNum, int i) {
  for (; i < lineNum; i++) {
    int len = strlen(lines[i]);
    if (len > 0) {
      if (sscanf(lines[i], "%f %d %f", &player->tune, &player->key,
                 &player->align) != 3) {
        printf("Malformed file: info line does not parse correctly.\n");
        return -1;
      }
      return i + 1;
    }
  }
  return -1;
}

// Same.
int parseSongScale(Player *player, char **lines, int lineNum, int i) {
  for (; i < lineNum; i++) {
    int len = strlen(lines[i]);
    if (len > 0) {
      char *scaleSizeStr = strtok(lines[i], " ");
      player->scale.size = atoi(scaleSizeStr);
      if (player->scale.size == 0) {
        printf("Malformed file: forbidden scale size at line %d: %s", i,
               scaleSizeStr);
        return -1;
      }

      player->scale.semitoneIndexes = calloc(player->scale.size, sizeof(int));
      for (int j = 0; j < player->scale.size; j++) {
        char *tok = strtok(NULL, " ");
        if (tok == NULL) {
          printf("Malformed file: not enough scale values (line %d).\n", i);
          return -1;
        }

        player->scale.semitoneIndexes[j] = atoi(tok);
      }
      return i + 1;
    }
  }

  return -1;
}

// Same.
int parseSongLines(Player *player, char **lines, int lineNum, int i) {
  Channels channel = CHANNEL_PULSE;

  // Add a single pause in the beggining to compensate the first frames.
  SongUnit spareUnit = (SongUnit){1 * player->align, 0, 0, 0};
  appendSongUnitArray(&player->pulseNotes, spareUnit);
  appendSongUnitArray(&player->sinNotes, spareUnit);

  int octave = 4;
  float volume = 1;
  for (; i < lineNum; i++) {
    if (sscanf(lines[i], "pulse %d %f", &octave, &volume) == 2) {
      channel = CHANNEL_PULSE;
    } else if (sscanf(lines[i], "sin %d %f", &octave, &volume) == 2) {
      channel = CHANNEL_SIN;

    } else if (strlen(lines[i]) > 0) {
      float time;
      char *timestr = strtok(lines[i], " ");
      time = atof(timestr);

      char *tok = strtok(NULL, " ");
      while (tok != NULL) {
        SongUnit unit = (SongUnit){time * player->align, atoi(tok), octave, volume};
        if (unit.note > player->scale.size) {
          printf("Note bigger than scale at line %d: %d\n", i, unit.note);
          return (errno = 1);
        }

        switch (channel) {
          case CHANNEL_PULSE:
            appendSongUnitArray(&player->pulseNotes, unit);
            break;
          case CHANNEL_SIN:
            appendSongUnitArray(&player->sinNotes, unit);
            break;
        }

        tok = strtok(NULL, " ");
      }
    }
  }

  return i + 1;
}

int parse(Player *player, char **lines, int lineNum) {
  if (lineNum < 4) {
    printf("Malformed file: less than 4 lines\n");
    return (errno = 1);
  }

  int i = 0;
  if ((i = parseSongName(player, lines, lineNum)) == -1) return (errno = 1);
  if ((i = parseSongInfo(player, lines, lineNum, i)) == -1) return (errno = 1);
  if ((i = parseSongScale(player, lines, lineNum, i)) == -1) return (errno = 1);
  initSongUnitArray(&player->pulseNotes);
  initSongUnitArray(&player->sinNotes);
  parseSongLines(player, lines, lineNum, i);

  return 0;
}

int initPlayer(Player *player, const char *file) {
  FILE *fp = fopen(file, "r");
  if (fp == NULL) return errno;

  fseek(fp, 0, SEEK_END);
  long size = ftell(fp);
  rewind(fp);

  char *buffer = calloc(size, 1);
  if (buffer == NULL) {
    fclose(fp);
    return errno;
  }

  fread(buffer, size, size, fp);
  if (buffer[size - 1] != '\n') {
    printf("Error with file format: does not end with an empty new line.\n");
    fclose(fp);
    return (errno = 1);
  }

  int lineNum = 0;
  for (int i = 0; i < size; i++) {
    if (buffer[i] == '\n') lineNum++;
  }

  char **lines = calloc(lineNum, 8);
  int lineIdx = 0;
  char *last = &(buffer[0]);
  for (int i = 0; i < size; i++) {
    if (buffer[i] == '\n') {
      buffer[i] = '\0';
      lines[lineIdx] = last;
      last = &(buffer[i + 1]);
      lineIdx++;
    }
  }

  parse(player, lines, lineNum);
  free(buffer);

  fclose(fp);
  return errno;
}

void deletePlayer(Player *player) {
  free(player->songName);
  free(player->scale.semitoneIndexes);
  deleteSongUnitArray(&player->pulseNotes);
  deleteSongUnitArray(&player->sinNotes);
}

void updateChannelWithArray(Player *player, SongUnitArray *array,
                            Channels channel, float deltaTime) {
  array->units[array->iterator].time -= deltaTime;
  if (array->units[array->iterator].time < 0) {
    array->units[array->iterator + 1].time +=
        array->units[array->iterator].time;
    array->iterator++;
  }

  SongUnit unit = array->units[array->iterator];
  toggleChannels(channel, unit.note != 0);

  float frequency = num2frequency(player->tune, player->key, unit.octave,
                                  unit.note, player->scale);
  setChannelsFrequency(channel, frequency);
  setChannelsVolume(channel, unit.volume);
}

bool update(Player *player) {
  if (player->pulseNotes.iterator == player->pulseNotes.size &&
      player->sinNotes.iterator == player->sinNotes.size)
    return false;

  float deltaTime = GetFrameTime();
  if (player->pulseNotes.iterator < player->pulseNotes.size)
    updateChannelWithArray(player, &player->pulseNotes, CHANNEL_PULSE,
                           deltaTime);
  if (player->sinNotes.iterator < player->sinNotes.size)
    updateChannelWithArray(player, &player->sinNotes, CHANNEL_SIN, deltaTime);

  BeginDrawing();
  ClearBackground(BLACK);

  DrawFPS(0, 0);
  DrawText(player->songName, 0, 30, 30, WHITE);
  DrawText(TextFormat("Tune: %f", player->tune), 0, 60, 30, WHITE);
  DrawText(TextFormat("Key: %d", player->key), 0, 90, 30, WHITE);
  DrawText(TextFormat("Alignment: %f", player->align), 0, 150, 30, WHITE);

  EndDrawing();

  return true;
}
