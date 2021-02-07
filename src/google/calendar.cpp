#include "calendar.h"
#include <time.h>
#include <ArduinoJson.h>
#include "../http/http.h"

const int MAX_EVENT_COUNT = 20;
const char *GOOGLE_CALENDAR_LIST = "https://www.googleapis.com/calendar/v3/users/me/calendarList";
const char *GOOGLE_CALENDAR_EVENT_LIST = "https://www.googleapis.com/calendar/v3/calendars/"; // + "/calendarId/events"
String timeToRFC3339(struct tm *tm) ;

GoogleCalendarEvent::GoogleCalendarEvent(const char *summary, const char *start, const char *end) {
    strncpy(_summary, summary, sizeof(_summary) -1);
    strncpy(_start, start, sizeof(_start) -1);
    strncpy(_end, end, sizeof(_end) -1);
}
const char *GoogleCalendarEvent::summary() {
    return _summary;
}
const char *GoogleCalendarEvent::start() {
    return _start;
}
const char *GoogleCalendarEvent::end() {
    return _end;
}


GoogleCalendarEventList::GoogleCalendarEventList(int maxLength) : _length(0), _maxLength(maxLength) {
    _events = new GoogleCalendarEvent*[maxLength];
}
GoogleCalendarEventList::~GoogleCalendarEventList() {
    delete[] _events;
}
bool GoogleCalendarEventList::add(GoogleCalendarEvent &event) {
    if (_length == _maxLength) {
        return false;
    }
    _events[_length] = new GoogleCalendarEvent(event);
    _length++;
    return true;
}
GoogleCalendarEvent *GoogleCalendarEventList::get (int pos) const {
    return _events[pos];
}

GoogleCalendarEventList *GoogleCalendar::getEvents(const char *accessToken, struct tm *start, struct tm *end) {
    MyHTTPClient client;
    KeyValues headers(1);
    headers.add("Authorization", accessToken);

    KeyValues data(5);
    data.add("timeMax", timeToRFC3339(start));
    data.add("timeMin", timeToRFC3339(end));
    data.add("maxResults", String(MAX_EVENT_COUNT));

    String res = client.post(GOOGLE_CALENDAR_EVENT_LIST, &headers, &data);

    if (res == "") {
        return NULL;
    }

    DynamicJsonDocument doc(30000);
    deserializeJson(doc, res);
   
    GoogleCalendarEventList *eventList = new GoogleCalendarEventList(MAX_EVENT_COUNT);
    JsonArray items = doc["items"];
    for (int i = 0 ; i < items.size() ; i++) {
        const char *summary = items[i]["summary"];
        const char *start = items[i]["start"];
        const char *end = items[i]["end"];
        GoogleCalendarEvent event(summary, start, end);

        eventList->add(event);
    }

    return eventList;
}


String timeToRFC3339(struct tm *tm) {
    char buf[30];
    strftime(buf, sizeof(buf)-1, "%Y-%m-%dT%H:%M:%S.%z", tm);
    return String(buf);
}