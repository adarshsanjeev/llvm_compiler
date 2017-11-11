declblock {
	int x[10];
	int i, j;
	int temp;
}
codeblock {
	LABEL1:
	for i = 0, 10 {
		print x[i];
	}
	println "";
}