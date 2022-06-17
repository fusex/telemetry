BUILD_PATH   := .build
ENTRY_POINY  := src/sketch.ino
TERMINAL     := picocom

arduino-path ?= /opt/arduino-1.8.1
serial-port  ?= /dev/ttyACM0
board-model  ?= uno

ARDUINO_CMD := $(arduino-path)/arduino -v --port $(serial-port) --board arduino:avr:$(board-model) --pref build.path=$(BUILD_PATH) --pref sketchbook.path=.

.DEFAULT_GOAL := compile

.PHONY: compile upload serial clean

build: compile

compile:
	$(ROOT)/create-version.sh
	$(ARDUINO_CMD) --verify $(ENTRY_POINY) 

upload:
	usbrelay BITFT_2=1
	fuser -k -15 $(serial-port) || true
	sleep 5
	$(ARDUINO_CMD) --upload $(ENTRY_POINY) 
	sleep 1
	usbrelay BITFT_2=0

serial:
	$(TERMINAL) -b 115200 $(serial-port)

clean:
	$(RM) -r $(BUILD_PATH)
