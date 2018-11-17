# gos
A small i386, multi-tasking os kernel + simple userspace.

This OS was built as a learning exercise for kernel-level/systems development. It has only ever been run on the [BOCHS](http://bochs.sourceforge.net/) emulator. 

The kernel is single-threaded, but supports preemptive multi-tasking for user processes. It also supports memory protection via page tables (virtual memory) but does not support paging memory to disk. 

The filesystem is extremely simple and should probably not be used as a reference for anyone wanting to create their own file system. It's basically just implemented as a linked-list of files.

The kernel supports execution of ELF binaries.
