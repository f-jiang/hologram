#!/bin/bash

if [ "$1" == "-u" ] ; then
    echo usage: ./get-2-legged-token.sh client-id client-secret
else
    curl -v 'https://developer.api.autodesk.com/authentication/v1/authenticate' -X 'POST' -H 'Content-Type: application/x-www-form-urlencoded' -d "client_id=$1&client_secret=$2&grant_type=client_credentials&scope=bucket:create&scope=bucket:read&scope=data:write&scope=data:read"
fi
