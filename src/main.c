#include <assert.h>
#include <raylib.h>
#include <stdio.h>
#include <string.h>

#if DEBUG_MODE
static bool DEBUG_GRAPHICS = true;
#else
static bool DEBUG_GRAPHICS = false;
#endif

#define KILOBYTES(x) ((size_t)(x) * 1024)
#define MEGABYTES(x) ((size_t)(x) * 1024 * 1024)
#define GIGABYTES(x) ((size_t)(x) * 1024 * 1024 * 1024)
#define TERABYTES(x) ((size_t)(x) * 1024 * 1024 * 1024 * 1024)

#define DEFAULT_ALIGNMENT (2 * sizeof(void *))

typedef struct String8 String8;
struct String8 {
  char *buffer;
  size_t len;
  size_t capacity;
};

typedef struct Arena Arena;
struct Arena {
  char *buffer;
  size_t capacity;
  size_t offset;
};

bool powerOfTwo(size_t x) {
  while (x % 2 == 0) {
    x = x / 2;
  }
  if (x == 1) {
    return true;
  }
  return false;
}

Arena arenaInit(char *buffer, size_t capacity) {
  return (Arena){
      .buffer = buffer,
      .capacity = capacity,
      .offset = 0,
  };
}

void arenaReset(Arena *arena) { arena->offset = 0; }

void *alignMemoryForward(void *mem, size_t alignment) {
  assert(powerOfTwo(alignment));
  size_t modulo = (size_t)mem % alignment;
  if (modulo != 0) {
    mem = (void *)(alignment - modulo);
  }
  return mem;
}

void *arenaAllocAlign(Arena *arena, size_t size, size_t alignment) {
  if (arena->offset + size > arena->capacity) {
    return NULL;
  }
  void *mem = &arena->buffer[arena->offset];
  mem = alignMemoryForward(mem, alignment);
  arena->offset += size;
  memset(mem, 0, size);
  return mem;
}

void *arenaAlloc(Arena *arena, size_t size) {
  return arenaAllocAlign(arena, size, DEFAULT_ALIGNMENT);
}

void appendCharToString(String8 *s, char c) {
  if (s->len >= s->capacity) {
    return;
  }
  s->buffer[s->len] = c;
  s->len++;
}

void chopLeftCharFromString(String8 *s) {
  if (s->len == 0) {
    return;
  }
  s->buffer++;
  s->len--;
}

void input(String8 *input_text_array, size_t number_of_text_input,
           int *text_input_focused_id) {

  if (IsMouseButtonPressed(0)) {
    *text_input_focused_id = 0;
  }
  if (*text_input_focused_id != 0) {
    if (IsKeyPressed(KEY_A)) {
      appendCharToString(&input_text_array[*text_input_focused_id - 1], 'a');
    }
    if (IsKeyPressed(KEY_B)) {
      appendCharToString(&input_text_array[*text_input_focused_id - 1], 'b');
    }
    if (IsKeyPressed(KEY_C)) {
      appendCharToString(&input_text_array[*text_input_focused_id - 1], 'c');
    }
    if (IsKeyPressed(KEY_D)) {
      appendCharToString(&input_text_array[*text_input_focused_id - 1], 'd');
    }
    if (IsKeyPressed(KEY_E)) {
      appendCharToString(&input_text_array[*text_input_focused_id - 1], 'e');
    }
  }
}

bool collisonBetweenMouseAndRect(Rectangle rect) {
  Vector2 mouse_pos = GetMousePosition();
  if (mouse_pos.x >= rect.x && mouse_pos.x <= rect.x + rect.width &&
      mouse_pos.y >= rect.y && mouse_pos.y <= rect.y + rect.height) {
    return true;
  }
  return false;
}

bool drawButton(Rectangle rect, float roundness, Color button_color, char *text,
                int font_size, Color text_color) {
  DrawRectangleRounded(rect, roundness, 10, button_color);
  int text_width = MeasureText(text, font_size);
  // NOTE: (omar) this only works on the default font
  int text_height = font_size;
  Vector2 text_pos = {
      .x = rect.x + (rect.width - text_width) / 2,
      .y = rect.y + (rect.height - text_height) / 2,
  };
  DrawText(text, text_pos.x, text_pos.y, 32, BLACK);

  if (DEBUG_GRAPHICS) {
    DrawRectangleLinesEx(rect, 1.0f, YELLOW);
    DrawRectangleLinesEx(
        (Rectangle){
            .x = text_pos.x,
            .y = text_pos.y,
            .width = text_width,
            .height = text_height,
        },
        1.0f, YELLOW);
  }
  if (collisonBetweenMouseAndRect(rect) && IsMouseButtonPressed(0)) {
    return true;
  }
  return false;
}

