#include <raylib.h>
#include <stdio.h>

void input() { return; }

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

  if (collisonBetweenMouseAndRect(rect) && IsMouseButtonPressed(0)) {
    return true;
  }
  return false;
}

int main() {
  InitWindow(800, 450, "imgui");
  Rectangle rect = {
      .x = 100,
      .y = 100,
      .width = 200,
      .height = 100,
  };
  char info_buffer[1000];
  int i = 0;
  while (!WindowShouldClose()) {
    input();
    BeginDrawing();
    ClearBackground(BLACK);
    if (drawButton(rect, 0.25, RAYWHITE, "increment", 32, BLACK)) {
      i++;
    }
    snprintf(info_buffer, sizeof(info_buffer), "i : %d", i);
    DrawText(info_buffer, 350, 150, 32, RAYWHITE);
    EndDrawing();
  }
  CloseWindow();
  return 0;
}
