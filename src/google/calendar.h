#pragma once
#include <time.h>

class GoogleCalendarEvent {
public:
    const char *summary;
    const time_t start;
    const time_t end;
};


class GoogleCalendarEventList {
public:
    int len;
    GoogleCalendarEvent *events;
};



class GoogleCalendar {
public:
    GoogleCalendarEventList *getEvents(const char *accessToken, time_t start, time_t end);
};