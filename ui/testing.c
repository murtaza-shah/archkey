#include "../third_party/clay.h"
#include <stdint.h>
#include <stdlib.h>

extern Clay_Color COLOR_WHITE;
extern const int FONT_ID_BODY_16;

void RenderHelloWorld(Clay_String text) {
  CLAY_AUTO_ID({.layout = {.padding = {16, 16, 8, 8}},
                .backgroundColor = {140, 140, 140, 255},
                .cornerRadius = CLAY_CORNER_RADIUS(5)}) {
    CLAY_TEXT(text, CLAY_TEXT_CONFIG({.fontId = FONT_ID_BODY_16,
                                      .fontSize = 16,
                                      .textColor = COLOR_WHITE}));
  }
}

typedef struct {
  intptr_t offset;
  intptr_t memory;
} FrameArena;

typedef struct {
  uint32_t bla;
  FrameArena testArena;
} TestData;

TestData InitializeTest() {
  return (TestData){
      .bla = 4,
      .testArena = {.memory = (intptr_t)malloc(1024), .offset = 0},
  };
}

Clay_RenderCommandArray CreateBabyLayout(TestData *data) {
  data->testArena.offset = 0;
  Clay_BeginLayout();
  CLAY_AUTO_ID({.layout = {.padding = {16, 16, 8, 8}},
                .backgroundColor = {140, 140, 140, 255},
                .cornerRadius = CLAY_CORNER_RADIUS(5)}) {
    CLAY_TEXT(CLAY_STRING("hello world"),
              CLAY_TEXT_CONFIG({.fontId = FONT_ID_BODY_16,
                                .fontSize = 16,
                                .textColor = COLOR_WHITE}));
  }
  Clay_RenderCommandArray renderCommands = Clay_EndLayout();
  return renderCommands;
}
