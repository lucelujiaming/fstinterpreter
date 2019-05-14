.global altera_trampoline
altera_trampoline:
	ldr r0, [pc]
	bx r0
.global altera_trampoline_jump
altera_trampoline_jump:
	.word
.global altera_trampoline_end
altera_trampoline_end:
