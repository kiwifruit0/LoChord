#include "UIController.h"
#include "screens.h"

UIController::UIController(Display& display) : display_(display) {}

void UIController::buttonPressed(uint8_t buttonId) {
  display_.switchTheme((active_theme_index + 1) % 3);
}
