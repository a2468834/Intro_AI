#include <iostream>
#include <string>
#include <vector>
#include <deque>
#include <cstdio>
#include <cstdlib>
#include <cstring>

using namespace std;

void ReadIris(FILE *input_file);

int main(int argc, char** argv)
{
	FILE *input_file;

	if( argc != 2 )
	{
		cout<<"Please check the number of input file."<<endl;
		exit(EXIT_FAILURE);
	}

	input_file = fopen(argv[1], "r");

	if( input_file == NULL )
	{
		cout<<"Openning "<<argv[1]<<" failed!"<<endl;
		exit(EXIT_FAILURE);
	}

	string input_file_name(argv[1]);

	if( input_file_name == "iris.txt" )ReadIris(input_file);

	return 0;
}


void ReadIris(FILE *input_file)
{
	cout<<"SUCCESSFUL!";
}
