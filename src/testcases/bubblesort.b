declblock {
	int x[1000];
	int i, j;
	int temp;
}
codeblock {
	for i = 0, 1000 {
		x[i] = 1000-i;
	}
	for i = 0, 1000 {
		print x[i];
	}
	println "";
	for i = 0, 1000 {
		for j = i, 1000 {
			if (x[i] > x[j]) {
			   temp = x[i];
			   x[i] = x[j];
			   x[j] = temp;
			}
		}		
	}
	for i = 0, 1000 {
		print x[i];
	}
	println "";
}