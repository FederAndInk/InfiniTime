#include "components/ble/CalendarEventService.h"

#include "components/datetime/DateTimeController.h"
#include "systemtask/SystemTask.h"

#include <nrf_log.h>

#include <algorithm>
#include <cstddef>

namespace {
  // 0005yyxx-78fc-48fe-8e23-433b3a1942d0
  constexpr ble_uuid128_t CalEventCharUuid(uint8_t x, uint8_t y) {
    return ble_uuid128_t {.u = {.type = BLE_UUID_TYPE_128},
                          .value = {0xd0, 0x42, 0x19, 0x3a, 0x3b, 0x43, 0x23, 0x8e, 0xfe, 0x48, 0xfc, 0x78, y, x, 0x05, 0x00}};
  }

  // 00050000-78fc-48fe-8e23-433b3a1942d0
  constexpr ble_uuid128_t CalEventBaseUuid() {
    return CalEventCharUuid(0x00, 0x00);
  }

  constexpr ble_uuid128_t calEventUuid {CalEventBaseUuid()};
  constexpr ble_uuid128_t calEventAddCharUuid {CalEventCharUuid(0x00, 0x01)};
  constexpr ble_uuid128_t calEventRemoveCharUuid {CalEventCharUuid(0x00, 0x02)};
  constexpr ble_uuid128_t calEventRejectedCharUuid {CalEventCharUuid(0x00, 0x03)};
  constexpr ble_uuid128_t calEventSpaceAvailableCharUuid {CalEventCharUuid(0x00, 0x04)};

  template <std::size_t N> //
  constexpr char const* NthStr(std::array<char, N> const& strs, std::uint8_t n) {
    auto beginIt = std::begin(strs);
    while (n > 0) {
      beginIt = std::find(beginIt, std::end(strs), '\0');
      // should always be true!
      if (beginIt == std::end(strs) || ++beginIt == std::end(strs)) {
        return "";
      }
      --n;
    }
    return beginIt;
  }

  int dummy_callback(uint16_t conn_handle, uint16_t attr_handle, ble_gatt_access_ctxt* ctxt, void* arg) {
    NRF_LOG_WARNING("dummy callback!");
    // return BLE_ATT_ERR_REQ_NOT_SUPPORTED;
    return 0;
  }
}

namespace Pinetime {
  namespace Controllers {
    constexpr std::size_t CalendarEventService::CalendarEvent::maxStringsSize;

    char const* CalendarEventService::CalendarEvent::GetDescription() const {
      return NthStr(strings, 2);
    }

    char const* CalendarEventService::CalendarEvent::GetLocation() const {
      return NthStr(strings, 1);
    }

    CalendarEventService::CalendarEventService(System::SystemTask& systemTask, DateTime& dateTimeController)
      : characteristicDefinition {{{.uuid = &calEventAddCharUuid.u,
                                    .access_cb =
                                      [](uint16_t conn_handle, uint16_t attr_handle, ble_gatt_access_ctxt* ctxt, void* arg) -> int {
                                      return static_cast<CalendarEventService*>(arg)->OnAddCalendarEvent(conn_handle, attr_handle, ctxt);
                                    },
                                    .arg = this,
                                    .flags = BLE_GATT_CHR_F_WRITE},
                                   {.uuid = &calEventRemoveCharUuid.u,
                                    .access_cb =
                                      [](uint16_t conn_handle, uint16_t attr_handle, ble_gatt_access_ctxt* ctxt, void* arg) -> int {
                                      return static_cast<CalendarEventService*>(arg)->OnRemoveCalendarEvent(conn_handle, attr_handle, ctxt);
                                    },
                                    .arg = this,
                                    .flags = BLE_GATT_CHR_F_WRITE},
                                   {.uuid = &calEventRejectedCharUuid.u,
                                    .access_cb = dummy_callback,
                                    .arg = this,
                                    .flags = BLE_GATT_CHR_F_NOTIFY,
                                    .val_handle = &rejectionHandle},
                                   {.uuid = &calEventSpaceAvailableCharUuid.u,
                                    .access_cb = dummy_callback,
                                    .arg = this,
                                    .flags = BLE_GATT_CHR_F_NOTIFY,
                                    .val_handle = &freeSpaceHandle},
                                   {nullptr}}},
        serviceDefinition {
          {{.type = BLE_GATT_SVC_TYPE_PRIMARY, .uuid = &calEventUuid.u, .characteristics = characteristicDefinition.data()},
           {BLE_GATT_SVC_TYPE_END}}},
        systemTask {systemTask},
        dateTimeController {dateTimeController} {
    }

    void CalendarEventService::Init() {
      uint8_t res = ble_gatts_count_cfg(serviceDefinition.data());
      ASSERT(res == 0);

      res = ble_gatts_add_svcs(serviceDefinition.data());
      ASSERT(res == 0);
    }

