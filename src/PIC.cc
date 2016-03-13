/* PIC.cc
 * vim: set tw=80:
 * Eryn Wells <eryn@erynwells.me>
 */
/**
 * Implementation of the kernel::x86::PIC class.
 */

#include "PIC.hh"
#include "IO.hh"

enum ICW1 {
    ICW4Required    = (1 << 0), // Bit 0: 1 = ICW4 required, 0 = ICW4 not required
    Single8259      = (1 << 1), // Bit 1: 1 = Single 8259, 0 = Cascading 8259s
    HalfSizeVectors = (1 << 2), // Bit 2: 1 = 4-byte vector size, 0 = 8-byte vector size
    LevelTriggered  = (1 << 3), // Bit 3: 1 = level triggered mode, 0 = edge triggered mode
    Initialize      = (1 << 4), // Bit 4: 1 = initialization mode (required for ICW1)
    // Remaining bits must be 0.
};

enum ICW4 {
    X86Mode         = (1 << 0), // Bit 0: 1 = 80x86 mode, 0 = MCS 80/85 mode
    AutoEOI         = (1 << 1), // Bit 1: 1 = Auto EOI, 0 = normal EOI
    PIC2Buffered    = (1 << 3), // Bit 3: 1 = PIC1 buffered mode
    PIC1Buffered    = (3 << 2), // Bits 2, 3: 1 = PIC2 buffered mode
};

enum OCW2 {
    NonSpecificEOI = (1 << 5),
    NOPEOI = (2 << 5),
    SpecificEOI = (3 << 5),
    RotateInAutomaticEOIMode = (4 << 5),
    RotateOnNonSpecificEOICommand = (5 << 5),
    SetPriorityCommand = (6 << 5),
    RotateOnSpecificEOICommand = (7 << 5),
};

namespace {
    const struct {
        uint16_t command;
        uint16_t data;
    } PIC1 { 0x0020, 0x0021 }, PIC2 { 0x00A0, 0x00A1 };
}

