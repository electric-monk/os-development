#ifndef __BLOCKING_SERVICE_H__
#define __BLOCKING_SERVICE_H__

#define BLOCKING_SYSCALL            0x81

/* Function: Create OR
 * Result 0: Error
 * Result 1: Or object handle (owned by caller)
 */
#define BLOCKING_FUNCTION_CREATE_OR     0x00

/* Function: Create AND
 * Result 0: Error
 * Result 1: And object handle (owned by caller)
 */
#define BLOCKING_FUNCTION_CREATE_AND    0x01

/* Function: Create timer
 * Result 0: Error
 * Result 1: Timer object handle (owned by caller)
 */
#define BLOCKING_FUNCTION_CREATE_TIMER  0x02

/* Function: Create simple signal
 * Parameter 0: Active low if true
 * Result 0: Error
 * Result 1: Simple signal object handle (owned by caller)
 */
#define BLOCKING_FUNCTION_CREATE_SIMPLE 0x03

/* Function: Block on object
 * Parameter 0: Object handle on which to block
 * Result 0: Error
 * Result 1: Handle to array of blocking results (owned by caller)
 */
#define BLOCKING_FUNCTION_BLOCK         0x04

/* Function: List-based signal: Add blocking object
 * Parameter 0: Signal list object
 * Parameter 1: Blocking object
 * Result 0: Error
 */
#define BLOCKING_FUNCTION_ADD           0x05

/* Function: List-based signal: Remove blocking object
 * Parameter 0: Signal list object
 * Parameter 1: Blocking object
 * Result 0: Error
 */
#define BLOCKING_FUNCTION_REMOVE        0x06

/* Function: Get blocking object state
 * Parameter 0: Blocking object
 * Result 0: Error
 * Result 1: Non-zero if not blocked
 */
#define BLOCKING_FUNCTION_IS_BLOCKED    0x07

/* Function: Get blocking object list
 * Parameter 0: Blocking object
 * Result 0: Error
 * Result 1: Handle to array of blocking results (owned by caller)
 */
#define BLOCKING_FUNCTION_LIST          0x08

/* Function: Timer: Reset
 * Parameter 0: Timer object handle
 * Parameter 1: Milliseconds to wait
 * Parameter 2: Non-zero to repeat
 * Result 0: Error
 */
#define BLOCKING_FUNCTION_TIMER_RESET   0x09

/* Function: Timer: Cancel
 * Parameter 0: Timer object handle
 * Result 0: Error
 */
#define BLOCKING_FUNCTION_TIMER_CANCEL  0x0A

/* Function: Simple: Set
 * Parameter 0: Simple signal object handle
 * Parameter 1: Non-zero for set, zero for reset
 * Result 0: Error
 */
#define BLOCKING_FUNCTION_SIMPLE_SET    0x0B

// Error codes
#define BLOCKING_ERROR_SUCCESS          0x00
#define BLOCKING_ERROR_BAD_FUNCTION     0x01
#define BLOCKING_ERROR_INVALID_HANDLE   0x02

#endif // __BLOCKING_SERVICE_H__
