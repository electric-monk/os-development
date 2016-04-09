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
 * Result 1: Object handle
 */
/* Function: Provider: Start service
 * Parameter 0: Provider handle
 * Parameter 1: Name (string handle)
 * Parameter 2: Type (string handle)
 * Result 0: Error
 * Result 1: Object handle
 */
/* Function: Provider: Terminate service
 * Parameter 0: Provider handle
 * Parameter 1: Object handle
 * Result 0: Error
 */
/* Function: Provider: Get event (error code indicates event type)
 * Parameter 0: Provider handle
 * Result 0: Error (success indicates no event, but no error)
 * Result 1: Connection handle (on start/message/end)
 * Result 2: Message handle (on message)
 */
/* Function: Provider connection: Get IPC
 * Parameter 0: Connection
 * Result 0: Error
 * Result 1: IPC connection object
 */
/* Function: Provider connection: Get name
 * Parameter 0: Connection
 * Result 0: Error
 * Result 1: Service name (string object)
 */
/* Function: Provider connection: Get type
 * Parameter 0: Connection
 * Result 0: Error
 * Result 1: Service type (string object)
 */

#define IPC_ERROR_SUCCESS                   0x00
#define IPC_ERROR_SECURITY                  0x01
#define IPC_ERROR_UNSUPPORTED               0x02
#define IPC_ERROR_EVENT_INPUT_CONNECT       0x03
#define IPC_ERROR_EVENT_INPUT_MESSAGE       0x04
#define IPC_ERROR_EVENT_INPUT_DISCONNECT    0x05
#define IPC_ERROR_EVENT_OUTPUT_CONNECT      0x06
#define IPC_ERROR_EVENT_OUTPUT_MESSAGE      0x07
#define IPC_ERROR_EVENT_OUTPUT_DISCONNECT   0x08

#endif // __IPC_SERVICE_H__