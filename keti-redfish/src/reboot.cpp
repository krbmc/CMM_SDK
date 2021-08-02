#include <iostream>
#include <unistd.h>
#include <cstdlib>
#include <string>

using namespace std;

int main(void)
{
	sleep(4);
	string redfish = "./keti-redfish";
	execlp(redfish.c_str(), NULL);
	// system(redfish.c_str());

	return 0;
}
