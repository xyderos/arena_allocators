## Allocators

This project attempts to implement __some small allocators__ designed with simplicity in mind.

The project utilizes a __naive approach on the internal memory handling__ and expands on the __on-top data structures__.

### Features
* __Error handling__ is implemented via a __custom jump handler__ that is implemented with the __ucontext library__.

    * _Keep in mind that the error handler is __compiler specific__, meaning that it is supported on __all architectures__ and the compiler supports the __GNU C Extensions___
* __Concurrency__ is supported, all the __on-top allocators__ are implemented with __semaphores__ in order to ensure that there are no data races.

### Current allocators implemented
* A __hash trie that utilizes an internal arena__
    * Supports __fast inserting__ and __lazy deletion__
    * __Reduced fragmentation__
    * __Memory locality__
    * __Scalability__
* A __probing map__ thata utilizes __open addressing with linear probing__
    * Inherits the same characteristics as the __trie allocator__
    * There is __a bug__ on the semaphore that locks upon insert and causes a __segfault__, needs to be investigated

Keep in mind that __this is not a production library__ and it __far away from becoming one yet__

### Bugs

* The probing map works the majority of times but quite rarely the __semaphore causes a seg fault__, needs to be investigated
* Possible bug with stack allocation on __pthread_create__, needs investigation

Feel free to contribute!
