#ifndef gameaudioh
#define gameaudioh

#include "portaudio.h"

typedef struct {
	float *samples;
	int samplenum;
	int channelnum;
	int byte_rate;
} GameAudioSamples;

typedef struct {
	GameAudioSamples *sample;
	int current;
	int loop;
	float volume;
	float fadein; // ms
	float fadeout; // ms
} GameAudioClip;

typedef struct {
	float volume;
	GameAudioClip *clip[256];
	float signal;
} GameAudioData;

#define SAMPLE_RATE (44100)
#define FRAMES_PER_BUFFER 256
#define OUTPUT_DEVICE (Pa_GetDefaultOutputDevice())

void gameaudio_init(GameAudioData *data);
void gameaudio_quit();

int gameaudio_add(GameAudioData *data, GameAudioSamples *sam, int loop);
void gameaudio_play(GameAudioData *data, int clipnum);
void gameaudio_stop(GameAudioData *data, int clipnum);

#endif