#!/bin/sh
# The partran binary writes to stdout, so we redirect to a file
THIS=`readlink -f $0`
DIR=`dirname ${THIS}`
${DIR}/partran $1 > $2
