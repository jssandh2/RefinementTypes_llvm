#include "../RefinementTypes.h"

unsigned int R_verify("__value > a")
dependentExample
(
	unsigned int a R_assume("__value != 4294967295")
)
{
	unsigned int r = a + 1;
	return r;
}
