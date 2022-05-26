#pragma once

#include "displayapp/screens/Screen.h"
#include <lvgl/lvgl.h>

#include <memory>

namespace Pinetime {
  namespace Controllers {
    class CalendarEventService;
  }

  namespace Applications {
    namespace Screens {
      class CalendarTimelinePage;

      class CalendarTimeline : public Screen {
      public:
        explicit CalendarTimeline(DisplayApp* app, Controllers::CalendarEventService& calendarEvent);
        ~CalendarTimeline() override;

        bool OnTouchEvent(TouchEvents event) override;

      private:
        Controllers::CalendarEventService& calendarEvent;
        std::unique_ptr<CalendarTimelinePage> page;
      };
    }
  }
}
