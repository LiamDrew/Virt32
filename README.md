================================================================================
Virt32
================================================================================
Created by: Virt32 (Liam Drew, Hameedah Lawal, Milo Goldstein, 
                         Jason Miller, Yoda Ermias>)
Presented at JumboHack 2025 under the Mad Hacker track.

An emulated 32-bit 4GB virtual memory system including
our own malloc(), calloc(), free(), and accessing functions. The UM can run 
midmark with our interface.

Team Member Contributions:
    Liam Drew: Implemented the memory management interface in driver.c and 
               integrated it with the UM
    Milo Goldstein: Implemented the memory management interface in driver.c
    Jason Miller: Implemented the memory management interface in driver.c
    Hameedah Lawal: Created the data structure (an array of stacks) to hold 
                    free segments. Implemented the logic of adding and removing 
                    free segments to the free list.
    Yoda Ermias: Created the data structure (an array of stacks) to hold 
                 free segments. Implemented the logic of adding and removing 
                 free segments to the free list.

ACKNOWLEDGEMENTS
CS40 Project 6 & 7 - UM & PROFILING

REFLECTION
<What motivated your team to choose this project?>
    Project 6 & 7 in CS40, The Universal Machine (UM), has students design and implement their own 32-Bit emulator. The project has students use an array of pointers to memory segments to handle the 64-bit to 32-bit memory conversion problem. However, it makes every single UM memory access slow. Every UM memory access requires 2 system memory accesses, which is particularly slow due to the double-pointer situation. This results in terrible spatial locality and a high likelihood of cache misses. So, we asked ourselves, what would happen if we got rid of the double-pointer problem? This led us down a memory rabbit hole with the idea of creating our own virtual memory to break through the inherent bottleneck of UM.

<Potential future work/improvements?>
    1. Add more bounds/safety checking
    2. Implement a concurrent segment daemon that will be able to zero memory 
       segments for us before we need them, and combine contiguous, freed memory segments.
<What is a challenge you encountered while making this?>
    Setting memory access/allocation limits, offsetting bookkeeping data 
    (capacity and size), and running our UM implementation with our interface. 
<What is a fun or interesting experience you had with another hacker?>
    On Saturday, our team had a great discussion on our project's design and 
    plan. We discussed ideas about data structures, performance improvements, 
    time-space trade-offs, how we want to incorporate the UM, etc. 
    
