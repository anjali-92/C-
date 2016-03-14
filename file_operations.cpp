#include<fstream>
#include<iostream>
using namespace std;
class FILE_OPERATIONS
{
	public:

	fstream FIL_HNDLR;

        FILE_OPERATIONS()
	{
		cout<< "please pass file name to access";	
	}	

	FILE_OPERATIONS(char *file_name)
	{
		FIL_HNDLR.open(file_name);
		cout<< "\nfile handler";
		cout<< FIL_HNDLR ;
	}

	~FILE_OPERATIONS()
	{
		FIL_HNDLR.close();
	}
};

int main()
{
	FILE_OPERATIONS *fo = new FILE_OPERATIONS("anjali.txt");

	return 0;
}