    int CalendarEventService::OnAddCalendarEvent(uint16_t conn_handle, uint16_t attr_handle, ble_gatt_access_ctxt* ctxt) {
      ClearOldEvents();
      if (ctxt->op == BLE_GATT_ACCESS_OP_WRITE_CHR) {
        // Ignore notifications with empty message
        const auto packetLen = OS_MBUF_PKTLEN(ctxt->om);
        if (packetLen <= CalendarEvent::headerSize) {
          return BLE_ATT_ERR_INVALID_ATTR_VALUE_LEN;
          NRF_LOG_INFO("Cal ev: empty!");
        }

        const auto strsSize = std::min(CalendarEvent::maxStringsSize, packetLen - CalendarEvent::headerSize);

        decltype(CalendarEvent::id) newEvId {};
        int res = os_mbuf_copydata(ctxt->om, CalendarEvent::idOffset, sizeof(newEvId), &newEvId);
        ASSERT(res == 0);
        NRF_LOG_INFO("Cal ev id: %ld", newEvId);
        // remove existing event with the same id for replacement
        {
          auto it = std::find_if(std::begin(calEvents), std::end(calEvents), [newEvId](CalendarEvent const& ev) {
            return ev.id == newEvId;
          });
          if (it != std::end(calEvents)) {
            NRF_LOG_INFO("Cal ev replace");
            calEvents.erase(it);
          }
        }

        decltype(CalendarEvent::timestamp) newEvTimestamp {};
        res = os_mbuf_copydata(ctxt->om, CalendarEvent::timestampOffset, sizeof(newEvTimestamp), &newEvTimestamp);
        ASSERT(res == 0);
        CalendarEvent* newEv = [&]() -> CalendarEvent* {
          if (calEvents.capacity() == calEvents.size()) {
            NRF_LOG_INFO("Cal ev no more space");
            // if the received event starts before the last stored event
            // -> replace the last event by the new
            if (calEvents.back().timestamp > newEvTimestamp) {
              NRF_LOG_INFO("Cal ev remove last for new");
              RejectEvent(calEvents.back().id);
              calEvents.pop_back();
            } else {
              NRF_LOG_INFO("Cal ev reject");
              // else, reject the event
              RejectEvent(newEvId);
              return nullptr;
            }
          }

          return static_cast<CalendarEvent*>(FindSpotForEvent(newEvTimestamp));
        }();

        if (newEv) {
          newEv->id = newEvId;
          res = os_mbuf_copydata(ctxt->om, CalendarEvent::durationOffset, sizeof(newEv->durationInSeconds), &newEv->durationInSeconds);
          ASSERT(res == 0);
          newEv->timestamp = newEvTimestamp;
          res = os_mbuf_copydata(ctxt->om, CalendarEvent::colorOffset, sizeof(newEv->color), &newEv->color);
          ASSERT(res == 0);
          res = os_mbuf_copydata(ctxt->om, CalendarEvent::stringsOffset, strsSize, newEv->strings.data());
          newEv->strings[strsSize] = '\0';
          ASSERT(res == 0);
        }
      }
      return BLE_ERR_SUCCESS;
    }

    int CalendarEventService::OnRemoveCalendarEvent(uint16_t conn_handle, uint16_t attr_handle, ble_gatt_access_ctxt* ctxt) {
      decltype(CalendarEvent::id) idToRemove {};
      int res = os_mbuf_copydata(ctxt->om, CalendarEvent::idOffset, sizeof(idToRemove), &idToRemove);
      ASSERT(res == 0);
      auto it = std::find_if(std::begin(calEvents), std::end(calEvents), [idToRemove](CalendarEvent const& ev) {
        return ev.id == idToRemove;
      });
      if (it != std::end(calEvents)) {
        NRF_LOG_INFO("Cal ev remove %ld", idToRemove);
        calEvents.erase(it);
        return BLE_ERR_SUCCESS;
      }
      return BLE_ATT_ERR_UNLIKELY;
    }

    CalendarEventService::EventRange::iterator CalendarEventService::FindSpotForEvent(std::int32_t timestamp) {
      return calEvents.emplace(std::find_if(std::begin(calEvents), std::end(calEvents), [timestamp](CalendarEvent const& ev) {
        return ev.timestamp > timestamp;
      }));
    }

    void CalendarEventService::ClearOldEvents() {
      static constexpr std::uint32_t keepTimeInSeconds = 5 * 60;

      auto currentTimeInSeconds =
        std::chrono::duration_cast<std::chrono::seconds>(dateTimeController.CurrentDateTime().time_since_epoch()).count();

      auto it = std::begin(calEvents);
      auto end = std::end(calEvents);
      while (it != end && it->timestamp < currentTimeInSeconds) {
        auto curIt = it++;
        if ((curIt->timestamp + curIt->durationInSeconds + keepTimeInSeconds) < currentTimeInSeconds) {
          NRF_LOG_INFO("Cal ev clearing %ld", curIt->id);
          calEvents.erase(curIt);
        }
      }
    }

    void CalendarEventService::RejectEvent(std::int64_t id) {
      auto* om = ble_hs_mbuf_from_flat(&id, sizeof(id));

      uint16_t connectionHandle = systemTask.nimble().connHandle();

      if (connectionHandle == 0 || connectionHandle == BLE_HS_CONN_HANDLE_NONE) {
        return;
      }

      ble_gattc_notify_custom(connectionHandle, rejectionHandle, om);
    }

    void CalendarEventService::NotifyFreeSpace() {
      std::uint16_t nbFreeSpots = calEvents.capacity() - calEvents.size();
      if (nbFreeSpots == 0) {
        return;
      }
      auto* om = ble_hs_mbuf_from_flat(&nbFreeSpots, sizeof(nbFreeSpots));

      uint16_t connectionHandle = systemTask.nimble().connHandle();

      if (connectionHandle == 0 || connectionHandle == BLE_HS_CONN_HANDLE_NONE) {
        return;
      }

      ble_gattc_notify_custom(connectionHandle, freeSpaceHandle, om);
    }
  }
}
