
main:
    # Storing the first word
    addi sp, sp, -4      
    addi t0, zero, -3    # t0 = -3
    sw t0, 0(sp)         

    # Storing the second word
    addi sp, sp, -4      
    addi t1, zero, 5     #  t1 = 5
    sw t1, 0(sp)         

    # Loading the second word
    lw t2, 0(sp)         # t2 = 5
    addi sp, sp, 4      

    # Loading the first word 
    lw t3, 0(sp)         # t3 = -3
    addi sp, sp, 4       
    
    # Performing add operation
    add t4, t2, t3       # t4 = 2
    jr ra

   
