int main(int argc,char ** argv)
{
	int retVal = 10;
	void * label = &&action4;
	goto action2;
	action1:
		retVal -= 4;
		label = &&endofcode;
		goto *label;
	action2:
		retVal -= 5;
		goto *label;
	action3:
		retVal += 40;
	action4:
		retVal -= 1;
		label = &&action1;
		goto *label;
	endofcode:
		return retVal;
}
