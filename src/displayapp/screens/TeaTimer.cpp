#include "displayapp/screens/TeaTimer.h"
#include "displayapp/DisplayApp.h"

#include <array>

using namespace Pinetime::Applications::Screens;

namespace {
  constexpr std::uint8_t BTN_NB {6};
  std::array<char const*, BTN_NB + 2> btnmMap {"2m", "3m", "4m", "\n", "5m", "6m", "7m", ""};
  constexpr std::array<std::uint8_t, BTN_NB> btnValues {2, 3, 4, 5, 6, 7};
}

TeaTimer::TeaTimer(DisplayApp* app, Controllers::TimerController& timerController)
  : Screen(app), timerController {timerController}, btnm1 {lv_btnmatrix_create(lv_scr_act(), nullptr)} {
  lv_btnmatrix_set_map(btnm1, btnmMap.data());
  lv_obj_set_size(btnm1, LV_HOR_RES - 16, LV_VER_RES - 60);
  lv_obj_align(btnm1, NULL, LV_ALIGN_CENTER, 0, 10);

  lv_obj_set_style_local_radius(btnm1, LV_BTNMATRIX_PART_BTN, LV_STATE_DEFAULT, 20);
  lv_obj_set_style_local_bg_opa(btnm1, LV_BTNMATRIX_PART_BTN, LV_STATE_DEFAULT, LV_OPA_50);
  lv_obj_set_style_local_bg_color(btnm1, LV_BTNMATRIX_PART_BTN, LV_STATE_DEFAULT, LV_COLOR_AQUA);
  lv_obj_set_style_local_bg_opa(btnm1, LV_BTNMATRIX_PART_BTN, LV_STATE_DISABLED, LV_OPA_50);
  lv_obj_set_style_local_bg_color(btnm1, LV_BTNMATRIX_PART_BTN, LV_STATE_DISABLED, lv_color_hex(0x111111));
  lv_obj_set_style_local_pad_all(btnm1, LV_BTNMATRIX_PART_BG, LV_STATE_DEFAULT, 0);
  lv_obj_set_style_local_pad_inner(btnm1, LV_BTNMATRIX_PART_BG, LV_STATE_DEFAULT, 10);

  // for (std::uint8_t i = 0; i < BTN_NB; i++) {
  //   lv_btnmatrix_set_btn_ctrl(btnm1, i, LV_BTNMATRIX_CTRL_CLICK_TRIG);
  // }

  btnm1->user_data = this;
  lv_obj_set_event_cb(btnm1, [](lv_obj_t* obj, lv_event_t event) {
    if (event != LV_EVENT_VALUE_CHANGED) {
      return;
    }

    auto* teaTimer = static_cast<TeaTimer*>(obj->user_data);
    auto eventData = *static_cast<uint32_t const*>(lv_event_get_data());
    teaTimer->OnValueChangedEvent(obj, eventData);
  });
}

TeaTimer::~TeaTimer() {
  lv_obj_clean(lv_scr_act());
}

void TeaTimer::OnValueChangedEvent(lv_obj_t* obj, uint32_t buttonId) {
  if (obj != btnm1) {
    return;
  }

  timerController.StartTimer(btnValues[buttonId] * 60 * 1000);
  app->StartApp(Apps::Timer, DisplayApp::FullRefreshDirections::Up);
  running = false;
}
