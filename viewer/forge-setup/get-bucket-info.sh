#!/bin/bash

if [ "$1" == "-u" ] ; then
    echo usage: ./get-bucket-info.sh bucket-key oauth-token
else
    curl -v "https://developer.api.autodesk.com/oss/v2/buckets/$1/details" -X "GET" -H "Authorization: Bearer $2"
fi
