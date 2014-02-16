// Task structure
typedef struct{
	double p, e, d;
}TASK;

// Array of tasks
TASK task[] = {
	{0,0,0},// Do no remove this line. Add tasks below this line. Since task nos. start with 1
	{5,1,5},
	{30,1.5,30},
	{250,1.25,250},
	{500,0.5,500}
};

// Total tasks in the system
const int TOTAL_TASKS = sizeof(task)/sizeof(TASK) - 1;

// Preemption cost table
double f[][20] = {
				//	{T1}	{T2}	{T1,T2}		{T3}	{T1,T3}		{T2,T3}		{T1,T2,T3}	
	/*Task 2*/	{	0,		0,		0,			0,		0,			0,			0},
	/*Task 3*/	{	1,		0,		2,			0,		0,			0,			0},	
	/*Task 4*/	{	1,		0,		2,			0,		2,			0,			2}
};
