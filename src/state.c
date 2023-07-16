#include "state.h"

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "snake_utils.h"

/* Helper function definitions */
static void set_board_at(game_state_t* state, unsigned int row, unsigned int col, char ch);
static bool is_tail(char c);
static bool is_head(char c);
static bool is_snake(char c);
static char body_to_tail(char c);
static char head_to_body(char c);
static unsigned int get_next_row(unsigned int cur_row, char c);
static unsigned int get_next_col(unsigned int cur_col, char c);
static void find_head(game_state_t* state, unsigned int snum);
static char next_square(game_state_t* state, unsigned int snum);
static void update_tail(game_state_t* state, unsigned int snum);
static void update_head(game_state_t* state, unsigned int snum);

/* Task 1 */
game_state_t* create_default_state() {
  // TODO: Implement this function.
  //1. initialize the board
  char** board = malloc(sizeof(char *) * 18);
  for (int i = 0; i < 18; i ++)
  {
    board[i] = malloc(sizeof(char) * 21);
  }

  strcpy(board[0], "####################");
  strcpy(board[1], "#                  #");
  strcpy(board[2], "# d>D    *         #");
  strcpy(board[17], "####################");
  for (int i = 3; i < 17; i ++)
  {
    strcpy(board[i], board[1]);
  }

  //2. initialize the snake
  snake_t* snakes = malloc(sizeof (snake_t));
  snakes[0] = (snake_t){2, 2, 2, 4, true};

  //3. initialize the game state
  game_state_t* default_state = malloc(sizeof(game_state_t));
  default_state->num_rows =18;
  default_state->num_snakes = 1;
  default_state->board = board;
  default_state->snakes = snakes;

  return default_state;
}

/* Task 2 */
void free_state(game_state_t* state) {
  // TODO: Implement this function.
  for(int i = 0; i < state->num_rows; i ++)
  {
    free(state->board[i]);
  }
  free(state->board);
  free(state->snakes);
  free(state);
  return;
}

/* Task 3 */
void print_board(game_state_t* state, FILE* fp) {
  // TODO: Implement this function.
  for (int i = 0; i < state->num_rows; i ++)
  {
    fprintf(fp, "%s\n", state->board[i]);
  }
  return;
}

/*
  Saves the current state into filename. Does not modify the state object.
  (already implemented for you).
*/
void save_board(game_state_t* state, char* filename) {
  FILE* f = fopen(filename, "w");
  print_board(state, f);
  fclose(f);
}

/* Task 4.1 */

/*
  Helper function to get a character from the board
  (already implemented for you).
*/
char get_board_at(game_state_t* state, unsigned int row, unsigned int col) {
  return state->board[row][col];
}

/*
  Helper function to set a character on the board
  (already implemented for you).
*/
static void set_board_at(game_state_t* state, unsigned int row, unsigned int col, char ch) {
  state->board[row][col] = ch;
}

/*
  Returns true if c is part of the snake's tail.
  The snake consists of these characters: "wasd"
  Returns false otherwise.
*/
static bool is_tail(char c) {
  // TODO: Implement this function.
  return c == 'w' || c == 'a' || c == 's' || c == 'd';
}

/*
  Returns true if c is part of the snake's head.
  The snake consists of these characters: "WASDx"
  Returns false otherwise.
*/
static bool is_head(char c) {
  // TODO: Implement this function.
  return c == 'W' || c == 'A' || c == 'S' || c == 'D';
}

/*
  Returns true if c is part of the snake.
  The snake consists of these characters: "wasd^<v>WASDx"
*/
static bool is_snake(char c) {
  // TODO: Implement this function.
  return is_head(c) || is_head(c) || c == '^' || c == '<' || c == '>' || c == 'v';
}

/*
  Converts a character in the snake's body ("^<v>")
  to the matching character representing the snake's
  tail ("wasd").
*/
static char body_to_tail(char c) {
  // TODO: Implement this function.
  switch (c)
  {
    case '^':
      return 'w';
    case '<':
      return 'a';
    case '>':
      return 'd';
    case 'v':
      return 's';
  }
  return '?';
}

/*
  Converts a character in the snake's head ("WASD")
  to the matching character representing the snake's
  body ("^<v>").
*/
static char head_to_body(char c) {
  // TODO: Implement this function.
  switch (c)
  {
    case 'W':
      return '^';
    case 'A':
      return '<';
    case 'D':
      return '>';
    case 'S':
      return 'v';
  }
  return '?';
}

/*
  Returns cur_row + 1 if c is 'v' or 's' or 'S'.
  Returns cur_row - 1 if c is '^' or 'w' or 'W'.
  Returns cur_row otherwise.
*/
static unsigned int get_next_row(unsigned int cur_row, char c) {
  // TODO: Implement this function.
  if (c == 'v' || c == 's' || c == 'S')
  {
    return cur_row + 1;
  }
  else if (c == '^' || c == 'w' || c == 'W')
  {
    return cur_row - 1;
  }
  return cur_row;
}

/*
  Returns cur_col + 1 if c is '>' or 'd' or 'D'.
  Returns cur_col - 1 if c is '<' or 'a' or 'A'.
  Returns cur_col otherwise.
*/
static unsigned int get_next_col(unsigned int cur_col, char c) {
  // TODO: Implement this function.
  if (c == '>' || c == 'd' || c == 'D')
  {
    return cur_col + 1;
  }
  else if(c == '<' || c == 'a' || c == 'A')
  {
    return cur_col - 1;
  }
  return cur_col;
}

