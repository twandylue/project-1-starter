#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define INITIAL_CAPACITY 10

int main(int argc, char *argv[])
{
  if (argc < 2 || argc > 3) {
    fprintf(stderr, "usage: reverse <input> <output>\n");
    return 1;
  }

  FILE *output_file = stdout;
  if (argc == 3) {
    output_file = fopen(argv[2], "w");
    // if output file cannot be opened, return error with code 1
    if (output_file == NULL) {
      fprintf(stderr, "error: cannot open file '%s'\n", argv[2]);
      return 1;
    }
  }

  FILE *file = fopen(argv[1], "r");
  if (file == NULL) {
    fprintf(stderr, "error: cannot open file '%s'\n", argv[1]);
    return 1;
  }

  // if input file is the same as output file, return error with code 1
  // TODO:
  if (argc == 3 && strcmp(argv[1], argv[2]) == 0) {
    fprintf(stderr, "error: input and output file must differ\n");
    return 1;
  }

  int exit_code = 0;
  size_t len = 0;
  ssize_t read;
  size_t capacity = INITIAL_CAPACITY;
  size_t line_number = 0;
  // pointer to the line
  char *line = NULL;
  // allocate memory for the array of pointers to the lines
  char **lines = malloc(capacity * sizeof(char *));
  if (lines == NULL) {
    fprintf(stderr, "error: memory safety\n");
    return 1;
  }

  while ((read = getline(&line, &len, file)) != -1) {
    if (line == NULL) {
      fprintf(stderr, "error: memory safety\n");
      exit_code = 1;
      goto free_line;
    }

    // allocate more memory to the lines
    if (line_number == capacity) {
      char **temp = realloc(lines, capacity * 2 * sizeof(char *));
      if (temp == NULL) {
        fprintf(stderr, "error: memory safety\n");
        exit_code = 1;
        goto free_all;
      }
      lines = temp;
      capacity *= 2;
    }

    // TODO:
    lines[line_number] = strdup(line);
    if (lines[line_number] == NULL) {
      fprintf(stderr, "error: memory safety\n");
      exit_code = 1;
      goto free_all;
    }
    line_number++;
  }

  if (ferror(file)) {
    fprintf(stderr, "error: memory safety\n");
    exit_code = 1;
    goto free_all;
  }

  // reverse the lines
  for (size_t i = line_number; i > 0; --i) {
    fprintf(output_file, "%s", lines[i - 1]);
  }

free_all:
  free(line);
  for (size_t i = 0; i < line_number; ++i) {
    free(lines[i]);
  }
  free(lines);
  fclose(file);
  if (output_file != stdout) {
    fclose(output_file);
  }
free_line:
  return exit_code;
}
