//g++ -pthread Main.cpp

#include<stdio.h>
#include<malloc.h>
#include<string.h>
#include<pthread.h>
#include<stdlib.h>
#include<unistd.h>
#define Managed_Mem 2048


struct Used_Mem
{
char *U_start;
int U_size;
char **Address; //to change address after defragmentation
int ref;
struct Used_Mem *U_next;
};

struct Free_Mem
{
char *F_start;
int F_size;
struct Free_Mem *F_next;
};

struct Args_Struct
{
	struct Used_Mem **UL;
	struct Free_Mem **FL;
};

//===============================================================================
char *Initialize_Mem(struct Free_Mem **);
//Coalescing memory is done Mem_Free
void Mem_Free(char *,struct Used_Mem **,struct Free_Mem **);
void Display_Lists(struct Used_Mem *,struct Free_Mem *);
void Sort_Used_List(struct Used_Mem *,struct Used_Mem **);

//Memory management strategies 
//file Malloc_Free.cpp

//first fit strategy
void Mem_Alloc(int ,char **,struct Used_Mem **,struct Free_Mem **);
//Best fit strategy
void Best_Fit(int ,char **,struct Used_Mem **,struct Free_Mem **);
//Worst fit strategy
void Worst_Fit(int ,char **,struct Used_Mem **,struct Free_Mem **);

//Defragmentation
void Defragmentation(struct Used_Mem **,struct Free_Mem **);

//Garbage Collector
void Garbage_Collector(struct Used_Mem **,struct Free_Mem **);

//Thread to handle garbage Collector
void *Collect(void *);

//=================================================================================
///////////////////////////////////////////////////////////////////////////////////////////
//
//Function Name: Initialize_Mem
//
//Input: Free List
//
//Returns: char pointer
//
//Description
//This function allocates memory to be used as managed memory.
//
///////////////////////////////////////////////////////////////////////////////////////////

char *Initialize_Mem(struct Free_Mem **Free_List)
{	
	char *Memory = (char *)malloc(Managed_Mem);
	if(Memory == NULL)
		return 0;

	//Adding a node in free list
	struct Free_Mem *newnode = (struct Free_Mem *)malloc(sizeof(Free_Mem)*1);
	newnode->F_start = Memory;
	newnode->F_size = 2048;
	newnode->F_next = NULL;
	*Free_List = newnode ;
	
	//returing address of managed memory
	return Memory;
}

