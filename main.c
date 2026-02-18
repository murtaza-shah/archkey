#include "renderers/raylib.h"
#define CLAY_IMPLEMENTATION
#include "third_party/clay.h"
#include <stdbool.h>
#include <stdio.h>
#include <string.h>

const int FONT_ID_BODY_16 = 0;
Clay_Color COLOR_WHITE = {255, 255, 255, 255};
const double DWELL_THRESHOLD = 0.500;

#include "renderers/raylib_clay.c"
#include "ui/keyboard.c"
#include "ui/testing.c"
#include "utils/string_builder.c"

// This function is new since the video was published
void HandleClayErrors(Clay_ErrorData errorData) {
  printf("%s", errorData.errorText.chars);
}

typedef struct {
  bool isActive;
  Clay_String keysVisited[32];
  int visitCount;
  double dwellSeconds[32];
  double currentKeyStart;

  int trailCount;
  Clay_Vector2 trail[64];
  int trailHead;
} SwipeState;

static bool ClayStringsEqual(Clay_String a, Clay_String b) {
  return a.length == b.length && memcmp(a.chars, b.chars, a.length) == 0;
}

static const Clay_String KEY_LABELS[] = {
    CLAY_STRING("q"), CLAY_STRING("w"), CLAY_STRING("e"),    CLAY_STRING("r"),
    CLAY_STRING("t"), CLAY_STRING("y"), CLAY_STRING("u"),    CLAY_STRING("i"),
    CLAY_STRING("o"), CLAY_STRING("p"), CLAY_STRING("a"),    CLAY_STRING("s"),
    CLAY_STRING("d"), CLAY_STRING("f"), CLAY_STRING("g"),    CLAY_STRING("h"),
    CLAY_STRING("j"), CLAY_STRING("k"), CLAY_STRING("l"),    CLAY_STRING("z"),
    CLAY_STRING("x"), CLAY_STRING("c"), CLAY_STRING("v"),    CLAY_STRING("b"),
    CLAY_STRING("n"), CLAY_STRING("m"), CLAY_STRING("Space")};
static const int KEY_LABELS_COUNT =
    (int)(sizeof(KEY_LABELS) / sizeof(KEY_LABELS[0]));

static int BuildSwipeString(const SwipeState *swipe, char *out, int outSize) {
  int len = 0;
  for (int i = 0; i < swipe->visitCount; i++) {
    if (swipe->dwellSeconds[i] < DWELL_THRESHOLD) {
      continue;
    }
    if (len + (int)swipe->keysVisited[i].length >= outSize - 1) {
      break;
    }
    memcpy(out + len, swipe->keysVisited[i].chars,
           swipe->keysVisited[i].length);
    len += (int)swipe->keysVisited[i].length;
  }
  out[len] = '\0';
  return len;
}

