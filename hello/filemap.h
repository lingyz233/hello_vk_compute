#include<fcntl.h>
#include<unistd.h>
#include<sys/mman.h>

#include<err.h>
#include<math.h>
#include<string.h>

/* struct filemap fm;
 * 打开已有文件 :
 * filemap_openfile (&fm, "./exemple.txt", O_RDWR, MAP_SHARED, PROT_READ|PROT_WRITE);
 * 使用文件 : 例如给文件第一个字节写'a'
 * fm.d[0] = 'a'; (注 : fm.d默认是char*类型)
 * 调整文件大小到1kb :
 * filemap_newsize (&fm, 1024)
 * 关闭文件 :
 * filemap_close (&fm);
*/

struct filemap {
  char * d; //public
  char * path; //public ro
  size_t fsize; //public //memory between fsize and msize is not for use
  size_t msize; //private
  int fd; //private
  int flags; //private
  int prot; //private
  int fflag; //private
};

struct struct_filemap_functions {
  _Bool (*open) (struct filemap * fmap, char * path, int fflag, int flags, int prot, int fmode, size_t hoped_size);
  _Bool (*creat) (struct filemap * fmap, char * path, int fflag, int flags, int prot, int fmode, size_t hoped_size);
  _Bool (*resize) (struct filemap * fmap, int newsize);
  _Bool (*close) (struct filemap * fmap);
};

extern struct struct_filemap_functions filemap;
