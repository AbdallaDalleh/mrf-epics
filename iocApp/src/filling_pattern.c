#include <aSubRecord.h>
#include <registryFunction.h>
#include <epicsExport.h>

#include <stdio.h>
#include <stdbool.h>

#define MAX_INPUTS		2
#define MAX_ITERATIONS	4
#define STEP			0.1

static long filling_pattern(aSubRecord* record)
{
	int i = 0;
	double   value;
	short*   enable;
	short*   iterations;
	double*  fill_delay;
	double** delays;
	double** base_delays;
	double** set_delays;

	enable      = (short*)    record->u;
	iterations  = (short*)    record->t;
	fill_delay  = (double*)   record->s;
	delays      = (double**) &record->a;
	base_delays = (double**) &record->k;
	set_delays  = (double**) &record->vala;

	static int count = 0;
	static short _iterations;
	static double _delay;
	if (count == 0)
	{
		_iterations = iterations[0];
		_delay = fill_delay[0];
	}

	for (i = 0; i < MAX_INPUTS; i++)
	{
		if (enable[0])
		{
			if (count == 0)
				value = base_delays[i][0];
			else
				value = delays[i][0] + _delay;
		}
		else
		{
			count = 0;
			value = base_delays[i][0];
		}

		set_delays[i][0] = value;
	}

	count++;
	if (count >= _iterations)
		count = 0;

	return 0;
}

epicsRegisterFunction(filling_pattern);

