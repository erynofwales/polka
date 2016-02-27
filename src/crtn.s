.section .init
    # crtend.o's .init section goes here.
    popl %ebp
    ret

.section .fini
    # crtend.o's .fini section goes here.
    popl %ebp
    ret
