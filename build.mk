BUILD_PATH   := .build
ENTRY_POINY  := src/sketch.ino
TERMINAL     := picocom

arduino-path ?= /opt/arduino-1.8.1
serial-port  ?= /dev/ttyACM0
board-model  ?= uno
ldflags      ?= --pref compiler.libraries.ldflags="-Wl,-u,vfprintf -lprintf_flt"

ARDUINO_CMD := $(arduino-path)/arduino -v --port $(serial-port) --board $(board-model) --pref build.path=$(BUILD_PATH) --pref sketchbook.path=. $(ldflags)

.DEFAULT_GOAL := compile

.PHONY: compile upload serial clean

build: compile

compile:
	$(ROOT)/create-version.sh
	$(ARDUINO_CMD) --verify $(ENTRY_POINY) 

upload:
	usbrelay BITFT_2=1 || true
	fuser -k -15 $(serial-port) || true
	sleep 5
	$(ARDUINO_CMD) --upload $(ENTRY_POINY) 
	sleep 1
	usbrelay BITFT_2=0 || true

serial:
	$(TERMINAL) -b 115200 $(serial-port)

clean:
	$(RM) -r $(BUILD_PATH)
