/* PIC.hh
 * vim: set tw=80:
 * Eryn Wells <eryn@erynwells.me>
 */
/**
 * The x86 Programmable Interrupt Controller. This is the simple one, not the
 * newer APIC.
 */

#ifndef __PIC_HH__
#define __PIC_HH__

#include <stdint.h>

namespace x86 {

/**
 * On x86 systems, there are a pair of 8259 PIC chips which together control
 * hardware interrupts. One is the master; the other is the slave. Each one has
 * 8 interrupts. The slave is connected to the master on IRQ 2.
 *
 * These chips must be programmed before use. When doing so, you provide a
 * vector offset for each chip that is the start of its range in the IDT.
 *
 * In modern CPUs, the pair of PIC chips has been replaced with the APIC and
 * IO-APIC. However, interrupts via the 8259s are still supported.
 */
struct PIC
{
    static PIC& systemPIC();

    /**
     * Initialize the PIC chips by mapping their ranges out of the default
     * (which overlap with the system exceptions). The offsets must be divisable
     * by 8. Once the chips are initialized disable all interrupts.
     *
     * @param [in] pic1Offset   Offset of PIC1.
     * @param [in] pic2Offset   Offset of PIC2.
     * @param [in] pic2IRQ      IRQ on PIC1 where PIC2 will be mapped to.
     */
    void initialize(uint8_t pic1Offset, uint8_t pic2Offset, uint8_t pic2IRQ = 2);

    /**
     * Remap the PIC chips into the interrupt vector space. The offsets must be
     * divisible by 8. The interrupt masks are saved before remapping and
     * restored afterwards.
     *
     * @param [in] pic1Offset   Offset of PIC1.
     * @param [in] pic2Offset   Offset of PIC2.
     * @param [in] pic2IRQ      IRQ on PIC1 where PIC2 will be mapped to.
     */
    void remap(uint8_t pic1Offset, uint8_t pic2Offset, uint8_t pic2IRQ = 2);

    /**
     * Notifies the PICs with an End Of Interrupt (EOI) command that the
     * interrupt has been handled. This _must_ be done after the kernel has
     * handled a hardware interrupt.
     */
    void endOfInterrupt(uint8_t irq);

    /**
     * Enable or disable the given IRQ. This is done by setting a mask bit in
     * the PIC's IMR register. If the bit is set, the IRQ is ignored.
     */
    void enableInterrupt(uint8_t irq, bool enabled);

private:
    uint16_t portForIRQ(uint8_t irq);
    void enableIRQ(uint8_t irq);
    void disableIRQ(uint8_t irq);

    void initializePICs(uint8_t pic1Offset, uint8_t pic2Offset, uint8_t pic2IRQ);
};

} /* namespace x86 */

#endif /* __PIC_HH__ */
