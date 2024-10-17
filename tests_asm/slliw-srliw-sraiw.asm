.text
main:
addi t0, zero, 13
slli t0, t0, 27
slliw t1, t0, 3
addi t2, zero, -1
srli t2, t2, 33
slli t2, t2, 32
add t2, t2, t0
slliw t3, t2, 2
slliw t4, t0, 4
add s0, zero, t1
add s1, zero, t3
add s2, zero, t4
srliw t1, t2, 3
srliw t3, t4, 4
add s3, zero, t1
add s4, zero, t3
sraiw s5, t2, 3
sraiw s6, t4, 4
