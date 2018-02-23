#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "pool.h"
/* A.G.McDowell,  2 May 1999
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

/*
 * Can get loads of compiler warnings here because I don't assume that
 * size_t is unsigned.
*/

/* define FRAGILE if you want more speed. This means that the worst 
 *case cost
 * of poolMalloc() and poolFree() grows only with the number of
 * different block sizes, not the size of each block. The flip side
 * of this is less error checking to pick up store corruption from
 * bugs here or in user code. The worst case dependency of realloc()
 * is inevitable - it may have to copy the old data
 */

/*  #define FRAGILE */

 static const char *version = "A.G.McDowell 19990501";

/* Bit macros, since it should be both faster and more compact to use
 * macros than functions for this
 */
 #define BITTEST(p, n) ((p)->map[(n) >> 3] & (128 >>((n) & 7)))
 #define BITSET(p, n) {(p)->map[(n) >> 3] |= (128 >> ((n) & 7));}
 #define BITCLEAR(p, n) {(p)->map[(n) >> 3] &= ~(128 >> ((n) & 7));}

/* The top of a free block contains HEADER_LEN size_t s. We could
 * use either pointers or block offsets. We choose block offsets
 * to make it
 * easy to run this in shared memory, where different processes
 * may see the memory at different addresses. Despite this, we don't
 * attempt to handle locking or recovering from processes that die
 * leaving shared memory inconsistent
 */

 /* use OURNULL in our own lists. */
#define OURNULL ((size_t)-1)

/* offset of magic number in a free block */
#define OFF_MAGIC 0
/* magic number. The first block in a series
 * of free blocks starts with this value. The
 * later blocks start with zero. Any non-zero
 * value would do, but picking something unlikely
 * helps us pick up corruption earlier
 */
#define MAGIC ((size_t)0xa7130423)
/* offset of pointer to prev free block */
#define OFF_PREV 1
/* offset of pointer to next free block */
#define OFF_NEXT 2
/* trailer.  1 << trailer value is # of blocks in this chunk */
#define OFF_BITS (-1)
/* offset on block count to make zero an unlikely value */
#define LEN_OFFSET 1234

 char *poolInit(void *startAddress, size_t bytes, int minBits,
     int numSizes, size_t *freeList, char *bitMap,
     struct PoolInfo *pi)
{
    int i;
    size_t setBits;
    if ((1 << minBits) < (sizeof(size_t) * FREEBLOCK))
	return "minBits too small";
    if (numSizes <= 0)
	return "numSizes <= 0";
    if ((1 << (minBits + numSizes - 1)) == 0)
	return "numSizes + minBits too large";
    if (((int)startAddress) & ((1 << minBits) - 1))
	return "startAddress not on block boundary";
    pi->poolStart = (char *)startAddress;
    setBits = bytes >> minBits;
    pi->poolBlocks = setBits;
    pi->minBits = minBits;
    pi->numSizes = numSizes;
    pi->freeList = freeList;
    pi->map = bitMap;
    pi->freeBytes = 0;
    /* The bitmap will think everything is allocated, so areas
     * not handed over to us will not get merged in with any
     * freed blocks */
    memset(bitMap, -1, (setBits + 7) >> 3);
    for (i = 0; i < numSizes; i++)
	pi->freeList[i] = OURNULL;
    return NULL;
}

/* test memory. Should be optimised to use word operations */
static int memAll(void *p, size_t bytes)
{
    char *pp = (char *)p;
    if (bytes <= 0)
        return 1;
    if (*pp != (char)0xff)
    {
        return 0;
    }
    if (memcmp(pp, pp + 1, bytes - 1))
        return 0;
    return 1;
}

/* test bits. Return 1 iff all bit in range is set
 */
static int testAll(struct PoolInfo *pi, size_t firstBit, size_t pastBit)
{
    size_t i;
    size_t firstFull = (firstBit + 7) & ~7;
    size_t pastFull = pastBit & ~7;
    size_t past = pastBit;
    size_t bytes;

    if (past > firstFull)
	past = firstFull;
    for (i = firstBit; i < past; i++)
    {
	if (!BITTEST(pi, i))
	    return 0;
    }
    if (pastFull > firstFull)
    {
	bytes = pastFull - firstFull;
	bytes = bytes >> 3;
	if (!memAll(pi->map + (firstFull >> 3), bytes))
	    return 0;
    }
    if (pastFull < past)
	pastFull = past;
    for (i = pastFull; i < pastBit; i++)
    {
	if (!BITTEST(pi, i))
	    return 0;
    }
    return 1;
}

