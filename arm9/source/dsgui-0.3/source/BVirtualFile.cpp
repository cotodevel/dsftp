// BVirtualFile.cpp (this is -*- C++ -*-)
// 
// \author: Bjoern Giesler <bjoern@giesler.de>
// 
// 
// 
// $Author: giesler $
// $Locker$
// $Revision$
// $Date: 2002-08-19 10:41:28 +0200 (Mon, 19 Aug 2002) $

/* system includes */
#include <nds.h>
#include <sys/dir.h>
#include <unistd.h>
#include "ipc_libnds_extended.h"

/* my includes */
#include "BVirtualFile.h"
#include "BException.h"

#if !defined(MAX)
#define MAX(x,y) ((x)<(y)?(x):(y))
#endif

#if !defined(FAT_libfat) && !defined(FAT_gba_nds_fat)
#error One of FAT_libfat and FAT_gba_nds_fat needs to be defined. See top-level Makefile!
#endif

#if defined(FAT_libfat) && defined(FAT_gba_nds_fat)
#error Only one of FAT_libfat and FAT_gba_nds_fat can be defined. See top-level Makefile!
#endif

#if defined(FAT_gba_nds_fat)
#include <gba_nds_fat/gba_nds_fat.h>
#define fopen FAT_fopen
#define fclose FAT_fclose
#define fread FAT_fread
#define fwrite FAT_fwrite
#define ftell FAT_ftell
#define fseek FAT_fseek
#define feof FAT_feof
#define chdir FAT_chdir
#define FILE FAT_FILE
#endif

#if defined(FAT_libfat)
#include <fat.h>
#endif

#if 0
int
BVirtualFile::getc()
{
  u8 c;
  if(eof()) return EOF;
  read(&c, 1);
  return c;
}
#endif

