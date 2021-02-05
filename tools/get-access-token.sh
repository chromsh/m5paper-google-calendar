#!/bin/bash

# read envirnment file
SCRIPT_DIR=$(cd $(dirname $0); pwd)
export $(cat $SCRIPT_DIR/../.env|grep -v ^\# |xargs )
echo $GOOGLE_APP_CLIENT_ID
echo $GOOGLE_APP_CLIENT_SECRET

REDIRECT_URI="urn:ietf:wg:oauth:2.0:oob"

# create authorization url

# scope list 
# https://developers.google.com/calendar/auth
scopes=(
    "https://www.googleapis.com/auth/calendar.readonly"
    "https://www.googleapis.com/auth/calendar.events.readonly"
)

scope=""
for url in "${scopes[@]}"; do 
    s=`echo "$url" | nkf -WwMQ | sed 's/=$//g' | tr = % | tr -d '\n'`
    scope="$scope%20$s"
done
# remobe first %20
scope=`echo $scope|sed -e s/%20//`

authorizationUrl="https://accounts.google.com/o/oauth2/v2/auth?response_type=code&client_id=$GOOGLE_APP_CLIENT_ID&redirect_uri=$REDIRECT_URI&access_type=offline&scope=$scope"

echo "please access below url"
echo $authorizationUrl
echo ""
echo -n  "please input your code :"
read code
echo $code

# get access token
json=`curl \
    --data "code=$code" \
    --data "client_id=$GOOGLE_APP_CLIENT_ID" \
    --data "client_secret=$GOOGLE_APP_CLIENT_SECRET" \
    --data "redirect_uri=$REDIRECT_URI" \
    --data "grant_type=authorization_code" \
    https://oauth2.googleapis.com/token`

accessToken=`echo $json | jq .access_token`
refreshToken=`echo $json | jq .refresh_token`

echo "your refresh token"
echo $refreshToken