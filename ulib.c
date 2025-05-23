#include "types.h"
#include "stat.h"
#include "fcntl.h"
#include "user.h"
#include "x86.h"
#include "thread.h"

#define PGSIZE 4096 

char*
strcpy(char *s, const char *t)
{
  char *os;

  os = s;
  while((*s++ = *t++) != 0)
    ;
  return os;
}

int
strcmp(const char *p, const char *q)
{
  while(*p && *p == *q)
    p++, q++;
  return (uchar)*p - (uchar)*q;
}

uint
strlen(const char *s)
{
  int n;

  for(n = 0; s[n]; n++)
    ;
  return n;
}

void*
memset(void *dst, int c, uint n)
{
  stosb(dst, c, n);
  return dst;
}

char*
strchr(const char *s, char c)
{
  for(; *s; s++)
    if(*s == c)
      return (char*)s;
  return 0;
}

char*
gets(char *buf, int max)
{
  int i, cc;
  char c;

  for(i=0; i+1 < max; ){
    cc = read(0, &c, 1);
    if(cc < 1)
      break;
    buf[i++] = c;
    if(c == '\n' || c == '\r')
      break;
  }
  buf[i] = '\0';
  return buf;
}

int
stat(const char *n, struct stat *st)
{
  int fd;
  int r;

  fd = open(n, O_RDONLY);
  if(fd < 0)
    return -1;
  r = fstat(fd, st);
  close(fd);
  return r;
}

int
atoi(const char *s)
{
  int n;

  n = 0;
  while('0' <= *s && *s <= '9')
    n = n*10 + *s++ - '0';
  return n;
}

void*
memmove(void *vdst, const void *vsrc, int n)
{
  char *dst;
  const char *src;

  dst = vdst;
  src = vsrc;
  while(n-- > 0)
    *dst++ = *src++;
  return vdst;
}

int thread_create(void (*start_routine)(void *, void *), void *arg1, void *arg2) {
  void *stack = malloc(PGSIZE);
  if(stack == 0)
    return -1;
  
  // Align stack to page boundary
  if((uint)stack % PGSIZE)
    stack = stack + (PGSIZE - ((uint)stack % PGSIZE));

  int pid = clone(start_routine, arg1, arg2, stack);
  if(pid < 0) {
    free(stack);
    return -1;
  }
  return pid;
}

int thread_join(void) {
  void *stack;
  int pid = join(&stack);
  if(pid != -1)
    free(stack);
  return pid;
}

// Add atomic fetch-and-add
static inline uint fetch_and_add(volatile uint *addr, uint val) {
  asm volatile("lock; xaddl %0, %1" :
               "+r" (val),  "+m" (*addr) :: "memory");
  return val;
}

void lock_init(ticket_lock_t *lock) {
  lock->ticket = 0;
  lock->serving = 0;
}

void lock_acquire(ticket_lock_t *lock) {
  int myturn = fetch_and_add(&lock->ticket, 1);
  while(lock->serving != myturn)
    ; // Spin
}

void lock_release(ticket_lock_t *lock) {
  lock->serving++;
}
