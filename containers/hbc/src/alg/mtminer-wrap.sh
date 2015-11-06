#!/bin/sh
# The mtminer binary writes to stdout, so we redirect to a file
THIS=`readlink -f $0`
DIR=`dirname ${THIS}`
${DIR}/mtminer $1 > $2
