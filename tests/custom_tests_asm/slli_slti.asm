main:
	addi t0, zero, 1024
	slli t1, t0, 3
	slti t2, t0, 2045
	slti t3, t0, -1024   
	jr ra                
