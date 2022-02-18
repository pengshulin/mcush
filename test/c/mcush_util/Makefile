all:
	$(CC) -Wall -I. -I.. -O2 -g $(CFLAGS) $(EXT_CFLAGS) \
		../mcush_api.c mcush_util.c mcush_base64.c -pthread $(LDFLAGS) -o mcush_util
	$(CC) -Wall -fPIC -I. -I.. -O2 -g $(CFLAGS) $(EXT_CFLAGS) \
		../mcush_api.c mcush_util_export.c $(LDFLAGS) -shared -o mcush_util.so
clean:
	rm -f *.o mcush_util mcush_util.so
