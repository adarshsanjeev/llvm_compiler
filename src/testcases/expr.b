declblock {
		  int x, y;
		  int z;
}

codeblock{
	read x;
	read y, z;
	read a[20];
	x = 3 + 3;
	y = x;
	while (x > y) {
		  x = x+1;
	}
	LABEL2:
	if ( x > y) {
	   y = y- 5;
	}
	if ( x > y) {
	   y = y- 5;
	}
	else {
		 y = y + 5;
	}
	LABEL1:
	for x = 1, x<2+2 {
		x = x + 4;
	}
	for x = 1, x<2, x+2 {
		x = x + 4;
	}
	goto LABEL1 ;
	goto LABEL2 if x>3;
	print "HELLO", "JOHN", x;
}