int main(void) {
  Clay_Raylib_Initialize(
      745, 320, "Archkey",
      FLAG_WINDOW_RESIZABLE | FLAG_WINDOW_HIGHDPI | FLAG_MSAA_4X_HINT |
          FLAG_VSYNC_HINT); // Extra parameters to this function are new since
                            // the video was published

  uint64_t clayRequiredMemory = Clay_MinMemorySize();
  Clay_Arena clayMemory = Clay_CreateArenaWithCapacityAndMemory(
      clayRequiredMemory, malloc(clayRequiredMemory));
  Clay_Initialize(
      clayMemory,
      (Clay_Dimensions){.width = GetScreenWidth(), .height = GetScreenHeight()},
      (Clay_ErrorHandler){HandleClayErrors}); // This final argument is new
                                              // since the video was published
  Font fonts[1];
  fonts[FONT_ID_BODY_16] =
      LoadFontEx("resources/Roboto-Regular.ttf", 48, 0, 400);
  SetTextureFilter(fonts[FONT_ID_BODY_16].texture, TEXTURE_FILTER_BILINEAR);
  Clay_SetMeasureTextFunction(Raylib_MeasureText, fonts);

  SwipeState swipe = {0};
  char lastSwipe[128] = {0};
  int lastSwipeLen = 0;

  SB_StringArena sbArena = {0};
  sb_init(&sbArena, 2048);

  while (!WindowShouldClose()) {
    // Run once per frame
    Clay_SetLayoutDimensions((Clay_Dimensions){.width = GetScreenWidth(),
                                               .height = GetScreenHeight()});

    Vector2 mousePosition = GetMousePosition();
    Vector2 scrollDelta = GetMouseWheelMoveV();
    Clay_SetPointerState((Clay_Vector2){mousePosition.x, mousePosition.y},
                         IsMouseButtonDown(0));
    Clay_UpdateScrollContainers(
        true, (Clay_Vector2){scrollDelta.x, scrollDelta.y}, GetFrameTime());

    if (IsKeyPressed(KEY_SPACE)) {
      if (!swipe.isActive) {
        swipe.isActive = true;
        swipe.visitCount = 0;
        swipe.currentKeyStart = GetTime();
        printf("Swipe start\n");
      } else {
        double now = GetTime();
        if (swipe.visitCount > 0) {
          swipe.dwellSeconds[swipe.visitCount - 1] +=
              now - swipe.currentKeyStart;
        }
        char buffer[128];
        BuildSwipeString(&swipe, buffer, sizeof(buffer));
        if (sb_length(&sbArena) > 0) {
          sb_append(&sbArena, " ");
        }
        sb_append(&sbArena, buffer);
        lastSwipeLen = (int)strlen(buffer);
        if (lastSwipeLen >= (int)sizeof(lastSwipe)) {
          lastSwipeLen = (int)sizeof(lastSwipe) - 1;
        }
        memcpy(lastSwipe, buffer, lastSwipeLen);
        lastSwipe[lastSwipeLen] = '\0';

        printf("Swipe sequence: %s\nDwell (s): ", buffer);
        for (int i = 0; i < swipe.visitCount; i++) {
          if (swipe.dwellSeconds[i] >= DWELL_THRESHOLD) {
            printf("%.3f ", swipe.dwellSeconds[i]);
          }
        }
        printf("\nSwipe stop\n");
        fflush(stdout);
        swipe.isActive = false;
      }
    }
    if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
      const char *full = sb_get(&sbArena);
      if (full && full[0] != '\0') {
        SetClipboardText(full);
        printf("Copied to clipboard: %s\n", full);
        sb_clear(&sbArena);
      }
    }

    const char *arenaText = sb_get(&sbArena);
    size_t arenaLen = sb_length(&sbArena);
    Clay_RenderCommandArray renderCommands =
        CreateKeyboardLayout((Clay_String){.chars = arenaText ? arenaText : "",
                                           .length = (int)arenaLen,
                                           .isStaticallyAllocated = false});

    // Collect hovered keys while space is held (skip Space key itself)
    if (swipe.isActive) {
      double now = GetTime();
      for (int i = 0; i < KEY_LABELS_COUNT; i++) {
        Clay_String label = KEY_LABELS[i];
        if (ClayStringsEqual(label, CLAY_STRING("Space"))) {
          continue;
        }
        Clay_ElementId id = Clay_GetElementId(label);
        if (Clay_PointerOver(id)) {
          if (swipe.visitCount == 0) {
            swipe.keysVisited[0] = label;
            swipe.dwellSeconds[0] = 0.0;
            swipe.visitCount = 1;
            swipe.currentKeyStart = now;
          } else if (!ClayStringsEqual(
                         label, swipe.keysVisited[swipe.visitCount - 1])) {
            // close previous key's dwell
            swipe.dwellSeconds[swipe.visitCount - 1] +=
                now - swipe.currentKeyStart;
            if (swipe.visitCount < (int)(sizeof(swipe.keysVisited) /
                                         sizeof(swipe.keysVisited[0]))) {
              swipe.keysVisited[swipe.visitCount] = label;
              swipe.dwellSeconds[swipe.visitCount] = 0.0;
              swipe.visitCount++;
              swipe.currentKeyStart = now;
            }
          }
          break; // assume only one key under pointer
        }
      }
    }
    BeginDrawing();
    ClearBackground(BLACK);
    Clay_Raylib_Render(renderCommands, fonts);
    EndDrawing();
  }
  // This function is new since the video was published
  sb_free(&sbArena);
  Clay_Raylib_Close();
}
