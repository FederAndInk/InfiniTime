#include "components/ble/CalendarEventService.h"

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
}

namespace Pinetime {
  namespace Controllers {
    constexpr std::size_t CalendarEventService::CalendarEvent::maxTitleSize;

    CalendarEventService::CalendarEventService()
      : characteristicDefinition {{{.uuid = &calEventAddCharUuid.u,
                                    .access_cb =
                                      [](uint16_t conn_handle, uint16_t attr_handle, ble_gatt_access_ctxt* ctxt, void* arg) -> int {
                                      return static_cast<CalendarEventService*>(arg)->OnCommand(conn_handle, attr_handle, ctxt);
                                    },
                                    .arg = this,
                                    .flags = BLE_GATT_CHR_F_WRITE},
                                   {nullptr}}},
        serviceDefinition {
          {{.type = BLE_GATT_SVC_TYPE_PRIMARY, .uuid = &calEventUuid.u, .characteristics = characteristicDefinition.data()}, {0}}} {
    }

    void CalendarEventService::Init() {
      uint8_t res = ble_gatts_count_cfg(serviceDefinition.data());
      ASSERT(res == 0);

      res = ble_gatts_add_svcs(serviceDefinition.data());
      ASSERT(res == 0);
    }

    int CalendarEventService::OnCommand(uint16_t conn_handle, uint16_t attr_handle, ble_gatt_access_ctxt* ctxt) {
      if (ctxt->op == BLE_GATT_ACCESS_OP_WRITE_CHR) {
        // Ignore notifications with empty message
        const auto packetLen = OS_MBUF_PKTLEN(ctxt->om);
        if (packetLen <= CalendarEvent::headerSize) {
          return BLE_ATT_ERR_INVALID_ATTR_VALUE_LEN;
        }

        const auto titleSize = std::min(CalendarEvent::maxTitleSize, packetLen - CalendarEvent::headerSize);

        decltype(CalendarEvent::timestamp) newEvTimestamp {};
        int res = os_mbuf_copydata(ctxt->om, CalendarEvent::timestampOffset, sizeof(CalendarEvent::timestamp), &newEvTimestamp);
        ASSERT(res == 0);

        CalendarEvent* newEv = [&]() -> CalendarEvent* {
          if (calEvents.capacity() == calEvents.size()) {
            // if the received event starts before the last stored event
            // -> replace the last event by the new
            if (calEvents.back().timestamp > newEvTimestamp) {
              // TODO: send desync lastEv.id back
              return &calEvents.back();
            } else {
              // else, reject the event
              return nullptr;
            }
          } else {
            return static_cast<CalendarEvent*>(FindSpotForEvent(newEvTimestamp));
          }
        }();
        if (newEv) {
          res = os_mbuf_copydata(ctxt->om, CalendarEvent::idOffset, sizeof(newEv->id), &newEv->id);
          ASSERT(res == 0);
          res = os_mbuf_copydata(ctxt->om, CalendarEvent::durationOffset, sizeof(newEv->durationInSeconds), &newEv->durationInSeconds);
          ASSERT(res == 0);
          newEv->timestamp = newEvTimestamp;
          res = os_mbuf_copydata(ctxt->om, CalendarEvent::titleOffset, titleSize, newEv->title.data());
          newEv->title[titleSize] = '\0';
          ASSERT(res == 0);
        }
      }
      return BLE_ERR_SUCCESS;
    }

    CalendarEventService::EventRange::iterator CalendarEventService::FindSpotForEvent(std::int32_t timestamp) {
      return calEvents.emplace(std::find_if(std::begin(calEvents), std::end(calEvents), [timestamp](CalendarEvent const& ev) {
        return ev.timestamp > timestamp;
      }));
    }

  }
}