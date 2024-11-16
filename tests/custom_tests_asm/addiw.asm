.text
main:
addi t1, zero, 2047
slli t1, t1, 11
addi t1, t1, 2047
slli t1, t1, 10
addi t1, t1, 1023
addiw s1, t1, 10
addiw s2, t1, -459
srli t1, t1, 1
addiw s3, t1, 2047
addiw s4, t1, -100
jr ra