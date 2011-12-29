#!/bin/bash

# Script for translating 1 bpp standard windows bitmaps
# in C style byte arrays.
#
# Ivan Stepaniuk
#
# Input image has to be Windows BMP, 1-bit "indexed palette"
# Works with Gimp generated images, may work with others
# as long as it is 1 bpp.
# output is stdout.

# requires 'mktemp', 'file' and 'hexdump' to be installed


if [ "$1" == "" ] ; then
  echo $0: usage: $0 file.bmp
  exit 1
fi
if [ -f "$1" ] ; then
  ISBMP=$(file "$1" | egrep -i -o "PC bitmap"|tr '[A-Z]' '[a-z]')
  if [ "$ISBMP" != "pc bitmap" ] ; then
    echo $0: $1 is not a PC Bitmap file
    exit 1
  fi
else
  echo $0: $1 file not found.
  exit 1
fi

# Get the pixel data start (or header size) from the BITMAPINFOHEADER
# byte 0x0A. (Usually 62) depends on the DIB header size.
HEADERSIZE=$(hexdump "$1" -d -n 1 -s 10|head -n1 |cut -d' ' -f4)
# Convert it to base 10, as in bash leading zeros means its octal
HEADERSIZE=$((10#$HEADERSIZE))

SIZEX=$(file "$1"|cut -d' ' -f7)
SIZEY=$(file "$1"|cut -d' ' -f9)
DEPTH=$(file "$1"|cut -d' ' -f11)
FSIZE=$(wc -c "$1"|cut -d' ' -f1)

if [ "$DEPTH" -gt "1" ] ; then
  echo $0: Bit depth is $DEPTH. Only indexed, 1 bit palette bitmaps supported.
  exit 1
fi

# Calculate the number of bytes in a row
let XBYTES=$SIZEX/8
let REMAINDER=$SIZEX%8
if [ "$REMAINDER" -gt "0" ] ; then
  let XBYTES+=1
fi

# Create a temporary file
TMPOUT=$(mktemp /tmp/bmp2header-XXXX)

#Skip the BMP and DIB Headers.
POS=$HEADERSIZE

# Files have extra bytes per row (BMP has 4 byte alignment)
# add bytes per row until total size matches
EXTRABYTE=0
let CALCSIZE=$XBYTES*$SIZEY
let CALCSIZE+=$HEADERSIZE
while [ "$CALCSIZE" -lt "$FSIZE" ] ; do
  let CALCSIZE+=$SIZEY
  let EXTRABYTE+=1
done

# Convert every row to C formated hexa
while [ "$POS" -lt "$FSIZE" ] ; do
  echo -n "  " >> $TMPOUT
  hexdump -s "$POS" -n "$XBYTES" "$1" -v -e ' "0x" 1/1 "%02X" ", "' >> $TMPOUT
  echo >> $TMPOUT
  let POS+=$XBYTES
  let POS+=$EXTRABYTE
done
echo "  //"$SIZEX x $SIZEY >> $TMPOUT

# Invert the file line order (BMPs are upside down)
cat -n $TMPOUT | sort -nr | cut -c 9-

# Remove the temporary file
rm -f $TMPOUT

exit 0
