#include "UIController.h"
#include "screens.h"

UIController::UIController(Display &display) : display_(display) {}

void UIController::setChordRoot(const char *rootText) {
  lv_label_set_text(objects.chord_root_label, rootText);
}

void UIController::setChordQuality(const char *qualityText) {
  lv_label_set_text(objects.chord_quality_label, qualityText);
}