/* clear
 */
static void clearAll(struct PoolInfo *pi, size_t firstBit, size_t pastBit)
{
    size_t i;
    size_t firstFull = (firstBit + 7) & ~7;
    size_t pastFull = pastBit & ~7;
    size_t past = pastBit;
    size_t bytes;

    if (past > firstFull)
	past = firstFull;
    for (i = firstBit; i < past; i++)
	  BITCLEAR(pi, i);
    if (pastFull > firstFull)
    {
	bytes = pastFull - firstFull;
	bytes = bytes >> 3;
	memset(pi->map + (firstFull >> 3), 0, bytes);
    }
    if (pastFull < past)
	pastFull = past;
    for (i = pastFull; i < pastBit; i++)
	BITCLEAR(pi, i);
}

/* clear bits, checking that they are set.
 */
static char *clearCheck(struct PoolInfo *pi, size_t firstBit,
    size_t pastBit)
{
    if (!testAll(pi, firstBit, pastBit))
	return "bit already clear";
    clearAll(pi, firstBit, pastBit);
    return NULL;
}

/* test memory. */
static int memAny(void *p, size_t bytes)
{
    char *pp = (char *)p;
    if (bytes <= 0)
	return 0;
    if (*pp)
	return 1;
    if (bytes <= 1)
	return 0;
    if (memcmp(pp, pp + 1, bytes - 1))
	return 1;
    return 0;
}

/* test bits. Return 1 iff any bit in range is set
 */
static int testAny(struct PoolInfo *pi, size_t firstBit, size_t pastBit)
{
    size_t i;
    size_t firstFull = (firstBit + 7) & ~7;
    size_t pastFull = pastBit & ~7;
    size_t past = pastBit;
    size_t bytes;

    if (past > firstFull)
	past = firstFull;
    for (i = firstBit; i < past; i++)
    {
	if (BITTEST(pi, i))
	{
	    return 1;
	}
    }
    if (pastFull > firstFull)
    {
	bytes = pastFull - firstFull;
	bytes = bytes >> 3;
	if (memAny(pi->map + (firstFull >> 3), bytes))
	    return 1;
    }
    if (pastFull < past)
	pastFull = past;
    for (i = pastFull; i < pastBit; i++)
    {
	if (BITTEST(pi, i))
	{
	    return 1;
	}
    }
    return 0;
}


/* Unchecked add to free list of a 1 << bitsFree blocks starting
 * at block firstFree in the pool */
static void makeFree(struct PoolInfo *pi, size_t firstFree,
    int bitsFree, int zap)
{
    size_t *ptr;
    size_t *pastPtr;
    size_t *nextPtr;
    size_t nextOff;
    int byteBits = bitsFree + pi->minBits;
    size_t bytesToFree = 1 << byteBits;
    size_t blocksToFree = 1 << bitsFree;
    ptr = (size_t *) (pi->poolStart + (firstFree << pi->minBits));
    if (zap)
    {
	/* Zap free area and bitmap to zero */
	/* Once the area has been initialised and released, it is
	 * never necessary to zap out ALL of an area - either free
	 * store or bitmap. You will always know that correctly
	 * functioning code either means all but a tiny part of
	 * this area is already clear (the bitmap), or will
	 * ever be accessed (only tail probed by
	 * poolFree() in memory). But for the moment it makes it
	 * easier to see this code is correct and easier to check
	 * for corruption with poolCheck() if we zap everything.
	 * The cost is always kept no more than linear in the
	 * amount of core the users is allocing or freeing anyway
	 *
	 * In fact, under FRAGILE we don't change this code,
	 * just change the zap argument
	 */
	memset(ptr, 0, bytesToFree);
	clearAll(pi, firstFree, firstFree + blocksToFree);
    }
    /* build up our own area and link in to free list */
    ptr[OFF_MAGIC] = MAGIC;
    ptr[OFF_PREV] = OURNULL;
    nextOff = pi->freeList[bitsFree];
    ptr[OFF_NEXT] = nextOff;
    pastPtr = (size_t *) (((char *)ptr) + (1 << byteBits));
    pastPtr[OFF_BITS] = bitsFree + LEN_OFFSET;
    if (nextOff != OURNULL)
    {
	size_t *nextPtr = (size_t *)(pi->poolStart +
	    (nextOff << pi->minBits));
	nextPtr[OFF_PREV] = firstFree;
    }
    pi->freeList[bitsFree] = firstFree;
    pi->freeBytes += (1 << byteBits);
}

