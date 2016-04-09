#ifndef __THREAD_SERVICE_H__
#define __THREAD_SERVICE_H__

#define THREAD_SYSCALL              0x83

/* Function: Create thread
 * Parameter 0: Entrypoint
 * Result 0: Error
 * Result 1: Object handle
 */
#define THREAD_FUNCTION_CREATE      0x00

/* Function: Kill thread
 * Parameter 0: Object handle
 * Result 0: Error
 */
#define THREAD_FUNCTION_KILL        0x01

/* Function: Allocate heap memory
 * Parameter 0: Maximum size
 * Result 0: Error
 * Result 1: Handle - release to free
 * Result 2: Address
 */
#define THREAD_FUNCTION_ALLOC       0x02

/* Function: Terminate current thread
 */
#define THREAD_FUNCTION_TERMINATE   0x03

// Errors
#define THREAD_ERROR_SUCCESS        0x00
#define THREAD_ERROR_BAD_FUNCTION   0x01
#define THREAD_ERROR_PROTECTED_ADDRESS 0x02
#define THREAD_INVALID_HANDLE       0x03

#endif // __THREAD_SERVICE_H__
