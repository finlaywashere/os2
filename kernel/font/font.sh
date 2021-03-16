#!/bin/bash
# Draw ascii characters 33-126 (minus some special characters)
convert -background none -fill black -pointsize 12 label:" ! #$     *+,-./0123456789:;<=>?@ABCDEFGHIJKLMNOPQRSTUVWXYZ[\]^_ abcdefghijklmnopqrstuvwxyz{|}~" font.png

# Make all the characters 8x8
convert font.png -resize 744x8! font.png

python font-to-header.py
