// Z'vonty Flugence (zaf17)

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dlfcn.h>

unsigned long (*compressBound)(unsigned long length);
int (*compress)(void *dest, unsigned long* destLen, const void* source, unsigned long sourceLen);
int (*uncompress)(void *dest, unsigned long* destLen, const void* source, unsigned long sourceLen);

unsigned long getFileSize(FILE *file) {
  unsigned long size = 0;
  fseek(file, 0, SEEK_END);
  size = ftell(file);
  fseek(file, 0, SEEK_SET);
  return size;
}

int main(int argc, char** argv) {
  if(argc < 3) {
    printf("Too few arguments!\n");
    return 1;
  }

  void* lib = dlopen("libz.so", RTLD_NOW);
  if(lib == NULL) {
    printf("Failed to load zlib\n");
    return 1;
  }

  FILE* file = fopen(argv[2], "rb");
  if(file == NULL) {
    printf("Failed to open file!\n");
    return 1;
  }

  if(strcmp(argv[1], "-c") == 0) {
    compressBound = dlsym(lib, "compressBound");
    compress = dlsym(lib, "compress");

    if(compressBound == NULL) {
      printf("Missing compression bound!\n");
      return 1;
    }

    if(compress == NULL) {
      printf("Missing compression algorithm!\n");
      return 1;
    }

    unsigned long len = getFileSize(file);
    char* inBuff = malloc(len+1);
    fread(inBuff, len, 1, file);
    unsigned long destLen = compressBound(len);
    char* outBuff = malloc(destLen);    
    unsigned long result = compress(outBuff, &destLen, inBuff, len);

    if(result < 0) {
      printf("Compression failed!\n");
      return 1;
    }
    fwrite(&len, sizeof(len), 1, stdout);
    fwrite(&destLen, sizeof(destLen), 1, stdout);
    fwrite(outBuff, destLen, 1, stdout);
  } else if(strcmp(argv[1], "-d") == 0) {
      uncompress = dlsym(lib, "uncompress");

      if(uncompress == NULL) {
        printf("Missing decompression algorithm!\n");
        return 1;
      }

      unsigned long len = 0;
      unsigned long compLen = 0;
      fread(&len, sizeof(len), 1, file);
      fread(&compLen, sizeof(compLen), 1, file);
      char* inBuff = malloc(compLen);
      char* outBuff = malloc(len);
      fread(inBuff, compLen, 1, file);
      unsigned long result = uncompress(outBuff, &len, inBuff, compLen);

      if(result < 0) {
        printf("Decompression failed!\n");
        return 1;
      }
      fwrite(outBuff, len, 1, stdout);
  } else {
    printf("Invalid operation!\n");
    return 1;
  }
  fclose(file);
  return 0;
}