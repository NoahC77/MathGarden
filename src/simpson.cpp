/*	A program that calculates a simpson estimate
 *of the area between a curve and the x axis within 2 
 bounds set by the user using 'n' specified by the user.
 Unfortunately right now the function must be hard coded 
 into the 'func' function. This program uses PASL to
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

//Below lines have double space after #include to prevent makefile
//from generating dependency file because idk how to fix it any other way
//right now
#include  "symbolicc++.h"
#include  "exprtk.hpp" 

namespace par = pasl::sched::native;

double func(double x);
double calculateError(int lBound, int hBound, int n);

//When you come back change this function to a template function to fix it a bit.
double calculateError(int lBound, int hBound, int n){ 

	//Preparing exprtk
	exprtk::symbol_table<double> symbolTable;
	exprtk::expression<double> expression;
	exprtk::parser<double> parser;
	
	//Used to convert Sybolicc++ expression to string for exprtk
	stringstream buffer;
	string stringExpression;

	int m;
	double max = -INFINITY, x;

	Symbolic x("x"), y;
	//FUNCTION NEEDS TO BE DEFINED HERE
	y = (x^5) + 12;

	for(int c = 1; c < 4; c++){
		y = df(y, x); 
	}
	
	//Converting Symbolicc++ expression to a string for exprtk
	buffer << y;
	buffer >> stringExpression;

	symbol_table_double symbolTable;
	symbolTable.add_variable("x", x);
	symbolTable.add_constands();

	expression_double expression;
	expression.register_symbol_table(symbol_table);

	parser_double parser;
	parser.compile(stringExpression, expression);



}
	int main(int argc, char** argv){
	
/*The var a and b are just the bounds for the area which will
be calculated. So the area being calculated will be bounded 
between x = a, x = b, the x-axis, and the function specified 
in func(). The var sum is the sum of both sum1 and sum2, this 
is calculated once the program joins the fork.
 
 The var pSize is the portion size, I 
 called it portion, because fraction seemed 
 misleading. Later in the program a fork occurs.
 The fork is SPMD with the cores performing
 the series portion of the Simpson estimate. This
 pSize variable  is added to the x position which
 is then ran through the target function each cycle
 of the while loop. The var med is the median point
 within the bounds b and a. This determines where the
 2 summations end. The first summation goes from a
 to the median - the quotient of the width divided by
 n. The second summation goes from b, decrementing by
 q, to the median. 
 	The variables sum1 and sum2 are just the 
sum of the area estimation for each fork respectively.
*/
  
	double a, b, n, sum;
	double pSize, med;
	double sum1 = 0, sum2 = 0;
	
//	This lambda expression is determining the 
//amount we will increment each cycle for the first 
//fork and the amount we will decrement each cycle 
//for the second fork.
	auto portion = [&] {
		pSize = (b-a)/n;
	};
//	This lambda expression is just finding
//the median, I named it medianish, because I'm
//insecure about medians. :  ^)
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
		double factor, area;

		calculateError(a, b, n);


	
		par::fork2(
/*	The first half of the fork is used to to sum together
the first half of the Simpson series. This means doing this
part.
[f(x_0) + 4*f(x_1) + 2*f(x_2) +  ...]
This first half is more simple than the second half of this
series. This is because  in the series as you can see we 
alternate between multiplying the function by 4 and 2 after 
the inital function call. This means we know everytime we
do the series the first half will first multiply the 
function output by 4 and then alternate to 2 and so on and
so forth. The double x1 is used to step through the first
half of the domain. Each step is the quoteint we calculated
earlier. To avoid a wasteful if, I just added to intial function
call to the sum before we even start the while loop and just
added the quotient to the x1 stepper within it's initialization.
The bool timesFour is  used to alternate between multiplying the
function call by 4 and 2. timesFour is initialized to true, because
we know this is the first half of the series, we know we first 
multiply by 4 everytime.*/
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
/*	As stated earlier the first half was much less
fun than the first half. Remember the pattern
[f(x_0) + 4*f(x_1) + 2*f(x_2) + 4*f(x_3) + ...  f(x_n)]
Notice when n is odd the last factor the series uses is 4,
otherwise the last factor used is 2 before doing the final
function call. This is the purpose of the first if statement.
TODO THE CURRENT IF STATEMENT TRUNCATES THE DECIMAL WHICH
COULD CAUSE ERROR, LOOK FOR MATH.H FUNCTION WHICH CALCULATES
IF A FLOATING POINTT NUMBER IS EVEN.
The if statement determines if n is even, because we are 
started from the later bound and decrementing to the 
median, if n is even we first use 2 as the factor. If
n isn't even then we use 4 as the first factor. Other than
what was previosuly mentioned the second branch is relatively
similar to the first, except this just sums the second half 
of the series.*/


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
//Combines the 2 summations and multiplies by the factor
//previously determined.
		factor  = (b - a)/(3*n);
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
double func(double x){
	return x*x*x*x*x + 12;
}
