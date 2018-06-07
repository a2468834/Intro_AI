#include "PR3.h"

// Function implementation
int main(int argc, char** argv)
{
	// Initialize random function with the seed.
	srand(time(NULL));
	
	if( argc != 2 )
	{
		cout<<"Please check the number of input file."<<endl;
		exit(EXIT_FAILURE);
	}

	fstream input_file(argv[1], ios::in);
	string input_file_name(argv[1]);
	struct node* total = new struct node;
	struct node* train_root = new struct node;
	struct node* valid_root = new struct node;

	if( input_file.fail() )
	{
		cout<<"Openning "<<argv[1]<<" failed!"<<endl;
		exit(EXIT_FAILURE);
	}

	if( input_file_name == "iris.txt" )
	{
		total = read_file(input_file, 4);
		holdout_method(total, train_root, valid_root);
		train_root->impurity = gini_impurity(train_root);

		// The 'attr_num' of iris.txt is 4.
		build_tree(train_root, 4);
		//print_tree(train_root, "");
		cout<<validation(train_root, valid_root->data)<<endl;
	}

	else if( input_file_name == "cross200.txt" )
	{
		total = read_cross200(input_file);
		holdout_method(total, train_root, valid_root);
		train_root->impurity = gini_impurity(train_root);

		// The 'attr_num' of cross200.txt is .
		build_tree(train_root, 2);
		//print_tree(train_root, "");
		cout<<validation(train_root, valid_root->data)<<endl;
	}

	else if( input_file_name == "optical-digits.txt" )
	{
		total = read_optical(input_file);
		holdout_method(total, train_root, valid_root);
		train_root->impurity = gini_impurity(train_root);

		// The 'attr_num' of optical-digits.txt is 64.
		build_tree(train_root, 64);
		//print_tree(train_root, "");
		cout<<validation(train_root, valid_root->data)<<endl;
	}

	else if( input_file_name == "winequality-red.txt"  )
	{
		total = read_wine(input_file);
		holdout_method(total, train_root, valid_root);
		train_root->impurity = gini_impurity(train_root);

		// The 'attr_num' of winequality-red.txt is 11.
		build_tree(train_root, 11);
		//print_tree(train_root, "");
		cout<<validation(train_root, valid_root->data)<<endl;
	}

	else cout<<"Unknown dataset!"<<endl;
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
	// '<' results in the ascending order (i.e. small -> large).
	return ( A.value[selected_attr].second < B.value[selected_attr].second );
}

void holdout_method(struct node* total, struct node* train_root, struct node* valid_root)
{
	int counter = 0;
	vector<int> selection_list;
	
	while( counter < (0.7*total->data.size()) )
	{
		int selected_sample = rand()%total->data.size();
		sort(selection_list.begin(), selection_list.end());

		// Return vector.end() when "404 NOT FOUND".
		vector<int>::iterator itr = find(selection_list.begin(), selection_list.end(), selected_sample);
		
		// This index has not been selected yet.
		if(itr == selection_list.end())
		{
			train_root->data.push_back( total->data[selected_sample] );
			selection_list.push_back(selected_sample);
			counter++;
		}
		else continue;
	}
	for(int i=0; i<(int)total->data.size(); i++)
	{
		// This index has not been selected into training subset.
		if( find(selection_list.begin(), selection_list.end(), i) == selection_list.end() )
			valid_root->data.push_back( total->data[i] );
	}
}

