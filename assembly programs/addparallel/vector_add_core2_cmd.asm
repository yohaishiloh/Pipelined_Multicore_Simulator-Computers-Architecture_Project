 	add,	$r10,	$zero	$imm,	loop2	
    sub,    $r12,   $zero,  $imm,   2048    # r12 = 2048 
    mul,    $r12,   $r12,   $imm,   2       # r12 = 4096
    mul,    $r13,   $r12,   $imm,   2       # r13 = 8192
    sub,    $r13,   $r13,   $imm,   16
loop2:
	lw,	    $r2,	$r11,	$imm,	8 
	lw,	    $r3,	$r11,	$imm,	9
	lw,	    $r4,	$r11,	$imm,	10
	lw,     $r5,	$r11,	$imm,	11
    add,    $r14,   $r14,   $imm,   1
    lw,	    $r6,	$r12,	$imm,	8
	lw,	    $r7,	$r12,	$imm,	9	
	lw,	    $r8,	$r12,	$imm,	10	
	lw,	    $r9,	$r12,	$imm,	11   
	add,	$r2,	$r2,	$r6,	0
	add,	$r3,	$r3,	$r7,	0
	add,	$r4,	$r4,	$r8,	0
	add,	$r5,	$r5,	$r9,	0
    add,    $r11,   $r11,   $imm,   16
    add,    $r12,   $r12,   $imm,   16   
    add,    $r13,   $r13,   $imm,   16 
	sw,	    $r2,	$r13,	$imm,	8
	sw,	    $r3,	$r13,	$imm,	9
	sw,	    $r4,	$r13,	$imm,	10
	bne,	$r10,	$r14,	$imm,	256 
	sw,	    $r5,	$r13,	$imm,	11
EXIT:
   	lw,	    $r2,	$r15,	$imm,	8
 	add,	$r10,	$zero	$imm,	EXIT
	bne,	$r10,	$r15,	$imm,	256
    add,    $r15,   $r15,   $imm,   16
	halt,	$zero	$zero	$zero	0