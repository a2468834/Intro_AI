#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <deque>
#include <utility>
#include <cstdio>
#include <cstdlib>
#include <cstring>

using namespace std;

int main(int argc, char** argv)
{
	if( argc != 2 )
	{
		cout<<"Please check the number of input file."<<endl;
		exit(EXIT_FAILURE);
	}

	if( input_file.fail() )
	{
		cout<<"Openning "<<argv[1]<<" failed!"<<endl;
		exit(EXIT_FAILURE);
	}

	fstream input_file(argv[1], ios::in);
	string input_file_name(argv[1]);

	if( input_file_name == "iris.txt" )
	{

	}

	return 0;
}