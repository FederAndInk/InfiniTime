#include <components/ble/CalendarEventService.h>

#include <catch2/catch_test_macros.hpp>

#include <array>
#include <algorithm>
#include <cstdint>
#include <iterator>
#include <list>
#include <random>
#include <utility>
#include <vector>

using namespace std::string_literals;

TEST_CASE("CalendarEvent", "[CalendarEvent]") {
  Pinetime::Controllers::CalendarEventService::CalendarEvent event;
  event.id = 0;
  event.durationInSeconds = 60;
  event.timestamp = 1653939820;
  event.strings = {"Title1\0Location1\0Description1"};
  REQUIRE(event.GetTitle() == "Title1"s);
  REQUIRE(event.GetLocation() == "Location1"s);
  REQUIRE(event.GetDescription() == "Description1"s);

  event.strings = {"LongTitle2                                        \0"
                   "LongLocation2                                     \0"
                   "LongDescription2                                  "};
  REQUIRE(event.GetTitle() == "LongTitle2                                        "s);
  REQUIRE(event.GetLocation() == "LongLocation2                                     "s);
  REQUIRE(event.GetDescription() == "LongDescription2                                  "s);

  event.strings = {"\0"
                   "LongLocation2                                     \0"
                   "LongDescription2                                  "};
  REQUIRE(event.GetTitle() == ""s);
  REQUIRE(event.GetLocation() == "LongLocation2                                     "s);
  REQUIRE(event.GetDescription() == "LongDescription2                                  "s);

  event.strings = {"LongTitle2                                        \0"
                   "\0"
                   "LongDescription2                                  "};
  REQUIRE(event.GetTitle() == "LongTitle2                                        "s);
  REQUIRE(event.GetLocation() == ""s);
  REQUIRE(event.GetDescription() == "LongDescription2                                  "s);

  event.strings = {"LongTitle2                                        \0"
                   "LongLocation2                                     \0"
                   ""};
  REQUIRE(event.GetTitle() == "LongTitle2                                        "s);
  REQUIRE(event.GetLocation() == "LongLocation2                                     "s);
  REQUIRE(event.GetDescription() == ""s);

  event.strings = {"\0"
                   "\0"
                   "\0NOPE"};
  REQUIRE(event.GetTitle() == ""s);
  REQUIRE(event.GetLocation() == ""s);
  REQUIRE(event.GetDescription() == ""s);

  event.strings = {"LongTitle3                                        "
                   "Long                                              "
                   "Long                                              \0\0"};
  REQUIRE(event.GetTitle() == "LongTitle3                                        "
                              "Long                                              "
                              "Long                                              "s);
  REQUIRE(event.GetLocation() == ""s);
  REQUIRE(event.GetDescription() == ""s);

  event.strings = {"\0\0LongDescription3                                  "
                   "Long                                              "
                   "Long                                              "};
  REQUIRE(event.GetTitle() == ""s);
  REQUIRE(event.GetLocation() == ""s);
  REQUIRE(event.GetDescription() == "LongDescription3                                  "
                                    "Long                                              "
                                    "Long                                              "s);
}
