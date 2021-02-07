#pragma once
#include <time.h>

// single event
class GoogleCalendarEvent {
public:
    GoogleCalendarEvent(const char *summary, const char *start, const char *end);
    const char *summary();
    const char *start();
    const char *end();
private:
    // fixed length
    char _summary[200];
    char _start[30];
    char _end[30];
};

// event list
class GoogleCalendarEventList {
public:
    GoogleCalendarEventList(int maxLength);
    ~GoogleCalendarEventList();
    bool add(GoogleCalendarEvent &event);
    GoogleCalendarEvent *get (int pos) const;
    int length() const {return _length;};
    
private:
    int _length;
    int _maxLength;
    GoogleCalendarEvent **_events;
};



class GoogleCalendar {
public:
    static GoogleCalendarEventList *getEvents(const char *accessToken, const char *calendarId, struct tm *start, struct tm *end);
};