
#include "Descriptors.h"

/** HID class report descriptor. This is a special descriptor constructed with values from the
 *  USBIF HID class specification to describe the reports and capabilities of the HID device. This
 *  descriptor is parsed by the host and its contents used to determine what data (and in what encoding)
 *  the device will send, and what it may be sent back from the host. Refer to the HID specification for
 *  more details on HID report descriptors.
 *
 *  This descriptor describes the mouse HID interface's report structure.
 */
const USB_Descriptor_HIDReport_Datatype_t PROGMEM myMouseReport[] =
{
	/* Use the HID class driver's standard Mouse report.
	 *   Min X/Y Axis values: -1
	 *   Max X/Y Axis values:  1
	 *   Min physical X/Y Axis values (used to determine resolution): -1
	 *   Max physical X/Y Axis values (used to determine resolution):  1
	 *   Buttons: 3
	 *   Absolute screen coordinates: false
	 */
	HID_DESCRIPTOR_MOUSE(-128, 127, -128, 127, 3, false)
};


/* 
  CALLBACK_USB_GetDescriptor ä÷êîÇ™éQè∆Ç∑ÇÈïœêîñºÇïœçXÅB
*/
