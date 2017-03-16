#!/bin/bash

if [ "$1" == "-u" ] ; then
    echo usage: ./create-bucket.sh oauth-token bucket-key policy-key
else
    curl -v 'https://developer.api.autodesk.com/oss/v2/buckets' -X 'POST' -H 'Content-Type: application/json' -H "Authorization: Bearer $1" -d "{ \"bucketKey\":\"$2\", \"policyKey\":\"$3\" }"
fi