//ori
/*
std::string
BVirtualFile::read()
{
  char buf[1024];
  std::string retval;
  while(!eof())
    {
      memset(buf, 0, 1024);
      read(buf, 1023);
      retval += buf;
    }
  return retval;
}

BFATFile::BFATFile(const char* filename, const char* mode)
{
  _file = fopen(filename, mode);
  if(!_file) BTHROW(BSystemException, "Couldn't open %s", filename);
  _close = true;
}

BFATFile::BFATFile(FILE* file, bool closeWhenDone)
{
  _file = file;
  _close = true;
}

BFATFile::~BFATFile()
{
  if(_close)
    fclose(_file);
}

int
BFATFile::read(void* buf, unsigned int nbytes)
{
  return fread(buf, 1, nbytes, _file);
}

std::string
BFATFile::read()
{
  char buf[1024];
  std::string retval;
  while(!eof())
    {
      memset(buf, 0, 1024);
      read(buf, 1023);
      retval += buf;
    }
  return retval;
}



int
BFATFile::write(const void* buf, unsigned int nbytes)
{
  return fwrite(buf, 1, nbytes, _file);
}

long
BFATFile::tell()
{
  return ftell(_file);
}

int
BFATFile::seek(long offset, Whence whence)
{
  switch(whence)
    {
    case WHENCE_START:
      return fseek(_file, offset, SEEK_SET);
      break;

    case WHENCE_CURRENT:
      return fseek(_file, offset, SEEK_CUR);
      break;

    case WHENCE_END:
      return fseek(_file, offset, SEEK_END);
      break;
    }

  return -1;
}

int
BFATFile::eof()
{
  return feof(_file);
}


BMemFile::BMemFile(u32 address, u32 len, bool freeWhenDone)
{
  _adr = _cur = address;
  _len = len;
  _free = freeWhenDone;
}

BMemFile::~BMemFile()
{
  if(_free) delete (char*)_adr;
}

int
BMemFile::read(void* buf, unsigned int nbytes)
{
  nbytes = MAX(nbytes, (_adr+_len)-_cur);
  memcpy(buf, (void*)_cur, nbytes);
  _cur += nbytes;
  return nbytes;
}

std::string
BMemFile::read()
{
  char buf[1024];
  std::string retval;
  while(!eof())
    {
      memset(buf, 0, 1024);
      read(buf, 1023);
      retval += buf;
    }
  return retval;
}

int
BMemFile::write(const void* buf, unsigned int nbytes)
{
  return 0;
}

long
BMemFile::tell()
{
  return _cur-_adr;
}

int
BMemFile::seek(long offset, Whence whence)
{
  u32 newpos;
  
  switch(whence)
    {
    case WHENCE_START:
      newpos = _adr + offset;
      break;
    case WHENCE_CURRENT:
      newpos = _cur + offset;
      break;
    case WHENCE_END:
      newpos = _adr + _len - offset;
      break;
    default:
      return -1;
    }

  if(newpos < _adr || newpos > _adr + _len) return -1;
  _cur = newpos;
  return 0;
}

int
BMemFile::eof()
{
  if(_cur == _adr+_len) return 1;
  return 0;
}

BFileManager*
BFileManager::get()
{
  if(!_mgr)
    {
      try {
	_mgr = new BFileManager;
      } catch(...) {
	return NULL;
      }
    };
  return _mgr;
}

std::vector<BFileManager::FileAndType>
BFileManager::directoryContents(const std::string& dir)
{
  std::vector<FileAndType> retval;
  FileAndType ft;

#if defined(FAT_libfat)
  DIR_ITER* dp = diropen(dir.c_str());
  if(dp == NULL)
    return retval;

  char lfn[1024];
  struct stat statbuf;
  
  while(true)
    {
      memset(lfn, 0, 1024);
      memset(&statbuf, 0, sizeof(struct stat));

      if(dirnext(dp, lfn, &statbuf) == -1)
	break;

      ft.filename = lfn;
      if(S_ISDIR(statbuf.st_mode)) ft.filetype = TYPE_DIRECTORY;
      else ft.filetype = TYPE_FILE;

      retval.push_back(ft);
    }
  
  dirclose(dp);
#endif

#if defined(FAT_gba_nds_fat)
  if(!FAT_chdir(dir.c_str()))
    return retval;

  char lfn[1024];
  memset(lfn, 0, 1024);

  FILE_TYPE type = FAT_FindFirstFileLFN(lfn);
  if(type == FT_NONE) return retval;

  ft.filename = lfn;
  if(type == FT_DIR) ft.filetype = TYPE_DIRECTORY;
  else ft.filetype = TYPE_FILE;
  retval.push_back(ft);

  while(true) {
    type = FAT_FindNextFileLFN(lfn);
    if(type == FT_NONE) break;

    ft.filename = lfn;
    if(type == FT_DIR) ft.filetype = TYPE_DIRECTORY;
    else ft.filetype = TYPE_FILE;
    retval.push_back(ft);
  }

  FAT_chdir(_curdir.c_str());
#endif
  
  return retval;
}

BFileManager::FileType
BFileManager::typeOfFile(const std::string& filename)
{
#if defined(FAT_libfat)
  struct stat statbuf;
  if(stat(filename.c_str(), &statbuf) == -1)
    return TYPE_NOEXIST;

  if(S_ISDIR(statbuf.st_mode)) return TYPE_DIRECTORY;
  return TYPE_FILE;
#endif
  
#if defined(FAT_gba_nds_fat)
  switch(FAT_FileExists(filename.c_str()))
    {
    case FT_NONE:
      return TYPE_NOEXIST;
      break;

    case FT_DIR:
      return TYPE_DIRECTORY;
      break;

    default:
      return TYPE_FILE;
    }
#endif
}

int 
BFileManager::sizeOfFile(const std::string& filename)
{
#if defined(FAT_libfat)
  struct stat statbuf;
  if(stat(filename.c_str(), &statbuf) == -1)
    return -1;
  return statbuf.st_size;
#endif
  
#if defined(FAT_gba_nds_fat)
  FILE* file = fopen(filename.c_str(), "r");
  if(!file) return -1;
  int size = FAT_GetFileSize();
  fclose(file);
  return size;
#endif
}

bool
BFileManager::changeDirectory(const std::string& dir)
{
  std::string dn = normalizePath(dir);

#if defined(FAT_libfat)
  if(chdir(dn.c_str()) == -1) return false;
#endif
  
#if defined(FAT_gba_nds_fat)
  bool retval = FAT_chdir(dn.c_str());
  if(!retval) return false;
#endif

  _curdir = dn;
  return true;
}

std::string
BFileManager::currentDirectory()
{
  return _curdir;
}

bool
BFileManager::makeDirectory(const std::string& dir)
{
#if defined(FAT_libfat)
  return mkdir(dir.c_str(), 0777) == 0;
#endif
  
#if defined(FAT_gba_nds_fat)
  return mkdir(dir.c_str()) == 0;
#endif
}

bool
BFileManager::renameFile(const std::string& fname1, const std::string& fname2)
{
  std::string tmpf1 = absolutePath(fname1);
  std::string tmpf2 = absolutePath(fname2);

#if defined(FAT_libfat)
  return rename(tmpf1.c_str(), tmpf2.c_str()) == 0;
#endif
  
#if defined(FAT_gba_nds_fat)
  FileType type = typeOfFile(tmpf1);

  // cannot rename directories
  if(type == TYPE_DIRECTORY)
    return false;
  
  FAT_FILE* f1 = FAT_fopen(tmpf1.c_str(), "r");
  if(!f1)
    return false;

  FAT_FILE* f2 = FAT_fopen(tmpf2.c_str(), "w");
  if(!f2)
    {
      FAT_fclose(f1);
      return false;
    }

  while(!FAT_feof(f1))
    {
      u32 num = FAT_fread(buf, 1, 4096, f1);

      // not enough space to move
      if(FAT_fwrite(buf, 1, num, f2) < num)
	{
	  FAT_fclose(f1);
	  FAT_fclose(f2);
	  FAT_remove(tmpf2.c_str());
	  return false;
	}
    }

  FAT_fclose(f1);
  FAT_fclose(f2);
  FAT_remove(tmpf1.c_str());
  return true;
#endif
}

bool
BFileManager::removeFile(const std::string& fname)
{
#if defined(FAT_libfat)
  if(unlink(fname.c_str()) == 0) return true;
  return false;
#endif

#if defined(FAT_gba_nds_fat)
  if(FAT_remove(fname.c_str()) == 0) return true;
  return false;
#endif
}

std::string
BFileManager::normalizePath(const std::string& str)
{
  if(str.size() == 0) return "";
  
  std::deque<std::string> list = split(str, "/");
  for(std::deque<std::string>::iterator iter = list.begin();
      iter != list.end(); iter++)
    {
      if(*iter == "." || *iter == "")
	{
	  iter = list.erase(iter);
	  if(iter == list.end()) break;
	}
      else if(*iter == "..")
	{
	  // erase the ".."
	  iter = list.erase(iter);

	  // not the first entry? erase the one before as well
	  if(iter != list.begin()) 
	    iter = list.erase(iter-1);

	  if(iter == list.end()) break;
	}
    }

  std::string retval;
  if(str[0] == '/') retval = "/";
  for(unsigned int i=0; i<list.size(); i++)
    {
      retval += list[i];
      if(i<list.size() - 1)
	retval += "/";
    }
    
  return retval;
}

std::string
BFileManager::lastPathComponent(const std::string& str)
{
  std::string fn = normalizePath(str);
  std::deque<std::string> list = split(str, "/");
  if(list.size() == 0) return "";
  return *(list.end()-1);
}

std::string BFileManager::absolutePath(const std::string& str)
{
  if(str.size() && str[0] == '/')
    return normalizePath(str);
  else
    return normalizePath(currentDirectory() + "/" + str);  
}

std::string
BFileManager::filenameExtension(const std::string& str)
{
  unsigned int pos = str.rfind(".");
  if(pos == std::string::npos) return str;
  return toLower(str.substr(pos, str.size()-pos));
}

std::string
BFileManager::filenameWithoutExtension(const std::string& str)
{
  unsigned int pos = str.rfind(".");
  if(pos == std::string::npos) return str;
  return str.substr(0, pos);
}

BFileManager::BFileManager()
{
#if defined(FAT_libfat)
  if(fatInitDefault() == false)
    BTHROW(BSystemException, "Couldn't init file system");
  _curdir = "/";
  chdir(_curdir.c_str());
#endif

#if defined(FAT_gba_nds_fat)
  FAT_InitFiles();
  _curdir = "/";
  FAT_chdir(_curdir.c_str());
#endif
}

BFileManager* BFileManager::_mgr = NULL;
*/


