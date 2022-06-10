#include "displayapp/screens/CalendarTimeline.h"

#include "components/ble/CalendarEventService.h"
#include "components/datetime/DateTimeController.h"
#include "displayapp/DisplayApp.h"

#include <date/date.h>

namespace {
  void RemoveNl(lv_obj_t* label) {
    char* pchar = strchr(lv_label_get_text(label), '\n');
    while (pchar != nullptr) {
      *pchar = ' ';
      pchar = strchr(pchar + 1, '\n');
    }
    lv_label_refr_text(label);
  }
}

namespace Pinetime {
  namespace Applications {
    namespace Screens {
      class CalendarTimelinePage {
      public:
        explicit CalendarTimelinePage(Controllers::CalendarEventService::EventRange const& events,
                                      Controllers::CalendarEventService::EventRange::const_iterator evIt,
                                      Controllers::DateTime& dateTimeController);
        ~CalendarTimelinePage();

        void AddEvent(Controllers::CalendarEventService::CalendarEvent const& ev);
        void SetDate(lv_obj_t* label, Controllers::CalendarEventService::CalendarEvent const& ev);

        bool HasNext() const {
          return curEv.HasNext() && std::next(curEv).HasNext();
        }

        bool HasPrev() const {
          return curEv.HasPrev();
        }

        Controllers::CalendarEventService::EventRange::const_iterator Next() const;
        Controllers::CalendarEventService::EventRange::const_iterator Prev() const;

      private:
        lv_obj_t* container1;

        Controllers::CalendarEventService::EventRange const& events;
        Controllers::CalendarEventService::EventRange::const_iterator curEv;

        Controllers::DateTime& dateTimeController;
      };

      CalendarTimeline::CalendarTimeline(DisplayApp* app,
                                         Controllers::CalendarEventService& calendarEvent,
                                         Controllers::DateTime& dateTimeController)
        : Screen(app), calendarEvent(calendarEvent), dateTimeController(dateTimeController) {
        calendarEvent.ClearOldEvents();
        calendarEvent.NotifyFreeSpace();
        page = std::make_unique<CalendarTimelinePage>(calendarEvent.GetEvents(), std::begin(calendarEvent.GetEvents()), dateTimeController);
      }

      CalendarTimeline::~CalendarTimeline() {
        lv_obj_clean(lv_scr_act());
      }

      bool CalendarTimeline::OnTouchEvent(TouchEvents event) {
        switch (event) {
          case TouchEvents::SwipeDown: {
            //   Controllers::NotificationManager::Notification previousNotification;
            //   if (validDisplay)
            //     previousNotification = notificationManager.GetPrevious(currentId);
            //   else
            //     previousNotification = notificationManager.GetLastNotification();

            //   if (!previousNotification.valid)
            //     return true;

            // validDisplay = true;
            // currentId = previousNotification.id;
            if (page->HasPrev()) {
              auto ev = page->Prev();
              page.reset();
              app->SetFullRefresh(DisplayApp::FullRefreshDirections::Down);
              page = std::make_unique<CalendarTimelinePage>(calendarEvent.GetEvents(), ev, dateTimeController);
            }
            return true;
          }
          case TouchEvents::SwipeUp: {
            if (page->HasNext()) {
              auto ev = page->Next();
              page.reset();
              app->SetFullRefresh(DisplayApp::FullRefreshDirections::Up);
              page = std::make_unique<CalendarTimelinePage>(calendarEvent.GetEvents(), ev, dateTimeController);
            }
            return true;
          }
          default:
            return false;
        }
      }

