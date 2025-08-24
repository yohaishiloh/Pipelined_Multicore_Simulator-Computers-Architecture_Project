loop0:
	lw, 	$r2,	$zero,	$zero,	0
	and, 	$r5,	$r2,	$imm,	3
	bne,	$imm,	$r5,	$zero,	loop0
	add,	$zero,	$zero,	$zero,	0		
	add,	$r2,	$r2,	$imm,	1
	sw,		$r2,	$zero,	$zero,	0
	add,	$r4,	$imm,	$zero,	127
	bne,	$imm,	$r3,	$r4,	loop0
	add,	$r3,	$r3,	$imm,	1
	halt,	$zero,	$zero,	$zero,	0