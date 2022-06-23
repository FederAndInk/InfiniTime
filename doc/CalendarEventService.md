# CalendarEvent Service

## Introduction

The Calendar Event service is receiving and storing a fixed amount of calendar events.

For details on the course of the project please read: [this](https://github.com/FederAndInk/my_projects/blob/main/infinitime_calendar_events.md)

## Service

The service UUID is **00050000-78fc-48fe-8e23-433b3a1942d0**

- header of the service: [/src/components/ble/CalendarEventService.h](/src/components/ble/CalendarEventService.h)
- source of the service: [/src/components/ble/CalendarEventService.cpp](/src/components/ble/CalendarEventService.cpp)

## Characteristics

### Add event (write) (UUID 00050001-78fc-48fe-8e23-433b3a1942d0)

Characteristic value layout:

- (`int64`) `id`
- (`int32`) `durationInSeconds`
- (`int32`) `timestamp`
- (`int32`) `color`
- (`char[150]`) `strings`

The `strings` contains the title, location and description separated by `'\0'`,
it does not end with a `'\0'` as the size is known,
that's why the strings array has is of size `150+1` to put a `'\0'` at the end no matter what

If a received event has the same `id` as one already stored, the later is replaced by the former.

Before adding the event, past events are cleaned up.

### Remove event (write) (UUID 00050002-78fc-48fe-8e23-433b3a1942d0)

Remove an event by its id.

Characteristic value layout:

- (`int64`) `id`

### event rejected (notify) (UUID 00050003-78fc-48fe-8e23-433b3a1942d0)

When receiving a new event, if there is no more space to store it the event starting last is "rejected",
meaning it is removed and the companion app is notified the event is no more stored on the device.

### number free spots available for n events (notify) (UUID 00050004-78fc-48fe-8e23-433b3a1942d0)

On certain occasion (when loading the timeline app for now), the companion app is notified if there is
space to receive previously rejected events.

## [FlatLinkedList](/src/FlatLinkedList.h) container

FlatLinkedList is a linked list with its capacity defined at compile time
it is stored in an array so there is no dynamic allocations, and works on POD types having these member variables:

- `std::uint8_t fllNextIdx;`
- `std::uint8_t fllPrevIdx;`

It was designed so elements could be inserted or removed anywhere to keep them sorted.

It is especially appropriate for storing events sorted by their start date whatever the order they are added.
It is also useful to remove any events quickly if either it is finished or removed by the user.
When sorting by start date the first event isn't necessarily the first to end.

Moreover, random access is not required, for a page based app only the previous and next event matter.

## UI

<img src="ui/timeline_app.gif" width="313"/>

The timeline application is accessible directly by swiping to the right from the main clock screen.
The UI contains pages of 2 events in cards. Arrows are used to denote if there is a page up or downward.

The first line contains the event's title, the second the location and the last is for the start time with the duration in parentheses.
The date is shown before the event card, if the event is starting today or tomorrow it is shown.

Before the UI is loaded, past events are cleaned up (they also get cleaned up on [add event characteristic write](#add-event-write-uuid-00050001-78fc-48fe-8e23-433b3a1942d0) ).

## Areas for improvement

- [ ] Show event details on click
- [x] Show today/tomorrow
- [x] Show weekday
- [x] Use month name
- [x] Use relative time for event starting in less than ~24h (e.g. "in 23min")
- [x] Show "Ends in" for ongoing events
- [x] Use the calendar color as a background for the event
- [x] Highlight with a border events starting today or in the next 6 hours

## Testing

Tests have been written using the catch2 v3 library

- tests for the FlatLinkedList: [test_FlatLinkedList.cpp](/tests/test_FlatLinkedList.cpp)
- tests for a CalendarEvent: [test_CalendarEvent.cpp](/tests/test_CalendarEvent.cpp)

[/tests/dummy_libs/](/tests/dummy_libs/) is there to make the tests compile in the host environement,
there is definitions and the functions do nothing.
