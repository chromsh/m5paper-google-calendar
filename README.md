# Overview

Display your google calendar on M5Paper!

![image](https://raw.githubusercontent.com/chromsh/m5paper-google-calendar/img/docs/m5paper-calendar.jpg)

# Configurations

## Google Calendar

You need to create Google Calendar App and OAuth

1. Go to https://console.cloud.google.com/apis/dashboard .
2. search Google Calendar and enable it.
3. Go to https://console.cloud.google.com/apis/credentials/oauthclient .
4. Create your OAuth client ID
5. Copy Client ID and Client Secret.

Now you have Client ID and Client Secret.
Next,

Copy `.env.sample` to `.env`, and open `.env` with editor.
Set `GOOGLE_APP_CLIENT_ID` and `GOOGLE_APP_CLIENT_SECRET`.

So you can now create your refresh token.

```
$ bash tools/get-access-token.sh
...
please access below url
https://accounts.google.com/o/oauth2/v2/auth?response_type=code&...

please input your code :<input your code here>
your refresh token
<refresh token>
```

You've got refresh token.

## setup your wifi and calendar configs

Copy `src\config.cpp.sample` to `src\config.cpp` and write your settings.

- WIFI_SSID
- WIFI_PASSWORD
- GOOGLE_REFRESH_TOKEN
    - your refresh token
- GOOGLE_APP_CLIENT_ID
- GOOGLE_APP_CLIENT_SECRET
- GOOGLE_CALENDAR_ID
    - your calendar ID (typically your mail address)

# build and upload

Use VSCode and Platform-IO plugin.

- board
    - m5stack-fire
- dependent libraries
    - M5EPD
    - M5Stack
    - ArduinoJson

# show wide characters

You need to install a SD card to your TF slot.
Your SD card must have `font.ttf` file in the root directory.

ex) [IPAex fornt](https://moji.or.jp/ipafont/)