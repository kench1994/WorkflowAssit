#include "client.hpp"


int main(void)
{
	for (int i = 0; i < 3; i++)
	{
		auto client_ptr = new client(i);
		client_ptr->connect("tcp://localhost:5557");
		client_ptr->send("This is django Test!");
	}
	getchar();
	return 0;
}