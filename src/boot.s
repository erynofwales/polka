# Declare some constants for the multiboot header.
.set ALIGN,     1<<0                # Align loaded modules on page boundaries
.set MEMINFO,   1<<1                # Provide memory map
.set FLAGS,     ALIGN | MEMINFO     # This is the multiboot 'flag' field
.set MAGIC,     0x1BADB002          # Magic number lets bootloader find the header
.set CHECKSUM,  -(MAGIC + FLAGS)    # Checksum of above, to prove we are multiboot

# Declare a header as in the Multiboot Standard. We put this into a special
# section so we can force the header to be in the start of the final program.
# You don't need to understand all these details as it is just magic values that
# is documented in the multiboot standard. The bootloader will search for this
# magic sequence and recognize us as a multiboot kernel.
.section .multiboot
.align 4
.long MAGIC
.long FLAGS
.long CHECKSUM

# Currently the stack pointer register (esp) points at anything and using it may
# cause massive harm. Instead, we'll provide our own stack. We will allocate
# room for a small temporary stack by creating a symbol at the bottom of it,
# then allocating 16384 bytes for it, and finally creating a symbol at the top.
.section .bootstrap_stack, "aw", @nobits
stack_buttom:
.skip 16384     # 16 KiB
stack_top:

# The linker script specifies _start as the entry point to the kernel and the
# bootloader will jump to this position once the kernel has been loaded. It
# doesn't make sense to return from this function as the bootloader is gone.
.section .text
.global _start
.type _start, @function
_start:
    # Set up some space for a call stack. The stack grows downwards, so esp gets set to the top of the stack.
    movl $stack_top, %esp
    movl %esp, %ebp

    # Very early initialization done here.
    call kearly

    # Global initialization done here.
    call _init

    # Here we go... Give kmain the address of the multiboot info structure.
    pushl %ebx
    call kmain

    # In case the function returns, we'll want to put the computer into an
    # infinite loop. To do that, we use the clear interrupt ('cli') instruction
    # to disable interrupts, the halt instruction ('hlt') to stop the CPU until
    # the next interrupt arrives, and jumping to the halt instruction if it ever
    # continues execution, just to be safe. We will create a local label rather
    # than real symbol and jump to there endlessly.
    cli
.Lhang:
    hlt
    jmp .Lhang

# Set the size of the _start symbol to the current location '.' minus its start.
# This is useful when debugging or when you implement call tracing.
.size _start, . - _start
