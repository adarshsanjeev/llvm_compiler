declblock {
	int x[5];
	int i, j;
	int temp;
}
codeblock {
	i = 0;
	LABEL1:
	i = i+1;
	println i;
	goto LABEL1 if i == 1;
}