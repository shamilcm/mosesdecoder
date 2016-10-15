#!/bin/bash


`dirname $0`/../recaser/detruecase.perl  | `dirname $0`/../tokenizer/detokenizer.perl | `dirname $0`/../tokenizer/depreprocess/word-tokenize.py