//new

std::string
BVirtualFile::read()
{
  char buf[1024];
  std::string retval;
  while(!eof())
    {
      memset(buf, 0, 1024);
      read(buf, 1023);
      retval += buf;
    }
  return retval;
}

BFATFile::BFATFile(const char* filename, const char* mode)
{
  _file = fopen(filename, mode);
  if(!_file) BTHROW(BSystemException, "Couldn't open %s", filename);
  _close = true;
}

BFATFile::BFATFile(FILE* file, bool closeWhenDone)
{
  _file = file;
  _close = true;
}

BFATFile::~BFATFile()
{
  if(_close)
    fclose(_file);
}

int
BFATFile::read(void* buf, unsigned int nbytes)
{
  return fread(buf, 1, nbytes, _file);
}

std::string
BFATFile::read()
{
  char buf[1024];
  std::string retval;
  while(!eof())
    {
      memset(buf, 0, 1024);
      read(buf, 1023);
      retval += buf;
    }
  return retval;
}



int
BFATFile::write(const void* buf, unsigned int nbytes)
{
  return fwrite(buf, 1, nbytes, _file);
}

long
BFATFile::tell()
{
  return ftell(_file);
}

int
BFATFile::seek(long offset, Whence whence)
{
  switch(whence)
    {
    case WHENCE_START:
      return fseek(_file, offset, SEEK_SET);
      break;

    case WHENCE_CURRENT:
      return fseek(_file, offset, SEEK_CUR);
      break;

    case WHENCE_END:
      return fseek(_file, offset, SEEK_END);
      break;
    }

  return -1;
}

