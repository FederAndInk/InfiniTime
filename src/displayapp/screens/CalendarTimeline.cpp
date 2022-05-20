#include "displayapp/screens/CalendarTimeline.h"

#include "components/ble/CalendarEventService.h"
#include "displayapp/DisplayApp.h"

using namespace Pinetime::Applications::Screens;

CalendarTimeline::CalendarTimeline(DisplayApp* app, Pinetime::Controllers::CalendarEventService& calendarEvent)
  : Screen(app), calendarEvent(calendarEvent) {
  lv_obj_t* container1 = lv_cont_create(lv_scr_act(), nullptr);

  lv_obj_set_style_local_bg_color(container1, LV_CONT_PART_MAIN, LV_STATE_DEFAULT, LV_COLOR_MAKE(0x38, 0x38, 0x38));
  lv_obj_set_style_local_pad_all(container1, LV_CONT_PART_MAIN, LV_STATE_DEFAULT, 10);
  lv_obj_set_style_local_pad_inner(container1, LV_CONT_PART_MAIN, LV_STATE_DEFAULT, 5);
  lv_obj_set_style_local_border_width(container1, LV_CONT_PART_MAIN, LV_STATE_DEFAULT, 0);

  lv_obj_set_pos(container1, 0, 50);
  lv_obj_set_size(container1, LV_HOR_RES, 190);

  lv_cont_set_layout(container1, LV_LAYOUT_COLUMN_LEFT);
  lv_cont_set_fit(container1, LV_FIT_NONE);

  lv_obj_t* alert_count = lv_label_create(lv_scr_act(), nullptr);
  lv_label_set_text_fmt(alert_count, "1/%d", calendarEvent.GetEvents().size());
  lv_obj_align(alert_count, nullptr, LV_ALIGN_IN_TOP_RIGHT, 0, 16);

  lv_obj_t* alert_type = lv_label_create(lv_scr_act(), nullptr);
  lv_obj_set_style_local_text_color(alert_type, LV_LABEL_PART_MAIN, LV_STATE_DEFAULT, LV_COLOR_MAKE(0xb0, 0xb0, 0xb0));
  lv_label_set_text_static(alert_type, "Notification");

  lv_label_set_long_mode(alert_type, LV_LABEL_LONG_SROLL_CIRC);
  lv_obj_set_width(alert_type, 180);
  lv_obj_align(alert_type, nullptr, LV_ALIGN_IN_TOP_LEFT, 0, 16);

  lv_obj_t* alert_subject = lv_label_create(container1, nullptr);
  lv_obj_set_style_local_text_color(alert_subject, LV_LABEL_PART_MAIN, LV_STATE_DEFAULT, LV_COLOR_MAKE(0xff, 0xb0, 0x0));
  lv_label_set_long_mode(alert_subject, LV_LABEL_LONG_BREAK);
  lv_obj_set_width(alert_subject, LV_HOR_RES - 20);
  if (calendarEvent.GetEvents().empty()) {
    lv_label_set_text_static(alert_subject, "No events");
  } else {
    lv_label_set_text_static(alert_subject, calendarEvent.GetEvents().begin()->GetTitle());
  }
}

CalendarTimeline::~CalendarTimeline() {
  lv_obj_clean(lv_scr_act());
}
