struct Atom;

#include "test2.cpp"

struct Atom
{
	int foo;
};


int main()
{
	Atom *a = 0;
	Atom *b = 0;

	// directly comparing pointers
	if(a == b)
	{
	}
	
	if(a != b)
	{
	}

	if(a < b)
	{
	}

	if(a >= b)
	{
	}

	Atom atom = {};
	if(&atom == b)
	{

	}

	// arrays
	Atom atoms[256];
	if(&atoms[0] == &atoms[1])
	{
	}

	if(&atoms[42] == a)
	{
	}

	// nested comparisons
	if((a == b) || true)
	{
	}

	if(true && (true && (false && (&atom != b)) || true && false))
	{
	}

	// other files
	test_func(a, b);
}