/*	A program that calculates a simpson estimate
 *of the area between a curve and the x axis within 2 
 bounds set by the user using 'n' specified by the user.
 Unfortunately right now the function must be hard coded 
 into the curve function. This program uses PASL to
 split up the summation of the series used in 
 Simpon's Rule. Simpson's rule is the following.

**********************************************************
 The definite integral from a to b of the function f(x) 
 with respect to x can be estimated with the following.
 (b-a)/3n * [f(x_0) + 4f(x_1) + 2f(x_2) + 4f(x_3) + f(x_n)]
 *********************************************************
*/


#include <math.h>
#include <fstream>
#include "benchmark.hpp"

namespace par = pasl::sched::native;

static double func(double x);

int main(int argc, char** argv){
	
/*	The var a and b are just the bounds for
 the area which will be calculated. So the area
 being calculated will be bounded between x = a,
 x = b, the x-axis, and the function specified in
 func(). The var sum is the sum of both sum1 and 
 sum2, this is calculated once the program joins
 the fork.
 	The var pSize is the portion size, I 
 called it portion, because fraction seemed 
 misleading. Later in the program a fork occurs.
 The fork is SPMD with the cores performing
 the series portion of the Simpson estimate. This
 pSize variable  is added to the x position which
 is then ran through the target function each cycle
 of the while loop. The var med is the median point
 within the bound b and a. I JUST THOUGHT THIS COULD BE
 CAUSING AN ERROR BECAUSE I ASSUMED THE MEDIAN WOULD LAND
 ON A MULTIPLE OF PSIZE THIS SHOULD BE FIXED SOON.
 
 	The variables sum1 and sum2 are just the 
sum of the area estimation for each fork respectively.
*/
  
	double a, b, n, sum;
	double pSize, med;
	double sum1 = 0, sum2 = 0;

	auto portion = [&] {
		pSize = (b-a)/n;
	};

	auto medianish = [&] {
		med = a + (b-a)/2;
	};
	
//Just receiving input for the lower bound 
//, upper bound, and number of shapes. Has
//access to all outisde variables by reference.
	auto init = [&] {
		std::cout << "Enter the lower bound" << std::endl;
		std::cin >> a;

		std::cout << "Enter the upper bound" << std::endl;
		std::cin >> b;

		std::cout << "Enter 'n'" << std::endl;
		std::cin >> n;
			
	};

	auto run = [&] (bool sequential)  {
		portion();
		std::cout << "pSize = " << pSize << std::endl;
		medianish();
		std::cout << "median executed" << std::endl;
		double factor = (b - a)/(3*n), area;
	
		par::fork2(
		//smmation of the lower half of the series	
		[&] {
			double x1 = a + pSize;
			bool timesFour = true;
			sum1 += func(a);

			while(x1 < med){
				if(timesFour)
					sum1 += 4 * func(x1);
				else
					sum1 += 2 * func(x1);

				timesFour = !timesFour;
				x1 += pSize;
				

			}

		}, [&]
		{
			double x2 = b - pSize;
			sum2 += func(b);
			bool timesTwo;

			if((int)n % 2 == 0)
				timesTwo = true;
			else
				timesTwo = false;

			while(x2 >= med){
				if(timesTwo)
					sum2 += 2 * func(x2);
				else
					sum2 += 4 * func(x2);

				timesTwo = !timesTwo;
				x2 -= pSize;

			}
		});
		area = factor * (sum1 + sum2);	
		std::cout << "area = " << area << std::endl;
	};

	auto output = [&] {
		
	};

	auto destroy = [&] {
		;
	};

		
	pasl::sched::launch(argc, argv, init, run, output, destroy);
	return 0;
}

//For now this is the actual function we are estimating, hardcoded in for
//now, ik im sorry will make more alpha don't worry. Hopefully it
//will be cin'd in the future.
static double func(double x){
	return x*x;
}
