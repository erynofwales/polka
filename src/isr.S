# isr.s
# Eryn Wells <eryn@erynwells.me>

# Interrupt service routine. See also: Interrupts.cc.

.section .text
.global unhandledInterrupt
.global handleDEException, handleNMIInterrupt, handleDFException, handleGPException
.global handleHardwareInterrupt0, handleHardwareInterrupt1

#define SaveContext \
    pushal; \
    cld;

#define RestoreContext \
    popal; \
    iret

#define RestoreContextAndHalt \
    popal; \
    cli; \
    hlt

// Generic handler
unhandledInterrupt:
    SaveContext
    call doUnhandledInterrupt
    RestoreContextAndHalt

// Divide error
handleDEException:
    SaveContext
    pushl $0
    call dispatchExceptionHandler
    add $4, %esp
    RestoreContextAndHalt

// NMI interrupt
handleNMIInterrupt:
    SaveContext
    pushl $2
    call dispatchExceptionHandler
    add $4, %esp
    RestoreContextAndHalt

// Double fault
handleDFException:
    SaveContext
    pushl $0x08
    call dispatchExceptionHandler
    add $4, %esp
    RestoreContextAndHalt

// General Protection fault
handleGPException:
    SaveContext
    pushl $0x0D
    call dispatchExceptionHandler
    add $4, %esp
    RestoreContextAndHalt
        
/*
 * Hardware Interrupts
 */

#define SaveContextForHardwareInterrupt(irq) \
    SaveContext \
    pushl $irq

#define RestoreContextForHardwareInterrupt(irq) \
    addl $4, %esp; \
    RestoreContext

handleHardwareInterrupt0:
    SaveContextForHardwareInterrupt(0)
    call dispatchHardwareInterrupt
    RestoreContextForHardwareInterrupt(0)

handleHardwareInterrupt1:
    SaveContextForHardwareInterrupt(1)
    call dispatchHardwareInterrupt
    RestoreContextForHardwareInterrupt(1)