pair<struct node*, struct node*> partition(struct node* parent, int selected_attr, float threshold)
{
	struct node* true_child = new struct node;
	struct node* false_child = new struct node;

	// Initialize the 'true_child'
	true_child->data.clear();
	true_child->parent = parent;
	true_child->true_branch = NULL;
	true_child->false_branch = NULL;
	true_child->impurity = 1;
	true_child->part_attr.clear();
	true_child->part_thres = 0;
	true_child->is_leaf = false;

	// Initialize the 'false_child'
	false_child->data.clear();
	false_child->parent = parent;
	false_child->true_branch = NULL;
	false_child->false_branch = NULL;
	false_child->impurity = 1;
	false_child->part_attr.clear();
	false_child->part_thres = 0;
	false_child->is_leaf = false;
	
	for(int i=0; i<(int)parent->data.size(); i++)
	{
		// Belong to true_branch
		if( parent->data[i].value[selected_attr].second >= threshold )
			true_child->data.push_back(parent->data[i]);

		// Belong to false_branch
		else false_child->data.push_back(parent->data[i]);
	}

	// Calculate Gini's impurity of both branches.
	true_child->impurity = gini_impurity(true_child);
	false_child->impurity = gini_impurity(false_child);
	
	return make_pair(true_child, false_child);
}

pair<float, float> find_best_partition(struct node* parent, int selected_attr)
{
	float best_info_gain = 0;
	float best_threshold = 0;

	// First at all, sort 'node.data' along with the "selected" attribute.
	sort(parent->data.begin(), parent->data.end(), sorter(selected_attr));

	for(int i=0; i<(int)(parent->data.size()-1); i++)
	{
		// v_i is the value of the "selected" attribute of a certain sample.
		// v_j is the same thing of the next sample.
		float v_i = parent->data[i].value[selected_attr].second;
		float v_j = parent->data[i+1].value[selected_attr].second;
		float threshold = (v_i + v_j)/2;
		pair<vector<pair<string, int>>, vector<pair<string, int>>> temp;

		// Try to "simulate" partitioning the node with a threshold.
		temp = pseudo_partition(parent, selected_attr, threshold);

		// Skip the threshold if it does not partition the node at all.
		if( temp.first.size() == 0 )continue;
		else if( temp.second.size() == 0 )continue;
		
		// Calculate the information gain after partitioning.
		float info_gain = 0;
		float prop = 0;
		float true_child_impurity = 1;
		float false_child_impurity = 1;
		int true_child_size = 0;
		int false_child_size = 0;

		for(int i=0; i<(int)temp.first.size(); i++)
			true_child_size = true_child_size + temp.first[i].second;
		
		for(int i=0; i<(int)temp.second.size(); i++)
			false_child_size = false_child_size + temp.second[i].second;

		prop = (float)true_child_size / ( true_child_size + false_child_size );
		
		for(int i=0; i<(int)temp.first.size(); i++)
		{
			float prob = (float)temp.first[i].second / true_child_size;
			true_child_impurity -= prob*prob;
		}

		for(int i=0; i<(int)temp.second.size(); i++)
		{
			float prob = (float)temp.second[i].second / false_child_size;
			false_child_impurity -= prob*prob;
		}

		info_gain = (float)parent->impurity - prop*true_child_impurity - (1-prop)*false_child_impurity;

		// Find a way to partition that will produce larger infromation gain than before.
		if(best_info_gain <= info_gain)
		{
			best_info_gain = info_gain;
			best_threshold = threshold;
		}
	}
	return make_pair(best_info_gain, best_threshold);
}

pair<vector<pair<string, int>>, vector<pair<string, int>>> pseudo_partition(
	struct node* parent, int selected_attr, float threshold)
{
	vector<pair<string, int>>parent_counts = class_counts(parent);
	vector<pair<string, int>>true_child_counts = parent_counts;
	vector<pair<string, int>>false_child_counts = parent_counts;

	// Clean up counting number in both of parent's children.
	for(int i=0; i<(int)true_child_counts.size(); i++)true_child_counts[i].second = 0;
	for(int i=0; i<(int)true_child_counts.size(); i++)false_child_counts[i].second = 0;

