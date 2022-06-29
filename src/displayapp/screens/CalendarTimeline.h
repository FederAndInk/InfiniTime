#pragma once

#include "displayapp/screens/Screen.h"
#include "components/ble/CalendarEventService.h"

#include <lvgl/lvgl.h>

#include <memory>

namespace Pinetime {
  namespace Controllers {
    class DateTime;
  }

  namespace Applications {
    namespace Screens {
      class CalendarTimelinePage;
      class CalendarTimelineEventDetail;

      class CalendarTimeline : public Screen {
      public:
        explicit CalendarTimeline(DisplayApp* app,
                                  Controllers::CalendarEventService& calendarEvent,
                                  Controllers::DateTime& dateTimeController);
        ~CalendarTimeline() override;

        bool OnTouchEvent(TouchEvents event) override;

        void OnClickCardEvent(Controllers::CalendarEventService::CalendarEvent const& event);

        struct EventCardData {
          CalendarTimeline* calendarTimeline {};
          Controllers::CalendarEventService::CalendarEvent const* event {};
        };

        EventCardData* StoreEventData(Controllers::CalendarEventService::CalendarEvent const& ev);
        void ClearEventData();

      private:
        Controllers::CalendarEventService& calendarEvent;
        Controllers::DateTime& dateTimeController;

        std::unique_ptr<CalendarTimelinePage> page;
        Controllers::CalendarEventService::EventRange::const_iterator currentEvent;
        std::array<EventCardData, 2> eventsCardData {};

        std::unique_ptr<CalendarTimelineEventDetail> eventDetail;
      };
    }
  }
}