namespace x86 {

/*
 * Static
 */

PIC&
PIC::systemPIC()
{
    static PIC sPIC;
    return sPIC;
}

/*
 * Public
 */

void
PIC::initialize(uint8_t pic1Offset,
                uint8_t pic2Offset,
                uint8_t pic2IRQ)
{
    initializePICs(pic1Offset, pic2Offset, pic2IRQ);

    // Turn off interrupts.
    kernel::io::outb(PIC1.data, 0xFF);
    kernel::io::outb(PIC2.data, 0xFF);
}


void
PIC::remap(uint8_t pic1Offset,
           uint8_t pic2Offset,
           uint8_t pic2IRQ)
{
    // Save the current IRQ masks for each PIC
    uint8_t pic1Mask = kernel::io::inb(PIC1.data);
    uint8_t pic2Mask = kernel::io::inb(PIC2.data);

    initializePICs(pic1Offset, pic2Offset, pic2IRQ);

    // Restore the saved masks
    kernel::io::outb(PIC2.data, pic1Mask);
    kernel::io::outb(PIC2.data, pic2Mask);
}

void
PIC::endOfInterrupt(uint8_t irq)
    const
{
    /*
     * Notifying the PICs that an interrupt has been completed uses OCW2 on the
     * command ports. If the IRQ came from one owned by PIC2, both chips must be
     * notified. Otherwise, only PIC1 needs to be notified.
     */
    if (irq >= 8) {
        kernel::io::outb(PIC2.command, OCW2::NonSpecificEOI);
    }
    kernel::io::outb(PIC1.command, OCW2::NonSpecificEOI);
}


void
PIC::enableInterrupt(uint8_t irq,
                     bool enabled)
{
    if (enabled) {
        enableIRQ(irq);
    } else {
        disableIRQ(irq);
    }
}

/*
 * Private
 */

inline uint16_t
PIC::portForIRQ(uint8_t irq)
{
    return irq < 8 ? PIC1.data : PIC2.data;
}


void
PIC::enableIRQ(uint8_t irq)
{
    uint16_t port = portForIRQ(irq);
    if (irq >= 8) {
        irq -= 8;
    }
    uint8_t value = kernel::io::inb(port) & ~(1 << irq);
    kernel::io::outb(port, value);
}


void
PIC::disableIRQ(uint8_t irq)
{
    uint16_t port = portForIRQ(irq);
    if (irq >= 8) {
        irq -= 8;
    }
    uint8_t value = kernel::io::inb(port) | (1 << irq);
    kernel::io::outb(port, value);
}


void
PIC::initializePICs(uint8_t pic1Offset,
                    uint8_t pic2Offset,
                    uint8_t pic2IRQ)
{
    /*
     * Initialization of the PIC chips requires programming both chips together.
     * Each requires a series of four bytes: the first (ICW1) on their command
     * ports, and the subsequent three (ICW2 through ICW4) on their data ports.
     *
     * ICW1 contains the following flags:
     *   Bit 0: 1 = ICW4 required, 0 = ICW4 not required
     *   Bit 1: 1 = single 8259, 0 = cascading 8259s
     *   Bit 2: 1 = 4 byte interrupt vectors, 0 = 8 byte interrupt vectors
     *   Bit 3: 1 = level triggered mode, 0 = edge triggered mode
     *   Bit 4: 1 = initialization (this is required to be 1 for ICW1)
     *   Bits 5, 6, 7 must all be 0
     *
     * ICW2 is the interrupt vector offset that this PIC's interrupt should be
     * mapped to. It must be divisible by 0x8.
     *
     * ICW3 is different for the PIC1 and PIC2. For the PIC1, ICW3 should have
     * one bit set indicating on which IRQ PIC2 (the cascaded PIC) will be. For
     * PIC2, the first three bits of ICW3 indicate on which IRQ it will reside
     * on the PIC1.
     *
     * ICW4 contains the following flags:
     *   Bit 0: 1 = 80x86 mode, 0 = MCS 80/85 mode
     *   Bit 1: 1 = auto EOI mode, 0 = normal EOI mode
     *   Bits 2, 3: PIC2/PIC1 buffering mode
     *   Bit 4: 1 = special fully nested mode (SFNM), 0 = sequential mode
     *   Bits 5, 6, 7 must all be 0
     *
     * The buffering modes mentioned above are as follows:
     *   00: not buffered
     *   01: not buffered
     *   10: buffered mode PIC2 (PC mode)
     *   11: buffered mode PIC1 (PC mode)
     *
     * See also:
     *   http://stanislavs.org/helppc/8259.html
     *   http://wiki.osdev.org/PIC
     */

    // Ensure divisiblity by 8.
    pic1Offset &= ~uint8_t(0x7);
    pic2Offset &= ~uint8_t(0x7);

    // TODO: Implement clamping and error handling.
    uint8_t pic2IRQMask = 0;
    if (pic2IRQ < 16) {
        pic2IRQMask = 1;
        for (int i = 1; i < pic2IRQ; i++) {
            pic2IRQMask <<= 1;
        }
    } else {
        // TODO: We have a problem.
    }

#ifdef CONFIG_PIC_SHOULD_WAIT
# define waitIfRequired() io::wait()
#else
# define waitIfRequired()
#endif

#define sendToPIC(port, byte) \
    kernel::io::outb((port), (byte)); \
    waitIfRequired()

    sendToPIC(PIC1.command, ICW1::Initialize + ICW1::ICW4Required);
    sendToPIC(PIC1.data, pic1Offset);
    sendToPIC(PIC1.data, pic2IRQMask);
    sendToPIC(PIC1.data, ICW4::X86Mode);

    sendToPIC(PIC2.command, ICW1::Initialize + ICW1::ICW4Required);
    sendToPIC(PIC2.data, pic2Offset);
    sendToPIC(PIC2.data, pic2IRQ);
    sendToPIC(PIC2.data, ICW4::X86Mode);

#undef sendToPIC
#undef waitIfRequired
}

} /* namespace x86 */
