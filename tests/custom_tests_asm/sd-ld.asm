
main:
    #Load first half into t0
    lui t0, 0x11223         
    ori t0, t0, 0x344     
    addi t0, t0, 0x5
    
    # Load the next part into t1
    lui t1, 0x56677        
    ori t1, t1, 0x788       

    # Combine the two parts into t2
    slli t1, t1, 32        
    or t2, t0, t1           

    # Store the doubleword in memory
    addi sp, sp, -8         
    sd t2, 0(sp)           

    # Load the doubleword 
    ld t3, 0(sp)  
    addi sp, sp, 8    
    jr ra      

