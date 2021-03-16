#!/bin/python

from PIL import Image

im = Image.open('font.png')
pix = im.load()

f = open("font.h", "w");

f.write("#ifndef FONT_H\n#define FONT_H\n");

f.write("char font[128][64] = {\n"); # Font array, first index is for ASCII code, 2nd is for data array

offset = 33
for x in range(0,offset):
	f.write("{")
	for x2 in range(0,64):
		if x2 != 63:
			f.write("0,")
		else:
			f.write("0")
	f.write("},\n")

for x in range(0,93):
	f.write("{")
	for x2 in range(x,x+8):
		for y in range (0,8):
			pixel = pix[x2,y][1]
			if y != 7 or x2 != x + 7:
				if pixel > 0:
					f.write("1,")
				else:
					f.write("0,")
			else:
				if pixel > 0:
					f.write("1")
				else:
					f.write("0")
	f.write("}")
	if x != 92:
		f.write(",\n");
	else:
		f.write("\n};\n");
f.write("#endif");
f.close()
