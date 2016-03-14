#include<stdio.h>


class Employee
{
	public:
		int salary;
		virtual void cal_sal();

	Employee()
	{
		salary = 1000;
	}
};

void Employee :: cal_sal()
{
	this->salary = this->salary + 1000;
	printf("\nEmployee  %d ", this->salary );
}



class Trainee : public Employee
{

	public:
		void cal_sal();
};

void Trainee :: cal_sal()
{
	this->salary = this->salary + 2000;
	printf("\nTrainee  %d ", this->salary );
}



class Associate : public Employee
{
	public:
		void cal_sal();
};

void Associate :: cal_sal()
{
	this->salary = this->salary + 3000;
	printf("\nAssociate %d ", this->salary );
}

int main()
{
	//Trainee tobj;
	Employee *emp = new Trainee();

	emp->cal_sal();
	return 0;
}
