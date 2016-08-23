#ifndef __IPC_SERVICE_H__
#define __IPC_SERVICE_H__

#define IPC_SYSCALL             0x84

/* Function: Memory mapping: Create
 * Parameter 0: Maximum size
 * Result 0: Error
 * Result 1: Object handle
 */
#define IPC_MEMORY_CREATE           0x00

/* Function: Memory mapping: Map
 * Parameter 0: Memory map object
 * Result 0: Error
 * Result 1: Mapping object (releasing it removes the mapping)
 * Result 2: Pointer
 */
#define IPC_MEMORY_MAP              0x01

/* Function: IPC: Send
 * Parameter 0: IPC connection object
 * Parameter 1: Memory map object
 * Result 0: Error
 */
#define IPC_CONNECTION_SEND         0x02

/* Function: IPC: Receive
 * Parameter 0: IPC connection object
 * Result 0: Error
 * Result 1: Memory map object
 */
#define IPC_CONNECTION_RECEIVE      0x03

/* Function: IPC: Get remote info
 * Parameter 0: IPC connection object
 * Result 0: Error
 * Result 1: Remote process object
 */

/* Function: Provider: Create
 * Parameter 0: Array handle (describing inputs)
 * Result 0: Error
 * Result 1: Provider handle
 */
#define IPC_PROVIDER_CREATE         0x10

/* Function: Provider: Start service
 * Parameter 0: Provider handle
 * Parameter 1: Name (string handle)
 * Parameter 2: Type (string handle)
 * Result 0: Error
 * Result 1: Service handle
 */
#define IPC_PROVIDER_OUTPUT_START   0x30

/* Function: Provider: Terminate service
 * Parameter 0: Provider handle
 * Parameter 1: Service handle
 * Result 0: Error
 */
#define IPC_SERVICE_TERMINATE       0x31

/* Function: Provider: Start client
 * Parameter 0: Provider handle
 * Parameter 1: Name
 * Result 0: Error
 * Result 1: Client handle
 */
#define IPC_PROVIDER_INPUT_START    0x32

/* Function: Provider: Terminate client
 * Parameter 0: Provider handle
 * Parameter 1: Client handle
 * Result 0: Error
 */
#define IPC_INPUT_TERMINATE         0x33

/* Function: Provider: Get event (error code indicates event type)
 * Parameter 0: Provider handle
 * Result 0: Error (success indicates no event, but no error)
 * Result 1: Connection handle (on start/message/end)
 * Result 2: Message handle (on message)
 * Result 3: On start, service/client that owns this 
 */
#define IPC_PROVIDER_GET_EVENT      0x12

/* Function: Connect
 * Parameter 0: Client handle
 * Parameter 1: Service handle
 * Result 0: Error
 */
#define IPC_PROVIDER_CONNECT        0x40

/* Function: Disconnect
 * Parameter 0: Client handle
 * Result 0: Error
 */
#define IPC_PROVIDER_DISCONNECT     0x41

#define IPC_ERROR_SUCCESS                   0x00
#define IPC_ERROR_SECURITY                  0x01
#define IPC_ERROR_UNSUPPORTED               0x02
#define IPC_ERROR_EVENT_INPUT_CONNECT       0x03
#define IPC_ERROR_EVENT_INPUT_MESSAGE       0x04
#define IPC_ERROR_EVENT_INPUT_DISCONNECT    0x05
#define IPC_ERROR_EVENT_OUTPUT_CONNECT      0x06
#define IPC_ERROR_EVENT_OUTPUT_MESSAGE      0x07
#define IPC_ERROR_EVENT_OUTPUT_DISCONNECT   0x08
#define IPC_ERROR_UNKNOWN_FUNCTION          0x09
#define IPC_ERROR_INVALID_HANDLE            0x0A
#define IPC_ERROR_NO_BUFFER                 0x0B

#endif // __IPC_SERVICE_H__
