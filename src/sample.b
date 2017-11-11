declblock {
	int x[5];
	int i, j;
	int temp;
}
codeblock {
 	for i = 0, 100 {
 		x[i] = i;
 	}
 	for i = 0, 100 {
 		print x[i];
 	}
	println "";
}