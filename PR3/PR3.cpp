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

// Function pre-declaration
size_t split( const string& input, vector<string>& output, char delimiter );
struct node* read_iris(fstream& input_file);
vector<pair<string, int>> class_counts(struct node* node);
float gini_impurity(struct node* node);
bool classify_function(float threshold, float subject);
pair<struct node*, struct node*> partition(struct node* node, string attr, float threshold);

// Some data structure
struct sample
{
	vector<pair<string, float>>value;
	string class_attr;
};

struct node
{
	vector<struct sample>data;
	struct node* parent;
	struct node* true_branch;
	struct node* false_branch;
	float impurity;
};

// Function implementation
int main(int argc, char** argv)
{
	if( argc != 2 )
	{
		cout<<"Please check the number of input file."<<endl;
		exit(EXIT_FAILURE);
	}

	fstream input_file(argv[1], ios::in);
	string input_file_name(argv[1]);
	struct node* root;

	if( input_file.fail() )
	{
		cout<<"Openning "<<argv[1]<<" failed!"<<endl;
		exit(EXIT_FAILURE);
	}

	if( input_file_name == "iris.txt" )
	{
		root = read_iris(input_file);
		pair<struct node*, struct node*>test = partition(root, "attr_0", 5.8);
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

struct node* read_iris(fstream& input_file)
{
	string one_line_input_string;
	//vector<vector<pair<string, float>>>dataset;
    struct node* root_node = new struct node;

    root_node->parent = NULL;
    root_node->true_branch = NULL;
    root_node->false_branch = NULL;
    root_node->impurity = 0;
    root_node->data.clear();
    
	while( getline(input_file, one_line_input_string) )
	{
		vector<string> temp;
		struct sample sample_temp;

		split(one_line_input_string, temp, ',');
		
		sample_temp.value.push_back( make_pair( "attr_0", stof(temp[0]) ) );
		sample_temp.value.push_back( make_pair( "attr_1", stof(temp[1]) ) );
		sample_temp.value.push_back( make_pair( "attr_2", stof(temp[2]) ) );
		sample_temp.value.push_back( make_pair( "attr_3", stof(temp[3]) ) );
		
		sample_temp.class_attr = temp[4];

		root_node->data.push_back(sample_temp);
	}
	return root_node;
}

pair<struct node*, struct node*> partition(struct node* node, string attr, float threshold)
{
	struct node* true_child = new struct node;
	struct node* false_child = new struct node;

	// Initialize the 'true_child'
	true_child->parent = node;
	true_child->true_branch = NULL;
	true_child->false_branch = NULL;
	true_child->impurity = 0;
	true_child->data.clear();

	// Initialize the 'true_child'
	false_child->parent = node;
	false_child->true_branch = NULL;
	false_child->false_branch = NULL;
	false_child->impurity = 0;
	false_child->data.clear();
	
	// Find the position of the selected attribute in each 'node->data.value'.
	int attr_pos = 0;
	for(; attr_pos<node->data[0].value.size(); attr_pos++)
		if(node->data[0].value[attr_pos].first == attr)break;

	
	for(int i=0; i<node->data.size(); i++)
	{
		struct sample temp = node->data[i];

		// Belong to true_branch
		if( temp.value[attr_pos].second >= threshold )
			true_child->data.push_back(temp);

		// Belong to false_branch
		else false_child->data.push_back(temp);
		
	}

	// Calculate Gini's impurity of both branches.
	true_child->impurity = gini_impurity(true_child);
	false_child->impurity = gini_impurity(false_child);
	
	return make_pair(true_child, false_child);
}

float gini_impurity(struct node* node)
{
	float impurity = 1;
	vector<pair<string, int>>counts = class_counts(node);
	
	for(int i=0; i<counts.size(); i++)
	{
		float prob = (float) counts[i].second / node->data.size();
		impurity = impurity - prob*prob;
	}
	return impurity;
}

vector<pair<string, int>> class_counts(struct node* node)
{
	vector<pair<string, int>>counts;

	for(vector<struct sample>::iterator itr = node->data.begin(); 
		itr != node->data.end(); 
		itr++)
	{
		int flag = 0;
		for(vector<pair<string, int>>::iterator itrr = counts.begin(); 
			itrr != counts.end(); 
			itrr++)
		{
			if( itr->class_attr == itrr->first )
			{
				flag = 1;
				itrr->second++;
			}
		}
		//This class_attr has not been counted yet.
		if( flag == 0 )counts.push_back( make_pair(itr->class_attr, 1) );
	}
	return counts;
}

// Unused nowadays
bool classify_function(float threshold, float subject)
{
	if( subject >= threshold )return true;
	else return false;
}