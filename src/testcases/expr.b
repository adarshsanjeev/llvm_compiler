declblock {

}

codeblock{
	read x;
	read y, z;
	read a[20];
	x = 3 + 3;
	y = x - 5;
	while (x > y) {
		  x = x+1;
	}
	if ( x > y) {
	   y = y- 5;
	}
	if ( x > y) {
	   y = y- 5;
	}
	else {
		 y = y + 5;
	}
	for x = 1, x<2+2 {
		x = x + 4;
	}
	for x = 1, x<2, x+2 {
		x = x + 4;
	}
}
