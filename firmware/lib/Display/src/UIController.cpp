#include "UIController.h"
#include "device/usbd.h"
#include "screens.h"

UIController::UIController(Display &display)
    : display_(display) {}

void UIController::setChordRoot(const char *rootText) {
  lv_label_set_text(objects.chord_root_label, rootText);
}

void UIController::setChordQuality(const char *qualityText) {
  lv_label_set_text(objects.chord_quality_label, qualityText);
}

void UIController::updateUsbStatus() {
  bool midiReady = tud_ready();

  if (midiReady == lastMidiReady_)
    return;

  lastMidiReady_ = midiReady;

  if (midiReady == true) {
    lv_obj_add_state(objects.usb_label, LV_STATE_CHECKED);
    lv_label_set_text(objects.usb_label, "* USB");
  } else {
    lv_obj_clear_state(objects.usb_label, LV_STATE_CHECKED);
    lv_label_set_text(objects.usb_label, "* NO USB");
  }
}

void UIController::setChordNotes(const char *notesText) {
  lv_label_set_text(objects.chord_notes_label, notesText);
}

void UIController::updateJoystickMatrix(
    uint8_t joystickPos, const std::array<const char *, 12> &map) {
  lv_buttonmatrix_set_button_ctrl(objects.joystick_matrix, joystickPos,
                                  LV_BUTTONMATRIX_CTRL_CHECKED);
  lv_buttonmatrix_set_map(objects.joystick_matrix, map.data());
}

void UIController::clearJoystickMatrix() {
  lv_buttonmatrix_clear_button_ctrl_all(objects.joystick_matrix,
                                        LV_BUTTONMATRIX_CTRL_CHECKED);
}
