.text
main:
addi t0, zero, 13
slli t0, t0, 27
addi t1, zero, 3
sllw t1, t0, t1
addi t2, zero, -1
srli t2, t2, 33
slli t2, t2, 32
add t2, t2, t0
addi t3, zero, 2
addi t4, zero, 4
sllw t3, t2, t3
sllw t4, t0, t4
add s0, zero, t1
add s1, zero, t3
add s2, zero, t4
addi t1, zero, 3
srlw t1, t2, t1
addi t3, zero, 4
srlw t3, t4, t3
add s3, zero, t1
add s4, zero, t3
addi t1, zero, 3
addi t3, zero, 4
sraw s5, t2, t1
sraw s6, t4, t3
addi t1, zero, 65
addi t3, zero 46
sllw s7, t2, t1
sllw s8, t2, t3
sraw s9, s2, t3
sraw s10, s2, t1
jr ra