/*
  Task 4.2

  Helper function for update_state. Return the character in the cell the snake is moving into.

  This function should not modify anything.
*/
static char next_square(game_state_t* state, unsigned int snum) {
  // TODO: Implement this function.
  char c = get_board_at(state, state->snakes[snum].head_row, state->snakes[snum].head_col);
  unsigned int next_row = get_next_row(state->snakes[snum].head_row, c);
  unsigned int next_col = get_next_col(state->snakes[snum].head_col, c);
  return get_board_at(state, next_row, next_col);
}

/*
  Task 4.3

  Helper function for update_state. Update the head...

  ...on the board: add a character where the snake is moving

  ...in the snake struct: update the row and col of the head

  Note that this function ignores food, walls, and snake bodies when moving the head.
*/
static void update_head(game_state_t* state, unsigned int snum) {
  // TODO: Implement this function.
  unsigned int row = state->snakes[snum].head_row;
  unsigned int col = state->snakes[snum].head_col;

  char c_head = get_board_at(state, row, col);
  char c_body = head_to_body(c_head);
  set_board_at(state, row, col, c_body);

  unsigned int next_row = get_next_row(row, c_head);
  unsigned int next_col = get_next_col(col, c_head);
  set_board_at(state, next_row, next_col, c_head);

  state->snakes[snum].head_row = next_row;
  state->snakes[snum].head_col = next_col;

  return;
}

/*
  Task 4.4

  Helper function for update_state. Update the tail...

  ...on the board: blank out the current tail, and change the new
  tail from a body character (^<v>) into a tail character (wasd)

  ...in the snake struct: update the row and col of the tail
*/
static void update_tail(game_state_t* state, unsigned int snum) {
  // TODO: Implement this function.
  unsigned int row = state->snakes[snum].tail_row;
  unsigned int col = state->snakes[snum].tail_col;

  char c_tail = get_board_at(state, row, col);
  set_board_at(state, row, col, ' ');
  unsigned int next_row = get_next_row(row, c_tail);
  unsigned int next_col = get_next_col(col, c_tail);
  char c_body = get_board_at(state, next_row, next_col);
  c_tail = body_to_tail(c_body);
  set_board_at(state, next_row, next_col, c_tail);

  state->snakes[snum].tail_row = next_row;
  state->snakes[snum].tail_col = next_col;

  return;
}

/* Task 4.5 */
void update_state(game_state_t* state, int (*add_food)(game_state_t* state)) {
  // TODO: Implement this function.
  for(unsigned int snum = 0; snum < state->num_snakes; snum ++)
  {
    unsigned int head_row = state->snakes[snum].head_row;
    unsigned int head_col = state->snakes[snum].head_col;
    char c_head = get_board_at(state, head_row, head_col);
    unsigned int head_next_row = get_next_row(head_row, c_head);
    unsigned int head_next_col = get_next_col(head_col, c_head);
    char c_head_next = get_board_at(state, head_next_row, head_next_col);
    if (c_head_next == ' ')
    {
      update_head(state, snum);
      update_tail(state, snum);
    }
    else if(c_head_next == '*')
    {
      update_head(state, snum);
      add_food(state);
    }
    else
    {
      state->snakes[snum].live = false;
      set_board_at(state, head_row, head_col, 'x');
    }

 }
  return;
}

/* Task 5 */
game_state_t* load_board(FILE* fp) {
  // TODO: Implement this function.
  game_state_t* state = malloc(sizeof(game_state_t));
  state->snakes = NULL;
  state->board = NULL;
  state->num_snakes = 0;
  state->num_rows = 0;
  char* buf = NULL;
  unsigned int buf_size = 0;
  char ch;

  do {
    ch = (char)fgetc(fp);
    if(ch == '\n')
    {
      ch = '\0';
    }

    buf = (char*)realloc(buf, ++ buf_size);
    buf[buf_size - 1] = ch;

    if(ch == '\0')
    {
      state->board = (char**)realloc(state->board, (++state->num_rows) * sizeof(char*));
      state->board[state->num_rows - 1] = buf;
      buf_size = 0;
      buf = NULL;
    }
  } while(ch != EOF);

  return state;
}

/*
  Task 6.1

  Helper function for initialize_snakes.
  Given a snake struct with the tail row and col filled in,
  trace through the board to find the head row and col, and
  fill in the head row and col in the struct.
*/
static void find_head(game_state_t* state, unsigned int snum) {
  // TODO: Implement this function.
  unsigned int row = state->snakes[snum].tail_row;
  unsigned int col = state->snakes[snum].tail_col;
  while(1)
  {
    char c = get_board_at(state, row, col);
    if (is_head(c))
    {
      state->snakes[snum].head_row = row;
      state->snakes[snum].head_col = col;
      return;
    }
    unsigned int next_row = get_next_row(row, c);
    unsigned int next_col = get_next_col(col, c);

    row = next_row;
    col = next_col;
  }
  return;
}

/* Task 6.2 */
game_state_t* initialize_snakes(game_state_t* state) {
  // TODO: Implement this function.
  state->snakes = NULL;
  for (unsigned row = 0; row < state->num_rows; row ++)
  {
    unsigned int len = (unsigned int)strlen(state->board[row]);
    for (unsigned col = 0; col < len; col ++)
    {
      char ch = get_board_at(state, row, col);
      if (is_tail(ch))
      {
        state->snakes = realloc(state->snakes, (sizeof(snake_t)) * (++ state->num_snakes));
        state->snakes[state->num_snakes - 1] = (snake_t){row, col, 0, 0, true};
        find_head(state, state->num_snakes - 1);
      }
    }
  }
  return state;
}
