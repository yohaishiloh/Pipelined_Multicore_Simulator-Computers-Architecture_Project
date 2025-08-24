	add,	$r6,	$imm,	$zero,	3
loop3:
	lw, 	$r2,	$zero,	$zero,	0
	and, 	$r5,	$r2,	$imm,	3
	bne,	$imm,	$r5,	$r6,	loop3
	add,	$zero,	$zero,	$zero,	0		
	add,	$r2,	$r2,	$imm,	1
	sw, 	$r2,	$zero,	$zero,	0
	add,	$r4,	$imm,	$zero,	127
	bne,	$imm,	$r3,	$r4,	loop3
	add,	$r3,	$r3,	$imm,	1
	lw,		$zero,	$zero,	$imm,	256
    halt,	$zero,	$zero,	$zero,	0
	