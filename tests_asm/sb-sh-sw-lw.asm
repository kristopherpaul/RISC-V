main:
 
    lui t0, 0x12345         # t0 = 0x12345000 
    ori t0, t0, 0x678      # t0 = 0x12345678 = 305419896

    # Storing the byte
    addi sp, sp, -4 
    sb t0, 0(sp)          	# 0x78 gets stored
    
    # Storing the halfword
    addi sp, sp, -4      
    sh t0, 0(sp)          # 0x 5678 gets stored

    # Storing the word
    addi sp, sp, -4      
    sw t0, 0(sp)          #  0x12345678 gets stored  
    
    # Loading the word (word)
    lw t1, 0(sp)          # t1 = 0x12345678 = 305419896
    addi sp, sp, 4 
    
    # Loading the word (halfword)
    lw t2, 0(sp)          # t2 = 0x5678 = 22136
    addi sp, sp, 4
    
    # Loading the word (byte)
    lw t3, 0(sp)          # t3 = 0x78 = 120
    addi sp, sp, 4

   #performing operations for sanity check   
   add t4, t1,t2       #t4 = 305442032
   add t5, t2, t3 	#t5 = 22256