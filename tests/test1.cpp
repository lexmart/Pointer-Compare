struct Atom
{
	int foo;
};

int main()
{
	Atom *a = 0;
	Atom *b = 0;

	// directly comparing pointers
	if(a == b) { }
	
	if(a != b) { }

	if(a < b) { }

	if(a >= b) { }

	Atom atom = {};
	if(&atom == b) { }

	// arrays
	Atom atoms[256];
	if(&atoms[0] == &atoms[1]) { }

	if(&atoms[42] == a) { }

	// macros
#define is_equal(a, b) (a == b)
	is_equal(a, b);

	// nested comparisons
	if((a == b) || true) { }

	bool bar = true && (true && (false && (&atom != b)) || true && false);

	// ternary
	bool test = (a == b) ? 1 : 0;
}