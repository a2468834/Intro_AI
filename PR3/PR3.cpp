#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <deque>
#include <utility>
#include <algorithm>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <ctime>

using namespace std;

// Function pre-declaration
size_t split( const string& input, vector<string>& output, char delimiter );
struct node* read_iris(fstream& input_file);
vector<pair<string, int>> class_counts(struct node* node);
float gini_impurity(struct node* node);
bool classify_function(float threshold, float subject);
pair<struct node*, struct node*> partition(struct node* node, string attr, float threshold);
float information_gain(struct node* parent, struct node* true_child, struct node* false_child);
bool comparison(const struct sample& A, const struct sample& B, const int& selected_attr);
pair<float, float> find_best_partition(struct node* node, int selected_attr);

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

class sorter
{
private:
	int criteria; // The alias of the position of the attribute in 'sample.value'.
public:
	sorter( const int& selected_attr )
	{
		criteria = selected_attr;
	}
	bool operator()( const struct sample& A, const struct sample& B )const
	{
		return comparison(A, B, criteria);
	}
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
		find_best_partition(root, 0);
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

bool comparison(const struct sample& A, const struct sample& B, const int& selected_attr)
{
	return ( A.value[selected_attr].second < B.value[selected_attr].second );
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
	root_node->impurity = gini_impurity(root_node);
	return root_node;
}

pair<struct node*, struct node*> partition(struct node* node, int attr, float threshold)
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
	
	for(int i=0; i<node->data.size(); i++)
	{
		struct sample temp = node->data[i];

		// Belong to true_branch
		if( temp.value[attr].second >= threshold )
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

float information_gain(struct node* parent, struct node* true_child, struct node* false_child)
{
	float prop = (float) true_child->data.size() / ( true_child->data.size() + false_child->data.size() );
	return parent->impurity - prop*true_child->impurity - (1-prop)*false_child->impurity;
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

bool is_leaf_node(struct node* node)
{
	return (node->impurity == 0);
}

struct node* build_tree(struct node* node, int attr_num)
{
	// Initialize random function with the seed.
	srand(time(NULL));
	// Determine which attribute is selected.
	int selected_attr = rand()%attr_num;
	// Find the best information gain and the best threshold of 'selected_attr'.
	pair<float, float> garbbage = find_best_partition(node, selected_attr);
	
	//if(information_gain == 0)
	struct node* garbage;
	return garbage;
}

pair<float, float> find_best_partition(struct node* node, int selected_attr)
{
	float best_gain = 0;
	sort(node->data.begin(), node->data.end(), sorter(selected_attr));

	for(int i=0; i<(node->data.size()-1); i++)
	{
		float v_i = node->data[i].value[selected_attr].second;
		float v_j = node->data[i+1].value[selected_attr].second;
		float threshold = (v_i + v_j)/2
		partition(node, selected_attr, threshold);
	}
	
	//partition(node, selected_attr, threshold);
	return make_pair(1.1, 2.2);
}