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
#include <chrono>
#include <date/date.h>

namespace Pinetime {
  namespace Controllers {
    class CalendarEventService {
    public:
      explicit CalendarEventService();

      void Init();

      int OnAddCalendarEvent(uint16_t conn_handle, uint16_t attr_handle, ble_gatt_access_ctxt* ctxt);
      int OnRemoveCalendarEvent(uint16_t conn_handle, uint16_t attr_handle, ble_gatt_access_ctxt* ctxt);

      struct CalendarEvent {
        std::int64_t id;
        std::int32_t durationInSeconds;
        /**
         * @brief start timestamp in the local timezone!
         *
         */
        std::int32_t timestamp;

        static constexpr std::size_t maxStringsSize {150};
        /// title + '\0' + location + '\0' + description + '\0'
        std::array<char, maxStringsSize + 3> strings;

        static constexpr std::size_t idOffset {0};
        static constexpr std::size_t durationOffset {sizeof(id)};
        static constexpr std::size_t timestampOffset {durationOffset + sizeof(durationInSeconds)};
        static constexpr std::size_t stringsOffset {timestampOffset + sizeof(timestamp)};
        static constexpr std::size_t headerSize {sizeof(id) + sizeof(durationInSeconds) + sizeof(timestamp)};

        std::uint8_t fllNextIdx;
        std::uint8_t fllPrevIdx;

        char const* GetTitle() const {
          return strings.data();
        }

        char const* GetDescription() const;

        char const* GetLocation() const;

        std::chrono::system_clock::time_point GetStartTimePoint() const {
          return std::chrono::system_clock::from_time_t(timestamp);
        }

        std::chrono::time_point<std::chrono::system_clock, date::days> GetStartDays() const {
          return date::floor<date::days>(GetStartTimePoint());
        }
      };

      using EventRange = FlatLinkedList<CalendarEvent, 10>;

      EventRange const& GetEvents() const {
        return calEvents;
      }

    private:
      EventRange::iterator FindSpotForEvent(std::int32_t timestamp);

      std::array<ble_gatt_chr_def, 3> characteristicDefinition;
      std::array<ble_gatt_svc_def, 2> serviceDefinition;

      EventRange calEvents;
    };
  }
}
