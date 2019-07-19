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

#include <string>
#include <math.h>
#include <fstream>
#include <thread>
#include "benchmark.hpp"

//Below lines have double space after #include to prevent makefile
//from generating dependency file because idk how to fix it any other way
//right now
#include  "symbolicc++.h"
#include  "exprtk.hpp" 

namespace par = pasl::sched::native;

double func(double x);
double calculateError(double lBound, double hBound, int n);
void allocateDom(double lBound, double hBound, int m, double*& bounds);
void searchMax(double lBound, double hBound, double deltaX, string stringExpression, double* max, double* root);
string fourthDerivatv();				//define function here aswell as in func() for now

//function whose area is being estimated defined here for now
//TODO: find a way for user to just defined function in cli 
string fourthDerivatv(){
	
	//Used to convert Sybolicc++ expression to string for the return
	stringstream buffer;
	string stringExpression;

	Symbolic x("x"), y;
	
	//FUNCTION NEEDS TO BE DEFINED HERE**************************************************************************
	y = (x^6) + sin(x);
	

	//Getting 4th derivative of the defined function
	for(int c = 1; c <= 4; c++){
		y = df(y, x); 
	}

	std::cout << "y(4) = " << y << std::endl;

	//Converting Symbolicc++ expression to a string for return
	buffer << y;
	buffer >> stringExpression;

	return stringExpression;
}
/*	allocateDom takes the lower bound, higher bound, the step size when
 *searching for maximum, and an array which will contain the bounds to 
 *where each fork will end and begin it's search for the local maximum. 
 *The way the array holds the lower and upper bounds of each fork's domain
 *of search is represented like this.
 *
 * search domain 1 - [bounds[0], bounds[1]]
 * search domain 2 - [bounds[2], bounds[3]]
 * search domain 3 - [bounds[4], bounds[5]]
 * search domain 4 - [bounds[6], bounds[7]]
 *
 * 	The purpose of allocateDom is to assignt correct bounds within
 * the array so the workload of finding the local max can be split up as evenly 
 * as possible. First allocateDom assigns the lower bound value to 
 * bounds[0]. The following for loop handles the the middle six indexes
 * within the bounds array. It does this by finding the value of x at 
 * every step that completes a quarter of the total steps. The size
 * of a quarter is decided using integer division so if your total number
 * of steps isn't divisble by four then the quarters will be slightly 
 * smaller than actual quarters. The high bound for the final search domain
 * is assigned to the high bound of the whole domain so compensate for this
 * integer divison meaning worst case the  4th search domain will be the
 * largest while the other 3 will always be the same.
 *
*/
void allocateDom(double lBound, double hBound, int m, double*& bounds){
	
	double stepThru = lBound, stepSize = (hBound - lBound)/m;
	int stepCount = 0;
	
	bounds[0] = stepThru;
	std::cout << "lBound assigned in allocDom" << std::endl;
	stepThru += stepSize;

	for(int index = 1; index < 7; stepThru += stepSize, stepCount++){
		
		if(stepCount == m / 4){
			std::cout << "Entered if" << std::endl;
			stepCount = 0;
			bounds[index] = stepThru;
			std::cout << "assigned index: " << index << std::endl;
			index++;
			bounds[index] = stepThru;
			std::cout << "assigned index: " << index << std::endl;
			index++;
			std::cout << "2nd index added" << std::endl;
		}	
	}

	bounds[7] = hBound;

}

