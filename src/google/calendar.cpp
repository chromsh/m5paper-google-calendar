#include "calendar.h"
#include <time.h>
#include <ArduinoJson.h>
#include "../http/http.h"

const int MAX_EVENT_COUNT = 20;
const char *GOOGLE_CALENDAR_LIST = "https://www.googleapis.com/calendar/v3/users/me/calendarList";
const char *GOOGLE_CALENDAR_EVENT_LIST_PREFIX = "https://www.googleapis.com/calendar/v3/calendars/"; // + "/calendarId/events"
const char *GOOGLE_CALENDAR_EVENT_LIST_POSTFIX = "/events";
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
    _events[_length] = new GoogleCalendarEvent(event.summary(), event.start(), event.end());
    _length++;
    return true;
}
GoogleCalendarEvent *GoogleCalendarEventList::get (int pos) const {
    return _events[pos];
}

GoogleCalendarEventList *GoogleCalendar::getEvents(const char *accessToken, const char *calendarId, struct tm *start, struct tm *end) {
    MyHTTPClient client;
    KeyValues headers(1);
    String bearer = "Bearer ";
    bearer += accessToken;
    headers.add("Authorization", bearer.c_str());
    Serial.println(bearer);

    KeyValues data(3);
    data.add("timeMin", timeToRFC3339(start));
    data.add("timeMax", timeToRFC3339(end));
    data.add("maxResults", String(MAX_EVENT_COUNT));

    String url = GOOGLE_CALENDAR_EVENT_LIST_PREFIX + String(calendarId) + GOOGLE_CALENDAR_EVENT_LIST_POSTFIX;
    String res = client.get(url.c_str(), &headers, &data);

    if (res == "") {
        return NULL;
    }

    Serial.println(res);

    DynamicJsonDocument doc(5000);
    deserializeJson(doc, res);
   
    GoogleCalendarEventList *eventList = new GoogleCalendarEventList(MAX_EVENT_COUNT);
    JsonArray items = doc["items"].as<JsonArray>();
    Serial.println("items = ");
    Serial.println(items.size());
    for (int i = 0 ; i < items.size() ; i++) {
        const char *summary = items[i]["summary"];
        Serial.println(summary);
        const char *start = items[i]["start"]["dateTime"];
        Serial.println(start);
        const char *end = items[i]["end"]["dateTime"];
        Serial.println(end);
        GoogleCalendarEvent event(summary, start, end);

        eventList->add(event);
    }

    return eventList;
}

std::vector<GoogleCalendarListItem> GoogleCalendar::getCalendars(const char *accessToken) {
    std::vector<GoogleCalendarListItem> calendars;

    MyHTTPClient client;
    KeyValues headers(1);
    String bearer = "Bearer ";
    bearer += accessToken;
    headers.add("Authorization", bearer.c_str());

    String res = client.get(GOOGLE_CALENDAR_LIST, &headers, NULL);

    if (res == "") {
        return calendars;
    }

    Serial.println(res);

    DynamicJsonDocument doc(5000);
    deserializeJson(doc, res);
    JsonArray items = doc["items"].as<JsonArray>();
    for (int i = 0 ; i < items.size() ; i++) {
        GoogleCalendarListItem item;
        const char *id = items[i]["id"];
        item.id = id;
        const char *summary = items[i]["summary"];
        item.summary = summary;
        calendars.push_back(item);
    }
    return calendars;
}


String timeToRFC3339(struct tm *tm) {
    char buf[30];
    strftime(buf, sizeof(buf)-1, "%Y-%m-%dT%H:%M:%S%z", tm);
    return String(buf);
}