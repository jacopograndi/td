#define _CRT_SECURE_NO_WARNINGS
#include <stdlib.h>
#include <stdio.h>
#include <direct.h>

#include "gameaudio.h"
#include "wavelib.h"

int bytes_to_int_big_endian (char c[]) {
	return (c[3] & 0x000000FF) | ((c[2] << 8) & 0x0000FF00) 
		| ((c[1] << 16) & 0x00FF0000) | ((c[0] << 24) & 0xFF000000);
}

int bytes_to_int_little_endian (char c[]) {
	return (c[0] & 0x000000FF) | ((c[1] << 8) & 0x0000FF00) 
		| ((c[2] << 16) & 0x00FF0000) | ((c[3] << 24) & 0xFF000000);
}

GameAudioSamples* wave_read (char filename[]) {
	WavfileHeader header;

	// construct cwd full path
	char *buffer = _getcwd( NULL, 0 );
	char fullpath[128]; int i;
	for (i=0; buffer[i] != '\0'; fullpath[i] = buffer[i], i++);
	fullpath[i] = '\\'; i++;
	for (int j=0; filename[j] != '\0'; fullpath[i] = filename[j], i++, j++);
	fullpath[i] = '\0';

	FILE *file = fopen(fullpath, "rb");
	if (file == NULL) { printf("wrong filename\n"); return; }

	// parsing header then data blocks
	int bytecount = 0; char databytes[4];
	char block[sizeof(WavfileHeader)]; int blockindex=0; 
	int dataflag = 0; int dataindex = 0;
	long num_samples, size_of_each_sample;
	fread(block, sizeof(WavfileHeader), 1, file);
	header.riff_tag[0] = block[0];
	header.riff_tag[1] = block[1];
	header.riff_tag[2] = block[2];
	header.riff_tag[3] = block[3];
	header.riff_length = bytes_to_int_little_endian(block+4);
	header.wave_tag[0] = block[8];
	header.wave_tag[1] = block[9];
	header.wave_tag[2] = block[10];
	header.wave_tag[3] = block[11];
	header.fmt_tag[0] = block[12];
	header.fmt_tag[1] = block[13];
	header.fmt_tag[2] = block[14];
	header.fmt_tag[3] = block[15];
	header.fmt_length = bytes_to_int_little_endian(block+16);
	header.audio_format = block[20] | (block[21]<<8);
	header.num_channels = block[22] | (block[23]<<8);
	header.sample_rate = bytes_to_int_little_endian(block+24);
	header.byte_rate = bytes_to_int_little_endian(block+28);
	header.block_align = block[32] | (block[33]<<8);
	header.bits_per_sample = block[34] | (block[35]<<8);
	header.data_tag[0] = block[36];
	header.data_tag[1] = block[37];
	header.data_tag[2] = block[38];
	header.data_tag[3] = block[39];
	header.data_length = bytes_to_int_little_endian(block+40);;
	
	//printf("Sample Rate:%#02x %#02x %#02x %#02x \n", block[24], block[25], block[26], block[27]);
	//printf("Sample Rate:%d \n", header.sample_rate);
	//printf("Number of channels:%u \n", header.num_channels);
	//printf("Bits per sample:%u \n", header.bits_per_sample);
	//printf("Data lenght:%d \n", header.data_length);
				
	num_samples = (8 * header.data_length) / (header.num_channels * header.bits_per_sample);
	//printf("Number of samples:%lu \n", num_samples);
	size_of_each_sample = (header.num_channels * header.bits_per_sample) / 8;
	//printf("Size of each sample:%ld bytes\n", size_of_each_sample);
	//printf("Riff lenght: %d\n", header.riff_length);
	//printf("Byte rate: %d\n", header.byte_rate);
	float duration_in_seconds = (float) header.riff_length / header.byte_rate;	
	//printf("Approx.Duration in seconds=%f\n", duration_in_seconds);	
	//printf("Audio format: %d\n", header.audio_format);
	
	char *datablock = malloc(1024*1024*24);
	
	fread(datablock, sizeof(char), 4, file); // ignore 4
	
	int numread = fread(datablock, sizeof(char), header.data_length, file);
	//printf("read %d\n", numread);
	/*
	for (int i=0; i<header.data_length; i+=1) {	
		datablock[i] = fgetc(file);
		printf("%#02x \n", datablock[is]);
	}*/
	GameAudioSamples *gas = malloc(sizeof(GameAudioSamples));
	gas->samplenum = num_samples;
	gas->channelnum = header.num_channels;
	gas->byte_rate = header.byte_rate;
	gas->samples = malloc(sizeof(float)*num_samples*header.num_channels);
	for (int i=0; i<num_samples*header.num_channels; i+=2) {
		int data0 = (datablock[i*2] & 0x000000FF) | ((datablock[i*2+1] << 8) & 0x0000FF00);
		int data1 = (datablock[i*2+2] & 0x000000FF) | ((datablock[i*2+3] << 8) & 0x0000FF00);
		if (data0 >= 256*128-1) data0-=256*256;
		if (data1 >= 256*128-1) data1-=256*256;
		gas->samples[i] = ((float)data0/(255*255));
		gas->samples[i+1] = ((float)data1/(255*255));
		//printf("%f, %f, %d, %#02x, %#02x, %#02x, %#02x\n", clip[i], clip[i+1], (datablock[i*2] & 0x000000FF) | ((datablock[i*2+1] << 8) & 0x0000FF00), datablock[i*2], datablock[i*2+1], datablock[i*2+2], datablock[i*2+3]);
	}
    fclose(file);
	return gas;
} 