      CalendarTimelinePage::CalendarTimelinePage(Controllers::CalendarEventService::EventRange const& events,
                                                 Controllers::CalendarEventService::EventRange::const_iterator evIt,
                                                 Controllers::DateTime& dateTimeController)
        : events(events), curEv(evIt), dateTimeController(dateTimeController) {
        container1 = lv_cont_create(lv_scr_act(), nullptr);

        lv_obj_set_style_local_bg_color(container1, LV_CONT_PART_MAIN, LV_STATE_DEFAULT, LV_COLOR_BLACK);
        lv_obj_set_style_local_pad_all(container1, LV_CONT_PART_MAIN, LV_STATE_DEFAULT, 0);
        lv_obj_set_style_local_pad_ver(container1, LV_CONT_PART_MAIN, LV_STATE_DEFAULT, 5);
        lv_obj_set_style_local_pad_inner(container1, LV_CONT_PART_MAIN, LV_STATE_DEFAULT, 4);
        lv_obj_set_style_local_border_width(container1, LV_CONT_PART_MAIN, LV_STATE_DEFAULT, 0);

        lv_obj_set_pos(container1, 0, 24);
        lv_obj_set_size(container1, LV_HOR_RES, 190);

        lv_cont_set_layout(container1, LV_LAYOUT_COLUMN_LEFT);
        lv_cont_set_fit(container1, LV_FIT_TIGHT);

        lv_obj_t* pages_indication = lv_label_create(lv_scr_act(), nullptr);
        lv_label_set_text_fmt(pages_indication, "%s%s", HasNext() ? "▼" : " ", HasPrev() ? "▲" : " ");
        lv_obj_align(pages_indication, nullptr, LV_ALIGN_IN_TOP_RIGHT, 0, 0);

        lv_obj_t* date_ev = lv_label_create(lv_scr_act(), nullptr);
        lv_obj_set_style_local_text_color(date_ev, LV_LABEL_PART_MAIN, LV_STATE_DEFAULT, LV_COLOR_MAKE(0xb0, 0xb0, 0xb0));
        lv_label_set_text_static(date_ev, "Timeline");
        lv_label_set_long_mode(date_ev, LV_LABEL_LONG_SROLL);
        lv_obj_set_width(date_ev, LV_HOR_RES - 30);
        lv_obj_align(date_ev, nullptr, LV_ALIGN_IN_TOP_LEFT, 0, 0);

        if (curEv != std::end(events)) {
          SetDate(date_ev, *curEv);
          AddEvent(*curEv);

          auto nextEv = std::next(curEv);
          if (nextEv != std::end(events)) {
            if (curEv->GetStartDays() != nextEv->GetStartDays()) {
              lv_obj_t* date_ev2 = lv_label_create(container1, nullptr);
              lv_obj_set_style_local_text_color(date_ev2, LV_LABEL_PART_MAIN, LV_STATE_DEFAULT, LV_COLOR_MAKE(0xb0, 0xb0, 0xb0));
              SetDate(date_ev2, *nextEv);
              lv_label_set_long_mode(date_ev2, LV_LABEL_LONG_CROP);
              lv_obj_set_width(date_ev2, LV_HOR_RES);
            } else {
              lv_obj_set_style_local_pad_inner(container1, LV_CONT_PART_MAIN, LV_STATE_DEFAULT, 10);
            }
            AddEvent(*nextEv);
          }
        }
      }

      CalendarTimelinePage::~CalendarTimelinePage() {
        lv_obj_clean(lv_scr_act());
      }

