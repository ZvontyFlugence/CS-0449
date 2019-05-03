// Z'vonty Flugence (zaf17)
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <ctype.h>

int get_dictionary_size(FILE* f) {
  char dict_size[20];
  return atoi(fgets(dict_size, sizeof(dict_size), f));
}

void get_line(char* input, FILE* f) {
  fgets(input, 20, f);
}

int random_range(int low_value, int high_value) {
  return(rand() % (high_value - low_value) + low_value);
}

void show_word_len(int len) {
  printf("Welcome to hangman! Your word has %d letters:\n", len);
}

void display_prompt(const char* guess, const char* word, int len) {
  for(int i = 0; i < len; i++) {
    printf("%c", guess[i]);
    printf(" ");
  }
  printf("Guess a letter or type the whole word: ");
}

void get_input(char* guess) {
  fgets(guess, 20, stdin);
}

void prep_guess_word(char* guess, char* word) {
  int len = strlen(word);
  for(int i = 0; i < len; i++) {
    guess[i] = '_';
  }
} 

int streq_nocase(const char* guess, const char* word) {
  for(; *guess && *word; guess++, word++) {
    if(tolower(*guess) != tolower(*word)) {
      return 0;
    }
  }
  return *guess == 0 && *word == 0;
}

void get_guess(char* guess, int size, int word_size) {
  fgets(guess, size, stdin);
  guess[word_size] = '\0';
}

// TODO: Finish method, then test, then done
int charcmp(const char* input, const char* word, char* guess) {
  int correct_int = 0;
  int i = 0;
  for(; *word; word++, i++) {
    if(*input == *word) {
      guess[i] = *input;
      correct_int = 1;
    }
  }
  guess[i] = '\0';
  return correct_int;
}

int main(int argc, char* argv[]) {
  // generate seed for random number generator
  srand((unsigned int)time(NULL));

  // game logic
  char* word;
  char guess[20];
  int num_strikes = 0;
  int game_over = 0;

  if(argc == 2) {
    word = argv[1];
  } else {
    int index = random_range(1, 10);

    // read dictionary
    FILE* f;
    f = fopen("dictionary.txt", "r");

    if(f == NULL) {
      printf("Failed to open dictionary");
      return 1;
    }

    int size = get_dictionary_size(f);
    char words[size][20];

    for(int i = 0; i < size; i++) {
      get_line(words[i], f);
    }
    word = words[index];
    word[strlen(word)-1] = '\0';
  }
  
  int word_len = strlen(word);
  show_word_len(word_len);
  prep_guess_word(guess, word);

  while(!game_over && num_strikes < 5) {
    char input[20];
    display_prompt(guess, word, word_len);  
    get_guess(input, 20, word_len);
    int input_len = strlen(input)-1;
    int result = 0;
    if(input_len == 1) {
      int sub_result = charcmp(input, word, guess);
      if(sub_result == 0) {
        num_strikes++;
        printf("Strike %d!\n", num_strikes);
      }
      result = streq_nocase(guess, word);
    } else {
      result = streq_nocase(input, word);
      if(result == 0) {
        num_strikes++;
        printf("Strike %d!\n", num_strikes);
      }
    }
    if(result == 1) {
      printf("Congrats! You Won!\n");
      game_over = 1;
    }
  }

  return 0;
}