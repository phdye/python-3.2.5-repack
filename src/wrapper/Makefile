# Paths
NAME = wrapper

SHARED = libwrapper.so

CFLAGS=-Wno-format-truncation

ALL = $(NAME) $(SHARED)

.PHONY: all clean build wrapper

all: $(ALL)

# Build the C wrapper for rustc-cyg
$(NAME):  $(NAME).c
	$(CC) ${CFLAGS} -g -o $@ $^

install: $(NAME)
	cp $^ ~/bin/.
	cp scripts/apply-win32-wrapper ~/scripts/.

# -nostartfiles
# -nodefaultlibs
# -nostdlib
CC_OPT= -fPIC --shared -nostartfiles
# -nodefaultlibs

# -Wl,-shared
# -Wl,--allow-shlib-undefined
LD_OPT= -Wl,-emain -Wl,--unresolved-symbols=ignore-all
SH_OPT= -Wl,-shared -Wl,-rpath,/usr/bin -lcygwin

$(SHARED): $(NAME).o
	( gcc ${CC_OPT} $^ -o $@ ${LD_OPT} ${SH_OPT} 2>&1 ) | head -4
	nm libwrapper.so | grep ' T ' | wc -l

clean:
	rm -rf $(ALL) *.exe *.so *.a *.o *.s *.t
