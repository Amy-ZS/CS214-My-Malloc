# CS214-My-Malloc

Zixin Shi, NetID: zs391

Test Plan:
1. Basic Functionality Tests
    These tests ensure that the basic functionality of mymalloc() and myfree() works as expected. The following tests will confirm that the program correctly allocates and frees memory.
    
    Test 1: Basic Allocation and Freeing
    Description: Test that a single allocation and free operation works properly.
    Steps:
    Allocate a small memory block using malloc().
    Free the allocated memory.
    Expected Outcome: No errors should occur, and the allocated memory should be correctly freed.
    
    Test 2: Multiple Allocations
    Description: Test multiple consecutive allocations and deallocations.
    Steps:
    Allocate several chunks of memory (e.g., 10 chunks of 32 bytes).
    Free each chunk.
    Expected Outcome: No errors, all memory chunks should be correctly allocated and freed without overlap or corruption.
    
    Test 3: Allocate and Immediately Free
    Description: Test allocating and immediately freeing memory repeatedly.
    Steps:
    Allocate and free a memory block (e.g., 64 bytes) 50 times.
    Expected Outcome: No memory leaks, and the operations should complete without any errors.

2. Error Detection Tests
    These tests ensure that the library can detect and report errors related to memory allocation and freeing. It also checks that invalid calls to free() are correctly handled.
    
    Test 4: Freeing Invalid Pointer
    Description: Test the case where free() is called on a pointer that was not allocated via malloc().
    Steps:
    Declare a local variable (e.g., int x;).
    Call free() on the address of the variable (free(&x)).
    Expected Outcome: The program should report an error message: free: Inappropriate pointer (source.c:line_number) and terminate with exit code 2.

    Test 5: Freeing Pointer Not at Start of Chunk
    Description: Test freeing a pointer that is not at the start of a chunk.
    Steps:
    Allocate a memory block (e.g., 8 bytes).
    Use pointer arithmetic to point to the middle of the allocated block.
    Call free() on the pointer pointing to the middle of the block.
    Expected Outcome: The program should report an error message: free: Inappropriate pointer (source.c:line_number) and terminate with exit code 2.

    Test 6: Double Freeing
    Description: Test freeing the same pointer twice.
    Steps:
    Allocate a memory block (e.g., 8 bytes).
    Free the memory once.
    Attempt to free the same pointer again.
    Expected Outcome: The program should report an error message: free: Inappropriate pointer (source.c:line_number) and terminate with exit code 2.

    Test 7: Invalid Free After Malloc Failure
    Description: Test the behavior when malloc() fails to allocate memory and free() is called afterward.
    Steps:
    Test allocating a large block of memory to force malloc() to fail (e.g., requesting more than available memory).
    Attempt to free a pointer returned by malloc().
    Expected Outcome: The program should not crash, and the error handling logic should prevent further invalid operations.

3. Leak Detection Tests
    These tests ensure that memory leaks are detected correctly.
    
    Test 8: Detecting Leaked Memory
    Description: Test that memory leaks are detected and reported at program exit.
    Steps:
    Allocate some memory blocks using malloc().
    Do not free some or all of the memory blocks.
    Run the program and let it terminate.
    Expected Outcome: The program should print a leak message at the end, e.g., mymalloc: 128 bytes leaked in 14 objects.
   
4. Coalescing Free Chunks Tests
    These tests verify that adjacent free chunks are coalesced correctly.
    
    Test 9: Coalescing Adjacent Free Chunks
    Description: Test that two adjacent free chunks are correctly coalesced into a larger chunk when a new malloc() request cannot be fulfilled by existing chunks.
    Steps:
    Allocate multiple blocks of memory.
    Free them in a particular order, leaving adjacent free chunks.
    Request a chunk of memory larger than any individual free chunk but smaller than the total of two adjacent chunks.
    Expected Outcome: The memory manager should coalesce the two adjacent free chunks into a larger one, and malloc() should return a pointer to this new chunk.

5. Edge Case Tests
  These tests handle unusual or extreme conditions to ensure robustness.
  
  Test 10: Allocation of Zero Bytes
  Description: Test allocating 0 bytes.
  Steps:
  Call malloc(0).
  Expected Outcome: The allocation should succeed, and it should return a non-NULL pointer. The memory should be valid for checking, and no errors should be reported.
  
  Test 11: Large Allocation
  Description: Test allocating a very large block of memory.
  Steps:
  Request an allocation larger than the available memory (e.g., more than MEMLENGTH).
  Verify that the allocation fails gracefully.
  Expected Outcome: The allocation should fail, and the error message malloc: Unable to allocate X bytes (source.c:line_number) should be printed.

6. Stress Testing
    These tests verify that the implementation can handle larger workloads.
    
    Test 12: Memgrind Stress Test
    Description: Test the performance and robustness of the mymalloc() and myfree() implementations under heavy use.
    Steps:
    Implement a workload that performs the following operations repeatedly 50 times:
    Allocate and free a 1-byte object.
    Allocate 120 1-byte objects and free them.
    Randomly choose between allocating and deallocating objects.
    Ensure that after each workload, all allocated memory is freed.
    Measure the average time taken for the workload.
    Expected Outcome: The memory manager should be able to handle the stress test without errors or crashes, and it should print the average execution time.

7. Boundary Condition Tests
    These tests ensure that the allocator correctly handles boundary cases, such as small allocations and edge memory locations.
    
    Test 13: Minimum Chunk Size Allocation
    Description: Test allocating the smallest chunk possible (16 bytes, due to alignment).
    Steps:
    Call malloc(1) to request a 1-byte allocation.
    Expected Outcome: The program should round the allocation up to 16 bytes, and malloc() should return a pointer to a 16-byte chunk.
    
    Test 14: Memory Overflow
    Description: Test the system's handling of requests that exceed the total available memory.
    Steps:
    Request an allocation that exceeds the available memory (greater than MEMLENGTH).
    Expected Outcome: The allocation should fail, and the appropriate error message should be printed.