int
BFATFile::eof()
{
  return feof(_file);
}


BMemFile::BMemFile(u32 address, u32 len, bool freeWhenDone)
{
  _adr = _cur = address;
  _len = len;
  _free = freeWhenDone;
}

BMemFile::~BMemFile()
{
  if(_free) delete (char*)_adr;
}

int
BMemFile::read(void* buf, unsigned int nbytes)
{
  nbytes = MAX(nbytes, (_adr+_len)-_cur);
  memcpy(buf, (void*)_cur, nbytes);
  _cur += nbytes;
  return nbytes;
}

std::string
BMemFile::read()
{
  char buf[1024];
  std::string retval;
  while(!eof())
    {
      memset(buf, 0, 1024);
      read(buf, 1023);
      retval += buf;
    }
  return retval;
}

int
BMemFile::write(const void* buf, unsigned int nbytes)
{
  return 0;
}

long
BMemFile::tell()
{
  return _cur-_adr;
}

int
BMemFile::seek(long offset, Whence whence)
{
  u32 newpos;
  
  switch(whence)
    {
    case WHENCE_START:
      newpos = _adr + offset;
      break;
    case WHENCE_CURRENT:
      newpos = _cur + offset;
      break;
    case WHENCE_END:
      newpos = _adr + _len - offset;
      break;
    default:
      return -1;
    }

  if(newpos < _adr || newpos > _adr + _len) return -1;
  _cur = newpos;
  return 0;
}

int
BMemFile::eof()
{
  if(_cur == _adr+_len) return 1;
  return 0;
}

BFileManager*
BFileManager::get()
{
  if(!_mgr)
    {
      try {
	_mgr = new BFileManager;
      } catch(...) {
	return NULL;
      }
    };
  return _mgr;
}

