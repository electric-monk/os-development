#ifndef __KERNELOBJECT_SERVICE_H__
#define __KERNELOBJECT_SERVICE_H__

#define KERNELOBJECT_SYSCALL        0x80

/* Function: AddRef
 * Parameter 0: Function select
 * Parameter 1: Object handle
 * Result 0: Error
 */
#define KERNELOBJECT_FUNCTION_ADDREF    0x00

/* Function: Release
 * Parameter 0: Function select
 * Parameter 1: Object handle
 * Result 0: Error
 */
#define KERNELOBJECT_FUNCTION_RELEASE   0x01

/* Function: Hash
 * Parameter 0: Function select
 * Parameter 1: Object handle
 * Result 0: Error
 * Result 1: Hash
 */
#define KERNELOBJECT_FUNCTION_HASH      0x02

/* Function: IsEqual
 * Parameter 0: Function select
 * Parameter 1: Object handle
 * Parameter 2: Object handle for comparison
 * Result 0: Error
 * Result 1: Non-zero if equal
 */
#define KERNELOBJECT_FUNCTION_EQUAL     0x03

/* Function: GetClassName
 * Parameter 0: Function select
 * Parameter 1: Superclass index
 * Parameter 2: Pointer
 * Parameter 3: Maximum length
 * Result 0: Error
 * Result 1: Length
 */
#define KERNELOBJECT_FUNCTION_GETNAME   0x04

// Error codes
#define KERNELOBJECT_ERROR_SUCCESS          0x00
#define KERNELOBJECT_ERROR_INVALID_HANDLE   0x01
#define KERNELOBJECT_ERROR_INVALID_HANDLE_2 0x02
#define KERNELOBJECT_ERROR_BAD_FUNCTION     0x03
#define KERNELOBJECT_ERROR_MEMORY_VIOLATION 0x04

#endif // __KERNELOBJECT_SERVICE_H__
