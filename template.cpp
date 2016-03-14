#include<iostream>

using namespace std;
template<class T> class Addition
{
	public:
	T add(T,T);
};

template<class T> T Addition<T> :: add(T no1, T no2)
{
	T ans;
	cout<< no1;
	cout<< no2;
	ans = no1 + no2;
	cout<<"------------------";
	cout<< ans;
	return ans;
}

int main()
{
	Addition<int> obj;
	obj.add(2,5);
	
	Addition<char> obj1;
	obj1.add('a','b');
	return 0;
}
