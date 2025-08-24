    sub,    $r12,   $zero,  $imm,   2048
    mul,    $r12,   $r12,   $imm,   2 
    mul,    $r13,   $r12,   $imm,   2 
	sub,	$r11,	$r12,	$imm,	4
    add,    $r15,   $r15,   $imm,   252	
loop:
	lw,	    $r2,	$r10,	$imm,	0 # load block
	lw,	    $r3,	$r10,	$imm,	1
	lw,	    $r4,	$r10,	$imm,	2
	lw,	    $r5,	$r10,	$imm,	3
	lw,	    $r6,	$r12,	$imm,	0 # load other block
	lw,	    $r7,	$r12,	$imm,	1	
	lw,	    $r8,	$r12,	$imm,	2	
	lw,	    $r9,	$r12,	$imm,	3	
	add,	$r2,	$r2,	$r6,	0
	add,	$r3,	$r3,	$r7,	0
	add,	$r4,	$r4,	$r8,	0
	add,	$r5,	$r5,	$r9,	0
	sw,	    $r2,	$r13,	$imm,	0
	sw,	    $r3,	$r13,	$imm,	1
	sw,	    $r4,	$r13,	$imm,	2
	sw,	    $r5,	$r13,	$imm,	3
    add,	$r12,	$r12,	$imm,	4 # for next block 		
	add,	$r13,	$r13,	$imm,	4 # for next block 		
	bne,	$imm,	$r10,	$r11,	loop # last possible index
	add,	$r10,	$r10,	$imm,	4 # for next block 
EXIT:
    lw,	    $r2,	$r14,	$imm,	0
    bne,    $imm,   $r14,   $r15,   EXIT
    add,	$r14,	$r14,	$imm,	4 # for next block 	 	
	halt,	$zero,	$zero,	$zero,	0