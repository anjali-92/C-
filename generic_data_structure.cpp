//typedef not possible with template syntax to be checked


#include<iostream>
using namespace std;

template <typename T>
struct sNode
{
	T data;
	struct sNode<T> *next;
};

template<class T> 
class GEN_SINGLY_LIST
{
	public:
	struct sNode<T> *head;
	struct sNode<T> *tail;
	
	GEN_SINGLY_LIST()
	{
		this->head = NULL;
		this->tail = NULL;
	}

	~GEN_SINGLY_LIST()
	{

	}

	struct sNode<T> *CreateNode(T data);
	void AddNodeSinglyList(struct sNode<T> *);
	void Display();
};

template <typename T>
void GEN_SINGLY_LIST<T> :: AddNodeSinglyList(struct sNode<T> *newNode)
{
	if( this->head == NULL )
	{
		this->head = newNode;
		return;
	}
	else
	{
		struct sNode<T> *temp;
		temp = this->head;
		while(temp->next != NULL)
		{
			temp = temp->next;
		}
		temp->next = newNode;		
	}
}

template <typename T>
struct sNode<T> *GEN_SINGLY_LIST<T> :: CreateNode(T data)
{
	struct sNode<T> *temp = new (struct sNode<T>);
	temp->data = data;
	temp->next = NULL ;
	return temp;
}
template <typename T>
void GEN_SINGLY_LIST<T> :: Display()
{
	struct sNode<T> *temp = this->head;
	while(temp != NULL )
	{
		cout<< temp->data;
		temp = temp->next;
	} 

	return ;
}
//cant take template type in main need to be specific with 
//type object is going to use
int main()
{
	GEN_SINGLY_LIST<int> obj;
	obj.AddNodeSinglyList( obj.CreateNode(5) );	
	obj.Display();
	return 0;
}
