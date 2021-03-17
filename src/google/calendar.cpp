#include "calendar.h"
#include <time.h>
#include <ArduinoJson.h>
#include "../http/http.h"

const int MAX_EVENT_COUNT = 20;
const char *GOOGLE_CALENDAR_LIST = "https://www.googleapis.com/calendar/v3/users/me/calendarList";
const char *GOOGLE_CALENDAR_EVENT_LIST_PREFIX = "https://www.googleapis.com/calendar/v3/calendars/"; // + "/calendarId/events"
const char *GOOGLE_CALENDAR_EVENT_LIST_POSTFIX = "/events";
String timeToRFC3339(struct tm *tm) ;

GoogleCalendarEvent::GoogleCalendarEvent(const char *summary, const char *start, const char *end, bool isPeriod) {
    strncpy(_summary, summary, sizeof(_summary) -1);
    strncpy(_start, start, sizeof(_start) -1);
    strncpy(_end, end, sizeof(_end) -1);
    _isPeriod = isPeriod;
}
bool GoogleCalendarEvent::isPeriod() {
    return _isPeriod;
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

// YYYY-MM-DD hh:mm - hh:mm
String GoogleCalendarEvent::startEndDateTimePeriodString() {
    // YYYY-MM-DDThh:mm:ss+HH:MM (RFC3339)
    String start = String(_start);
    String end = String(_end);

    // YYYY-MM-DD
    String year = start.substring(0, 10);

    String startTime = start.substring(11, 16);
    String endTime = end.substring(11, 16);

    return year + " " + startTime + " - " + endTime;
}

// YYYY-MM-DD - YYYY-MM-DD
// only if isPeriod() == true
String GoogleCalendarEvent::startEndDatePeriodString() {
    return String(_start) + " - " + _end;
}

// hh:mm - hh:mm
String GoogleCalendarEvent::startEndTimePeriodString() {
    String start = String(_start);
    String end = String(_end);
    String startTime = start.substring(11, 16);
    String endTime = end.substring(11, 16);

    return startTime + " - " + endTime;
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
    _events[_length] = new GoogleCalendarEvent(event.summary(), event.start(), event.end(), event.isPeriod());
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

    KeyValues data(6);
    data.add("timeMin", timeToRFC3339(start));
    data.add("timeMax", timeToRFC3339(end));
    data.add("maxResults", String(MAX_EVENT_COUNT));
    data.add("orderBy", "startTime");
    data.add("singleEvents", "true");
    data.add("maxAttendees", "1");
    //data.add("timeZone", "Asia/Tokyo");

    String url = GOOGLE_CALENDAR_EVENT_LIST_PREFIX + String(calendarId) + GOOGLE_CALENDAR_EVENT_LIST_POSTFIX;
    Serial.println(url);
    String res = client.get(url.c_str(), &headers, &data);
    //Serial.println("res is ");
    //Serial.println(res);

    if (res == "") {
        return NULL;
    }
    Serial.println("parse json");
    DynamicJsonDocument doc(35000);
    Serial.println(String("doc capacity() = ") + doc.capacity());
    DeserializationError error = deserializeJson(doc, res);
    Serial.println("deserialized json");
    if (error) {
        Serial.println("deserializeJson() failed");
        Serial.println(error.c_str());
    }
    if (doc.overflowed()) { 
        Serial.println(" overflowed ");
    }
    Serial.println(String("memoryUsage = ") + doc.memoryUsage());
   
    GoogleCalendarEventList *eventList = new GoogleCalendarEventList(MAX_EVENT_COUNT);
    JsonArray items = doc["items"].as<JsonArray>();
    Serial.println(String("item size = ") + items.size());
    for (int i = 0 ; i < items.size() ; i++) {
        Serial.print(" item =  ");
        Serial.println(i);
        const char *summary = items[i]["summary"];
        
        String start = "";
        bool isPeriodEvent = false;
        const char *datetime = items[i]["start"]["dateTime"];
        Serial.println(datetime);
        // datetimeは基本常に!=NULL、なので空文字列で存在チェック
        if (datetime != NULL && strcmp("", datetime) != 0) {
            Serial.println("dateTime use(start)");
            start += (const char *)items[i]["start"]["dateTime"];
        }
        else if (items[i]["start"]["date"] != NULL) {
            isPeriodEvent = true;
            Serial.println("date use(start)");
            start += (const char *)items[i]["start"]["date"];
        }
        else {
            Serial.println("no dateTime or date");
        }
        Serial.println(start);
        String end = "";
        datetime = items[i]["end"]["dateTime"];
        if (datetime != NULL && strcmp("", datetime) != 0) {
            end += (const char *)items[i]["end"]["dateTime"];
        }
        else if (items[i]["end"]["date"] != NULL) {
            Serial.println("date use(end)");
            end += (const char *)items[i]["end"]["date"];
        }
        else {
            Serial.println("no dateTime or date");
        }
        Serial.println(end);

        GoogleCalendarEvent event(summary, start.c_str(), end.c_str(), isPeriodEvent);

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