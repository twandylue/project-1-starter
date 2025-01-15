#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/*
* Node structure in the doubly linked list
* val: the string value of the node
* next: the next node in the linked list
* prev: the previous node in the linked list
*/
struct Node {
  char *val;
  struct Node *next;
  struct Node *prev;
};

/*
* Create a new node, and return the new node.
* If the line is NULL, the default value is "default".
* If the memory allocation fails, return NULL.
* If the memory allocation for the string fails, free the string and the node and return NULL.
*/
static struct Node *Node(const char *line) {
  struct Node *node = malloc(sizeof(struct Node));
  if (node == NULL) {
    fprintf(stderr, "error: malloc failed in creating Node\n");
    return NULL;
  }
  if (line == NULL) {
    line = "default";
  }
  node->val = strdup(line);
  if (node->val == NULL) {
    fprintf(stderr, "error: memory safety in duplicating string\n");
    free(node);
    return NULL;
  }
  node->next = NULL;
  node->prev = NULL;
  return node;
}

/*
* List structure in the doubly linked list
* head: the head node of the linked list
* tail: the tail node of the linked list
*/
struct Lines {
  struct Node *head;
  struct Node *tail;
};

/*
* Create a new linked list, and return the linked list.
* If the memory allocation fails, return NULL.
* If the memory allocation for the head or tail fails, free the linked list and return NULL.
*/
static struct Lines *Lines(void) {
  struct Lines *lines = malloc(sizeof(struct Lines));
  if (lines == NULL) {
    fprintf(stderr, "error: malloc failed in creating Lines\n");
    return NULL;
  }
  lines->head = Node("head");
  lines->tail = Node("tail");
  if (lines->head == NULL || lines->tail == NULL) {
    free(lines);
    return NULL;
  }
  lines->head->next = lines->tail;
  lines->tail->prev = lines->head;
  return lines;
}

/*
* Insert a new node at the end of the linked list, and return the new node.
* If the memory allocation for the new node fails, free the new node and return NULL.
*/
static struct Node *insert_lines_end(struct Lines* lines, const char *line) {
  struct Node *node = Node(line);
  if (node == NULL) {
    return NULL;
  }
  struct Node *tmp = lines->tail->prev;
  tmp->next = node;
  node->prev = tmp;
  lines->tail->prev = node;
  node->next = lines->tail;
  return node;
}

/*
* Print the lines in the linked list in reverse order.
*/
static void print_lines_reverse(struct Lines *lines, FILE *output_file) {
  struct Node *node = lines->tail->prev;
  while (node != lines->head) {
    fprintf(output_file, "%s", node->val);
    node = node->prev;
  }
}

/*
* Free the linked list.
*/
static void free_lines(struct Lines *lines) {
  struct Node *node = lines->head;
  while (node != NULL) {
    struct Node *tmp = node;
    node = node->next;
    free(tmp->val);
    free(tmp);
  }
  free(lines);
}

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
  char *line = NULL;
  struct Lines *lines = Lines();
  if (lines == NULL) {
    goto free_default;
  }

  while ((read = getline(&line, &len, file)) != -1) {
    if (line == NULL) {
      fprintf(stderr, "error: memory safety in reading line\n");
      exit_code = 1;
      goto free_lines;
    }
    struct Node *node = insert_lines_end(lines, line);
    if (node == NULL) {
      fprintf(stderr, "error: memory safety in inserting new node\n");
      exit_code = 1;
      goto free_lines;
    }
  }

  if (ferror(file)) {
    fprintf(stderr, "error: memory safety in reading file\n");
    exit_code = 1;
    goto free_lines;
  }

  print_lines_reverse(lines, output_file);

free_lines:
  free_lines(lines);
free_default:
  free(line);
  fclose(file);
  if (output_file != stdout) {
    fclose(output_file);
  }
  return exit_code;
}
