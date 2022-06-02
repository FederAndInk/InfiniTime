#pragma once

#include "displayapp/screens/Screen.h"
#include <lvgl/lvgl.h>

#include <memory>

namespace Pinetime {
  namespace Controllers {
    class CalendarEventService;
    class DateTime;
  }

  namespace Applications {
    namespace Screens {
      class CalendarTimelinePage;

      class CalendarTimeline : public Screen {
      public:
        explicit CalendarTimeline(DisplayApp* app,
                                  Controllers::CalendarEventService& calendarEvent,
                                  Controllers::DateTime& dateTimeController);
        ~CalendarTimeline() override;

        bool OnTouchEvent(TouchEvents event) override;

      private:
        Controllers::CalendarEventService& calendarEvent;
        Controllers::DateTime& dateTimeController;
        std::unique_ptr<CalendarTimelinePage> page;
      };
    }
  }
}
