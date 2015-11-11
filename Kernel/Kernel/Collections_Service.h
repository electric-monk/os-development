#ifndef __COLLECTIONS_SERVICE_H__
#define __COLLECTIONS_SERVICE_H__

#define COLLECTIONS_SYSCALL         0x82

/* Function: Create string
 * Parameter 0: String pointer
 * Parameter 1: String length
 * Result 0: Error
 * Result 1: Object handle
 */
#define COLLECTIONS_FUNCTION_CREATE_STRING  0x00

/* Function: Create number
 * Parameter 0: Value
 * Result 0: Error
 * Result 1: Object handle
 */
#define COLLECTIONS_FUNCTION_CREATE_NUMBER  0x01

/* Function: Create array
 * Result 0: Error
 * Result 1: Object handle
 */
#define COLLECTIONS_FUNCTION_CREATE_ARRAY   0x02

/* Function: Create dictionary
 * Result 0: Error
 * Result 1: Object handle
 */
#define COLLECTIONS_FUNCTION_CREATE_DICT    0x03

/* Function: Create FIFO
 * Result 0: Error
 * Result 1: Object handle
 */
#define COLLECTIONS_FUNCTION_CREATE_FIFO    0x04

/* Function: String: Length
 * Parameter 0: Object handle
 * Result 0: Error
 * Result 1: Length
 */
#define COLLECTIONS_FUNCTION_STRING_LENGTH  0x05

/* Function: String: Get
 * Parameter 0: Object handle
 * Parameter 1: Output pointer
 * Parameter 2: Output maximum length
 * Result 0: Error
 * Result 1: Output length (up to and including maximum)
 */
#define COLLECTIONS_FUNCTION_STRING_GET     0x06

/* Function: Number: Get
 * Parameter 0: Object handle
 * Result 0: Error
 * Result 1: Value
 */
#define COLLECTIONS_FUNCTION_NUMBER_GET     0x07

/* Function: Number: Reset
 * Parameter 0: Object handle
 * Parameter 1: New value
 * Result 0: Error
 */
#define COLLECTIONS_FUNCTION_NUMBER_SET     0x08

/* Function: FIFO: Push
 * Parameter 0: FIFO object handle
 * Parameter 1: Pushing object handle
 * Result 0: Error
 */
#define COLLECTIONS_FUNCTION_FIFO_PUSH      0x09

/* Function: FIFO: Pop
 * Parameter 0: FIFO object handle
 * Result 0: Error
 * Result 1: Popped object handle
 */
#define COLLECTIONS_FUNCTION_FIFO_POP       0x0A

/* Function: Dictionary: Set
 * Parameter 0: Dictionary object handle
 * Parameter 1: Key object handle
 * Parameter 2: Value object handle
 * Result 0: Error
 */
#define COLLECTIONS_FUNCTION_DICT_SET       0x0B

/* Function: Dictionary: Clear
 * Parameter 0: Dictionary object handle
 * Parameter 1: Key object handle
 * Result 0: Error
 */
#define COLLECTIONS_FUNCTION_DICT_CLEAR     0x0C

/* Function: Dictionary: Get
 * Parameter 0: Dictionary object handle
 * Parameter 1: Key object handle
 * Result 0: Error
 * Result 1: Value object handle
 */
#define COLLECTIONS_FUNCTION_DICT_GET       0x0D

/* Function: Dictionary: Get all keys
 * Parameter 0: Dictionary object handle
 * Result 0: Error
 * Result 1: Array object handle
 */
#define COLLECTIONS_FUNCTION_DICT_GET_KEYS  0x0E

/* Function: Dictionary: Get all values
 * Parameter 0: Dictionary object handle
 * Result 0: Error
 * Result 1: Array object handle
 */
#define COLLECTIONS_FUNCTION_DICT_GET_VALUES 0x0F

/* Function: Array: Add
 * Parameter 0: Array object handle
 * Parameter 1: Object handle to add
 * Result 0: Error
 */
#define COLLECTIONS_FUNCTION_ARRAY_ADD      0x10

/* Function: Array: Remove
 * Parameter 0: Array object handle
 * Parameter 1: Object handle to remove
 * Result 0: Error
 */
#define COLLECTIONS_FUNCTION_ARRAY_REMOVE   0x11

/* Function: Array: Find
 * Parameter 0: Array object handle
 * Parameter 1: Object handle to find
 * Result 0: Error
 * Result 1: Index, or -1
 */
#define COLLECTIONS_FUNCTION_ARRAY_FIND     0x12

/* Function: Array: Count
 * Parameter 0: Array object handle
 * Result 0: Error
 * Result 1: Count
 */
#define COLLECTIONS_FUNCTION_ARRAY_COUNT    0x13

/* Function: Array: Get
 * Parameter 0: Array object handle
 * Parameter 1: Index
 * Result 0: Error
 * Result 1: Object handle
 */
#define COLLECTIONS_FUNCTION_ARRAY_GET      0x14

/* Function: Array: Add array
 * Parameter 0: Array object handle
 * Parameter 1: Other array object handle
 * Result 0: Error
 */
#define COLLECTIONS_FUNCTION_ARRAY_ADDARRAY 0x15

// Error codes
#define COLLECTIONS_ERROR_SUCCESS           0x00
#define COLLECTIONS_ERROR_INVALID_HANDLE    0x01
#define COLLECTIONS_ERROR_BAD_FUNCTION      0x02
#define COLLECTIONS_ERROR_FIFO_EMPTY        0x03
#define COLLECTIONS_ERROR_NOT_FOUND         0x04

#endif // __COLLECTIONS_SERVICE_H__
