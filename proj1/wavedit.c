// Z'vonty Flugence (zaf17)

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>

typedef struct {
  char riff_id[4];
  uint32_t file_size;
  char wave_id[4];
  char fmt_id[4];
  uint32_t fmt_size;
  uint16_t data_format;
  uint16_t number_of_channels;
  uint32_t samples_per_second;
  uint32_t bytes_per_second;
  uint16_t block_alignment;
  uint16_t bits_per_sample;
  char data_id[4];
  uint32_t data_size;
} WAVHeader;

void display_help() {
  printf("Usage: ./wavedit [FILE] [OPTION]\n");
  printf("See WAV file info, change sample rate, and reverse audio.\n\n");
  printf("-rate [RATE]   Change sample rate to given rate\n");
  printf("-reverse       Reverses file audio\n\n");
  printf("With no FILE, display help message.\n\n");
  printf("Examples:\n");
  printf("./wavedit                    Display help message.\n");
  printf("./wavedit f.wav              Display f's file info.\n");
  printf("./wavedit f.wav -rate 449    Change f's sample rate.\n");
  printf("./wavedit f.wav -reverse     Reverse f's audio.\n");
}

FILE* get_file(const char* file_name, const char* mode) {
  return fopen(file_name, mode);
}

void read_file(WAVHeader* buffer, FILE* file) {
  fread(buffer, sizeof(*buffer), 1, file);
}

int check_fmt(const WAVHeader* file) {
  if(strncmp(file->riff_id, "RIFF", 4) != 0)
    return 0;
  if(strncmp(file->wave_id, "WAVE", 4) != 0)
    return 0;
  if(strncmp(file->fmt_id, "fmt ", 4) != 0)
    return 0;
  if(strncmp(file->data_id, "data", 4) != 0)
    return 0;
  if(file->fmt_size != 16)
    return 0;
  if(file->data_format != 1)
    return 0;
  if(file->number_of_channels != 1 && file->number_of_channels != 2)
    return 0;
  if(file->samples_per_second > 192000 && file->samples_per_second <= 0)
    return 0;
  if(file->bits_per_sample != 8 && file->bits_per_sample != 16)
    return 0;
  if(file->bytes_per_second != file->samples_per_second * (file->bits_per_sample)/8 * file->number_of_channels)
    return 0;
  if(file->block_alignment != (file->bits_per_sample)/8 * file->number_of_channels)
    return 0;
  return 1;
}

void write_file(WAVHeader* buffer, FILE* file) {
  fwrite(buffer, sizeof(*buffer), 1, file);
}

int main(int argc, char** argv) {
  if(argc == 1) {
    display_help();
    exit(0);
  } else if(argc == 2) {
    FILE* f = get_file(argv[1], "rb");
    if(f == NULL) {
      printf("File %s doesn't exist!", argv[1]);
      exit(0);
    }
    WAVHeader file;
    read_file(&file, f);
    fclose(f);

    int is_valid = check_fmt(&file);
    if(is_valid == 0) {
      printf("This is an invalid WAV file!\n");
      exit(0);
    }

    // Display Information
    printf("This is a %d-bit %dHz ", file.bits_per_sample, file.samples_per_second);
    if(file.number_of_channels == 1) {
      printf("mono");
    } else {
      printf("stero");
    }
    int sample_len = file.data_size / file.block_alignment;
    printf(" sound.\n It is %d samples (%.3f seconds) long.\n", (sample_len), ((float)sample_len/file.samples_per_second));
  } else if(argc == 4 && strcmp(argv[2], "-rate") == 0) {
    int rate = atoi(argv[3]);
    if(rate <= 0 || rate > 192000) {
      printf("Invalid rate!\n");
      exit(0);
    }
    FILE* f = get_file(argv[1], "rb+");
    WAVHeader file;
    read_file(&file, f);

    int is_valid = check_fmt(&file);
    if(is_valid == 0) {
      printf("This is an invalid WAV file!\n");
      exit(0);
    }
    file.samples_per_second = rate;
    file.bytes_per_second = file.samples_per_second * (file.bits_per_sample)/8 * file.number_of_channels;
    fseek(f, 0, SEEK_SET);
    write_file(&file, f);
    fclose(f);
  } else if(argc == 3 && strcmp(argv[2], "-reverse") == 0) {
    FILE* f = get_file(argv[1], "rb+");
    WAVHeader file;
    read_file(&file, f);
    
    int is_valid = check_fmt(&file);
    if(is_valid == 0) {
     printf("This is an invalid WAV file!\n");
      exit(0);
    }

    int sample_len = file.data_size / file.block_alignment;
    int bits = file.bits_per_sample;
    int channels = file.number_of_channels;

    if(bits == 8 && channels == 1) {
      uint8_t samples[sample_len];
      fread(&samples, sizeof(samples), 1, f);
      for(int start = 0, end = sample_len-1; start < end; start++, end--) {
        uint8_t temp = samples[start];
        samples[start] = samples[end];
        samples[end] = temp;
      }
      fseek(f, sizeof(WAVHeader), SEEK_SET);
      fwrite(&samples, sizeof(samples), 1, f);
     } else if((bits == 8 && channels == 2) || (bits == 16 && channels == 1)) {
      uint16_t samples[sample_len];
      fread(&samples, sizeof(samples), 1, f);
      for(int start = 0, end = sample_len-1; start < end; start++, end--) {
        uint16_t temp = samples[start];
        samples[start] = samples[end];
        samples[end] = temp;
      }
      fseek(f, sizeof(WAVHeader), SEEK_SET);
      fwrite(&samples, sizeof(samples), 1, f);
    } else {
      uint32_t samples[sample_len];
      fread(&samples, sizeof(samples), 1, f);
      for(int start = 0, end = sample_len-1; start < end; start++, end--) {
        uint32_t temp = samples[start];
        samples[start] = samples[end];
        samples[end] = temp;
      }
      fseek(f, sizeof(WAVHeader), SEEK_SET);
      fwrite(&samples, sizeof(samples), 1, f);
    }
    fclose(f);
  } else {
    display_help();
  }

  return 0;
}