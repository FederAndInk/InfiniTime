#pragma once

#include "displayapp/screens/Screen.h"
#include "displayapp/LittleVgl.h"
#include "components/timer/TimerController.h"

#include <array>

namespace Pinetime {
  namespace Applications {
    namespace Screens {
      class TeaTimer : public Screen {
      public:
        explicit TeaTimer(DisplayApp* app, Controllers::TimerController& timerController);
        ~TeaTimer() override;

      private:
        void OnValueChangedEvent(lv_obj_t* obj, uint32_t buttonId);

        Controllers::TimerController& timerController;
        lv_obj_t* btnm1;
      };
    }
  }
}
