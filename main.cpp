#include <iostream>

int main(int ac, char *argv[])
{
	if (ac != 2)
	{
		std::cout << "Wrong number of arguments." << std::endl;
		return -1;
	}

	return 0;
}