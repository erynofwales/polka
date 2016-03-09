/* Interrupts.hh
 * vim: set tw=80:
 * Eryn Wells <eryn@erynwells.me>
 */
/**
 * Interrupts, exceptions. Spooky scary...
 */

namespace kernel {

} /* namespace kernel */

namespace x86 {

enum class Interrupt {
    DE = 0,     // Divide error exception
    DB = 1,     // Debug exception
    NMI = 2,    // Non-maskable interrupt
    BP = 3,     // Breakpoint exception
    OF = 4,     // Overflow exception
    BR = 5,     // BOUND range exceeded exception
    UD = 6,     // Invalid opcode exception
    NM = 7,     // Device not available exception
    DF = 8,     // Double fault exception
    // Interrupt 9 not used on IA-32; reserved
    TS = 10,    // Invalid TSS exception
    NP = 11,    // Segment not present
    SS = 12,    // Stack fault exception
    GP = 13,    // General protection exception
    PF = 14,    // Page-fault exception
    MF = 16,    // x87 FPU floating-point error
    AC = 17,    // Alignment check exception
    MC = 18,    // Machine-check exception
    XM = 19,    // SIMD floating-point exception
    VE = 20,    // Virtualization exception
    // Interrupts 21 through 31 reserved
}

} /* namespace x86 */
