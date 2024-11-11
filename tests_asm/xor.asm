main:
	addi t0, zero, 512
	addi t1, zero, 365
	xor  t2, t0, t1
	addi t3, zero, -512
	addi t4, zero, -365
	xor  t5, t3, t4
	jr ra