/* Add a chunk to the free list */
static void toFree(struct PoolInfo *pi, size_t firstBlock,
    size_t pastBlock, int zap)
{
    size_t i;
    size_t tryBits = 0;
    size_t tryLen = 1;
    for(i = firstBlock; i < pastBlock;)
    {
        /* To avoid cost quadratic in the number of different
	 * blocks produced from this chunk of store, we have to
	 * use the size of the previous block produced from this
	 * chunk as the starting point to work out the size of the
	 * next block we can produce. If you look at the binary
	 * representation of the starting points of the blocks 
	 * produced, you can see that you first of all increase the 
	 * size of the blocks produced up to some maximum as the
	 * address dealt with gets offsets added on which zap out
	 * low order bits, then decrease as the low order bits of the
	 * final block produced get added in. E.g. as you go from
	 * 001 to 0111 you generate blocks
	 * of size 001 at 001 taking you to 010
	 * of size 010 at 010 taking you to 100
	 * of size 010 at 100 taking you to 110
	 * of size 001 at 110 taking you to 111
	 * So the maximum total cost of the loops below this comment
	 * is one trip from the lowest blocksize to the highest and
	 * back again.
	 */
	for(;tryBits < pi->numSizes - 1;)
	{
	    size_t nextTry = tryBits + 1;
	    size_t nextLen = tryLen + tryLen;
	    if(i + nextTry > pastBlock) 
	    { /* off end of chunk to be freed */
	        break;
	    }
	    if (i & (nextLen - 1))
	    { /* block would not be on boundary */
	        break;
	    }
	    tryBits = nextTry;
	    tryLen = nextLen;
	}
	for(;tryBits >= 0;tryBits--, tryLen >>= 1)
	{
	    if(i + tryLen > pastBlock) 
	    { /* off end of chunk to be freed */
	        continue;
	    }
	    if (i & (tryLen - 1))
	    { /* block would not be on boundary */
	        continue;
	    }
	    /* OK */
	    break;
	}
	if (tryBits < 0)
	    break;
	makeFree(pi, i, tryBits, zap);
	i += tryLen;
    }
}

char *poolRelease(struct PoolInfo *pi, size_t startBlock,
    size_t blocks)
{
    size_t pastBlockOff;
    char *ret;
    if (startBlock < 0 || startBlock >= pi->poolBlocks ||
	blocks < 0 || startBlock + blocks > pi->poolBlocks)
	return "bad parms";
    pastBlockOff = startBlock + blocks;
    ret = clearCheck(pi, startBlock, pastBlockOff);
    if (ret != NULL)
	return ret;
    /* Now add everything in range to the free list */
    toFree(pi, startBlock, pastBlockOff, 1);
    /*
    {
	int i;
	for (i = 0; i < pi->numSizes; i++)
	{
	    fprintf(stderr, "Offset %d is %d\n",
		i, pi->freeList[i]);
	}
    }
    */
    return NULL;
}

