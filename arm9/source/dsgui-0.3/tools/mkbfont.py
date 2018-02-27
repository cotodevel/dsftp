#!/usr/bin/env python

# BFont encoder.
#
# See documentation for the format this program outputs.
#
# Usage: mkbfont FONTFILE HEIGHT
#        FONTFILE: TrueType font file
#        HEIGHT: Height of the font (in pixels)
#
# Note: By default, 

#
ranges = ((0x00000020, 0x0000007e), # basic ASCII
          (0x000000a1, 0x000000ff), # Latin-1 additions
          )


from PIL import Image, ImageFont, ImageDraw
import sys, string

if len(sys.argv) != 3:
    print "Usage:", sys.argv[0], "FONTFILE HEIGHT"
    sys.exit(1)

FLAG_BOLD   = 0x01
FLAG_ITALIC = 0x02

fontfile = sys.argv[1]
fontheight = int(sys.argv[2])

font = ImageFont.truetype(fontfile, fontheight)
name = font.getname()

fontflags = 0
fontname = name[0]
fontfullname = string.join(name, " ")
if len(name) > 1:
    flaglist = name[1].split(" ")
    for item in flaglist:
            if item in ("Bold",):
                fontflags = fontflags | FLAG_BOLD
            elif item in ("Italic", "Oblique"):
                fontflags = fontflags | FLAG_ITALIC
            elif item in ("Roman",):
                pass # ignore
            else:
                print "Unknown font flag", item

filename = fontfullname + "_%d" % fontheight

for char in " ", "-", ",", ".", "+":
    filename = filename.replace(char, "_")
    
if fontheight > 255:
    fontheight = 255
elif fontheight < 1:
    fontheight = 1

print "Font '%s' (short name '%s', flags 0x%x)" % (fontfullname, fontname, fontflags)

isostr = ""
for r in ranges:
    for char in range(r[0], r[1]+1):
        isostr = isostr + unichr(char)

def requiredSize():
    img = Image.new("L", (256,256), "#ffffff")
    draw = ImageDraw.Draw(img)
    draw.setfont(font)

    return draw.textsize(isostr)

def write32(f, num):
    f.write(chr((num & 0xff000000) >> 24))
    f.write(chr((num & 0x00ff0000) >> 16))
    f.write(chr((num & 0x0000ff00) >> 8))
    f.write(chr(num & 0x000000ff))
    
def write16(f, num):
    f.write(chr((num & 0xff00) >> 8))
    f.write(chr(num & 0xff))

def write8(f, num):
    f.write(chr(num & 0xff))

def writestr(f, s):
    s = s[:255]
    write8(f, len(s))
    f.write(s)

reqsz = requiredSize()
if reqsz[0] > 65535 or reqsz[1] > 255:
    print "Image size", reqsz, "too large."
    sys.exit(-1)

img = Image.new("L", reqsz, "#ffffff")
draw = ImageDraw.Draw(img)

indices = []
widths = []
coords = [0,0]

# draw text as one -- helps reduce artifacts
draw.text(coords, isostr, font=font)
for char in isostr:
    indices.append(coords[0])
    size = draw.textsize(char, font=font)
    coords[0] = coords[0] + size[0]
    widths.append(size[0])

print "Writing %dx%d png image to '%s'" % (reqsz[0], reqsz[1], filename+".png")
img.save(filename+".png")

print "Writing BFT font file to '%s'" % (filename+".bfont",)
fontfile = open(filename+".bfont", "wb")

fontfile.write("BFNT")

write8(fontfile, 1) # version
write8(fontfile, 0) # format flags
write8(fontfile, 16) # encoding flags

# write fontname and size
writestr(fontfile, fontname)
writestr(fontfile, fontfullname)
write8(fontfile, fontheight)
write8(fontfile, fontflags)

# write ranges
write8(fontfile, len(ranges))
for r in ranges:
    write32(fontfile, r[0])
    write32(fontfile, r[1])

# write indices
for i in range(len(indices)):
    # Later version: Store more information about a glyph
    write16(fontfile, indices[i])
    write8(fontfile, widths[i])

# write image
write16(fontfile, reqsz[0])
write8(fontfile, reqsz[1])

num = 0
for r in range(reqsz[1]):
    for c in range(reqsz[0]):
        pix = img.getpixel((c,r))
        if pix > 250: pix = 255
        pix = int((pix/255.0)*31.0)
        write8(fontfile, pix)
        num = num + 1

fontfile.close()

               
