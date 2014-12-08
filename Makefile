UNAME := $(shell uname)

LDFLAGS += -Ldeps/.libs

DEPS = deps/.libs/libhl.a


ifeq ($(UNAME), Linux)
LDFLAGS += -pthread
else
LDFLAGS +=
endif

ifeq ($(UNAME), Darwin)
SHAREDFLAGS = -dynamiclib
SHAREDEXT = dylib
else
SHAREDFLAGS = -shared
SHAREDEXT = so
endif

ifeq ("$(LIBDIR)", "")
LIBDIR=/usr/local/lib
endif

ifeq ("$(INCDIR)", "")
INCDIR=/usr/local/include
endif

IS_CLANG := $(shell $(CC) --version | grep clang)

ifneq ("$(IS_CLANG)", "")
CLANG_FLAGS=-Wno-undefined-inline -Wno-unknown-warning-option
endif

#CC = gcc
TARGETS = $(patsubst %.c, %.o, $(wildcard src/*.c))
TESTS = $(patsubst %.c, %, $(wildcard test/*_test.c))

TEST_EXEC_ORDER = expression_test

all: CFLAGS += -Ideps/.incs
all: $(DEPS) objects static shared

.PHONY: build_deps
build_deps:
	@make -eC deps all

.PHONY: static
static: $(DEPS) objects
	ar -r libexpr.a src/*.o

$(DEPS): build_deps

.PHONY: shared
shared: $(DEPS) objects
	$(CC) src/*.o $(LDFLAGS) $(DEPS) $(SHAREDFLAGS) -o libexpr.$(SHAREDEXT)

.PHONY: dynamic
dynamic: LDFLAGS += -lhl
dynamic: $(DEPS) objects
	$(CC) src/*.o $(LDFLAGS) $(SHAREDFLAGS) -o libexpr.$(SHAREDEXT)

.PHONY: objects
objects: CFLAGS += -fPIC -Isrc -Wall -Werror -Wno-parentheses -Wno-pointer-sign -Wno-unused-function $(CLANG_FLAGS) -DTHREAD_SAFE -g -O3
objects: $(TARGETS)

clean:
	rm -f src/*.o
	rm -f test/*_test
	rm -f libexpr.a
	rm -f libexpr.$(SHAREDEXT)
	@if [ -f support/libut/Makefile ]; then make -C support/libut clean; fi

.PHONY:tests
tests: CFLAGS += -Isrc -Ideps/.incs -Wall -Werror -Wno-parentheses -Wno-pointer-sign $(CLANG_FLAGS) -DTHREAD_SAFE -g -O3
tests: libut static
	@for i in $(TESTS); do\
	  echo "$(CC) $(CFLAGS) $$i.c -o $$i libexpr.a $(LDFLAGS) -lm";\
	  $(CC) $(CFLAGS) $$i.c -o $$i libexpr.a deps/.libs/libhl.a deps/.libs/libut.a $(LDFLAGS) -lm;\
	done;\
	for i in $(TEST_EXEC_ORDER); do echo; test/$$i; echo; done

.PHONY: test
test: tests

install:
	 @echo "Installing libraries in $(LIBDIR)"; \
	 cp -v libexpr.a $(LIBDIR)/;\
	 cp -v libexpr.$(SHAREDEXT) $(LIBDIR)/;\
	 echo "Installing headers in $(INCDIR)"; \
	 cp -v src/*.h $(INCDIR)/;

.PHONY: docs
docs:
	@doxygen libexpr.doxycfg
