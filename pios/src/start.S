.section ".text.boot"

.globl _start
_start:
    mrs	x0, mpidr_el1		
    and	x0, x0,#0xFF         // Check processor id
    cbz	x0, master           // Hang for all non-primary CPU
    b	proc_hang

proc_hang: 
    b 	proc_hang

.globl memzero
memzero:
    str xzr, [x0], #8
    subs x1, x1, #8
    b.gt memzero
    ret

.globl PUT32
PUT32:
    str w1,[x0]
    ret

.globl GET32
GET32:
    ldr w0,[x0]
    ret

.globl delay
delay:
    subs x0, x0, #1
    bne delay
    ret

.globl wait_op
wait_op:
    ret

master:
    adr	x0, bss_begin
    adr	x1, bss_end
    sub	x1, x1, x0
    bl 	memzero

    bl  kernel_init
    b 	proc_hang            // should never come here
