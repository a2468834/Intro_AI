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

size_t split( const string& input, vector<string>& output, char delimiter );
vector<vector<pair<string, float>>> ReadIris(fstream& input_file);

int main(int argc, char** argv)
{
	fstream input_file(argv[1], ios::in);
	string input_file_name(argv[1]);
	vector<vector<pair<string, float>>> dataset;
	
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

	if( input_file_name == "iris.txt" )
	{
		dataset = ReadIris(input_file);
	}

	return 0;
}

size_t split( const string& input, vector<string>& output, char delimiter )
{
    size_t pos = input.find( delimiter );
    size_t initialPos = 0;
    output.clear();

    // Decompose statement
    while( pos != string::npos )
    {
        output.push_back( input.substr( initialPos, (pos-initialPos) ) );
        initialPos = pos + 1;

        pos = input.find( delimiter, initialPos );
    }

    // Add the last one
    output.push_back( input.substr( initialPos, (min(pos, input.size())-initialPos+1) ) );

    return output.size();
}

vector<vector<pair<string, float>>> ReadIris(fstream& input_file)
{
	string one_line_input_string;
	vector<vector<pair<string, float>>>dataset;

	while(getline(input_file, one_line_input_string))
	{
		vector<string> temp;
		vector<pair<string, float>>sample;

		split(one_line_input_string, temp, ',');
		
		sample.push_back( make_pair( "attr_0", stof(temp[0]) ) );
		sample.push_back( make_pair( "attr_1", stof(temp[1]) ) );
		sample.push_back( make_pair( "attr_2", stof(temp[2]) ) );
		sample.push_back( make_pair( "attr_3", stof(temp[3]) ) );
		
		if(temp[4] == "Iris-setosa" )sample.push_back( make_pair( "class_attr", 0 ) );
		else if(temp[4] == "Iris-versicolor" )sample.push_back( make_pair( "class_attr", 1 ) );
		else if(temp[4] == "Iris-virginica" )sample.push_back( make_pair( "class_attr", 2 ) );

		dataset.push_back(sample);
	}
	return dataset;
}
