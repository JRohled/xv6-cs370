#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"
#include "kernel/param.h"

// Memory allocator by Kernighan and Ritchie,
// The C programming Language, 2nd ed.  Section 8.7.

typedef long Align;

union header {
  struct {
    union header *ptr;
    uint size;
  } s;
  Align x;
};

typedef union header Header;

static Header base;
static Header *freep;

void
free(void *ap)
{
  Header *bp, *p;

  bp = (Header*)ap - 1;
  for(p = freep; !(bp > p && bp < p->s.ptr); p = p->s.ptr)
    if(p >= p->s.ptr && (bp > p || bp < p->s.ptr))
      break;
  if(bp + bp->s.size == p->s.ptr){
    bp->s.size += p->s.ptr->s.size;
    bp->s.ptr = p->s.ptr->s.ptr;
  } else
    bp->s.ptr = p->s.ptr;
  if(p + p->s.size == bp){
    p->s.size += bp->s.size;
    p->s.ptr = bp->s.ptr;
  } else
    p->s.ptr = bp;
  freep = p;
}

static Header*
morecore(uint nu)
{
  char *p;
  Header *hp;

  if(nu < 4096)
    nu = 4096;
  p = sbrk(nu * sizeof(Header));
  if(p == SBRK_ERROR)
    return 0;
  hp = (Header*)p;
  hp->s.size = nu;
  free((void*)(hp + 1));
  return freep;
}

void*
malloc(uint nbytes)
{
  Header *p, *prevp;
  uint nunits;

  nunits = (nbytes + sizeof(Header) - 1)/sizeof(Header) + 1;
  if((prevp = freep) == 0){
    base.s.ptr = freep = prevp = &base;
    base.s.size = 0;
  }
  for(p = prevp->s.ptr; ; prevp = p, p = p->s.ptr){
    if(p->s.size >= nunits){
      if(p->s.size == nunits)
        prevp->s.ptr = p->s.ptr;
      else {
        p->s.size -= nunits;
        p += p->s.size;
        p->s.size = nunits;
      }
      freep = prevp;
      return (void*)(p + 1);
    }
    if(p == freep)
      if((p = morecore(nunits)) == 0)
        return 0;
  }
}

/*Returns number of bytes allocated but not used*/
uint64 frag_bytes(void)
{
  Header *p;
  uint64 freeBytes = 0;

  if(freep == 0) return 0;

  freeBytes += freep->s.size * sizeof(Header);
  p = freep->s.ptr;
  while(p != freep){
    freeBytes += p->s.size * sizeof(Header);
    p = p->s.ptr;
  }

  return freeBytes;
}

typedef struct Chunk{
  struct Chunk *next;
} Chunk;

typedef struct Page{
  int chunk_size;
  struct Page *next; 
  Chunk *free_chunks; //Free list of chunks
  char data[4096]; //All the bytes in the page(will be divided into chunks)
} Page;

static Page *pList;

/* Allocates nbytes from a segregated list*/
void* block_alloc(int nbytes)
{
  int pageSize = 4096; // Used for sbrk()
  int chunkSize = 1; // Size of every chunk in a page
  int chunkAmt = 0; // Num of chunks in a page
  
  
  // Check if bytes evenly divides page size
  while(chunkSize < nbytes){
    chunkSize *= 2;
  }

  Page *p;
  Chunk *c;
  Chunk *cUsing; //Chunk to return
  int makeNewpg = 1;
  p = pList;
  //Find Page With Chunk Size
  while(p){
    if(p->chunk_size == chunkSize){
      c = p->free_chunks;

      // Free space in page?
      if(!c){
        p = p->next;
        continue;
      }

      //Use first available free chunk and move next to head(p->free_chunks)
      cUsing = c;
      p->free_chunks = c->next;
      makeNewpg = 0;
      break;
    }
    p = p->next;
  }
  
  //If none exist or all are full, create new page
  if(makeNewpg){
    p = (Page *) sbrk(pageSize);
    if((char *)p == SBRK_ERROR)
      return 0;
    p->chunk_size = chunkSize;
    p->next = 0;
    char *dPtr = p->data;
    
    Chunk *cPrev = (Chunk *) dPtr;
    for(int i = 1; i < chunkAmt; i++){
      Chunk *cNew = (Chunk *)(dPtr + i * chunkSize);
      cPrev->next = cNew;
      cPrev = cNew;
    }
    cPrev->next = 0;

    p->free_chunks = (Chunk *) dPtr;
    cUsing = p->free_chunks;
    p->free_chunks = cUsing->next;

    //Add new page to page list
    Page *pg = pList;
    Page *pgPrev;
    if(!pg){
      //Page list empty
      pList = p;
    }else{
      //Not empty
      while(pg){
        pgPrev = pg;
        pg = pg->next;
      }
      pgPrev->next = p;
    }

  }
  
  return (void *) cUsing;
}

/* Frees an allocation made from a segregated list*/
void block_free(void *ptr)
{
  Page *p = pList;
  //Find page ptr belongs to
  while(p){
    if((char *)ptr >= (char *)p->data && (char *)ptr < (char *)p->data + 4096){
      break;
    }
    p = p->next;
  }

  //Add chunk back to free list
  Chunk *c = (Chunk *) ptr;
  c->next = p->free_chunks;
  p->free_chunks = c;

}

uint64 frag_bytes_segList()
{
  Page *p = pList;
  uint64 freeB = 0;

  while(p){
    Chunk *c = p->free_chunks;
    while(c){
      freeB += (uint64) p->chunk_size;
      c = c->next;
    }
    p = p->next;
  }

  return freeB;
}