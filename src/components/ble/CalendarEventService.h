#pragma once

#include "FlatLinkedList.h"

#define min // workaround: nimble's min/max macros conflict with libstdc++
#define max
#include <host/ble_gap.h>
#include <host/ble_uuid.h>
#undef max
#undef min

#include <cstdint>
#include <array>

namespace Pinetime {
  namespace Controllers {
    class CalendarEventService {
    public:
      explicit CalendarEventService();

      void Init();

      int OnCommand(uint16_t conn_handle, uint16_t attr_handle, ble_gatt_access_ctxt* ctxt);

      struct CalendarEvent {
        std::int64_t id;
        std::int32_t durationInSeconds;
        std::int32_t timestamp;

        static constexpr std::size_t maxTitleSize {50};
        std::array<char, maxTitleSize + 1> title;

        static constexpr std::size_t idOffset {0};
        static constexpr std::size_t durationOffset {sizeof(id)};
        static constexpr std::size_t timestampOffset {durationOffset + sizeof(durationInSeconds)};
        static constexpr std::size_t titleOffset {timestampOffset + sizeof(timestamp)};
        static constexpr std::size_t headerSize {sizeof(id) + sizeof(durationInSeconds) + sizeof(timestamp)};

        std::uint8_t fllNextIdx;
        std::uint8_t fllPrevIdx;

        char const* GetTitle() const {
          return title.data();
        }

        char const* GetDescription() const {
          return title.data();
        }

        char const* GetLocation() const {
          return title.data();
        }
      };

      using EventRange = FlatLinkedList<CalendarEvent, 10>;

      EventRange const& GetEvents() const {
        return calEvents;
      }

    private:
      EventRange::iterator FindSpotForEvent(std::int32_t timestamp);

      std::array<ble_gatt_chr_def, 2> characteristicDefinition;
      std::array<ble_gatt_svc_def, 2> serviceDefinition;

      EventRange calEvents;
    };
  }
}
