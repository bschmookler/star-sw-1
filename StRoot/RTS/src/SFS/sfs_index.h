#ifndef _SFS_INDEX_H_
#define _SFS_INDEX_H_

#include <string.h>
#include "fs_index.h"
#include <sys/uio.h>

typedef unsigned int UINT32;
typedef unsigned short UINT16;
typedef unsigned char UINT8;

#include "sfs_base.h"

// File Format
//
// ----------------
// | VolumeSpec
// | HEAD  
// | FILE / (data)  
// | FILE / (data)
// | FILE / (data) 
// | .... 
// | TAIL 
// ----------------
//
// FILE Attributes:
//
// SFS_ATTR_INVALID  : deleted file
//
//
// Filenames:
//          
//   /xxx      absolute path
//   xxx       relative to directory part of previous entry
//   0 length  used to reset "previous entry"
//   xxx/      directory
//   xxx       file
//   xxx/xxx/yyy   xxx/xxx/ is "directory part" 




class SFS_ittr {
 public:
  union {
    SFS_File entry;
    char entryBuff[256];
  };
  char stickypath[256];
  char ppath[256];
  char fullpath[256];

#ifdef __USE_LARGEFILE64
  long long int fileoffset;  // from start of file.
#else
  int fileoffset ;
#endif

  int filepos;  // 0 start of header, 1 end of header, 2 end of file record, -1 at end of file system
  int skipped_bytes;   // bytes skipped due to ignored info...

  int legacy;
  SFS_ittr() {
    fileoffset = 0;
    skipped_bytes = 0;
  };

#ifdef __USE_LARGEFILE64
  SFS_ittr(long long int offset) 
#else
  SFS_ittr(int offset)
#endif
  {
    fileoffset = offset;
  };

  int get(wrapfile *wrap);
  //int legacy_get(wrapfile *wrap);
  //  int get(char *buff, int sz);

  int next();
  //int legacy_next();

  // DAQ SPECIFIC
  int nextLRHD();
  int nextDatap();
  int findEventNumber();
  int findFullLength();
  // END DAQ SPECIFIC

  wrapfile *wfile;
  void swapEntry();
};


inline int seeksize(int filesize)
{
  return (filesize + 3) & 0xfffffffc;
}

inline int get_sfsFileSize(SFS_File *file)
{
  return seeksize(strlen(file->name)+1) + sizeof(SFS_File) - 4;
}

// sfs_lib prototypes
void write_env(char *var, char *value);
char *read_env(char *var);
void getFullPath(char *out, char *in);
int seeksize(int filesize);
void striptofirst(char *str);
char *striptofile(char *str);
void stripfile(char *str);
char *striptodir(char *str);

char *SFS_getpayload(char *buff);  // takes a pointer to a FILE and gets the payload...


class sfs_index : public fs_index {
 public:

  int singleDirMount;
  SFS_ittr *singleDirIttr;

#ifdef __USE_LARGEFILE64
  int mountSingleDir(char *fn, long long int offset=0);
  int getSingleDirSize(char *fn, long long int offset);
#else
  int mountSingleDir(char *fn, int offset=0);
  int getSingleDirSize(char *fn, int offset);
#endif

  int mountSingleDirMem(char *buff, int size);
  int mountSingleDir();
  int mountNextDir();


  sfs_index();
  int _create();
  void dump(int) { dump("/",root); };
 
  int getwritevsz(fs_iovec *fsiovec, int n);
  int writev_call_retry(int fd, iovec *iovec, int vec);
  int writev(fs_iovec *fsiovec, int n);
  int writev_sticky(fs_iovec *iovec, int n, char *sticky);
  int write(char *fn, char *buff, int sz);
  
  static int getfileheadersz(char *fn);
  int putfileheader(char *ptr, char *fn, int filesz, int flags=SFS_ATTR_NOCD);

 private:

  void addnode(SFS_ittr *ittr);
 
#ifdef __USE_LARGEFILE64
  fs_inode *add_inode(fs_inode *parent, char *name, long long int offset, int sz);
  fs_inode *add_inode_from(fs_inode *neighbor, char *name, long long int offset, int sz);
#else 
  fs_inode *add_inode(fs_inode *parent, char *name, int offset, int sz);
  fs_inode *add_inode_from(fs_inode *neighbor, char *name, int offset, int sz);
#endif

  fs_inode *find_last_lesser_child(fs_inode *parent, char *name, int &first, int &eq);
  fs_inode *find_last_lesser_neighbor(fs_inode *neighbor, char *name, int &eq);
  void dump(char *path, fs_inode *inode);

  int writeFsHeader();
};


#endif
