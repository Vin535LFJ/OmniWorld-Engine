# Memory Architecture

Memory design must make residency, ownership, sharing, copy count, and synchronization explicit.

## Required metadata

Every frame, tensor, image, and buffer should eventually declare: backend, device, allocation owner, format, dimensions, pitch/stride, lifetime, sharing handle, synchronization state, and timestamp.
