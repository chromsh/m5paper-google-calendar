#include "http.h"
#include <HTTPClient.h>
#include <string.h>

String urlEncode(const char *val) ;

KeyValue::KeyValue(const char *key, const char *val) {
    int keyLen = strlen(key);
    int valLen = strlen(val);
    _key = (char *)malloc(keyLen + 1);
    if (_key == NULL || _val == NULL) {
        Serial.print("malloc failed");
        return;
    }
    _val = (char *)malloc(valLen + 1);
    strncpy(_key, key, keyLen + 1);
    strncpy(_val, val, valLen + 1);
}

void KeyValue::destroy() {
    if (_key != NULL) {
        free(_key);
        _key = NULL;
    }
    if (_val != NULL) {
        free(_val);
        _val = NULL;
    }
}



KeyValues::KeyValues(int maxLen) : _length(0), _maxLength(maxLen) {
    _kvs    = (KeyValue *)malloc(sizeof(KeyValue) * maxLen);
    if (_kvs == NULL) {
        Serial.print("KeyValues: malloc failed");
        return;
    }
}
KeyValues::~KeyValues() {
    if (_kvs != NULL) {
        for (int i = 0 ; i < _length ; i++) {
            _kvs[i].destroy();
        }
        free(_kvs);
    }
}

bool KeyValues::add(const char *key, const char *val) {
    if (_maxLength == _length) {
        return false;
    }
    KeyValue header(key, val);
    _kvs[_length] = header;
    _length++;
    return true;
}

bool KeyValues::add(const char *key, String val) {
    if (_maxLength == _length) {
        return false;
    }
    char buf[128];
    val.toCharArray(buf, sizeof(buf));
    KeyValue header(key, buf);
    _kvs[_length] = header;
    _length++;
    return true;
}


KeyValue *KeyValues::get(int pos) const {
    return &_kvs[pos];
};


//////////////////////
// MyHTTPClient class
//////////////////////
String MyHTTPClient::get(const char *url, const KeyValues *headers, const KeyValues *data) {
    HTTPClient client;
    String params = "";
    if (data != NULL) {
        params  = "?";
        for (int i = 0 ; i < data->length() ; i++) {
            KeyValue *kv = data->get(i);
            params += urlEncode(kv->key()) + "=" + urlEncode(kv->val());
            if (i < data->length() - 1) {
                params += "&";
            }
        }
    }

    client.begin(url + params);

    if (headers != NULL) {
        for (int i = 0 ; i < headers->length() ; i++) {
            KeyValue *kv = headers->get(i);
            client.addHeader(kv->key(), kv->val());
        }
    }

    int code = client.GET();
    if (code == HTTP_CODE_OK) {
        return client.getString();
    }
    else {
        Serial.print("http get error");
        Serial.print(client.getString());
    }

    return "";
}
String MyHTTPClient::post(const char *url, const KeyValues *headers, const KeyValues *data) {
    HTTPClient client;
    //Serial.println(url);
    client.begin(url);

    if (headers != NULL) {
        for (int i = 0 ; i < headers->length() ; i++) {
            KeyValue *kv = headers->get(i);
            client.addHeader(kv->key(), kv->val());
            //Serial.println("---------- header ---------");
            //Serial.println(kv->val());
        }
    }
    String body;
    if (data != NULL) {
        for (int i = 0 ; i < data->length() ; i++) {
            KeyValue *kv = data->get(i);
            body += urlEncode(kv->key()) + "=" + urlEncode(kv->val());
            if (i < data->length() - 1) {
                body += "&";
            }
        }
    }

    int code = client.POST(body);
    if (code == HTTP_CODE_OK) {
        return client.getString();
    }
    else {
        Serial.print("http post error");
        Serial.print(client.getString());
    }

    return "";
}


// URL encode
// ref ) https://hardwarefun.com/tutorials/url-encoding-in-arduino
// notice: This code doesn't comply with rfc specs.
String urlEncode(const char *val) {
	const char *hex = "0123456789abcdef";
    int len = strlen(val);
    String buf;

    for (int i = 0 ; i < len ; i++) {
        char c = val[i];

		if( ('a' <= c && c <= 'z')
		|| ('A' <= c && c <= 'Z')
		|| ('0' <= c && c <= '9') 
        || ('-' == c)
        || ('_' == c)
        || ('.' == c)
        || ('*' == c)
        || ('/' == c)
        || (':' == c)){
			buf += c;
		} else {
            buf += '%';
			buf += hex[c >> 4];
			buf += hex[c & 15];
		}
    }
    return buf;
}
