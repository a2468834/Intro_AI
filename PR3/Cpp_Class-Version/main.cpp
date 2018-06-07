#include "PR3.h"

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