/* find store for 1 << bits blocks */
static char *poolBuddyMalloc(struct PoolInfo *pi, int bits,
    void **newStore)
{
    int retBits;
    size_t *ptr;
    size_t *pastPtr;
    size_t offset;
    size_t nextOffset;
    size_t *nextPtr;
    size_t past;
    /*
    fprintf(stderr, "Request for %d bits\n", bits);
    */
    if (newStore == NULL)
	return "null pointer to new store";
    if (pi == NULL)
	return "null pool";
    if (pi->freeBytes < (1 << (bits + pi->minBits)))
    {
	return NO_STORE;
    }
    /* Find a list with blocks big enough on it */
    for (retBits = bits; retBits < pi->numSizes; retBits++)
    {
	if (pi->freeList[retBits] != OURNULL)
	    break;
    }
    if (retBits >= pi->numSizes)
    {
	return NO_STORE;
    }
    offset = pi->freeList[retBits];
    if (offset < 0 || offset >= pi->poolBlocks)
	return "corrupted free list";
    ptr = (size_t *)(pi->poolStart + (offset << pi->minBits));
    if (ptr[OFF_MAGIC] != MAGIC)
	return "corrupted free area";
    if (ptr[OFF_PREV] != OURNULL)
        return "corrupted free area";
    pastPtr = (size_t *)(((char *)ptr) +
        (1 << (retBits + pi->minBits)));
    if (pastPtr[OFF_BITS] != retBits + LEN_OFFSET)
        return "corrupted free area";

#ifndef FRAGILE
    /* Check out area you are going to return to the user as
     * allocated store. Don't check out the rest, as that would make
     * the cost of this operation unpredictable. This code produces
     * no result except a check for errors which should never occur
     * so it's check code only, which you could delete to save time
     */
    if (testAny(pi, offset, offset + (1 << bits)))
	return "free area does not match bitmap";
    if (memAny(ptr + HEADER_LEN, (1 << (bits + pi->minBits)) -
	FREEBLOCK * sizeof(void *)))
	return "corrupted free area";
#endif


    /* remove from free list */
    nextOffset = ptr[OFF_NEXT];
    if (nextOffset != OURNULL)
    {
	if (nextOffset < 0 || nextOffset >= pi->poolBlocks)
	    return "corrupted free list";
	nextPtr = (size_t *)(pi->poolStart + (nextOffset << pi->minBits));
	if (nextPtr[OFF_MAGIC] != MAGIC ||
	    nextPtr[OFF_PREV] != offset)
	    return "corrupted free area";
	nextPtr[OFF_PREV] = OURNULL;
    }
    pi->freeList[retBits] = nextOffset;
    /* Mark end of allocated area */
    past = offset + (1 << bits);
    BITSET(pi, past - 1);
    /* If we used a larger free block than we needed, free the rest */
    pi->freeBytes -= (1 << (retBits + pi->minBits));
	/* accounts for chunks about to be freed again by toFree */
    toFree(pi, past, offset + (1 << retBits), 0);
    *newStore = ptr;
    return NULL;
}

char *poolMalloc(struct PoolInfo *pi, size_t bytes, void **newStore)
{
    int bits;
    size_t size;
    if (newStore == NULL)
	return "null pointer to new store";
    if (pi == NULL)
	return "null pool";
    size = 1 << pi->minBits;
    for (bits = 0; size < bytes; bits++)
    {
	if (bits >= pi->numSizes)
	    return NO_STORE;
	size += size;
	if (size == 0)
	    return NO_STORE;
    }
    return poolBuddyMalloc(pi, bits, newStore);
}

static char *poolBuddyAllocSize(struct PoolInfo *pi,
    void *sss, int *numBits)
{
    size_t byteOff;
    size_t blockOff;
    char *storage = (char *)sss;
    int ourBits;
    size_t blocks;
    if (numBits == NULL)
	return "nowhere to store result";
    if (storage < pi->poolStart)
	return "storage not in pool";
    byteOff = storage - pi->poolStart;
    blockOff = byteOff >> pi->minBits;
    if (byteOff != (blockOff << pi->minBits))
	return "storage not on block boundary";
    if (blockOff >= pi->poolBlocks)
	return "storage not in pool";
    blocks = 1;
    for(ourBits = 0;;ourBits++)
    {
	if ((blockOff + blocks > pi->poolBlocks) ||
	    (ourBits >= pi->numSizes) ||
	    (blockOff & (blocks - 1)))
	{
	    return "bad alloc pointer";
	}
	if (BITTEST(pi, blockOff + blocks - 1))
	    break;
	blocks += blocks;
    }
    *numBits = ourBits;
    return NULL;
}

char *poolRealloc(struct PoolInfo *pi, void *ptrv, size_t size,
    void **newPtr)
{
    int canBits;
    size_t blockNo;
    int bits;
    char *error;
    size_t oldBlocks;
    size_t newBlocks;
    int lg2Bytes;
    char *ptr = (char *)ptrv;
    if (size < 0)
	return "-ve size to poolRealloc";
    if (size == 0)
    {
	return poolFree(pi, ptr);
    }
    if (ptr == NULL)
    {
	return poolMalloc(pi, size, newPtr);
    }
    error = poolBuddyAllocSize(pi, ptr, &bits);
    if (error != NULL)
	return error;
    lg2Bytes = bits + pi->minBits;
    if ((1 << lg2Bytes) < size)
    {
	void *store;
	error = poolMalloc(pi, size, newPtr);
	if (error != NULL)
	    return error;
	memcpy(*newPtr, ptr, 1 << lg2Bytes);
	return poolFree(pi, ptr);
    }
    *newPtr = ptr;
    for (canBits = pi->minBits; (1 << canBits) < size; canBits++);
    if (canBits >= lg2Bytes)
    {
	return NULL;
    }
    oldBlocks = 1 << bits;
    newBlocks = 1 << (canBits - pi->minBits);
    blockNo = (ptr - pi->poolStart) >> pi->minBits;
    /* set end bits to mark new size */
    BITSET(pi, blockNo + newBlocks - 1);

#ifdef FRAGILE
    BITCLEAR(pi, blockNo + oldBlocks - 1);
    toFree(pi, blockNo + newBlocks, blockNo + oldBlocks, 0);
#else
    /* The new free block is set up by toFree */
    toFree(pi, blockNo + newBlocks, blockNo + oldBlocks, 1);
#endif
    return NULL;
}

