#include <stdio.h>
#include <stdlib.h>
#include "portaudio.h"
#include "tdaudio.h"

static void mix (GameAudioData *data, float *signal) {
	signal[0] = 0; signal[1] = 0; 
	int count = 0, play = 0, bytelenght;
	float fadeinvol = 1, fadeoutvol = 1;
	float fadeintime, fadeouttime; // in seconds
	for (int i=0; i<256; i++) {
		play = 0; 
		if (data->clip[i] != NULL) {
			fadeinvol = 1; fadeoutvol = 1; 
			bytelenght = data->clip[i]->sample->samplenum
				*data->clip[i]->sample->channelnum;
			fadeintime = data->clip[i]->fadein/1000.0f;
			fadeouttime = data->clip[i]->fadeout/1000.0f;
			if (data->clip[i]->current < bytelenght)
			{
				play = 1;
			} else { // looping
				if (data->clip[i]->loop == -1) {
					data->clip[i]->current = 0;
					play = 1;
				}
			}
		}
		if (play) {
			if (data->clip[i]->current < fadeintime*data->clip[i]->sample->byte_rate) 
			{
				fadeinvol = 1-(fadeintime - ((float)data->clip[i]->current
					/data->clip[i]->sample->byte_rate))
					/fadeintime;
			}
			if (bytelenght-data->clip[i]->current < fadeouttime*data->clip[i]->sample->byte_rate) 
			{
				fadeoutvol = 1-(fadeouttime - ((float)(data->clip[i]->current-bytelenght)
					/data->clip[i]->sample->byte_rate))
					/fadeouttime;
			}
			signal[0] += data->clip[i]->sample->samples[data->clip[i]->current]
				*data->clip[i]->volume*fadeinvol*fadeoutvol;
			signal[1] += data->clip[i]->sample->samples[data->clip[i]->current+1]
				*data->clip[i]->volume*fadeinvol*fadeoutvol;
			data->clip[i]->current += 2;
			count += 1;
		}
	}
}

static int gameaudio_callback( const void *inputBuffer, void *outputBuffer,
    unsigned long framesPerBuffer,
    const PaStreamCallbackTimeInfo* timeInfo,
    PaStreamCallbackFlags statusFlags,
    void *userData )
{
    /* Cast data passed through stream to our structure. */
    GameAudioData *data = (GameAudioData*)userData; 
    float *out = (float*)outputBuffer;
    unsigned int i;
    (void) inputBuffer; /* Prevent unused variable warning. */
    
	float signal[2] = { 0, 0 };
    for( i=0; i<framesPerBuffer; i++ ) {
		mix(data, signal);
		*out++ = signal[0]*data->volume;  /* left */
		*out++ = signal[1]*data->volume;  /* right */
	}
    return 0;
}

void gameaudio_init(GameAudioData *data) {
	data->volume = 0;
	for (int i=0; i<256; i++) data->clip[i] = NULL;

	// portaudio
	// ---------
	PaStream *stream;
    PaError err = paNoError;
    PaStreamParameters outputParameters;    
	
    err = Pa_Initialize();
    if( err != paNoError ) 
		printf("Error: No default output device.\n");
    // printf("PortAudio Test: output device = %d\n", OUTPUT_DEVICE );

    outputParameters.device = OUTPUT_DEVICE;
    if (outputParameters.device == paNoDevice) {
		printf("Error: No default output device.\n");
    }
    outputParameters.channelCount = 2;         /* stereo output */
    outputParameters.sampleFormat = paFloat32; /* 32 bit floating point output */
    outputParameters.suggestedLatency = Pa_GetDeviceInfo( outputParameters.device )->defaultLowOutputLatency;
    outputParameters.hostApiSpecificStreamInfo = NULL;

    /* Open an audio I/O stream. */
    err = Pa_OpenStream(
        &stream,
        NULL, 
        &outputParameters,
        SAMPLE_RATE,
        FRAMES_PER_BUFFER,
        paNoFlag,
        gameaudio_callback,
        data );
	if( err != paNoError ) printf( "PortAudio open error: %s\n", Pa_GetErrorText( err ) );

	err = Pa_StartStream( stream );
	if( err != paNoError ) printf( "PortAudio start error: %s\n", Pa_GetErrorText( err ) );
}

void gameaudio_quit() {
	// portaudio
	int err = Pa_Terminate();
	if( err != paNoError )
	   printf(  "PortAudio error: %s\n", Pa_GetErrorText( err ) );

}

int gameaudio_add(GameAudioData *data, GameAudioSamples *sam, int loop) {
	int cl = -1;
	for (int i=0; i<256; i++) {
		if (data->clip[i] == NULL) {
			printf("clip loaded in %d\n", i);
			data->clip[i] = malloc(sizeof(GameAudioClip));
			data->clip[i]->sample = sam;
			data->clip[i]->current = 0;
			data->clip[i]->loop = loop;
			data->clip[i]->volume = 1.0f;
			data->clip[i]->fadein = 3.0f;
			data->clip[i]->fadeout = 3.0f;
			cl = i;
			break;
		}
	}
	return cl;
}

void gameaudio_play(GameAudioData *data, int clipnum) {
	data->clip[clipnum]->current = 0;
	data->clip[clipnum]->volume = 1;
}

void gameaudio_stop(GameAudioData *data, int clipnum) {
	data->clip[clipnum]->volume = 0;
}