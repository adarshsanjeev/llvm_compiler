declblock {
		  int x, y;
		  int z;
		  int a[10];
}

codeblock{
	x = 3 + 3;
	y = -5;
	println "y=", y;
	for z=1, z<10, z=z+1{
		z = z+1;
		a[z] = 2;
		print z;
	}
	println " ";
	if (3>3) {
	z = 2;
	}
	else {
	z = 3;
	}
	a[9] = 5;
	println "HELLO", "WORLD", x, z;
	println a[10];
}
