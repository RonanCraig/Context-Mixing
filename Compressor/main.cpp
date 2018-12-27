#include "Compressor.h"

using namespace std;

int main(int argc, char **argv)
{
	string directory = argv[0];// Program full path + name of binary file.
	directory.erase(directory.find_last_of('\\') + 1);// Remove name of binary file.
	string message = "ab";
	Compressor compressor(directory, message);
}