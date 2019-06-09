#ifndef __SYSTEM_SERVICE_H__
#define __SYSTEM_SERVICE_H__

#define SYSTEM_SYSCALL              0x85

/* Function: Create object graph monitor
 * Result 0: Error
 * Result 1: Monitor handle
 */
#define SYSTEM_MONITOR_CREATE       0x00

/* Function: Capture current object graph
 * Parameter 0: Monitor handle
 * Result 0: Error
 * Result 1: Array handle, containing all currently active providers in the system
 *    Each entry: dictionary
 *      "Provider": provider handle
 *      "Inputs": array
 *        "Name": string
 *        "Connection": IPC GUID
 *        <etc>
 *      "Outputs": array
 *        "Name": string
 *        "Connections": array
 *          IPC GUID
 *        <etc>
 */
#define SYSTEM_MONITOR_CAPTURE      0x01

/* Function: Get object for capture identifier
 * Parameter 0: Monitor handle
 * Parameter 1: Capture identifier
 * Result 0: Error
 * Result 1: Object handle
 */
#define SYSTEM_MONITOR_GETOBJECT    0x02

/* Function: Get update
 * Parameter 0: Monitor handle
 * Result 0: Error
 * Result 1: Array handle
 *      Each entry: dictionary
 *          "Update": integer <0 = start, 1 = start input, 2 = stop input, 3 = start output, 4 = stop output, 5 = connection start, 6 = connection stop>
 *          "Provider": handle (for type 0, 1, 2, 3, 4)
 *          "Service": handle (for type 3, 4)
 */

/* Function: Connect two objects
 * Parameter 0: Monitor handle
 * Parameter 1: Output provider handle
 * Parameter 2: Output name
 * Parameter 3: Input provider handle
 * Parameter 4: Input name
 * Result 0: Error
 */

/* Function: Disconnect two objects
 * Parameter 0: Monitor handle
 * Parameter 1: Input provider handle
 * Parameter 2: Input name
 * Result 0: Error
 */

/* Function: Get input info
 * Parameter 0: Monitor handle
 * Parameter 1: Input provider handle
 * Parameter 2: Input index
 * Result 0: Error
 * Result 1: Input dictionary
 */

/* Function: Get root device
 * Result 0: Error
 * Result 1: Handle
 */
#define SYSTEM_DRIVER_GET_ROOT      0xA0

/* Function: Get device children
 * Parameter 0: Device
 * Result 0: Error
 * Result 1: Array
 */
#define SYSTEM_DRIVER_CHILDREN      0xA1

// Errors
#define SYSTEM_ERROR_SUCCESS        0x00
#define SYSTEM_ERROR_BAD_FUNCTION   0x01
#define SYSTEM_INVALID_HANDLE       0x02

#endif // __SYSTEM_SERVICE_H__

/*

 provider:
 - create
 - destroy
 - add service
 - change service
 - remove service
 connection:
 - connected (provider.input -> provider.output)
 - disconnected (provider.input (output implied))
 
 */
