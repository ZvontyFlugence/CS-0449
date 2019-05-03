// Z'vonty Flugence (zaf17)

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main(int argc, char** argv) {
  if(argc != 2) {
    printf("Invalid args");
    return 1;
  }
  FILE* file = fopen(argv[1], "rb");

  if(file == NULL) {
    printf("Couldnt find file");
    return 1;
  }

  while(!feof(file)) {
    char curr;
    long currPos = ftell(file);
    long strPos = 0;
    int count = 0;
    fread(&curr, 1, 1, file);

    while(curr >= 32 && curr <= 126) {
      strPos = currPos;
      count++;
      fread(&curr, 1, 1, file);
    }

    if(count >= 4) {
      char buffer[count+1];
      fseek(file, strPos, SEEK_SET);
      fread(&buffer, count, 1, file);
      buffer[count] = '\0';
      printf("%s\n", buffer);
    }
    count = 0;
  }
  fclose(file);
  return 0;
}