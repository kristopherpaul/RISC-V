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
    #lhu t4, 0(sp)
    
    #Loading the unsigned word
    #lbu t5, 0(sp)
    
   #performing operations for sanity check   
   add t3, t0,t1       
   add t4, t1, t2
   jr ra