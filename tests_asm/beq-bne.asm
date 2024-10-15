.text
main:
	# assuming add-addi and j works
	addi t0, zero, 5
	addi t1, zero, 9
	add t2, t0, t1
	add t3, t1, zero
	test1:
	beq t0, t1, equal1
	notequal1:
	addi s0, zero, 1
	j test2
	equal1:
	addi s0, zero, 0
	test2:
	beq t1, t3, equal2
	notequal2:
	addi s1, zero, 0
	j test3
	equal2:
	addi s1, zero, 1
	test3:
	bne t1, t3, notequal3
	equal3:
	addi s2, zero, 1
	j test4
	notequal3:
	addi s2, zero, 0
	test4:
	bne t0, t1, notequal4
	equal4:
	addi s3, zero, 0
	j end
	notequal4:
	addi s3, zero, 1
	end:
	addi s4, zero, 1
	
	