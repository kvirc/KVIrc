int main(int argc, char **)
{
	static void * const targets[] = { &&first, &&second };
	goto *targets[argc == 1];

first:
	return 1;
second:
	return 0;
}
