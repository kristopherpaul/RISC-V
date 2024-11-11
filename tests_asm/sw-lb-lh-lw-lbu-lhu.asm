main:
    lui t0, 0xFFF8F  # t0= 0xFFF8F000

    # Storing the word
    addi sp, sp, -4     
    sw t0, 0(sp)          
    
    # Loading the word
    lw t1, 0(sp)         
    
    # Loading the halfword
    lh t2, 0(sp)          
    
    # Loading the byte
    lb t3, 0(sp)          
    
    #Loading the unsigned halfword
    lhu t4, 0(sp)
    
    #Loading the unsigned byte
    lbu t5, 0(sp)
    addi sp, sp, 4
   jr ra
