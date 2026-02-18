#include "../third_party/clay.h"

extern const int FONT_ID_BODY_16;

typedef struct {
  Clay_String id;      // logical key id (lowercase)
  Clay_String display; // what we render (uppercase)
} KeyDef;

static void RenderKey(KeyDef key, float keyWidth) {
  Clay_ElementId id = Clay_GetElementId(key.id);
  bool hovered = Clay_PointerOver(id);
  Clay_Color bg = hovered ? (Clay_Color){120, 120, 120, 255}
                          : (Clay_Color){90, 90, 90, 255};

  CLAY(id, {.layout = {.sizing = {.width = CLAY_SIZING_FIXED(keyWidth),
                                  .height = CLAY_SIZING_FIXED(56)},
                       .padding = CLAY_PADDING_ALL(12)},
            .backgroundColor = bg,
            .cornerRadius = CLAY_CORNER_RADIUS(6)}) {
    CLAY_TEXT(key.display,
              CLAY_TEXT_CONFIG({.fontId = FONT_ID_BODY_16,
                                .fontSize = 20,
                                .textColor = {255, 255, 255, 255}}));
  }
}

Clay_RenderCommandArray CreateKeyboardLayout(Clay_String recentText) {
  const KeyDef row1[] = {
      {CLAY_STRING("q"), CLAY_STRING("Q")},
      {CLAY_STRING("w"), CLAY_STRING("W")},
      {CLAY_STRING("e"), CLAY_STRING("E")},
      {CLAY_STRING("r"), CLAY_STRING("R")},
      {CLAY_STRING("t"), CLAY_STRING("T")},
      {CLAY_STRING("y"), CLAY_STRING("Y")},
      {CLAY_STRING("u"), CLAY_STRING("U")},
      {CLAY_STRING("i"), CLAY_STRING("I")},
      {CLAY_STRING("o"), CLAY_STRING("O")},
      {CLAY_STRING("p"), CLAY_STRING("P")},
  };

  const KeyDef row2[] = {
      {CLAY_STRING("a"), CLAY_STRING("A")},
      {CLAY_STRING("s"), CLAY_STRING("S")},
      {CLAY_STRING("d"), CLAY_STRING("D")},
      {CLAY_STRING("f"), CLAY_STRING("F")},
      {CLAY_STRING("g"), CLAY_STRING("G")},
      {CLAY_STRING("h"), CLAY_STRING("H")},
      {CLAY_STRING("j"), CLAY_STRING("J")},
      {CLAY_STRING("k"), CLAY_STRING("K")},
      {CLAY_STRING("l"), CLAY_STRING("L")},
  };

  const KeyDef row3[] = {
      {CLAY_STRING("z"), CLAY_STRING("Z")},
      {CLAY_STRING("x"), CLAY_STRING("X")},
      {CLAY_STRING("c"), CLAY_STRING("C")},
      {CLAY_STRING("v"), CLAY_STRING("V")},
      {CLAY_STRING("b"), CLAY_STRING("B")},
      {CLAY_STRING("n"), CLAY_STRING("N")},
      {CLAY_STRING("m"), CLAY_STRING("M")},
  };

  const KeyDef spacebar[] = {
      {CLAY_STRING("Space"), CLAY_STRING("Space")},
  };

  Clay_BeginLayout();

  CLAY_AUTO_ID({.layout = {.layoutDirection = CLAY_TOP_TO_BOTTOM,
                           .padding = CLAY_PADDING_ALL(16),
                           .childGap = 12,
                           .sizing = {.width = CLAY_SIZING_GROW(0),
                                      .height = CLAY_SIZING_GROW(0)}},
                .backgroundColor = {30, 30, 30, 255}}) {
    // Row 1
    CLAY_AUTO_ID(
        {.layout = {.layoutDirection = CLAY_LEFT_TO_RIGHT, .childGap = 8}}) {
      for (int i = 0; i < (int)(sizeof(row1) / sizeof(row1[0])); i++) {
        RenderKey(row1[i], 64);
      }
    }

    // Row 2 (slight indent)
    CLAY_AUTO_ID({.layout = {.layoutDirection = CLAY_LEFT_TO_RIGHT,
                             .childGap = 8,
                             .padding = {.left = 24}}}) {
      for (int i = 0; i < (int)(sizeof(row2) / sizeof(row2[0])); i++) {
        RenderKey(row2[i], 64);
      }
    }

    // Row 3 (indent)
    CLAY_AUTO_ID({.layout = {.layoutDirection = CLAY_LEFT_TO_RIGHT,
                             .childGap = 8,
                             .padding = {.left = 48}}}) {
      for (int i = 0; i < (int)(sizeof(row3) / sizeof(row3[0])); i++) {
        RenderKey(row3[i], 64);
      }
    }

    // Spacebar row
    CLAY_AUTO_ID({.layout = {.layoutDirection = CLAY_LEFT_TO_RIGHT,
                             .childGap = 8,
                             .padding = {.left = 96}}}) {
      RenderKey(spacebar[0], 320);
    }

    // Recent swipe display
    Clay_String displayText = recentText;
    if (displayText.length == 0) {
      displayText = CLAY_STRING("(none)");
    }

    CLAY_AUTO_ID({.layout = {.layoutDirection = CLAY_LEFT_TO_RIGHT,
                             .childGap = 8,
                             .padding = CLAY_PADDING_ALL(8)},
                  .backgroundColor = {60, 60, 60, 255},
                  .cornerRadius = CLAY_CORNER_RADIUS(6)}) {
      CLAY_TEXT(CLAY_STRING("Swipe result:"),
                CLAY_TEXT_CONFIG({.fontId = FONT_ID_BODY_16,
                                  .fontSize = 16,
                                  .textColor = {200, 200, 200, 255}}));
      CLAY_TEXT(displayText,
                CLAY_TEXT_CONFIG({.fontId = FONT_ID_BODY_16,
                                  .fontSize = 18,
                                  .textColor = {255, 255, 255, 255}}));
    }
  }

  Clay_RenderCommandArray renderCommands = Clay_EndLayout();
  return renderCommands;
}
