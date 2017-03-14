#!/bin/bash

if [ "$1" == "-u" ] ; then
    echo usage: ./upload-to-bucket.sh bucket-key file-name oauth-token
else
    curl -v "https://developer.api.autodesk.com/oss/v2/buckets/$1/objects/$2" -X "PUT" -H "Authorization: Bearer $3" -H "Content-Type: application/octet-stream" -H "Content-Length: 308331" -T "$2"
fi