std::vector<BFileManager::FileAndType>
BFileManager::directoryContents(const std::string& dir)
{
  std::vector<FileAndType> retval;
  FileAndType ft;

#if defined(FAT_libfat)
  /*
  DIR_ITER* dp = diropen(dir.c_str());
  if(dp == NULL)
    return retval;

  char lfn[1024];
  struct stat statbuf;
  
  while(true)
    {
      memset(lfn, 0, 1024);
      memset(&statbuf, 0, sizeof(struct stat));

      if(dirnext(dp, lfn, &statbuf) == -1)
	break;

      ft.filename = lfn;
      if(S_ISDIR(statbuf.st_mode)) ft.filetype = TYPE_DIRECTORY;
      else ft.filetype = TYPE_FILE;

      retval.push_back(ft);
    }
  
  dirclose(dp);
  */
  
  DIR *direc = opendir (dir.c_str()); 
	
	
  if(direc == NULL)
    return retval;

  char lfn[1024];
  struct stat statbuf;
  
  while(true)
    {
      memset(lfn, 0, 1024);
      memset(&statbuf, 0, sizeof(struct stat));
	
	struct dirent* pent = readdir(direc);
		if(pent == NULL) break;
			
		
      ft.filename = lfn;
	  if ( pent->d_type & DT_DIR ) ft.filetype = TYPE_DIRECTORY;
      else ft.filetype = TYPE_FILE;

      retval.push_back(ft);
    }
  
  closedir(direc);
  
  
#endif

#if defined(FAT_gba_nds_fat)
  if(!FAT_chdir(dir.c_str()))
    return retval;

  char lfn[1024];
  memset(lfn, 0, 1024);

  FILE_TYPE type = FAT_FindFirstFileLFN(lfn);
  if(type == FT_NONE) return retval;

  ft.filename = lfn;
  if(type == FT_DIR) ft.filetype = TYPE_DIRECTORY;
  else ft.filetype = TYPE_FILE;
  retval.push_back(ft);

  while(true) {
    type = FAT_FindNextFileLFN(lfn);
    if(type == FT_NONE) break;

    ft.filename = lfn;
    if(type == FT_DIR) ft.filetype = TYPE_DIRECTORY;
    else ft.filetype = TYPE_FILE;
    retval.push_back(ft);
  }

  FAT_chdir(_curdir.c_str());
#endif
  
  return retval;
}

BFileManager::FileType
BFileManager::typeOfFile(const std::string& filename)
{
#if defined(FAT_libfat)
  struct stat statbuf;
  if(stat(filename.c_str(), &statbuf) == -1)
    return TYPE_NOEXIST;

  if(S_ISDIR(statbuf.st_mode)) return TYPE_DIRECTORY;
  return TYPE_FILE;
#endif
  
#if defined(FAT_gba_nds_fat)
  switch(FAT_FileExists(filename.c_str()))
    {
    case FT_NONE:
      return TYPE_NOEXIST;
      break;

    case FT_DIR:
      return TYPE_DIRECTORY;
      break;

    default:
      return TYPE_FILE;
    }
#endif
}

int 
BFileManager::sizeOfFile(const std::string& filename)
{
#if defined(FAT_libfat)
  struct stat statbuf;
  if(stat(filename.c_str(), &statbuf) == -1)
    return -1;
  return statbuf.st_size;
#endif
  
#if defined(FAT_gba_nds_fat)
  FILE* file = fopen(filename.c_str(), "r");
  if(!file) return -1;
  int size = FAT_GetFileSize();
  fclose(file);
  return size;
#endif
}

bool
BFileManager::changeDirectory(const std::string& dir)
{
  std::string dn = normalizePath(dir);

#if defined(FAT_libfat)
  if(chdir(dn.c_str()) == -1) return false;
#endif
  
#if defined(FAT_gba_nds_fat)
  bool retval = FAT_chdir(dn.c_str());
  if(!retval) return false;
#endif

  _curdir = dn;
  return true;
}

std::string
BFileManager::currentDirectory()
{
  return _curdir;
}

