#include <Decompressor.h>
#include <vector>
#include "config.h"

using namespace std;
using namespace compression;

int main(int argc, char **argv)
{
	string directory = argv[0];// Program full path + name of binary file.
	directory.erase(directory.find_last_of('\\') + 1);// Remove name of binary file.
	vector<int> ordersToRun;
	ordersToRun.push_back(2);
	Decompressor decompressor(directory, "code-" + config::inputfile + "-order-2", ordersToRun);
}