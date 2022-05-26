#include "displayapp/screens/CalendarTimeline.h"

#include "components/ble/CalendarEventService.h"
#include "displayapp/DisplayApp.h"

namespace Pinetime {
  namespace Applications {
    namespace Screens {
      class CalendarTimelinePage {
      public:
        explicit CalendarTimelinePage(Controllers::CalendarEventService::EventRange const& events,
                                      Controllers::CalendarEventService::EventRange::const_iterator evIt);
        ~CalendarTimelinePage();

        void AddEvent(Controllers::CalendarEventService::CalendarEvent const& ev);

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
      };

      CalendarTimeline::CalendarTimeline(DisplayApp* app, Controllers::CalendarEventService& calendarEvent)
        : Screen(app), calendarEvent(calendarEvent) {
        page = std::make_unique<CalendarTimelinePage>(calendarEvent.GetEvents(), std::begin(calendarEvent.GetEvents()));
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
              page = std::make_unique<CalendarTimelinePage>(calendarEvent.GetEvents(), ev);
            }
            return true;
          }
          case TouchEvents::SwipeUp: {
            if (page->HasNext()) {
              auto ev = page->Next();
              page.reset();
              app->SetFullRefresh(DisplayApp::FullRefreshDirections::Up);
              page = std::make_unique<CalendarTimelinePage>(calendarEvent.GetEvents(), ev);
            }
            return true;
          }
          default:
            return false;
        }
      }

      CalendarTimelinePage::CalendarTimelinePage(Controllers::CalendarEventService::EventRange const& events,
                                                 Controllers::CalendarEventService::EventRange::const_iterator evIt)
        : events(events), curEv(evIt) {
        container1 = lv_cont_create(lv_scr_act(), nullptr);

        lv_obj_set_style_local_bg_color(container1, LV_CONT_PART_MAIN, LV_STATE_DEFAULT, LV_COLOR_BLACK);
        lv_obj_set_style_local_pad_all(container1, LV_CONT_PART_MAIN, LV_STATE_DEFAULT, 0);
        lv_obj_set_style_local_pad_ver(container1, LV_CONT_PART_MAIN, LV_STATE_DEFAULT, 5);
        lv_obj_set_style_local_pad_inner(container1, LV_CONT_PART_MAIN, LV_STATE_DEFAULT, 5);
        lv_obj_set_style_local_border_width(container1, LV_CONT_PART_MAIN, LV_STATE_DEFAULT, 0);

        lv_obj_set_pos(container1, 0, 30);
        lv_obj_set_size(container1, LV_HOR_RES, 190);

        lv_cont_set_layout(container1, LV_LAYOUT_COLUMN_LEFT);
        lv_cont_set_fit(container1, LV_FIT_TIGHT);

        lv_obj_t* alert_count = lv_label_create(lv_scr_act(), nullptr);
        lv_label_set_text_fmt(alert_count, "%s%s", HasNext() ? "▼" : " ", HasPrev() ? "▲" : " ");
        lv_obj_align(alert_count, nullptr, LV_ALIGN_IN_TOP_RIGHT, 0, 0);

        lv_obj_t* alert_type = lv_label_create(lv_scr_act(), nullptr);
        lv_obj_set_style_local_text_color(alert_type, LV_LABEL_PART_MAIN, LV_STATE_DEFAULT, LV_COLOR_MAKE(0xb0, 0xb0, 0xb0));
        lv_label_set_text_static(alert_type, "Timeline");

        lv_label_set_long_mode(alert_type, LV_LABEL_LONG_SROLL_CIRC);
        lv_obj_set_width(alert_type, 180);
        lv_obj_align(alert_type, nullptr, LV_ALIGN_IN_TOP_LEFT, 0, 0);
        if (curEv != std::end(events)) {
          AddEvent(*curEv);
          auto nextEv = std::next(curEv);
          if (nextEv != std::end(events)) {
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
        lv_label_set_long_mode(ev_title_label, LV_LABEL_LONG_DOT);
        lv_obj_set_width(ev_title_label, LV_HOR_RES);
        lv_label_set_text_static(ev_title_label, ev.GetTitle());

        lv_obj_t* ev_loc_label = lv_label_create(cont_event, nullptr);
        lv_obj_set_style_local_text_color(ev_loc_label, LV_LABEL_PART_MAIN, LV_STATE_DEFAULT, LV_COLOR_MAKE(0xff, 0xb0, 0x0));
        lv_label_set_long_mode(ev_loc_label, LV_LABEL_LONG_DOT);
        lv_obj_set_width(ev_loc_label, LV_HOR_RES);
        lv_label_set_text_static(ev_loc_label, ev.GetLocation());

        lv_obj_t* ev_time_label = lv_label_create(cont_event, nullptr);
        lv_obj_set_style_local_text_color(ev_time_label, LV_LABEL_PART_MAIN, LV_STATE_DEFAULT, LV_COLOR_MAKE(0xff, 0xb0, 0x0));
        lv_label_set_long_mode(ev_time_label, LV_LABEL_LONG_CROP);
        lv_obj_set_width(ev_time_label, LV_HOR_RES);
        lv_label_set_text_static(ev_time_label, "11:30 AM - 12:30 PM");
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