bool
BFileManager::makeDirectory(const std::string& dir)
{
#if defined(FAT_libfat)
  return mkdir(dir.c_str(), 0777) == 0;
#endif
  
#if defined(FAT_gba_nds_fat)
  return mkdir(dir.c_str()) == 0;
#endif
}

bool
BFileManager::renameFile(const std::string& fname1, const std::string& fname2)
{
  std::string tmpf1 = absolutePath(fname1);
  std::string tmpf2 = absolutePath(fname2);

#if defined(FAT_libfat)
  return rename(tmpf1.c_str(), tmpf2.c_str()) == 0;
#endif
  
#if defined(FAT_gba_nds_fat)
  FileType type = typeOfFile(tmpf1);

  // cannot rename directories
  if(type == TYPE_DIRECTORY)
    return false;
  
  FAT_FILE* f1 = FAT_fopen(tmpf1.c_str(), "r");
  if(!f1)
    return false;

  FAT_FILE* f2 = FAT_fopen(tmpf2.c_str(), "w");
  if(!f2)
    {
      FAT_fclose(f1);
      return false;
    }

  while(!FAT_feof(f1))
    {
      u32 num = FAT_fread(buf, 1, 4096, f1);

      // not enough space to move
      if(FAT_fwrite(buf, 1, num, f2) < num)
	{
	  FAT_fclose(f1);
	  FAT_fclose(f2);
	  FAT_remove(tmpf2.c_str());
	  return false;
	}
    }

  FAT_fclose(f1);
  FAT_fclose(f2);
  FAT_remove(tmpf1.c_str());
  return true;
#endif
}

bool
BFileManager::removeFile(const std::string& fname)
{
#if defined(FAT_libfat)
  if(unlink(fname.c_str()) == 0) return true;
  return false;
#endif

#if defined(FAT_gba_nds_fat)
  if(FAT_remove(fname.c_str()) == 0) return true;
  return false;
#endif
}

std::string
BFileManager::normalizePath(const std::string& str)
{
  if(str.size() == 0) return "";
  
  std::deque<std::string> list = split(str, "/");
  for(std::deque<std::string>::iterator iter = list.begin();
      iter != list.end(); iter++)
    {
      if(*iter == "." || *iter == "")
	{
	  iter = list.erase(iter);
	  if(iter == list.end()) break;
	}
      else if(*iter == "..")
	{
	  // erase the ".."
	  iter = list.erase(iter);

	  // not the first entry? erase the one before as well
	  if(iter != list.begin()) 
	    iter = list.erase(iter-1);

	  if(iter == list.end()) break;
	}
    }

  std::string retval;
  if(str[0] == '/') retval = "/";
  for(unsigned int i=0; i<list.size(); i++)
    {
      retval += list[i];
      if(i<list.size() - 1)
	retval += "/";
    }
    
  return retval;
}

std::string
BFileManager::lastPathComponent(const std::string& str)
{
  std::string fn = normalizePath(str);
  std::deque<std::string> list = split(str, "/");
  if(list.size() == 0) return "";
  return *(list.end()-1);
}

std::string BFileManager::absolutePath(const std::string& str)
{
  if(str.size() && str[0] == '/')
    return normalizePath(str);
  else
    return normalizePath(currentDirectory() + "/" + str);  
}

std::string
BFileManager::filenameExtension(const std::string& str)
{
  unsigned int pos = str.rfind(".");
  if(pos == std::string::npos) return str;
  return toLower(str.substr(pos, str.size()-pos));
}

std::string
BFileManager::filenameWithoutExtension(const std::string& str)
{
  unsigned int pos = str.rfind(".");
  if(pos == std::string::npos) return str;
  return str.substr(0, pos);
}

BFileManager::BFileManager()
{
#if defined(FAT_libfat)
  if(fatInitDefault() == false)
    BTHROW(BSystemException, "Couldn't init file system");
  _curdir = "/";
  chdir(_curdir.c_str());
#endif

#if defined(FAT_gba_nds_fat)
  FAT_InitFiles();
  _curdir = "/";
  FAT_chdir(_curdir.c_str());
#endif
}

BFileManager* BFileManager::_mgr = NULL;
