#pragma once
#include <time.h>
#include <vector>
#include <Arduino.h>

// single event
class GoogleCalendarEvent {
public:
    GoogleCalendarEvent(const char *summary, const char *start, const char *end);
    const char *summary();
    const char *start();
    const char *end();
    // YYYY-MM-DD hh:mm - hh:mm
    String startEndDateTimePeriodString();
    // hh:mm - hh:mm
    String startEndTimePeriodString();
private:
    // fixed length
    char _summary[200];
    // YYYY-MM-DDThh:mm:ss+HH:MM (RFC3339)
    char _start[30];
    char _end[30];
};

// event list
// TODO: rewrite with std::vector
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


// calendar list
class GoogleCalendarListItem {
public:
    String id;
    String summary;
};

class GoogleCalendar {
public:
    static GoogleCalendarEventList *getEvents(const char *accessToken, const char *calendarId, struct tm *start, struct tm *end);
    static std::vector<GoogleCalendarListItem> getCalendars(const char *accessToken);
};