//	F = The set of values of x that appear when stepping through the domain in deltaX sized steps
//		starting from lBound.
//
//	A function that takes the low and high bound of the domain where we are looking for the local
//maximum of as doubles, delta x, and the function/expression we are looking for the max of
//THE DIFFERENCE BETWEEN lBound AND hBound MUST BE EVEN DIVISIBLE BY deltaX OR THE FUNCTION'S RETURN
//WILL BE WRONG. The function parses the 'stringExpression' variable into a exprtk expression<double>
//object and finds the member of F that yields that largest value of f(x) by checking each member's value
//in a loop. The member of F that yeilds the largest value of f(x) is returned. 
void searchMax(double lBound, double hBound, double deltaX, string stringExpression, double* max, double* root){

	std::cout << "searchMax running..." << std::endl;

	double  x, y, expValHold;
	const double NEGINFINITE = -INFINITY;

	//Setting up exprtk to parse and eval function	
	exprtk::symbol_table<double> symbolTable;
	exprtk::expression<double> expression;
	exprtk::parser<double> expressionParser;
		
	symbolTable.add_variable("x", x);
	symbolTable.add_constants();
	
	expression.register_symbol_table(symbolTable);

	expressionParser.compile(stringExpression, expression);

	//TODO make exception if difference between lBound and 
	//hBound isn't evenly divisble by deltaX.
	//then put the follow loop in a try catch block.
	

	
        *max = NEGINFINITE;
	std::cout << "max initialized to "<< *max << std::endl;

	for(double x = lBound; x <= hBound; x += deltaX){
	std::cout << "for loop cycle started" << std::endl;
		if(expression.value() > *max){
			*max = expression.value(); 
			*root = x;	
			std::cout << "New max f(x) = " << *max << " found @ x = " << *root << std::endl;
		}	
	}

	*max = expression.value();	
	*max = abs(*max);	
	

	std::cout << "Max found to be " << *max << " @ " << *root << std::endl;
	std::cout << "searchMax finished!" << std::endl;
	
}

double calculateError(double lBound, double hBound, double n){ 

	int m;
       	double* subDoms = new double [8];
	double** maximums = new double* [4];
	double** rootXs = new double* [4];
	double factor,  deltaX, finalMax, finalRoot;
	string stringExpression = fourthDerivatv();

	//Begin search for maximum value given delta x	
	std::cout << "Enter divsor of the function's bounded " << std::endl;
	std::cout << "to search for local maximum domain" << std::endl;

	cin >> m;
	deltaX = (hBound - lBound)/m;

	allocateDom(lBound, hBound, m, subDoms);

	std::cout << "subDom array" << std::endl;
	for(int c = 0; c < 8; c++){
		std::cout << "[" << subDoms[c] << "]";
	}
	std::cout << std::endl;



	//Linear searching for  maximum values within each quadrant.
	par::fork2(
	[=, &maximums, &rootXs] {	
		std::cout << "fork 1 running..." << std::endl;	

		std::thread firstMax(searchMax, subDoms[0], subDoms[1], deltaX, stringExpression, maximums[0], rootXs[0]);
		std::thread secondMax(searchMax, subDoms[2], subDoms[3], deltaX, stringExpression, maximums[1], rootXs[1]);

		firstMax.join();
		secondMax.join();

		std::cout << "first 2 threads join finished" << std::endl;
	}, [=, &maximums, &rootXs] {	
		std::cout << "fork 2 running..." << std::endl;	

		std::thread thirdMax(searchMax, subDoms[4], subDoms[5], deltaX, stringExpression, maximums[2], rootXs[2]);	
		std::thread fourthMax(searchMax, subDoms[6], subDoms[7], deltaX, stringExpression, maximums[3], rootXs[3]);

		thirdMax.join();
		fourthMax.join();
		std::cout << "second 2 threads join finished" << std::endl;
	});
	
	//DEBUG: Testing allocation of maximums, roots, and subdoms.***
	
	std::cout << "Maximums: ";
	for(int c = 0; c < 4; c++){
		std::cout << "[" << maximums[c] << "]";
	}
	std::cout << std::endl;

	std::cout << "Roots: ";
	for(int c = 0; c < 4; c++){
		std::cout << "[" << rootXs[c] << "]";
	}
	std::cout << std::endl;

	std::cout << "Sub Domains:";
	for(int c = 0; c < 8; c++){
		std::cout << "[" << subDoms[c];
		std::cout << ", " << subDoms[++c] << "]";
	}
	std::cout << std::endl;
	
	//*************************************************************
	
	//Maximum Value within [a, b] of fourth deriv f(x) 
	//@ rootX so we assign to x to get value.	
	double max = 0;
	factor = pow((hBound - lBound), 5)/(180 * pow(n, 4));	

	//placeholder
	return max * factor;
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
		medianish();
		double factor, area, errorSpace;
		double *bounds = new double [8];		
		errorSpace = calculateError(a, b, n);


	
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
		std::cout << "error <= " << errorSpace << std::endl;
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
	return	pow(x, 6) + sin(x);
}
