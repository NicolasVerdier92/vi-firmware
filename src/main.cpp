#include "serialutil.h"
#include "usbutil.h"
#include "listener.h"
#include "signals.h"
#include "log.h"

#define VERSION_CONTROL_COMMAND 0x80
#define RESET_CONTROL_COMMAND 0x81

// USB
#define DATA_ENDPOINT 1

extern void reset();
extern void setup();
extern void loop();

const char* VERSION = "2.0-pre";

#ifdef __CHIPKIT__
SerialDevice SERIAL_DEVICE = {&Serial1};
#else
SerialDevice SERIAL_DEVICE;
#endif // __CHIPKIT__

UsbDevice USB_DEVICE = {
#ifdef CHIPKIT
    USBDevice(usbCallback),
#endif // CHIPKIT
    DATA_ENDPOINT,
    MAX_USB_PACKET_SIZE_BYTES};

Listener listener = {&USB_DEVICE, &SERIAL_DEVICE};

int main(void) {
#ifdef CHIPKIT
	init();
#endif
	setup();

	for (;;)
		loop();

	return 0;
}

bool handleControlRequest(uint8_t request) {
    switch(request) {
    case VERSION_CONTROL_COMMAND:
        char combinedVersion[strlen(VERSION) + strlen(getMessageSet()) + 4];
        sprintf(combinedVersion, "%s (%s)", VERSION, getMessageSet());
        debug("Version: %s\r\n", combinedVersion);

        sendControlMessage((uint8_t*)combinedVersion, strlen(combinedVersion));
        return true;
    case RESET_CONTROL_COMMAND:
        debug("Resetting...\r\n");
        reset();
        return true;
    default:
        return false;
    }
}

