#include <iostream>
#include <iomanip>

using namespace std;

int main()
{
	unsigned int in = 0;
	unsigned int not_in = 0;
	double reponse;


	for(int i = -32768; i < 32768; i++)
	{
		for(int j = -32768; j < 32768; j++)
		{
			if((i+j) < -32768 || (i+j) > 32767)
				not_in++;
			else
				in++;
		}
	}

	cout<<"reponse correct addition "<<in<<endl;
	cout<<"reponse incorrect addition "<<not_in<<endl;
	cout<<"pourcentage de reponse correct "<<in<<endl;
	return 0;


	for(int i = -32768; i < 32768; i++)
	{
		for(int j = -32768; j < 32768; j++)
		{
			if((i*j) < -32768 || (i*j) > 32767)
				not_in++;
			else
				in++;
		}
	}
	cout<<"reponse correct multiplication "<<in<<endl;
	cout<<"reponse incorrect multiplication "<<not_in<<endl;
	reponse = (in/(not_in + in)) * 100;
	cout<<"pourcentage de reponse correct "<<setprecision(4)<<reponse<<endl;

	return 0;
}