///////////////////////////////////////////////////////////////////////////////////////////
//
//Function Name: Mem_Alloc
//
//Input: 1.number of bytes to allocate
//       2.Address at which memory is allocated
//	  3.Used List
//	  4.Free List
//Returns: nothing
//
//Description
//This function allocates memory by first fit strategy
//
///////////////////////////////////////////////////////////////////////////////////////////
void Mem_Alloc(int size,char **Address,struct Used_Mem **Used_List,struct Free_Mem **Free_List)
{
	static int flag = 0;
	*Address = NULL;
	//If user has passed NULL arguments
	if(Used_List == NULL || Free_List == NULL)
	{
		return ;
	}
	//If demanded size is greater than Mangaed memory size
	if(size > Managed_Mem)
	{
		return ;
	}
	//if Free list is empty
	if(*Free_List == NULL)
	{
		printf("\n\nNo enough memory space is present in free list\n");
		return ;
	}
	
	//Temporary pointer to traverse Free List
	struct Free_Mem *temp1,*temp2 ;
	temp1 = temp2 = *Free_List;
	
	//Find memory of appropriate size
	while(temp1 != NULL)
	{
		if(temp1->F_size == size)
		{//if size equal to required size		
			struct Used_Mem *newnode = (struct Used_Mem *)malloc(sizeof(Used_Mem)*1);
				newnode->U_start = temp1->F_start;
				newnode->Address = Address;
				newnode->U_size = temp1->F_size;
				newnode->U_next = NULL;
				*Address = newnode->U_start;
				newnode->ref = 0;
			//allot memory and remove node/temp from free list
			if(*Used_List == NULL)
			{                                                 
				*Used_List = newnode ;
			}
			else
			{
				Sort_Used_List(newnode,Used_List);
			}
			//first entry in freelist while deleting manipulate free list pointer
			if( (size = (temp1->F_size)) && (size = (temp2->F_size)) && (temp1 == temp2))
			{
				*Free_List = temp1->F_next;
			}
			else //delete temp  at in middle,at end in free list
			{
				temp2->F_next = temp1->F_next;
			}
			free(temp1);

			//return (newnode->U_start);
		}  //memory of size greater thaan required
		else if(temp1->F_size > size)	      
		{//if size greater than required size
			struct Used_Mem *newnode = (struct Used_Mem *)malloc(sizeof(Used_Mem)*1);
			
			newnode->U_start = temp1->F_start;
			newnode->Address = Address;
			newnode->U_size = size;
			newnode->U_next = NULL;
			*Address = newnode->U_start;
			newnode->ref = 0;
			//add info in used list			
			if(*Used_List == NULL)
			{
				*Used_List = newnode ;
			}
			else
			{
				Sort_Used_List(newnode,Used_List);
			}				
			//add remaining bytes into free list
			temp1->F_start = temp1->F_start + size;
			temp1->F_size = (temp1->F_size) - size;
			//return (newnode->U_start);
		}
		temp2=temp1;
		temp1=temp1->F_next;
	}
	if(*Address == NULL)
	{
		if(flag == 0)
		{
			//even if after defragmentation need cannot be fulfilled return NULL
			flag = 1;
			Defragmentation(Used_List ,Free_List);
			Mem_Alloc(size,Address,Used_List,Free_List);
			return;
		}
		else
		{
			
			return;
		}
	}
return ;
}
///////////////////////////////////////////////////////////////////////////////////////////
//
//Function Name: Best_Fit
//
//Input: 1.number of bytes to allocate
//       2.Address at which memory is allocated
//	  3.Used List
//	  4.Free List
//Returns: nothing
//
//Description
//This function allocates memory by best fit strategy
//
///////////////////////////////////////////////////////////////////////////////////////////
void Best_Fit(int size,char **Address,struct Used_Mem **Used_List,struct Free_Mem **Free_List)
{
	static int flag = 0;
	//If user has passed NULL arguments
	if(Used_List == NULL || Free_List == NULL)
	{
		return ;
	}
	//If demanded size is greater than Mangaed memory size
	if(size > Managed_Mem)
	{
		*Address = NULL;
		return ;
	}
	//if Free list is empty
	
	if(*Free_List == NULL)
	{
		printf("\n\nNo enough memory space is present in free list\n");
		return ;
	}
	int Minimum_Size = 0;
	struct Free_Mem *temp1,*temp2,*temp3;
	//temp1		to traverse free list
	//temp2		to keep track of previous node as singly linked list
	//temp3		to maintain node with minimum size that satisfy need
	temp1 = temp2 = *Free_List;
	temp3 = NULL;
	//to find best memory size
	while(temp1 != NULL)
	{
		if((temp1->F_size) == size)
		{
			Minimum_Size = size;
			temp3 = temp1;
			/*temp3->F_start = temp1->F_start;
			temp3->F_size = temp1->F_size;
			temp3->F_next = temp1->F_next;*/
			break;
		}
		else if(temp1->F_size > size && Minimum_Size == 0)
		{
			//to assign minimum for first time
			temp3 = temp1;
			Minimum_Size = temp1->F_size;
		}
		else if(temp1->F_size > size && temp1->F_size < Minimum_Size)
		{
			temp3 = temp1;
			Minimum_Size = temp1->F_size;
		}
		temp2 = temp1;
		temp1 = temp1->F_next;
	}
	if(Minimum_Size == 0)
	{
		if(flag == 0)
		{
			flag = 1;
			Defragmentation(Used_List ,Free_List);
			Best_Fit(size,Address,Used_List,Free_List);
			return ;
		}
		else
		{
			*Address = NULL;
			return ;
		}
	}
	//if size is equal to minimum size
	if((temp3->F_size) == size)
		{
			//remove node from free list 
			if(temp1 == temp2)
			{
				*Free_List = temp1->F_next;
			}
			else
			{
			temp2->F_next = temp1->F_next;
			}
			//add into used list		
			struct Used_Mem *newnode = (struct Used_Mem *)malloc(sizeof(struct Used_Mem)*1);
				newnode->U_start = temp3->F_start;
				newnode->Address = Address;			
				newnode->U_size = temp3->F_size;
				newnode->U_next = NULL;
				*Address = newnode->U_start;
				newnode->ref = 0;			
				Sort_Used_List(newnode,Used_List);
				free(temp1);
				//return (newnode->U_start);
		}
	
	else if((temp3->F_size) > size)
	{
		//manage used list 
		struct Used_Mem *newnode = (struct Used_Mem *)malloc(sizeof(struct Used_Mem)*1);
			newnode->U_start = temp3->F_start;
			newnode->Address = Address;
			newnode->U_size = size;
			newnode->U_next = NULL;
			*Address = newnode->U_start;
			newnode->ref = 0;
			Sort_Used_List(newnode,Used_List); 
		
		
		//change values of free list
		temp3->F_start = temp3->F_start + size;
		temp3->F_size = temp3->F_size - size;
			//return (newnode->U_start);
	}
return ;
}
///////////////////////////////////////////////////////////////////////////////////////////
//
//Function Name: Worst_Fit
//
//Input: 1.number of bytes to allocate
//       2.Address at which memory is allocated
//	  3.Used List
//	  4.Free List
//Returns: nothing
//
//Description
//This function allocates memory by worst fit strategy
//
///////////////////////////////////////////////////////////////////////////////////////////
void Worst_Fit(int size,char **Address,struct Used_Mem **Used_List,struct Free_Mem **Free_List)
{
	static int flag = 0;
	//If user has passed NULL arguments
	if(Used_List == NULL || Free_List == NULL)
	{
		return ;
	}
	//If demanded size is greater than Mangaed memory size
	if(size > Managed_Mem)
	{
		*Address = NULL;
		return ;
	}
	//if Free list is empty
	if(*Free_List == NULL)
	{
		printf("\n\nNo enough memory space is present in free list\n");
		return ;
	}
	int Maximum_Size = 0;
	struct Free_Mem *temp1,*temp2,*temp3;
	//temp1		to traverse free list
	//temp2		to keep track of previous node as singly linked list
	//temp3		to maintain node with minimum size that satisfy need
	temp1=(*Free_List);
	temp2=(*Free_List);
	temp3 = NULL;
	//to find best memory size
	while(temp1 != NULL)
	{
		if(temp1->F_size >= size && Maximum_Size < temp1->F_size)
		{
			temp3 = temp1;
			Maximum_Size = temp1->F_size;

		}
		temp2 = temp1;
		temp1 = temp1->F_next;
	}
	
	if(Maximum_Size == 0)
	{
		if(flag == 0)
		{
			struct Used_Mem *newnode = (struct Used_Mem *)malloc(sizeof(struct Used_Mem)*1);
			flag = 1;
			Defragmentation(Used_List ,Free_List);
			Worst_Fit(size,Address,Used_List,Free_List);
			return;
			//return newnode->U_start;
		}
		if(flag == 1)
		{
			*Address = NULL;
			return;
		}

	}
    //manage used list 
    struct Used_Mem *newnode = (struct Used_Mem *)malloc(sizeof(struct Used_Mem)*1);
    newnode->U_start = temp3->F_start;
    newnode->Address = Address;
    newnode->U_size = size;
    newnode->U_next = NULL;
    *Address = newnode->U_start;
    newnode->ref = 0;
	Sort_Used_List(newnode,Used_List); 
    
    
    //change values of free list
	if((temp3->F_size - size) == 0)
	{
		*Free_List = temp3->F_next;
		free(temp3);
	}
	else
	{
		temp3->F_start = temp3->F_start + size;
		temp3->F_size = temp3->F_size - size;
	}
       // return (newnode->U_start);
}
///////////////////////////////////////////////////////////////////////////////////////////
//
//Function Name: Mem_Free
//
//Input: 1.Address at which memory is to be de-allocated 
//       2.Used List
//	  3.Free List
//Returns: nothing
//
//Description
//This function de-allocates memory.
//
///////////////////////////////////////////////////////////////////////////////////////////
void Mem_Free(char *address,struct Used_Mem **Used_List,struct Free_Mem **Free_List)
{
	if(*Used_List == NULL)
	{                                                 
		printf("\n\nNo memory is allocated at this address\n");
	}
	else
	{
		struct Used_Mem *temp1,*temp2;
		temp1 = temp2 = *Used_List;
		//to do sort list according to address
		while(temp1 != NULL) 
		{
			if(temp1->U_start == address)
				break;
			temp2 = temp1;
			temp1 = temp1->U_next;
		}
		if(temp1 != NULL)
		{
			//manage used list 
			if(temp1 == temp2)
			{
				//if only one element  NULL assignment
				*Used_List =temp1->U_next;
			}
			else
			{
				temp2->U_next = temp1->U_next;
			}
			
			//add this used memory node to free list
			//as temp1 was used list node copy contents to free list node
			struct Free_Mem *newnode = (struct Free_Mem *)malloc(sizeof(struct Free_Mem)*1);
			newnode->F_start = temp1->U_start;
			newnode->F_size = temp1->U_size;
			newnode->F_next = NULL;

			free(temp1);
			//sort free list && manage free list
			struct Free_Mem *temp3 ,*temp4;
			temp3 = temp4 = *Free_List;
			
			while(temp3 != NULL)
				{
					if((temp3 == temp4) && (newnode->F_start < temp3->F_start))
					{
						//coalescing code in start
						
						if( ((newnode->F_start)+(newnode->F_size)) == (*Free_List)->F_start  )
						{
							//printf("\nCoalecing");	
							newnode->F_size = newnode->F_size + temp3->F_size;
							newnode->F_next = temp3->F_next;
							free(temp3);
						}
						else
						{
						newnode->F_next = *Free_List;
						}
						*Free_List = newnode ;
						break;
					}
					else if(newnode->F_start < temp3->F_start )
					{
					/*	printf("\nnewnode->F_start :%u ",newnode->F_start);
						printf("\ntemp3->F_start :%u ",temp3->F_start);
						printf("\ntemp4->F_start :%u ",temp4->F_start);
				printf("\nnewnode->F_start)+(newnode->F_size : %u",((newnode->F_start)+(newnode->F_size)));*/
						//check upper and lower coalecing  temp4    newnode  temp3 
						//if upper coalacing
						if( ((newnode->F_start)-(temp4->F_size)) == (temp4->F_start) )
						{
							temp4->F_size = temp4->F_size + newnode->F_size;
							if( ((temp4->F_start) + (temp4->F_size)) ==  temp3->F_start)
							{
								temp4->F_size = temp4->F_size + temp3->F_size;
								temp4->F_next = temp3->F_next;
								free(temp3);
							}
							free(newnode);
						}
						else if(  ((newnode->F_start)+(newnode->F_size)) == (temp3->F_start) )
						{
						//else if lower coalacing
							temp3->F_size = temp3->F_size + newnode->F_size;
							temp3->F_start = newnode->F_start;
							if(temp4->F_start + temp4->F_size == temp3->F_start)
							{
								temp4->F_size =temp4->F_size + temp3->F_size;
								temp4->F_next = temp3->F_next;
								free(temp3);
							}
							free(newnode);
						}
						else
						{
						//else insert node
						newnode->F_next = temp4->F_next;
						temp4->F_next = newnode;
						}
						break;
					}
					temp4 = temp3;
					temp3 = temp3->F_next;
				}
				if(temp3 == NULL)
				{
					temp4->F_next = newnode;
				}
		}
		else
		{
			printf("\n\nNo memory is allocated at this address\n");
		}
	}
}
//=======================================================================================
///////////////////////////////////////////////////////////////////////////////////////////
//
//Function Name: Collect
//
//Input: address of structure object.
//
//Returns: nothing
//
//Description
//This function is invoked by thread.
//
///////////////////////////////////////////////////////////////////////////////////////////
void *Collect(void *arg)
{
	struct Args_Struct *temp=(struct Args_Struct *)arg;
	
	while(1)
	{
		sleep(5);
		//if Used list is empty
		if((*(temp->UL)) == NULL)
		{
			Collect(arg);
		}   
		Garbage_Collector(temp->UL,temp->FL);
	}
	pthread_exit((void *)0);
	
}
///////////////////////////////////////////////////////////////////////////////////////////
//
//Function Name: Garbage_Collector
//
//Input:1.Used List
//	 2.Free List
//
//Returns: nothing
//
//Description
//This function increments reference count and 
//frees the memory having refrence count greater than 3.
//
///////////////////////////////////////////////////////////////////////////////////////////
void Garbage_Collector(struct Used_Mem **Used_List,struct Free_Mem **Free_List)
{
	printf("\nIn Garbage collector");
	char *address=NULL;
	struct Used_Mem *Utemp,*Ftemp;
	Utemp = Ftemp = *Used_List;
	//increament refrence count of each node in used list
	  while(Utemp != NULL)
	  {
		  Utemp->ref = Utemp->ref + 1;
		  //if ref count equal to 3 remove that node from used list add it to Free List	
		  if(Utemp->ref == 3)
		  { 
			address = Utemp->U_start;
			Utemp = Utemp->U_next;
			Mem_Free(address,Used_List,Free_List);
		  }
		  else
		  {
			  Utemp = Utemp->U_next;
		  }
		  printf("\n--------------------------------------");
     		  Display_Lists(*Used_List,*Free_List);
  		  printf("\n--------------------------------------");
	  }
	
}
///////////////////////////////////////////////////////////////////////////////////////////
//
//Function Name: Defragmentation
//
//Input:1.Used List
//	 2.Free List
//
//Returns: nothing
//
//Description
//This function shifts the memory blocks to bring free memory together. 
//
///////////////////////////////////////////////////////////////////////////////////////////

