#include <Compressor.h>
#include <Decompressor.h>

using namespace std;
using namespace compression;

int main(int argc, char **argv)
{
	string directory = argv[0];// Program full path + name of binary file.
	directory.erase(directory.find_last_of('\\') + 1);// Remove name of binary file.
	//Compressor compressor(directory);
	//Decompressor decompressor(directory);
}