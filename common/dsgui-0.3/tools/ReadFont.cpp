// ReadFont.cpp : Definiert den Einstiegspunkt für die Konsolenanwendung.
//

#include <deque>
#include <string>

typedef unsigned int uint32;
typedef unsigned short uint16;
typedef unsigned char uint8;

typedef struct {
  unsigned int start;
  unsigned int width;
} GlyphDescription;

typedef struct {
  uint8 formatflags, encodingflags;
  std::string name, fullname;
  uint8 height;
  uint8 fontflags;
} FontHeader;

typedef struct {
  uint32 start, end;
} CharacterRange;

typedef struct {
  FontHeader header;
  std::deque<CharacterRange> ranges;
  std::deque<GlyphDescription> indices;
	
  uint16 bmpw, bmph;
  uint8* bmp;
} Font;

static uint8 read8(FILE* f)
{
  uint8 byte;
  fread(&byte, 1, 1, f);
  return byte;
}

static uint16 read16(FILE* f)
{
  return read8(f) << 8 | read8(f);
}

static uint32 read32(FILE* f)
{
  return read8(f) << 24 | read8(f) << 16 | read8(f) << 8 | read8(f);
}

static std::string readstring(FILE* f)
{
  uint8 len = read8(f);
  char* buf = new char[len+1];
  memset(buf, 0, len+1);
  fread(buf, len, 1, f);
  std::string str = buf;
  delete buf;
  return str;
}

bool readfontheader(FILE* f, FontHeader& header)
{
  char magic[4];
  memset(magic, 0, 4);
  if(fread(magic, 1, 4, f) < 4) return false;
  if(strncmp(magic, "BFNT", 4) != 0) return false;

  uint8 version = read8(f);
  if(version != 1)
    {
      fprintf(stderr, "Unknown font version %d", version);
      return false;
    }
  
  header.formatflags = read8(f);
  header.encodingflags = read8(f);

  header.name = readstring(f);
  header.fullname = readstring(f);

  header.height = read8(f);

  header.fontflags = read8(f);

  return true;
}

bool readfontbody(FILE* f, Font& font)
{
  int numranges = read8(f);
  int numchars = 0;

  for(int i=0; i<numranges; i++)
    {
      CharacterRange range;
      range.start = read32(f);
      range.end = read32(f);
      numchars += (range.end - range.start) + 1;
      font.ranges.push_back(range);
    }

  for(int i=0; i<numchars; i++)
    {
      GlyphDescription descr;
      descr.start = read16(f);
      descr.width = read8(f);
      font.indices.push_back(descr);
    }

  font.bmpw = read16(f);
  font.bmph = read8(f);

  long pos = ftell(f);
  fseek(f, 0, SEEK_END);
  long bufsize = ftell(f) - pos;

  if(bufsize != font.bmpw*font.bmph)
    {
      fprintf(stderr, "Invalid bmp size (%dx%d != %d)\n",
	      font.bmpw, font.bmph, bufsize);
      return false;
    }

  fseek(f, pos, SEEK_SET);
  uint8 *bmp = new uint8[bufsize];
  fread(bmp, bufsize, 1, f);

  return true; 
}

int main(int argc, char* argv[])
{
  FILE* f = fopen(argv[1], "rb");
  if(!f) 
    {
      printf("Couldn't open %s\n", argv[1]);
      return -1;
    }
  Font font;
  if(!readfontheader(f, font.header))
    {
      printf("Couldn't read font header\n");
      return -1;
    }
  if(!readfontbody(f, font))
    {
      printf("Couldn't read font body\n");
      return -1;
    }
  printf("Read %s\n", argv[1]);
  printf("    Short name: \"%s\"\n", font.header.name.c_str());
  printf("     Full name: \"%s\"\n", font.header.fullname.c_str());
  printf("        Height: %d\n", font.header.height);
  printf("  Format flags: 0x%02x\n", font.header.formatflags);
  printf("Encoding flags: 0x%02x\n", font.header.encodingflags);
  printf("    Font flags: 0x%02x\n", font.header.fontflags);
  printf("   Char ranges: %d\n", font.ranges.size());
  printf("    Characters: %d\n", font.indices.size());
  printf("   Bitmap size: %dx%d\n", font.bmpw, font.bmph);
  return 0;
}

