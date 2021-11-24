
CFLAGS =-std=c11

all: f2ba checksum

install:
	mv $(TARGET) $(HOME)/.local/bin


checksum: checksum.c
	$(CC) $(CFLAGS) $< -o $@

f2ba: f2ba.c
	$(CC) $(CFLAGS) $< -o $@


clean:
	rm -f f2ba checksum