char *poolFree(struct PoolInfo *pi, void *storage2)
{
    size_t *sptr;
    size_t *nextPtr;
    size_t nextOffset;
    size_t *pastPtr;
    char *ret;
    int bits;
    size_t blockOff;
    size_t blocksHere;
    int oldBits;
    char *storage = (char *)storage2;
    if (storage == NULL)
	return NULL;
    ret = poolBuddyAllocSize(pi, storage, &bits);
    if (ret != NULL)
	return ret;
    blocksHere = 1 << bits;
    blockOff = (storage - pi->poolStart) >> pi->minBits;
    /* mark as free in bitmap */
    BITCLEAR(pi, blockOff + blocksHere - 1);
    /* clear store. For absolute speed and much less error
     * checking could only clear areas checked during buddy
     * merge
     */
#ifndef FRAGILE
    memset(storage, 0, 1 << (bits + pi->minBits));
#endif
    /* merge with any buddies */
    oldBits = bits;
    while(bits < pi->numSizes - 1)
    {	/* while you can merge two blocks and get a legal
	   block size */
	size_t *buddyPtr;
	size_t *prevPtr;
	size_t prevOffset;
	int trueBits;
	size_t buddy = blockOff ^ (1 << bits);
	/* Could buddy be out of range? */
	if (buddy < 0)
	    break;
	if (buddy + (1 << bits) > pi->poolBlocks)
	    break;
	if (BITTEST(pi, buddy + (1 << bits) - 1))
	    break; /* buddy is allocated */
	pastPtr = (size_t *)(pi->poolStart + (buddy << pi->minBits) +
	                     (1 << (bits + pi->minBits)));
	trueBits = pastPtr[OFF_BITS] - LEN_OFFSET;
	if (trueBits < 0 || trueBits >= pi->numSizes)
	    return "corruption 1.1 in free store";
	if (trueBits != bits) /* not completely free */
	    break;
	/* Found buddy on free list - remove it and merge */
	pastPtr[OFF_BITS] = 0;
	buddyPtr = (size_t *)(pi->poolStart + (buddy << pi->minBits));
	if (buddyPtr[OFF_MAGIC] != MAGIC)
	    return "corruption 2 in free store";
	if (pi->freeList[bits] == OURNULL)
	    return "corruption 3 in free store";
	prevOffset = buddyPtr[OFF_PREV];
	if (prevOffset == OURNULL)
	{
	    size_t *nextPtr;
	    size_t nextOffset;
	    nextOffset = buddyPtr[OFF_NEXT];
	    if (nextOffset != OURNULL)
	    {
		if (nextOffset < 0 || nextOffset >= pi->poolBlocks)
		    return "corruption 3.1 in free store";
		nextPtr = (size_t *)(pi->poolStart +
		    (nextOffset << pi->minBits));
		if (nextPtr[OFF_MAGIC] != MAGIC)
		    return "corruption 4 in free store";
		if (nextPtr[OFF_PREV] != buddy)
		    return "corruption 5 in free store";
		nextPtr[OFF_PREV] = OURNULL;
	    }
	    pi->freeList[bits] = nextOffset;
	}
	else
	{
	    size_t nextOffset;
	    if (prevOffset < 0 || prevOffset >= pi->poolBlocks)
		return "corruption 5.1 in free store";
	    prevPtr = (size_t *)(pi->poolStart +
		(prevOffset << pi->minBits));
	    if (prevPtr[OFF_MAGIC] != MAGIC)
		return "corruption 6 in free store";
	    if (prevPtr[OFF_NEXT] != buddy)
		return "corruption 7 in free store";
	    nextOffset = buddyPtr[OFF_NEXT];
	    if (nextOffset != OURNULL)
	    {
		if (nextOffset < 0 || nextOffset >= pi->poolBlocks)
		    return "corruption 7.1 in free store";
		nextPtr = (size_t *)(pi->poolStart +
		    (nextOffset << pi->minBits));
		if (nextPtr[OFF_MAGIC] != MAGIC)
		    return "corruption 8 in free store";
		if (nextPtr[OFF_PREV] != buddy)
		    return "corruption 9 in free store";
		nextPtr[OFF_PREV] = prevOffset;
	    }
	    prevPtr[OFF_NEXT] = nextOffset;
	}
	/* clear buddy's free block */
	memset(buddyPtr, 0, HEADER_LEN * sizeof(size_t));
	bits++;
	if (buddy < blockOff)
	{ /* Merged block starts at buddy */
	    blockOff = buddy;
	    storage = (char *)buddyPtr;
	}
    }
    /* add to free list */
    sptr = (size_t *)storage;
    sptr[OFF_MAGIC] = MAGIC;
    sptr[OFF_PREV] = OURNULL;
    nextOffset = pi->freeList[bits];
    if (nextOffset != OURNULL)
    {
	if (nextOffset < 0 || nextOffset >= pi->poolBlocks)
	    return "corruption 9.1 in free store";
	nextPtr = (size_t *)(pi->poolStart +
	    (nextOffset << pi->minBits));
	if (nextPtr[OFF_MAGIC] != MAGIC)
	{
	    return "corruption 10 in free store";
	}
	if (nextPtr[OFF_PREV] != OURNULL)
	    return "corruption 11 in free store";
	nextPtr[OFF_PREV] = blockOff;
    }
    sptr[OFF_NEXT] = nextOffset;
    pastPtr = (size_t *)(storage + (1 << (pi->minBits + bits)));
    pastPtr[OFF_BITS] = bits + LEN_OFFSET;
    pi->freeList[bits] = blockOff;
    pi->freeBytes += (1 << (oldBits + pi->minBits));
    return NULL;
}

