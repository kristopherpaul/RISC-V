main:
         
    lui t0, 0xFFF8F  
    
    addi sp, sp, -4      
    sw t0, 0(sp)         
    
    # Loading the word
    lw t1, 0(sp)         
    
    # Loading the word unsigned
    lwu t2, 0(sp)          
    addi sp, sp, 4
   

   #performing operations for sanity check   
   add t3, t1,t2      
   jr ra