	for(int i=0; i<(int)parent->data.size(); i++)
	{
		// Belong to true_branch
		if( parent->data[i].value[selected_attr].second >= threshold )
		{
			vector<pair<string, int>>::iterator itr = true_child_counts.begin();
			for(; itr!=true_child_counts.end(); itr++)
				if(itr->first == parent->data[i].class_attr)break;
			itr->second++;
		}

		// Belong to false_branch
		else
		{
			vector<pair<string, int>>::iterator itr = false_child_counts.begin();
			for(; itr!=false_child_counts.end(); itr++)
				if(itr->first == parent->data[i].class_attr)break;
			itr->second++;
		}
	}

	bool empty_partition = true;
	
	for(int i=0; i<(int)true_child_counts.size(); i++)
		if( true_child_counts[i].second != 0)empty_partition = false;
	if(empty_partition == true)true_child_counts.clear();

	empty_partition = true;
	for(int i=0; i<(int)false_child_counts.size(); i++)
		if( false_child_counts[i].second != 0)empty_partition = false;
	if(empty_partition == true)false_child_counts.clear();

	return make_pair(true_child_counts, false_child_counts);
}

float gini_impurity(struct node* node)
{
	float impurity = 1;
	vector<pair<string, int>>counts = class_counts(node);
	
	for(int i=0; i<(int)counts.size(); i++)
	{
		float prob = (float)counts[i].second / node->data.size();
		impurity -= prob*prob;
	}

	return impurity;
}