char *poolAvailable(struct PoolInfo *pi, size_t *bytesFree)
{
    if (pi == NULL)
	return "null pointer to pi";
    if (bytesFree == NULL)
	return "no room to store bytes free";
    *bytesFree = pi->freeBytes;
    return NULL;
}

char *poolCheck(struct PoolInfo *pi, size_t *counts)
{
    int size;
    size_t seenStore = 0;
    if (pi == NULL)
	return "null pointer to pi";
    /* Check the free list for all possible sizes */
    for (size = 0; size < pi->numSizes; size++)
    {
	size_t offset;
	size_t *ptr;
	size_t *pastPtr;
	size_t prevOffset = OURNULL;
	/* maxBlocks catches loops in the free list */
	size_t maxBlocks = pi->poolBlocks >> size;
	size_t blocksSeen = 0;
	for (offset = pi->freeList[size];
	     offset != OURNULL && blocksSeen < maxBlocks; blocksSeen++)
	{
	    if (offset < 0 || offset >= pi->poolBlocks)
		return "bad pointer";
	    if (testAny(pi, offset,
		offset + (1 << size)))
		return "free area not free in bitmap";
	    ptr = (size_t *)(pi->poolStart + (offset << pi->minBits));
	    if (ptr[OFF_MAGIC] != MAGIC)
		return "bad magic";
	    if (ptr[OFF_PREV] != prevOffset)
		return "crossed pointers";
	    pastPtr = (size_t *)(((char *)ptr) +
		(1 << (pi->minBits + size)));
	    if (pastPtr[OFF_BITS] != size + LEN_OFFSET)
	    {
	        return "size mismatch";
	    }
#ifndef FRAGILE
	    if (memAny(ptr + HEADER_LEN,
		       (1 << (size + pi->minBits))
		       - FREEBLOCK * sizeof(size_t)))
	    {
		return "free area corrupt";
	    }
#endif
	    prevOffset = offset;
	    offset = ptr[OFF_NEXT];
	    seenStore += (1 << size);
	}
	if (offset != OURNULL)
	   return "free list impossibly long - must be cycle";
        if (counts != NULL)
	    counts[size] = blocksSeen;
    }
    if ((seenStore << pi->minBits) != pi->freeBytes)
    {
	return "store accounting does not balance";
    }
    return NULL;
}
