#include<iostream>

using namespace std;
class Diamond
{
	public:
	int i,j;
		
		Diamond()
		{
			cout<< "\n In Diamond Constructor";
			this->i = 3;
			this->j = 5;
		}
		
		~Diamond()
		{
			cout<< "\n In Diamond Destructor";

		}
};

class Base1 : public virtual Diamond
{

	public:
		Base1()
		{
			cout<< "\n In Base1 Constructor";
		}

		~Base1()
		{
			cout<< "\n In Base1 Destructor";

		}
};

class Base2 : public virtual Diamond
{

	public:
		Base2()
		{
			cout<< "\n In Base2 Constructor";
		}

		~Base2()
		{
			cout<< "\n In Base2 Destructor";

		}
};

class Derived : public Base2, public Base1
{
	public:
		Derived()
		{
			cout<< "\n In Derived Constructor";
		}

		~Derived()
		{
			cout<< "\n In Derived Destructor";
		}
};

int main()
{
	Derived dobj;
	cout<< dobj.i ;
	return 0;
}
