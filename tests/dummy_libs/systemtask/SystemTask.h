#pragma once

#include <cstdint>

namespace Pinetime {
  namespace Controllers {
    class NimbleController {
    public:
      std::uint16_t connHandle() {
        return -1;
      }
    };
  }
  namespace System {
    class SystemTask {
    public:
      Pinetime::Controllers::NimbleController& nimble() {
        return nimbleController;
      };

    private:
      Pinetime::Controllers::NimbleController nimbleController;
    };
  }
}
