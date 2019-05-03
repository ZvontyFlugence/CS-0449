// Z'vonty Flugence (zaf17)

#include <stdio.h>
#include <string.h>
#include <ctype.h>

void get_line(char* buffer, int size) {
  fgets(buffer, size, stdin);
  int len = strlen(buffer);
  // this is a little more robust than what we saw in class.
  if(len != 0 && buffer[len-1] == '\n')
    buffer[len-1] = '\0';
}

// returns 1 if the 2 strings are equal, and 0 otherwise.
int streq(const char* a, const char* b) {
  return strcmp(a, b) == 0;
}

// returns 1 if the 2 strings are equal, ignoring case, and 0 otherwise.
// so "earth" and "Earth" and "EARTH" will all be equal.
int streq_nocase(const char* a, const char* b) {
  // hoohoho aren't I clever
  for(; *a && *b; a++, b++) if(tolower(*a) != tolower(*b)) return 0;
  return *a == 0 && *b == 0;
}

float weight_on_planet(const char* planet_name, int user_weight) {
  if(streq_nocase(planet_name, "mercury")) {
    return user_weight * 0.38;
  } else if(streq_nocase(planet_name, "venus")) {
    return user_weight * 0.91;
  } else if(streq_nocase(planet_name, "mars")) {
    return user_weight * 0.38;
  } else if(streq_nocase(planet_name, "jupiter")) {
    return user_weight * 2.54;
  } else if(streq_nocase(planet_name, "saturn")) {
    return user_weight * 1.08;
  } else if(streq_nocase(planet_name, "uranus")) {
    return user_weight * 0.91;
  } else if(streq_nocase(planet_name, "neptune")) {
    return user_weight * 1.19;
  } else {
    return -1;
  }
}

int main() {
  printf("Uh, how much do you weigh? ");
  char input[100];
  get_line(input, sizeof(input)); // notice the sizeof!
  int weight;
  sscanf(input, "%d", &weight); // DON'T FORGET THE & or it'll crash.
  while(1) {
    printf("What planet do you wanna go to ('exit' to exit)? ");
    get_line(input, sizeof(input));
    if(streq_nocase(input, "earth")) {
      printf("uh, you're already there, buddy\n");
    } else if(streq_nocase(input, "exit")) {
      break;
    } else {
      float scaled_weight = weight_on_planet(input, weight);
      if(scaled_weight < 0) {
        printf("That's not a planet.\n");
      } else {
        printf("You'd weigh %.2f there.\n", scaled_weight);
      }
    }
  }
}