void Defragmentation(struct Used_Mem **Used_List,struct Free_Mem **Free_List)
{
//while used list not equal to null
	struct Used_Mem *Utemp = *Used_List;
	struct Free_Mem *Ftemp1,*Ftemp2;
		Ftemp1 = Ftemp2 = *Free_List;
	while(Utemp != NULL)
	{
		if(Utemp->U_start > Ftemp1->F_start)
		{
			Utemp->U_start = (char *)memcpy(Ftemp1->F_start,Utemp->U_start,Utemp->U_size);
			*(Utemp->Address) = Utemp->U_start;
			Ftemp1->F_start = Ftemp1->F_start + Utemp->U_size;
			//collescing code
			if((Ftemp1 != NULL) && (Ftemp1->F_next != NULL))
			{			
				if( ((Ftemp1->F_start)+(Ftemp1->F_size)) == Ftemp1->F_next->F_start  )
				{   //change size of node
					Ftemp1->F_size = Ftemp1->F_size + Ftemp1->F_next->F_size;
					//change next of node
					Ftemp2 = Ftemp1->F_next;
					Ftemp1->F_next = Ftemp2->F_next;
					//free colleced node
					free(Ftemp2);
					Ftemp2=Ftemp1;
				}
			}

		}
		Utemp=Utemp->U_next;
	}

}
///////////////////////////////////////////////////////////////////////////////////////////
//
//Function Name: Defragmentation
//
//Input:1.Used List
//	 2.Free List
//
//Returns: nothing
//
//Description
//This function displays FreeList and Used List
//
///////////////////////////////////////////////////////////////////////////////////////////
void Display_Lists(struct Used_Mem *Used_List,struct Free_Mem *Free_List)
{
	printf("\nUsed Memory:");
	//used list is empty
	if(Used_List == NULL)
	{
		printf("Used_List is Empty");
	}	
	else
	{
		while(Used_List != NULL)
		{
			printf("|%p\t%d\t%d|",Used_List->U_start,Used_List->U_size,Used_List->ref);
			Used_List = Used_List->U_next;
		}
	}
	printf("\n\nFree Memory :");
	//free list is empty
	if(Free_List == NULL)
	{
		printf("Free list is Empty");
		return;
	}	
	else
	{
		while(Free_List != NULL)
		{
			printf("|%p\t%d|",Free_List->F_start,Free_List->F_size);
			Free_List = Free_List->F_next;
		}
	}
}
///////////////////////////////////////////////////////////////////////////////////////////
//
//Function Name: Sort_Used_List
//
//Input:1.Used List
//	 2.Free List
//
//Returns: nothing
//
//Description
//This function inserts the new used list node in used list.
//
///////////////////////////////////////////////////////////////////////////////////////////
void Sort_Used_List(struct Used_Mem *newnode,struct Used_Mem **Used_List)
{
	struct Used_Mem *temp1,*temp2;
	temp1 = temp2 = *Used_List;
	//to do sort list according to address
	if(temp1 == NULL && temp2 == NULL)
	{
		*Used_List = newnode;
		return;
	}
	while(temp1 != NULL)
	{
		if((temp1 == temp2) && (newnode->U_start < temp1->U_start))
		{//only one node
			newnode->U_next = *Used_List;
			*Used_List = newnode ;
			break;
		}
		else if(newnode->U_start < temp1->U_start )
		{
			newnode->U_next = temp2->U_next;
			temp2->U_next = newnode;
			break;
		}
		temp2 = temp1;
		temp1 = temp1->U_next;
	}
	if(temp1 == NULL)
	{
		temp2->U_next = newnode;
	}
}
//======================================================================================
///////////////////////////////////////////////////////////////////////////////////////////
//
//Function Name: main
//
//Input:nothing
//
//Returns: integer
//
//Description
//This function interacts with the user and starts a thread for garbage collection.
//
///////////////////////////////////////////////////////////////////////////////////////////
int main()
{
	struct Used_Mem *Used_List_Head = NULL;
	struct Free_Mem *Free_List_Head = NULL;
	struct Args_Struct obj;
	char *Mem = Initialize_Mem(&Free_List_Head);
	
	int iChoice,iSize;
	char *mem[100];
	//_flushall();

	obj.UL=&Used_List_Head;
	obj.FL=&Free_List_Head;
	pthread_t tid;
	int err =pthread_create(&tid,NULL,Collect,&obj);
	if(err != 0)
	{
		//err_quit("can't create thread :%s\n",strerror(err));
	}

	for(int i=0;i<100;i++)
	{	
		printf("Press 0 to exit");	
		printf("\nDo you want to alloacte or deallocate");
		printf("\nTo alloacte press 1 \nTo deallocate press 2\n");
		scanf("%d",&iChoice);
		switch(iChoice)
		{
		case 1:printf("\n\nBy which strategy to allocate\n1.First Fit\n2.Best Fit\n3.Worst Fit");
			iChoice =0;
			printf("\nEnter your choice");
			scanf("%d",&iChoice);
			printf("\nHow much memory do you want to allocate");
			scanf("%d",&iSize);

			switch(iChoice)
			{
			case 1:Mem_Alloc(iSize ,&mem[i],&Used_List_Head ,&Free_List_Head);
				//printf("\n%s\n",mem[i]);
				if(mem[i] == NULL)
				{
					i = i-1;
					break;
				}
				printf("\n%d Memory allocated at offset %d",iSize,i);
				printf("\n-------------First Fit---------------------");
				Display_Lists(Used_List_Head,Free_List_Head);
				printf("\n-------------------------------------------\n");				
				break;

			case 2:Best_Fit(iSize ,&mem[i],&Used_List_Head ,&Free_List_Head);
				if(mem[i] == NULL)
				{
					i = i-1;
					break;
				}
		  	    printf("\n%d Memory allocated at offset %d",iSize,i);
				printf("\n---------------Best Fit--------------------");
				Display_Lists(Used_List_Head,Free_List_Head);
				printf("\n-------------------------------------------\n");
				break;

			case 3:Worst_Fit(iSize ,&mem[i],&Used_List_Head ,&Free_List_Head);
				if(mem[i] == NULL)
				{
					i = i-1;
					break;
				}
		  	       printf("\n%d Memory allocated at offset %d",iSize,i);					
				printf("\n---------------Worst Fit--------------------");
				Display_Lists(Used_List_Head,Free_List_Head);
				printf("\n--------------------------------------------\n");
				break;
			}
			break;
		case 2:printf("\nEnter offset of memory to delete");
			iChoice =0;i=i-1;
			scanf("%d",&iChoice);
			Mem_Free(mem[iChoice],&Used_List_Head ,&Free_List_Head);
			printf("\n-----------Free------------------------");
			Display_Lists(Used_List_Head,Free_List_Head);
			printf("\n-----------------------------------\n");
			break;
	
		}
		if( iChoice == 0)
			break;

	}	
	pthread_join(tid,NULL);
	free(Mem);
	return 0;
}
