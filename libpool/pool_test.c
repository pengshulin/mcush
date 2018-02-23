#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "pool.h"

/* A.G.McDowell, 2 May 1999 */

/* test poolxxx() memory allocation routines */
static int memCmpChar(void *p, size_t bytes, char x)
{
    char *pp = (char *)p;
    char difference;
    if (bytes <= 0)
	return 0;
    difference = *pp - x;
    if (difference != 0)
    {
	return difference;
    }
    if (bytes <= 1)
	return 0;
    return memcmp(pp, pp + 1, bytes - 1);
}

main(int argc, char *argv[])
{
    struct PoolInfo swimming;
    int poolBytes = 100000;
    char *poolSpace;
    char *alignedPoolSpace;
    int minBits = 4;
    int bitsReq = 6;
    int re = 2;
    int maxBits = 18;
    char *bitMap;
    int bitSize;
    char *error;
    int mask;
    size_t *freeList;
    void **myList;
    int bytesBack;
    size_t avail;
    int linked;
    int unlinked;
    int numBlocks;
    int check = 1;
    int argp;
    int trouble = 0;
    int allocCount = 0;
    int freeCount = 0;
    int reallocCount = 0;
    int checkCount = 0;
    size_t *countsBefore;
    size_t *countsAfter;
    int passes = 1;
    int pass;

    for (argp = 1; argp < argc; argp++)
    {
	if (argp < argc - 1 && !strcmp(argv[argp], "-bitsReq"))
	{
	    argp++;
	    if (sscanf(argv[argp], " %d", &bitsReq) != 1)
	    {
		fprintf(stderr, "Could not read bitsReq in %s\n",
		    argv[argp]);
		trouble = 1;
	    }
	}
	else if (argp < argc - 1 && !strcmp(argv[argp], "-maxBits"))
	{
	    argp++;
	    if (sscanf(argv[argp], " %d", &maxBits) != 1)
	    {
		fprintf(stderr, "Could not read maxBits in %s\n",
		    argv[argp]);
		trouble = 1;
	    }
	}
	else if (argp < argc - 1 && !strcmp(argv[argp], "-minBits"))
	{
	    argp++;
	    if (sscanf(argv[argp], " %d", &minBits) != 1)
	    {
		fprintf(stderr, "Could not read minBits in %s\n",
		    argv[argp]);
		trouble = 1;
	    }
	}
	else if (argp < argc - 1 && !strcmp(argv[argp], "-passes"))
	{
	    argp++;
	    if (sscanf(argv[argp], " %d", &passes) != 1)
	    {
		fprintf(stderr, "Could not read passes in %s\n",
		    argv[argp]);
		trouble = 1;
	    }
	}
	else if (argp < argc - 1 && !strcmp(argv[argp], "-poolBytes"))
	{
	    argp++;
	    if (sscanf(argv[argp], " %d", &poolBytes) != 1)
	    {
		fprintf(stderr, "Could not read pool bytes in %s\n",
		    argv[argp]);
		trouble = 1;
	    }
	}
	else if (argp < argc - 1 && !strcmp(argv[argp], "-re"))
	{
	    argp++;
	    if (sscanf(argv[argp], " %d", &re) != 1)
	    {
		fprintf(stderr, "Could not read re in %s\n",
		    argv[argp]);
		trouble = 1;
	    }
	}
	else if (!strcmp(argv[argp], "-unChecked"))
	{
	    check = 0;
	}
	else
	{
	    fprintf(stderr, "Cannot handle flag %s\n", argv[argp]);
	    trouble = 1;
	}
    }

#define SILLYMIN 4
    if (minBits < SILLYMIN)
    {
	fprintf(stderr, "minBits=%d < %d is too small\n",
	    minBits, SILLYMIN);
	trouble = 1;
    }
    if (maxBits < minBits)
    {
	fprintf(stderr, "maxBits = %d < minBits = %d\n",
	    maxBits, minBits);
	trouble = 1;
    }
    if (re < 0)
    {
	fprintf(stderr, "re + %d < 0\n", re);
	trouble = 1;
    }

    if (trouble)
    {
	fprintf(stderr, "Usage is %s [-bitsReq #] [-maxBits #] "
	    "[-minBits #] [-poolBytes #] [-unChecked]\n", argv[0]);
	return EXIT_FAILURE;
    }

    fprintf(stdout,
"bitsReq = %d maxBits = %d minBits = %d passes = %d poolBytes = %d re = %d check = %d\n",
	bitsReq, maxBits, minBits, passes, poolBytes, re, check);
    fflush(stdout);

    poolSpace = (char *)malloc(poolBytes);
    if (poolSpace == NULL)
    {
	fprintf(stderr, "Could not alloc %d bytes\n",
	    poolBytes);
	return EXIT_FAILURE;
    }
    mask = (1 << minBits) - 1;
    fprintf(stdout, "Unaligned pool starts %p mask %x\n",
	poolSpace, mask);
    alignedPoolSpace = (char *)((((int)poolSpace) + mask) & ~mask);
    poolBytes += poolSpace - alignedPoolSpace;
    poolBytes &= ~mask;
    numBlocks = poolBytes >> minBits;
    fprintf(stdout, "Creating %d blocks of %d bytes\n", numBlocks,
	1 << minBits);
    fprintf(stdout, "Will allocate blocks of %d bytes, and realloc"
	" them to %d bytes and back\n", 1 << bitsReq,
	1 << (bitsReq + re));
    bitSize = (numBlocks + 7) >> 3;
    bitMap = (char *)malloc(bitSize);
    if (bitMap == NULL)
    {
	fprintf(stderr, "Could not alloc %d bytes for bitmap\n",
	    bitSize);
	return EXIT_FAILURE;
    }
    freeList = (size_t *)malloc(sizeof(size_t) * (maxBits - minBits + 1));
    if (freeList == NULL)
    {
	fprintf(stderr, "Cannot alloc freelist\n");
	return EXIT_FAILURE;
    }
    countsBefore = (size_t *)malloc(sizeof(size_t) * (maxBits - minBits + 1));
    if (countsBefore == NULL)
    {
	fprintf(stderr, "Cannot alloc countsBefore\n");
	return EXIT_FAILURE;
    }
    countsAfter = (size_t *)malloc(sizeof(size_t) * (maxBits - minBits + 1));
    if (countsAfter == NULL)
    {
	fprintf(stderr, "Cannot alloc countsAfter\n");
	return EXIT_FAILURE;
    }

    fprintf(stdout, "Aligned pool starts at %p size %d\n",
	alignedPoolSpace, poolBytes);
    error = poolInit(alignedPoolSpace, poolBytes, minBits,
	maxBits - minBits + 1, freeList, bitMap, &swimming);
    if (error != NULL)
    {
	fprintf(stderr, "poolInit error %s\n", error);
	return EXIT_FAILURE;
    }
    if (check)
    {
	checkCount++;
	error = poolCheck(&swimming, NULL);
	if (error != NULL)
	{
	    fprintf(stderr, "Failed pool check: %s\n", error);
	    return EXIT_FAILURE;
	}
    }
    error = poolRelease(&swimming, 0, numBlocks);
    if (error != NULL)
    {
	fprintf(stderr, "Release error %s\n", error);
	return EXIT_FAILURE;
    }
    if (check)
    {
	checkCount++;
	error = poolCheck(&swimming, countsBefore);
	if (error != NULL)
	{
	    fprintf(stderr, "Failed pool check after release: %s\n", error);
	    return EXIT_FAILURE;
	}
    }
    error = poolAvailable(&swimming, &avail);
    if (error != NULL)
    {
	fprintf(stderr, "avail error %s\n", error);
	return EXIT_FAILURE;
    }
    fprintf(stdout, "Past initialisation: %d available\n", avail);
    fflush(stdout);
    for (pass = 0; pass < passes; pass++)
    {
	myList = NULL;
	bytesBack = 0;
	linked = 0;
	unlinked = 0;
	for(;;)
	{
	    void *store;
	    void **pStore;
	    allocCount++;
	    error = poolMalloc(&swimming, 1 << bitsReq, &store);
	    if (error != NULL)
	    {
		if (!strcmp(error, NO_STORE))
		    break;
		fprintf(stderr, "Error: %s\n", error);
		return EXIT_FAILURE;
	    }
	    /* This byte pattern is checked just before we free everything
	       by working out what linked must have been */
#ifndef FRAGILE
	    memset(store, linked & 0xff, 1 << bitsReq);
#endif
	    /*
	    fprintf(stderr, "Got store at %p\n", store);
	    */
	    if (check)
	    {
		checkCount++;
		error = poolCheck(&swimming, NULL);
		if (error != NULL)
		{
		    fprintf(stderr, "Failed check after alloc: %s\n", error);
		    return EXIT_FAILURE;
		}
	    }
	    /* Here we realloc up, then down, a block of store.
	     * Since reallocing up forces us on to a higher boundary,
	     * this produces worst case free store fragmentation
	     */
	    pStore = (void **) store;
	    error = poolRealloc(&swimming, pStore, 1 << (bitsReq + re),
		&store);
	    if (error != NULL)
	    {
		if (!strcmp(error, NO_STORE))
		{
		    freeCount++;
		    error = poolFree(&swimming, pStore);
		    if (error != NULL)
		    {
			fprintf(stderr, "Failed free after realloc: %s\n",
			    error);
			return EXIT_FAILURE;
		    }
		    break;
		}
		fprintf(stderr, "Failed realloc: %s\n", error);
		return EXIT_FAILURE;
	    }
#ifndef FRAGILE
	    memset(((char *)store) + (1 << bitsReq),
	     linked & 0xff, (1 << (bitsReq + re)) -
					     (1 << bitsReq));
#endif
	    pStore = (void **)store;
	    /*
	    fprintf(stderr, "Realloced store to %p\n", store);
	    */
	    if (check)
	    {
		checkCount++;
		error = poolCheck(&swimming, NULL);
		if (error != NULL)
		{
		    fprintf(stderr, "Failed check after realloc: %s\n", error);
		    return EXIT_FAILURE;
		}
	    }
	    reallocCount++;
	    error = poolRealloc(&swimming, pStore, 1 << bitsReq,
		&store);
	    if (error != NULL)
	    {
		fprintf(stderr, "Failed realloc down: %s\n", error);
		return EXIT_FAILURE;
	    }
	    if (pStore != (void **)store)
	    {
		fprintf(stderr, "Moved store\n");
		return EXIT_FAILURE;
	    }
	    if (check)
	    {
		checkCount++;
		error = poolCheck(&swimming, NULL);
		if (error != NULL)
		{
		    fprintf(stderr, "Failed check realloc down: %s\n", error);
		    return EXIT_FAILURE;
		}
	    }
	    *pStore = myList;
	    myList = pStore;
	    bytesBack += 1 << bitsReq;
	    linked++;
	}
	error = poolAvailable(&swimming, &avail);
	if (error != NULL)
	{
	    fprintf(stderr, "avail error %s\n", error);
	    return EXIT_FAILURE;
	}
	fprintf(stdout, "Got %d of %d bytes back: %d available\n",
	    bytesBack, poolBytes, avail);
	fflush(stdout);
	for(;;)
	{
	    void *store;
	    void **pStore;
	    allocCount++;
	    error = poolMalloc(&swimming, 1 << bitsReq, &store);
	    if (error != NULL)
	    {
		if (!strcmp(error, NO_STORE))
		    break;
		fprintf(stderr, "Error: %s\n", error);
		return EXIT_FAILURE;
	    }
#ifndef FRAGILE
	    memset(store, linked & 0xff, 1 << bitsReq);
#endif
	    /*
	    fprintf(stderr, "Got store at %p\n", store);
	    */
	    if (check)
	    {
		checkCount++;
		error = poolCheck(&swimming, NULL);
		if (error != NULL)
		{
		    fprintf(stderr, "Failed check after alloc: %s\n", error);
		    return EXIT_FAILURE;
		}
	    }
	    pStore = (void **) store;
	    *pStore = myList;
	    myList = pStore;
	    bytesBack += 1 << bitsReq;
	    linked++;
	}
	error = poolAvailable(&swimming, &avail);
	if (error != NULL)
	{
	    fprintf(stderr, "avail error %s\n", error);
	    return EXIT_FAILURE;
	}
	fprintf(stdout, "Got %d of %d bytes back: %d available\n",
	    bytesBack, poolBytes, avail);
	fflush(stdout);
	while (myList != NULL)
	{
	    char shouldBe = (char)(linked - unlinked - 1);
	    void **p = myList;
#ifndef FRAGILE
	    if (memCmpChar(p + 1, (1 << bitsReq) - sizeof(void *),
		shouldBe))
	    {
		fprintf(stderr,
		    "Store corruption at linked %d unlinked %d\n",
		    linked, unlinked);
		return EXIT_FAILURE;
	    }
#endif
	    myList = (void **)*myList;
	    freeCount++;
	    error = poolFree(&swimming, p);
	    if (error != NULL)
	    {
		fprintf(stderr, "Error %s from free\n", error);
		return EXIT_FAILURE;
	    }
	    if (check)
	    {
		checkCount++;
		error = poolCheck(&swimming, NULL);
		if (error != NULL)
		{
		    fprintf(stderr, "Failed pool check: %s\n", error);
		    return EXIT_FAILURE;
		}
	    }
	    unlinked++;
	}
	if (linked != unlinked)
	{
	    fprintf(stderr, "Didn't count them all in again!\n");
	    return EXIT_FAILURE;
	}
	error = poolAvailable(&swimming, &avail);
	if (error != NULL)
	{
	    fprintf(stderr, "avail error %s\n", error);
	    return EXIT_FAILURE;
	}
	if (check)
	{
	    int j;
	    checkCount++;
	    error = poolCheck(&swimming, countsAfter);
	    if (error != NULL)
	    {
		fprintf(stderr, "Failed pool check at end: %s\n", error);
		return EXIT_FAILURE;
	    }
	    for (j = minBits; j <= maxBits; j++)
	    {
		if (countsBefore[j - minBits] != countsAfter[j - minBits])
		{
		    fprintf(stderr, "Block count for %d was %d is %d\n",
			j , countsBefore[j - minBits],
			countsAfter[j - minBits]);
		    return EXIT_FAILURE;
		}
	    }
	}
    }
    fprintf(stdout, "%d allocs %d reallocs %d frees %d expensive checks\n",
	allocCount, reallocCount, freeCount, checkCount);
    fprintf(stdout, "%d bytes available\n", avail);
    fprintf(stdout, "linked %d unlinked %d\n", linked, unlinked);
    fprintf(stdout, "All OK!\n");
    return EXIT_SUCCESS;
}
