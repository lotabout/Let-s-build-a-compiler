# Notes

x86 instructions is different to 68000 in some ways, the generated code is
a trying to simulate the author's 68k codes. For example, the "MOVE"
expression in 68k only requires the type information like "MOVE.L A(PC),D0" or
"MOVE.B A(PC),D0" while x86 specify length by register types like "MOV EAX, A"
or "MOV AL, A". More over, the AT&T syntax require the type information with
operator like "movl A, %eax" or "movb A, %al".

So the generating procedure might look like a giant compared to the one
generating 68k codes.
