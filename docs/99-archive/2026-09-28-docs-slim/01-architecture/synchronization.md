# Synchronization Architecture

Synchronization is a first-class runtime concern. CUDA events, Vulkan semaphores/fences/barriers, CPU waits, queue ownership transfers, and timeline values must be visible to metrics and benchmarks.

## Rule

No path may claim `0 synchronization`; the correct target is minimal explicit synchronization.
