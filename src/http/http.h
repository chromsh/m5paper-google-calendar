#pragma once

#include <M5Stack.h>

// single key value pair
class KeyValue {
public:
    KeyValue(const char *key, const char *val);
    const char *key() const {return _key;};
    const char *val() const {return _val;};
    // need call before remove
    void destroy();
private:
    char *_key;
    char *_val;
};

class KeyValues {
public:
    KeyValues(int maxLen);
    ~KeyValues();
    bool add(const char *key, const char *val);
    KeyValue *get (int pos) const;
    int length() const {return _length;};

private:
    int _length;
    int _maxLength;
    KeyValue *_kvs;
};

class MyHTTPClient {
public:
    String post(const char *url, const KeyValues *headers, const KeyValues *data);

};