declblock {
	int x[10];
	int i, j;
	int temp;
}
codeblock {
	for i = 0, 10 {
		x[i] = 10-i;
	}
	for i = 0, 10 {
		print x[i];
	}
	println "";
	for i = 0, 10 {
		for j = i, 10 {
			if (x[i] > x[j]) {
			   temp = x[i];
			   x[i] = x[j];
			   x[j] = temp;
			}
		}		
	}
	for i = 0, 10 {
		print x[i];
	}
	println "";
}