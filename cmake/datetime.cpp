#include <iostream>
#include <ctime>

int main(int,char **)
{
	std::time_t tNow = std::time(NULL);
	struct std::tm * tNowParts = std::gmtime(&tNow);
	if(!tNowParts)
		std::cout << "ERROR" << std::endl;
	else
		std::cout
			<< (tNowParts->tm_year + 1900)
			<< "/"
			<< ((tNowParts->tm_mon + 1) / 10)
			<< ((tNowParts->tm_mon + 1) % 10)
			<< "/"
			<< ((tNowParts->tm_mday) / 10)
			<< ((tNowParts->tm_mday) % 10)
			<< " "
			<< ((tNowParts->tm_hour) / 10)
			<< ((tNowParts->tm_hour) % 10)
			<< ":"
			<< ((tNowParts->tm_min) / 10)
			<< ((tNowParts->tm_min) % 10)
			<< ":"
			<< ((tNowParts->tm_sec) / 10)
			<< ((tNowParts->tm_sec) % 10)
			<< " UTC"
			<< std::endl;
	return 0;
}
