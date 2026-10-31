#include <raylib.h>

int main() {
  InitWindow(800, 450, "imgui");
  while (!WindowShouldClose()) {
    BeginDrawing();
    DrawText("hello", 100, 100, 32, RAYWHITE);
    EndDrawing();
  }
  CloseWindow();
  return 0;
}
