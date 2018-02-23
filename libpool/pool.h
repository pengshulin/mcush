#ifndef AGMPOOL990423
#define AGMPOOL990423

/* A.G.McDowell, 2 May 1999
 * This comes WITHOUT WARRANTY - it's really just demo code. You
 * can find a PROPER memory allocator, by Doug Lea, at
 * http://g.oswego.edu
 * There is a paper pointing out why it is very good and this
 * (the buddy block system) wastes memory in comparison
 * at http://www.cs.utexas.edu/usrs/oops/papers.html -
 * "The Memory Fragmentation Problem: solved?"
 * by Wilson and Johnstone
 * Thanks to Niall Murphy - http://www.iol.ie/~nmurphy for these
 * references
 */

/* These routines manage a pool of free storage, using the Buddy
system. See "The Art of Computer Programming", by D.E.Knuth, Vol. 1,
section 2.5, Algorithm R, and Exercise 29 of that section.

We manage a pool of storage of some fixed length and starting point,
using an auxiliary bitmap, and some small space for linked lists. We
only ever deal with block of free and allocated storage of size
exactly 2**k, for some integer k in a pre-determined range. Each block
starts at an address which is a multiple of its blocksize. When a
block is allocated, all of it may be used. The auxiliary bitmap needs
one bit for each possible block of the smallest size. The minimum
block size must be large enough to cover at least FREEBLOCK ints

The worst case time taken to allocate a block grows only with the
size of the block required plus the
number of different possible blocksizes, which is constant for all
practical purposes (certainly less than 64, since each size is a power
of two). The worst case time taken to free a block is the same.

If this allocator fails, there is no way to place a block of the size
requested within the pool starting at an address that is a multiple of
its blocksize without overlapping some already allocated block.
*/


/* Size of header + trailer in a free block, in ints */
#define HEADER_LEN 3
#define TRAILER_LEN 1
#define FREEBLOCK (HEADER_LEN + TRAILER_LEN)

struct PoolInfo
{
    char *poolStart; /* First byte in the pool */
    int poolBlocks;

    int minBits;     /* Minimum block size is 1 << minBits */
    int numSizes;    /* sizes go up by powers of two */
    size_t *freeList; /* free lists */
    char *map;	     /* bit map */
    size_t freeBytes;
};

/* Each minimum size block has one corresponding bit in the bit map.
 * An allocated block is made up of 2**n minimum size blocks. Each
 * of their corresponding bits in the bit map are clear, except for
 * the last, which is set. This means that if you get passed a pointer
 * to an allocated block (e.g. via free()), you can tell how big it is
 * by looking at corresponding bits at offsets 0, 1, 3, 7, ... 2**n-1
 * and stopping when you find one that is set.

 * A free block has all of its corresponding bits clear. Since all of
 * the blocks are of size 2**n, each block above the maximum size is
 * made up of two blocks of the next lower size. These are buddies of
 * each other. When you have worked out the size of a previously
 * allocated block, it is important to know if its buddy is completely
 * free or not. You could just check all of the buddy's bitmap, but
 * I want to do it quicker than that, because the size of the buddys
 * we have to deal with is not predictable from the size of the store
 * that is being freed

 * First look at the last bit in your buddy. If it is
 * set, at least one of your buddy's blocks is still allocated. If it
 * is clear, at least that block is free, because...
 * Suppose that you are a block to the right of your buddy. Then your
 * buddy's last block comes just before your first. So if the last
 * block is allocated, it is the last block in a sequence of allocated
 * blocks, so its bit must be set. Suppose that you are a block to the
 * left of your buddy. Then your buddy's last block is the last block
 * in the larger block formed by both you and your buddy. If your
 * buddy was part of a larger block, it would have to include you, so
 * it's not. So your buddy's last block must be the last block in a
 * sequence if it is allocated, so its bit must be set.

 * If the last bit in your buddy's block is clear, then that block is
 * free. Now you need to know of the whole buddy is
 * free. You can find this out by looking at the last word in the
 * free block. This holds the size of the free block (+ an offset,
 * to make the code more robust). If that size tells you the length
 * of the free block is the whole buddy block you can merge it with
 * the current block
 */

 /* All routines return NULL for success, a static ASCIZ error message
  * otherwise.
  */

 /* Initialise a pool, initially with no free store. You will have
  * to call poolRelease() to say which bytes in the pool you want
  * to allow to be allocated. This is because all of the offsets
  * are relative to the start of a block, and you might want to
  * allocate all of a chunk of 2^n bytes except for some region
  * at the beginning, and still want to treat the start of that
  * region as offset 0 when working out what an even block boundary
  * is.  startAddress is the first byte in the pool. This address
  * must be a multiple of 1 << minBits.  bytes is
  * the number of bytes in the pool.  1 << minBits
  * is the smallest block size. This must be
  * big enough that the smallest possible block has room for 3 ints.
  * The block sizes go up in powers of 2. There are numSizes of them
  * and you can't allocate anything bigger than the biggest block size
  * freeList is an array of
  * numSizes ints which is used to hold the head pointers of a free
  * list for every possible block size. bitMap is a pointer to
  * an array capable of holding 1 bit for each minimum sized
  * block in the chunk. So this array has at least
  * (((bytes >> minBits) + 7) >> 3
  * bytes in it. pi is a pointer to a struct PoolInfo that is
  * used to store all the bookkeeping info we need.
  */

char *poolInit(void *startAddress, size_t bytes, int minBits,
    int numSizes, size_t *freeList, char *bitMap, struct PoolInfo *pi);

/* Release storage within an initialised pool. This is NOT free -
 * you normally call this a small number of times just after poolInit.
 * This should never be called on storage that has already be
 * released, allocated, or freed. This routine doesn't coalesce
 * blocks with those previously released, so release the regions
 * you want to use in chunks of maximum possible size.
 */

 char *poolRelease(struct PoolInfo *pi, size_t startBlock,
     size_t blocks);

 /* Allocate storage of <bytes> bytes. Notice that the pointer
  * to allocated storage
  * is returned as the third parameter, because the return value is
  * NULL on success
  */
char *poolMalloc(struct PoolInfo *pi, size_t bytes, void **newStore);

/* Realloc as with malloc. ptr is old storage, size is required
 * size in bytes. newPtr will be set to any new storage allocated
 */
char *poolRealloc(struct PoolInfo *pi, void *ptr, size_t size,
    void **newPtr);

/* Return if no store */
#define NO_STORE "no store"

/* Free previously allocated storage */
char *poolFree(struct PoolInfo *pi, void *storage);

/* Work out the total number of free bytes */
char *poolAvailable(struct PoolInfo *pi, size_t *bytesFree);

/* Check the pool. This
 * takes time proportional to the amount of available storage.
 * if counts is not NULL, it should contain one int for every
 * block size. That int will be set to a count of the number
 * of blocks of that size
 */
 char *poolCheck(struct PoolInfo *pi, size_t *counts);

 #endif
