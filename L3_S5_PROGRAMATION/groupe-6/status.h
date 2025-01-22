#ifndef __STATUS_H_
#define __STATUS_H_

typedef enum
{
  OK = 0,
  ERR_ALLOC = 1,
  ERR_READ = 2,
  ERR_WRITE = 3,
  ERR_MAGIC_BYTES = 4,
} ERR_CODE;

#endif // __STATUS_H__
