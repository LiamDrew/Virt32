
# Virt32

A 32-bit userspace virtual memory system designed to accelerate 32-bit to 64-bit
address translations for the Universal Machine, a 32-bit virtual machine 
with a RISC-style instruction set. 

Created by Liam Drew, Hameedah Lawal, Milo Goldstein, Jason Miller, and Yoda Ermias.  
Presented at JumboHack 2025 under the Mad Hacker track.

## Overview
This thing is a memory allocator that also supports address translation
our own malloc(), calloc(), free(), and accessing functions. The UM can run 

Currently, this project is little more than a memory allocator. 

### What is the Universal Machine?
TODO: I think Milo should take a stab here

### Why does our Memory System make the Universal Machine faster?
REFLECTION
<What motivated your team to choose this project?>
    Project 6 & 7 in CS40, The Universal Machine (UM), has students design and implement their own 32-Bit emulator. The project has students use an array of pointers to memory segments to handle the 64-bit to 32-bit memory conversion problem. However, it makes every single UM memory access slow. Every UM memory access requires 2 system memory accesses, which is particularly slow due to the double-pointer situation. This results in terrible spatial locality and a high likelihood of cache misses. So, we asked ourselves, what would happen if we got rid of the double-pointer problem? This led us down a memory rabbit hole with the idea of creating our own virtual memory to break through the inherent bottleneck of UM.

## To Use
This project may be applicable to a variety of VM or embedded use cases. The
project is (will be) MIT-Licensed and free for use. The Universal Machine 
specification, however, does not belong to us and is not open source.

For a stable and consistent environment, it is recommended to run this project in a Docker
container. However, it should also build out of the box on MacOS with the clang compiler.

## Design
TODO: Draw some nice pictures explaining how the memory system and the address translations work

## Performance
TODO: Compare performance

## Potential Improvements
Add more bounds/safety checking
Implement a concurrent segment daemon that will be able to zero memory 
       segments for us before we need them, and combine contiguous, freed memory segments.


## Acknowledgements
CS40 Project 6 & 7 - UM & PROFILING  
The JumboHack Team
    
