#!/bin/bash

if [ $# -ne 1 ]; then
	echo "`basename $0` [input-nbest]"
	exit 1
fi

INPUT_FILE=$1
TMP_DIR=`dirname $1`
SCRIPTS_DIR=`dirname $0`/../
INPUT_FILE_PREFIX=`basename $1`

cat $INPUT_FILE | awk -F' \|\|\| ' '{print $2}' > $TMP_DIR/$INPUT_FILE_PREFIX.sents
cat $TMP_DIR/$INPUT_FILE_PREFIX.sents | $SCRIPTS_DIR/tokenizer/deprepro.sh > $TMP_DIR/$INPUT_FILE_PREFIX.deprepro.sents
paste $TMP_DIR/$INPUT_FILE_PREFIX.deprepro.sents $INPUT_FILE | awk -F' \|\|\| |\t'  -v OFS=' ||| '  '{$3=$1; $1=""; sub(/ \|\|\| /,""); print $0}'