bool drawTextInput(Rectangle rect, float cursor_animation_time,
                   String8 input_text, int font_size, bool cursor_blink) {
  DrawRectangleRoundedLines(rect, 0.25, 10, RAYWHITE);
  int text_width = MeasureText(input_text.buffer, font_size);
  // NOTE: (omar) this only works on the default font
  int text_height = font_size;
  float text_padding = 0.1 * font_size;
  Vector2 text_pos = {
      .x = rect.x + text_padding,
      .y = rect.y + (rect.height - text_height) / 2,
  };
  Vector2 cursor_padding = {
      .x = 0.1 * font_size,
      .y = 0.05 * rect.height,
  };
  float cursor_width = 5.0f;
  while (text_width + text_padding + cursor_width + cursor_padding.x >
         rect.width) {
    chopLeftCharFromString(&input_text);
    text_width = MeasureText(input_text.buffer, font_size);
    text_pos.x = rect.x;
  }
  Rectangle cursor = {
      .x = rect.x + cursor_padding.x + text_width + text_padding,
      .y = rect.y + cursor_padding.y,
      .width = cursor_width,
      .height = rect.height - 2 * cursor_padding.y,
  };
  DrawText(input_text.buffer, text_pos.x, text_pos.y, font_size, WHITE);
  if (cursor_animation_time > 0.5 && cursor_blink) {
    DrawRectangleRec(cursor, RAYWHITE);
  }

  if (DEBUG_GRAPHICS) {
    DrawRectangleLinesEx(rect, 1.0f, YELLOW);
    DrawRectangleLinesEx(
        (Rectangle){
            .x = text_pos.x,
            .y = text_pos.y,
            .width = text_width,
            .height = text_height,
        },
        1.0f, YELLOW);

    DrawRectangleLinesEx(
        (Rectangle){
            .x = cursor.x - cursor_padding.x,
            .y = cursor.y,
            .width = cursor.width + cursor_padding.x,
            .height = cursor.height,
        },
        1.0f, YELLOW);
  }
  if (collisonBetweenMouseAndRect(rect) && IsMouseButtonPressed(0)) {
    return true;
  }
  return false;
}

int main() {
  InitWindow(800, 450, "imgui");
  size_t persistant_arena_size = MEGABYTES(10);
  char b[persistant_arena_size];
  Arena persistant_arena = arenaInit(b, persistant_arena_size);
  Rectangle rect = {
      .x = 100,
      .y = 50,
      .width = 500,
      .height = 50,
  };
  Rectangle rect2 = {
      .x = 100,
      .y = 100,
      .width = 200,
      .height = 50,
  };
  Rectangle rect3 = {
      .x = 100,
      .y = 150,
      .width = 100,
      .height = 50,
  };
  Rectangle rect4 = {
      .x = 100,
      .y = 200,
      .width = 50,
      .height = 50,
  };
  Rectangle rect5 = {
      .x = 100,
      .y = 250,
      .width = 20,
      .height = 50,
  };
  char info_buffer[1000];
  int i = 0;
  float dt = 0.0f;
  float cursor_animation_time = 0.0f;
  size_t number_of_text_input = 5;
  String8 *input_text_array =
      arenaAlloc(&persistant_arena, number_of_text_input * sizeof(String8));

  char text_buffer[100] = {0};
  input_text_array[0].buffer = text_buffer;
  input_text_array[0].capacity = 100;

  char text_buffer2[100] = {0};
  input_text_array[1].buffer = text_buffer2;
  input_text_array[1].capacity = 100;

  char text_buffer3[100] = {0};
  input_text_array[2].buffer = text_buffer3;
  input_text_array[2].capacity = 100;

  char text_buffer4[100] = {0};
  input_text_array[3].buffer = text_buffer4;
  input_text_array[3].capacity = 100;

  char text_buffer5[100] = {0};
  input_text_array[4].buffer = text_buffer5;
  input_text_array[4].capacity = 100;

  int text_input_focused_id = 0;
  while (!WindowShouldClose()) {
    dt = GetFrameTime();
    cursor_animation_time += dt;
    if (cursor_animation_time > 1.0) {
      cursor_animation_time = 0.0f;
    }
    input(input_text_array, number_of_text_input, &text_input_focused_id);
    BeginDrawing();
    ClearBackground(BLACK);
    // if (drawButton(rect, 0.25, RAYWHITE, "increment", 32, BLACK)) {
    //   i++;
    // }
    if (drawTextInput(rect, cursor_animation_time, input_text_array[0], 32,
                      text_input_focused_id == 1)) {
      text_input_focused_id = 1;
    }
    if (drawTextInput(rect2, cursor_animation_time, input_text_array[1], 32,
                      text_input_focused_id == 2)) {
      text_input_focused_id = 2;
    }
    if (drawTextInput(rect3, cursor_animation_time, input_text_array[2], 32,
                      text_input_focused_id == 3)) {
      text_input_focused_id = 3;
    }
    if (drawTextInput(rect4, cursor_animation_time, input_text_array[3], 32,
                      text_input_focused_id == 4)) {
      text_input_focused_id = 4;
    }
    if (drawTextInput(rect5, cursor_animation_time, input_text_array[4], 32,
                      text_input_focused_id == 5)) {
      text_input_focused_id = 5;
    }
    // snprintf(info_buffer, sizeof(info_buffer), "i : %d", i);
    // DrawText(info_buffer, 350, 150, 32, RAYWHITE);
    EndDrawing();
  }
  CloseWindow();
  return 0;
}