float information_gain(struct node* parent, struct node* true_child, struct node* false_child)
{
	float prop = (float)true_child->data.size() / ( true_child->data.size() + false_child->data.size() );
	return (float)parent->impurity - prop*true_child->impurity - (1-prop)*false_child->impurity;
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

bool is_leaf_node(struct node* node)
{
	// A node is the leaf when its impurity is zero (i.e. it's totally pure).
	return ( node->is_leaf == true );
}

void build_tree(struct node* node, int attr_num)
{
	bool pseudo_leaf = false;
	int hidden_attr = 0;
	pair<float, float>check_carefully;

	// Determine which attribute is selected.
	int selected_attr = (int) rand()%attr_num;

	// Find the best information gain and the best threshold of 'selected_attr'.
	pair<float, float>next_partition = find_best_partition(node, selected_attr);

	// When we gain no info. from partitioning, we must be careful.
	if( next_partition.first == 0 )
	{
		for(int i=0; i<attr_num; i++)
		{
			check_carefully = find_best_partition(node, i);
			// It still can gain some info. from other attribute which isn't the 'selected_attr'.
			if(check_carefully.first != 0)
			{	
				pseudo_leaf = true;
				hidden_attr = i;
				break;
			}
		}
	}

	// Pseudo-leaf is a node that can only be partitioned by a certain attribute.
	if(pseudo_leaf == true)
	{
		// Split the node in reality.
		pair<struct node*, struct node*>partition_children = partition(node, hidden_attr, check_carefully.second);
		node->true_branch = partition_children.first;
		node->false_branch = partition_children. second;
		node->part_attr = node->data[0].value[hidden_attr].first;
		node->part_thres = check_carefully.second;

		// Recursively call the function 'build_tree'.
		build_tree(node->true_branch, attr_num);
		build_tree(node->false_branch, attr_num);
	}

	else
	{
		// If this node cannot gain any info. from any attribute, it means that it's really a leaf.
		if(next_partition.first == 0)
		{
			node->is_leaf = true;
			node->true_branch = NULL;
			node->false_branch = NULL;
			node->part_attr.clear();
			node->part_thres = 0;

			return ;
		}

		else
		{
			// Split the node in reality.
			pair<struct node*, struct node*>partition_children = partition(node, selected_attr, next_partition.second);
			node->true_branch = partition_children.first;
			node->false_branch = partition_children. second;
			node->part_attr = node->data[0].value[selected_attr].first;
			node->part_thres = next_partition.second;
			
			// Recursively call the function 'build_tree'.
			build_tree(node->true_branch, attr_num);
			build_tree(node->false_branch, attr_num);
		}
	}
}

void print_tree(struct node* node, string space)
{
	if( node->is_leaf == true )
	{
		vector<pair<string, int>> temp = class_counts(node);
		for(int i=0; i<(int)temp.size(); i++)
			cout<<space<<temp[i].first<<": "<<temp[i].second<<endl;
		if(node->true_branch!=NULL)cout<<"Wr";
		return;
	}

	// The meaning of pratition in this node
	cout<<space<<node->part_attr<<" >= "<<node->part_thres<<endl;

	// Recursively call the function 'print_tree()'.
	if(node->true_branch != NULL)
	{
		cout<<space<<"-->TRUE"<<endl;
		print_tree(node->true_branch, space + '\t');
	}
	
	if(node->false_branch != NULL)
	{
		cout<<space<<"-->FALSE"<<endl;
		print_tree(node->false_branch, space + '\t');
	}
}

float validation(struct node* train_root, vector<struct sample> valid_data)
{
	int correct_classified_num = 0;

	for(int i=0; i<(int)valid_data.size(); i++)
	{
		// The result of classification matches to the class attribute.
		if(classify_function(valid_data[i], train_root) == valid_data[i].class_attr)
			correct_classified_num++;
	}
	return (float)correct_classified_num / (float)valid_data.size();
}

string classify_function(struct sample sample, struct node* node)
{
	// 'const struct node* node' equals to an "iterator" moving on the trained decision tree;
	
	// When this "iterator" moves on a leaf of the trained decision tree, it means that we 
	// obtain the classification result of this sample.
	if( node->is_leaf == true )return node->data[0].class_attr;

	else
	{		
		int part_attr_index = 0;
		for(; part_attr_index<(int)sample.value.size(); part_attr_index++)
			if( sample.value[part_attr_index].first == node->part_attr )break;

		if( sample.value[part_attr_index].second >= node->part_thres )
			return classify_function(sample, node->true_branch);
		else return classify_function(sample, node->false_branch);
	}
}

struct node* read_iris(fstream& input_file)
{
	string one_line_input_string;
    struct node* root_node = new struct node;

    root_node->data.clear();
    root_node->parent = NULL;
    root_node->true_branch = NULL;
    root_node->false_branch = NULL;
    root_node->impurity = 1;
   	root_node->part_attr.clear();
	root_node->part_thres = 0;
	root_node->is_leaf = false;
    
	while( getline(input_file, one_line_input_string) )
	{
		vector<string> temp;
		struct sample sample_temp;

		split(one_line_input_string, temp, ',');
		
		// The number of attribute in "iris.txt" is 4.
		for(int i=0; i<4; i++)
		{
			string temmp("attr_");
			temmp = temmp + to_string(i);
			sample_temp.value.push_back( make_pair( temmp, stof(temp[i]) ) );
		}
		
		for(int i=0; i<(int)(temp[4].size()-1); i++)
		{
			sample_temp.class_attr = sample_temp.class_attr + temp[4][i];
		}

		root_node->data.push_back(sample_temp);
	}
	root_node->impurity = gini_impurity(root_node);
	return root_node;
}

struct node* read_optical(fstream& input_file)
{
	string one_line_input_string;
	//vector<vector<pair<string, float>>>dataset;
    struct node* root_node = new struct node;

    root_node->data.clear();
    root_node->parent = NULL;
    root_node->true_branch = NULL;
    root_node->false_branch = NULL;
    root_node->impurity = 1;
   	root_node->part_attr.clear();
	root_node->part_thres = 0;
	root_node->is_leaf = false;
    
	while( getline(input_file, one_line_input_string) )
	{
		vector<string> temp;
		struct sample sample_temp;

		split(one_line_input_string, temp, ',');
		
		// The number of attribute in "iris.txt" is 64.
		for(int i=0; i<64; i++)
		{
			string temmp("attr_");
			temmp = temmp + to_string(i);
			sample_temp.value.push_back( make_pair( temmp, stof(temp[i]) ) );
		}

		for(int i=0; i<(int)(temp[64].size()-1); i++)
		{
			sample_temp.class_attr = sample_temp.class_attr + temp[64][i];
		}

		root_node->data.push_back(sample_temp);
	}
	root_node->impurity = gini_impurity(root_node);
	return root_node;
}

struct node* read_cross200(fstream& input_file)
{
	string one_line_input_string;
    struct node* root_node = new struct node;

    root_node->data.clear();
    root_node->parent = NULL;
    root_node->true_branch = NULL;
    root_node->false_branch = NULL;
    root_node->impurity = 1;
   	root_node->part_attr.clear();
	root_node->part_thres = 0;
	root_node->is_leaf = false;
    
	while( getline(input_file, one_line_input_string) )
	{
		vector<string> temp;
		struct sample sample_temp;

		split(one_line_input_string, temp, ',');
		
		// The number of attribute in "iris.txt" is 2.
		for(int i=0; i<2; i++)
		{
			string temmp("attr_");
			temmp = temmp + to_string(i);
			sample_temp.value.push_back( make_pair( temmp, stof(temp[i]) ) );
		}
		
		// Get rid of the last char CR (0x0D).
		for(int i=0; i<(int)(temp[2].size()-1); i++)
		{
			sample_temp.class_attr = sample_temp.class_attr + temp[2][i];
		}

		root_node->data.push_back(sample_temp);
	}
	//root_node->impurity = gini_impurity(root_node);
	return root_node;
}

struct node* read_wine(fstream& input_file)
{
	string one_line_input_string;
    struct node* root_node = new struct node;

    root_node->data.clear();
    root_node->parent = NULL;
    root_node->true_branch = NULL;
    root_node->false_branch = NULL;
    root_node->impurity = 1;
   	root_node->part_attr.clear();
	root_node->part_thres = 0;
	root_node->is_leaf = false;
    
	while( getline(input_file, one_line_input_string) )
	{
		vector<string> temp;
		struct sample sample_temp;

		split(one_line_input_string, temp, ',');
		
		// The number of attribute in "winequality-red.txt" is 11.
		for(int i=0; i<11; i++)
		{
			string temmp("attr_");
			temmp = temmp + to_string(i);
			sample_temp.value.push_back( make_pair( temmp, stof(temp[i]) ) );
		}
		
		// Get rid of the last char CR (0x0D).
		for(int i=0; i<(int)(temp[11].size()-1); i++)
		{
			sample_temp.class_attr = sample_temp.class_attr + temp[11][i];
		}

		root_node->data.push_back(sample_temp);
	}
	//root_node->impurity = gini_impurity(root_node);
	return root_node;
}


struct node* read_file(fstream& input_file, int attr_num)
{
	string one_line_input_string;
	struct node* root_node = new struct node;

	root_node->data.clear();
	root_node->parent = NULL;
	root_node->true_branch = NULL;
	root_node->false_branch = NULL;
	root_node->impurity = 1;
	root_node->part_attr.clear();
	root_node->part_thres = 0;
	root_node->is_leaf = false;
    
	while( getline(input_file, one_line_input_string) )
	{
		vector<string> temp;
		struct sample sample_temp;

		split(one_line_input_string, temp, ',');
		
		for(int i=0; i<attr_num; i++)
		{
			string temmp("attr_");
			temmp = temmp + to_string(i);
			sample_temp.value.push_back( make_pair( temmp, stof(temp[i]) ) );
		}
		
		// Get rid of the last char CR (0x0D).
		for(int i=0; i<(int)(temp[attr_num].size()-1); i++)
			sample_temp.class_attr += temp[attr_num][i];

		root_node->data.push_back(sample_temp);
	}
	return root_node;
}