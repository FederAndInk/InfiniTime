#pragma once

#include "displayapp/screens/Screen.h"
#include <lvgl/lvgl.h>

namespace Pinetime {
  namespace Controllers {
    class CalendarEventService;
  }

  namespace Applications {
    namespace Screens {
      class CalendarTimeline : public Screen {
      public:
        explicit CalendarTimeline(DisplayApp* app, Pinetime::Controllers::CalendarEventService& calendarEvent);
        ~CalendarTimeline() override;

      private:
        Pinetime::Controllers::CalendarEventService& calendarEvent;
      };
    }
  }
}
