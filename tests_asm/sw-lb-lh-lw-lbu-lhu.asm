main:
     lui t0, 0xFFF8F  # t0= 0xFFF80000

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
    
    #Loading the unsigned word
    lbu t5, 0(sp)
    
   #performing operations for sanity check   
   add s1, t1,t2       
   add s2, t2, t3 	
   add s3, t3, t4
   add s4, t4, t5
