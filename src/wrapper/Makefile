# Paths
NAME    = wrapper

CFLAGS  = -I. -Wno-format-truncation -static

.PHONY: all clean install

all: $(NAME)

$(NAME): $(NAME).c whereami.c
	$(CC) $(CFLAGS) -o $@ $^

install: $(NAME)
	install -Dm755 $^ $(DESTDIR)/usr/local/bin/$(NAME)

clean:
	rm -f $(NAME) *.o
