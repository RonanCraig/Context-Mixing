#include <Compressor.h>
#include <vector>

using namespace std;
using namespace compression;

int main(int argc, char **argv)
{
	string directory = argv[0];// Program full path + name of binary file.
	directory.erase(directory.find_last_of('\\') + 1);// Remove name of binary file.
	vector<int> ordersToRun;
	Compressor* compressor;

	{
		// Order 2
		ordersToRun.push_back(2);
		compressor = new Compressor(directory, "code-" + config::inputfile + "-order-2", ordersToRun);
	}
	
	{
		// Order 4
		ordersToRun.clear();
		ordersToRun.push_back(4);
		delete compressor;
		compressor = new Compressor(directory, "code-" + config::inputfile + "-order-4", ordersToRun);
	}

	{
		// Order 6
		ordersToRun.clear();
		ordersToRun.push_back(6);
		delete compressor;
		compressor = new Compressor(directory, "code-" + config::inputfile + "-order-6", ordersToRun);
	}

	{
		// Order 8
		ordersToRun.clear();
		ordersToRun.push_back(8);
		delete compressor;
		compressor = new Compressor(directory, "code-" + config::inputfile + "-order-8", ordersToRun);
	}

	{
		// Order 2,4,6,8
		ordersToRun.clear();
		ordersToRun.push_back(2);
		ordersToRun.push_back(4);
		ordersToRun.push_back(6);
		ordersToRun.push_back(8);
		delete compressor;
		compressor = new Compressor(directory, "code-" + config::inputfile + "-order-2-4-6-8", ordersToRun);
	}
}