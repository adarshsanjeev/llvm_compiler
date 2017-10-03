declblock {
		  int x, y;
		  int z;
}

codeblock{
	x = 3 + 3;
	y = x - 5;
	for z=1, z<10, z=z+1{
		z = z+1;
		print z;
	}
	println "HELLO", "WORLD", x, z;
}
