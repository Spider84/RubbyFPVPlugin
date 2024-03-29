VER_MAJOR := 1
VERSION := $(VER_MAJOR).0.1
PREFIX := arm-linux-gnueabihf-
GPP := $(PREFIX)g++
GCC := $(PREFIX)gcc
PLATFORM = -march=armv7-a -mfloat-abi=hard -mfpu=neon-vfpv4

INCLUDES += -I../../RubyFPV/code
LIBS += -L/usr/arm-linux-gnueabihf/lib

LDFLAGS= -lrt -lpthread -lc -shared $(LIBS)
CFLAGS += $(PLATFORM) -fno-stack-protector -Wall -O0 -D _GNU_SOURCE -Wno-address-of-packed-member $(INCLUDES)
CPPFLAGS= $(PLATFORM) $(CFLAGS) --std=gnu++17

OUTPUT := build

.PHONY: clean all
all: my_plugin

$(OUTPUT)/%.o: %.cpp mk-dirs
	$(GPP) $(CPPFLAGS) -c -o $@ $<

my_plugin: $(OUTPUT)/my_plugin_source.o mk-dirs
	$(GCC) $(CFLAGS) $(LDFLAGS) $(OUTPUT)/my_plugin_source.o -Wl,-soname,ruby_control.so.$(VER_MAJOR) -o $(OUTPUT)/ruby_control.so.$(VERSION)

mk-dirs:
	mkdir ./$(OUTPUT)

clean:
	rm -fr $(OUTPUT)
