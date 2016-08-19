.arch armv7a

.section .vitalink.fstubs,"ax",%progbits

	.align 4
	.global _vshIoMount
	.type _vshIoMount, %function
_vshIoMount:
	.word 0x00000000
	.word 0x35C5ACD4
	.word 0x3C522C35
	.align 4

