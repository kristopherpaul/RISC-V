main:
  addi t0, zero, 1
  addi t1, zero, 2
  addi t2, zero, 3
  csrrw zero, 0x300, t0
  csrrs zero, 0x305, t1
  csrrw zero, 0x341, t2
  csrrc t2, 0x341, zero
  csrrwi zero, 0x100, 4
  csrrsi zero, 0x105, 5
  csrrwi zero, 0x141, 6
  csrrci zero, 0x141, 0
  jr ra