      void CalendarTimelinePage::AddEvent(Controllers::CalendarEventService::CalendarEvent const& ev) {
        lv_obj_t* cont_event = lv_cont_create(container1, nullptr);

        lv_obj_set_style_local_bg_color(cont_event, LV_CONT_PART_MAIN, LV_STATE_DEFAULT, LV_COLOR_MAKE(0x07, 0x5e, 0x88));
        lv_obj_set_style_local_pad_all(cont_event, LV_CONT_PART_MAIN, LV_STATE_DEFAULT, 5);
        lv_obj_set_style_local_pad_inner(cont_event, LV_CONT_PART_MAIN, LV_STATE_DEFAULT, 5);
        lv_obj_set_style_local_border_width(cont_event, LV_CONT_PART_MAIN, LV_STATE_DEFAULT, 0);

        lv_obj_set_size(cont_event, LV_HOR_RES, 0);

        lv_cont_set_layout(cont_event, LV_LAYOUT_COLUMN_LEFT);
        lv_cont_set_fit2(cont_event, LV_FIT_PARENT, LV_FIT_TIGHT);

        lv_obj_t* ev_title_label = lv_label_create(cont_event, nullptr);
        lv_obj_set_style_local_text_color(ev_title_label, LV_LABEL_PART_MAIN, LV_STATE_DEFAULT, LV_COLOR_MAKE(0xff, 0xb0, 0x0));
        lv_label_set_text(ev_title_label, ev.GetTitle());
        RemoveNl(ev_title_label);
        lv_label_set_long_mode(ev_title_label, LV_LABEL_LONG_CROP);
        lv_obj_set_width(ev_title_label, LV_HOR_RES);

        lv_obj_t* ev_loc_label = lv_label_create(cont_event, nullptr);
        lv_obj_set_style_local_text_color(ev_loc_label, LV_LABEL_PART_MAIN, LV_STATE_DEFAULT, LV_COLOR_MAKE(0xff, 0xb0, 0x0));
        lv_label_set_text(ev_loc_label, ev.GetLocation());
        RemoveNl(ev_loc_label);
        lv_label_set_long_mode(ev_loc_label, LV_LABEL_LONG_CROP);
        lv_obj_set_width(ev_loc_label, LV_HOR_RES);

        lv_obj_t* ev_time_label = lv_label_create(cont_event, nullptr);
        lv_obj_set_style_local_text_color(ev_time_label, LV_LABEL_PART_MAIN, LV_STATE_DEFAULT, LV_COLOR_MAKE(0xff, 0xb0, 0x0));
        // lv_label_set_text_static(ev_time_label, "11:30 AM - 12:30 PM");
        auto start = ev.GetStartTimePoint();
        auto dp = date::floor<date::days>(start);
        auto time = date::make_time(start - dp);
        std::uint8_t hours = time.hours().count();
        std::uint8_t minutes = time.minutes().count();

        auto duration = date::make_time(std::chrono::seconds {ev.durationInSeconds});
        std::uint32_t hdur = duration.hours().count();
        std::uint32_t mdur = duration.minutes().count();
        if (hours == 0 && minutes == 0 && hdur == 24 && mdur == 0) {
          lv_label_set_text_static(ev_time_label, "All day");
        } else {
          if (hdur != 0) {
            if (mdur != 0) {
              lv_label_set_text_fmt(ev_time_label, "%02d:%02d (%dh%dm)", hours, minutes, hdur, mdur);
            } else {
              lv_label_set_text_fmt(ev_time_label, "%02d:%02d (%dh)", hours, minutes, hdur);
            }
          } else {
            if (mdur != 0) {
              lv_label_set_text_fmt(ev_time_label, "%02d:%02d (%dm)", hours, minutes, mdur);
            } else {
              lv_label_set_text_fmt(ev_time_label, "%02d:%02d", hours, minutes);
            }
          }
        }

        lv_label_set_long_mode(ev_time_label, LV_LABEL_LONG_CROP);
        lv_obj_set_width(ev_time_label, LV_HOR_RES);
      }

      void CalendarTimelinePage::SetDate(lv_obj_t* label, Controllers::CalendarEventService::CalendarEvent const& ev) {
        auto dp = ev.GetStartDays();
        auto yearMonthDay = date::year_month_day(dp);

        auto year = static_cast<int>(yearMonthDay.year());
        auto month = static_cast<unsigned>(yearMonthDay.month());
        auto day = static_cast<unsigned>(yearMonthDay.day());
        auto dayOfWeek = static_cast<Pinetime::Controllers::DateTime::Days>(date::weekday(yearMonthDay).iso_encoding());

        auto todayDays = date::floor<date::days>(dateTimeController.CurrentDateTime());
        if (todayDays == dp) {
          lv_label_set_text_fmt(label, "%d/%02u/%02u today", year, month, day);
        } else if ((todayDays + date::days {1}) == dp) {
          lv_label_set_text_fmt(label, "%d/%02u/%02u tomorrow", year, month, day);
        } else if (dp < todayDays) {
          lv_label_set_text_fmt(label, "%d/%02u/%02u past", year, month, day);
        } else {
          // TODO: weekday
          lv_label_set_text_fmt(label, "%d/%02u/%02u", year, month, day);
        }
      }

      Controllers::CalendarEventService::EventRange::const_iterator CalendarTimelinePage::Next() const {
        auto n = std::next(curEv);
        if (n != std::end(events)) {
          return ++n;
        } else {
          return n;
        }
      }
      Controllers::CalendarEventService::EventRange::const_iterator CalendarTimelinePage::Prev() const {
        auto n = std::prev(curEv);
        if (n != std::begin(events)) {
          return --n;
        } else {
          return n;
        }
      }
    }
  }
}
