#ifndef wavelibh
#define wavelibh

typedef struct wavfileHeader {
	char riff_tag[4];
	unsigned int riff_length;
	char wave_tag[4];
	char fmt_tag[4];
	unsigned int fmt_length;
	unsigned short audio_format;
	unsigned short num_channels;
	unsigned int sample_rate;
	unsigned int byte_rate;
	unsigned short block_align;
	unsigned short bits_per_sample;
	char data_tag[4];
	unsigned int data_length;
} WavfileHeader;

GameAudioSamples* wave_read (char filename[]);

#endif