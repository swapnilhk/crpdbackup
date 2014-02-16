#include<stdio.h>
#include<math.h>
#include "../lib/lp_solve/lp_lib.h"
#include"taskstruct.h"

// Array to store the worst case response times of tasks
double R[10];

double min(double a, double b);
double solve_constraints(int this_task);
double PC(int this_task);
double sigma_tda(int this_task);
double wcrt(int this_task);
void set_difference(int a[],int b[], int n);
void var_no_to_name(const int this_task, const int var_no, char * var_name);
void itoa(int i, char s[10]);
void strrev(char * s);

void main(){
	int task_no;
	for(task_no = 1; task_no <= TOTAL_TASKS; task_no++){
		printf("---------------------------------------Task T%d---------------------------------------\n", task_no);
		wcrt(task_no);
		printf("***Task T%d Deadline: %0.2f Response Time: %0.2f => ", task_no, task[task_no].d, R[task_no]);
		if(R[task_no] > task[task_no].d){
			printf("NOT schedulable\n");
			break;
		}
		else printf("Scheulabel\n");
	}
}

// Returns worst case response time of task 'task_no'
double wcrt(int this_task){	
	double R_new;
	R_new = task[this_task].e;
	R[this_task] = 0;
	while(R_new != R[this_task]){
		printf("-------------------------------Iteration for Task T%d---------------------------------\n", this_task);
		R[this_task] = R_new;
		R_new = task[this_task].e + sigma_tda(this_task) + PC(this_task);// Time demand equation
		printf("Task T%d, Old Response time = %lf, New response time = %lf\n",this_task, R[this_task], R_new );
	}
	return R_new;
}

double sigma_tda(int this_task){
	double R_new = 0;
	int hp_task = this_task - 1;
	while(hp_task >= 1){
		R_new += ceil(R[this_task]/task[hp_task].p) * task[hp_task].e;
		hp_task = hp_task - 1;
	}
	return R_new;
}

// Returns preemption cost for task 'this_task'
double PC(int this_task){
	if (this_task >= 2){
		return solve_constraints(this_task);
	}
	else return 0;
}

double solve_constraints(int this_task){
	lprec *lp;
	int numVar = 0, *var = NULL, ret = 0, i, j, k, var_count, H, var_no;
	double *coeff = NULL, r, obj;
	char col_name[20];

	/* Creating a model */	
	for(i = 1;i < this_task; i++){/* Counting the no. of variables for this_task*/
		numVar+=pow(2,i)-1;
	}
	lp = make_lp(0, numVar);
	if(lp == NULL)
		ret = 1;
		
	if(ret == 0) {
		var_count = 1;
		for(i = 1 ; i <= numVar; i++){
			var_no_to_name(this_task, i, col_name);
			set_col_name(lp, var_count, col_name);
			var_count++;
		}
		/* create space large enough for one row(i.e. equation) */
		var = (int *) malloc(numVar * sizeof(*var));
		coeff = (double *) malloc(numVar * sizeof(*coeff));
		if((var == NULL) || (coeff == NULL))
			ret = 2;
	}
	
	/* add the equations to lpsolve */
	if(ret == 0) {
		set_add_rowmode(lp, TRUE);
		/* --------------------adding first constraint-------------------- */
		for(j = 2; j <= this_task; j++){
			var_count = 0;
			var_no = 1;
			for(i = 2; i <= j; i++){
				int end = var_no + pow(2,i-1) -1;
				for(; var_no < end; var_no++){
					var[var_count] = var_no;
					coeff[var_count] = 1;
					var_count++;
				}
			}
			r = 0;
			for(i = 1; i <= j -1; i++)
				r += ceil(R[this_task]/task[i].p);
			if(!add_constraintex(lp, var_count, coeff, var, LE, r))
				ret = 3;
		}
	}
	
	if(ret == 0) {
		/* --------------------adding second constraint------------------- */
		/* Example: For this task = T4, variables have been nubmered from 1 to 11 as follows:
		 * g2({T1}),g3({T1}),g3({T2}),g3({T1,T2}),g4({T1}),g4({T2}),g4({T1,T2}),g4({T3}),g4({T1,T3}),g4({T2,T3}),g4({T1,T2,T3})
		 */
		for(j = 2; j <= this_task; j++){
			int offset = 0;// offset => offset i.e. starting place
			double end;
			for(i = 1; i < j; i++){
				offset += pow(2,i-1) - 1;
			}
			end = offset + pow(2,j-1);// end => value we want ot reach to
			for(i = 1; i < j; i++){// All higher priority jobs
				int jump = pow(2,i-1);// jump => value we want ot jump to
				var_count = 0;
				for(var_no = offset + jump; var_no < end; var_no += jump){
					int end2 = var_no + jump;
					for(; var_no < end2; var_no++){//var_no reaching to end2
						var[var_count] = var_no;
						coeff[var_count] = 1;
						var_count++;
					}
				}
				r = min(ceil(R[this_task]/task[j].p)*ceil(R[j]/task[i].p),ceil(R[this_task]/task[i].p));
				if(!add_constraintex(lp, var_count, coeff, var, LE, r))
					ret = 3;
			}
		}
	}
	
	if(ret == 0) {
		/* --------------------adding constraint related to double-counting------------------- */	
		int size = 0;
		for(i = 1;i < this_task; i++){// Finding matrix size
			size += ceil((pow(2,i)-1)/2);
		}
		{
			int large_2darray[this_task-1][size];
			int hp_task;
			// Initializing matrix
			for(i = 0;i < this_task-1; i++){
				for(j = 0; j < size; j++){
					large_2darray[i][j] = 0;
				}
			}
			
			// Logic to generate sequence of var_no
			for(k = 1; k < this_task; k++){
				int jump = pow(2,k-1);
				int var_count = 0;
				int end;
				for(j = 2; j <= this_task; j++){
					int offset = 0;// offset => starting place
					for(i = 1; i < j; i++){
						offset += pow(2,i-1) - 1;
					}
					var_no = offset;
					end = offset + pow(2,j-1);				
					for(var_no += jump; var_no < end; var_no += jump){
						int end2 = var_no + jump;
						for(; var_no < end2; var_no++){//var_no reaching to jump
							large_2darray[k-1][var_count] = var_no;
							var_count++;
						}
					}
				}
			}
			// Algorithm for following section:
			// 1. Select next hp task if exists, otherwise exit loop
			// 2. Goto the corresponding index of matrix just generated thus selecting a row
			// 3. While next subset can be generated from the row selected, generate next subset, othervise goto 6
			// 4. Generate rhs depending on this task and the hp task slected and add equation to lp_solve
			// 5. Goto 3
			// 6. Goto 1
			for(hp_task = 1; hp_task < this_task; hp_task++){
				// Selecteding a row
				int * row = large_2darray[hp_task-1];
				// Creating subsets
				for(j = hp_task+1; j <= this_task; j++){
					int subset[size];
					int offset = 0;
					// offset : is a number, greater than which 'var_no's are to be selected
					for(i = 2; i < j; i++){
						offset += pow(2,i-1) - 1;
					}
					for(i = 0; i < size; i++){
						subset[i] = 0;
					}
					k = 0;
					for(i = 0; i < size && row[i] != 0; i++){
						if(row[i] > offset)
							subset[k++] = row[i];
					}
					for(i = j; i < this_task; i++){
						set_difference(subset, large_2darray[i-1], size);
					}
					// Now we have the final subset that can be added as an equation to lp_solve
					for(var_count = 0; subset[var_count] != 0 && var_count < size; var_count++){
						var[var_count] = subset[var_count];
						coeff[var_count] = 1;
					}
					r = ceil(R[this_task]/task[hp_task].p);
					if(!add_constraintex(lp, var_count, coeff, var, LE, r))
						ret = 3;
				}
			}
		}
	}
	
	set_add_rowmode(lp, FALSE);
	if(ret == 0) {
		/* -------------------adding objective function------------------- */
		var_count = 0;
		var_no = 1;
		for(j = 2; j <= this_task; j++){
			for(H = 1; H < pow(2,j-1); H++){
				var[var_count] = var_no;
				coeff[var_count] = f[j-2][H-1];
				var_count++;
				var_no++;
			}
		}
		if(!set_obj_fnex(lp, var_count, coeff, var))
			ret = 4;
		set_maxim(lp);
		write_LP(lp, stdout);
		set_verbose(lp,IMPORTANT);
		ret = solve(lp);
		if(ret == OPTIMAL)
			ret = 0;
		else
			ret = 5;
	}
	if(ret == 0) {
		obj = get_objective(lp);
		/* Displaying calculated values */		
		/* variable values */
		printf("/* Variable values */\n");
		get_variables(lp, coeff);
		for(j = 0; j < numVar; j++)
			printf("%s: %0.2f\n", get_col_name(lp, j + 1), coeff[j]);		
		/* objective value */
		printf("/* Objective value */\n%0.2f\n", obj);
	}
	if(ret != 0)
		printf("LP ERROR = %d\n", ret);
	
	/* free allocated memory */
	if(coeff != NULL)
		free(coeff);
	if(var != NULL)
		free(var);
	if(lp != NULL) 
		delete_lp(lp);

	return ret == 0 ? obj : 0;
}

double min(double a, double b){
	return a < b ? a : b;
}

/*Find set difference a - b. In place of elements that are removed, 
 *0 is inserted and then and then nonzero elements are shifted left*/
void set_difference(int a[],int b[], int n){
	int i = 0, j = 0;
	while(i < n && j < n && a[i] != 0 && b[j] != 0){
		if(a[i] == b[j]){
			a[i] = 0;
			i++;
			j++;
		}
		else if(a[i] < b[j])
			i++;
		else j++;
	}
	// Shifting non zero elements to the left
	i = 0; j = 0;
	while(j < n){
		if(a[j] != 0){
			a[i] = a[j];
			if(i != j)
				a[j] = 0;
			i++; j++;
		}
		else
			j++;
	}
}

/* Example: For this task = T4, variables have been nubmered from 1 to 11 as follows:
* g2({T1}),g3({T1}),g3({T2}),g3({T1,T2}),g4({T1}),g4({T2}),g4({T1,T2}),g4({T3}),g4({T1,T3}),g4({T2,T3}),g4({T1,T2,T3})
* Given a var_no, the following function creates variable accoeding to the above order
*/
void var_no_to_name(const int this_task, const int var_no, char * var_name){
	int i, lp_task = 2, offset = 0, hp_task;
	// Append this task nmae to var_name
	itoa(this_task, var_name);
	// Append Letter 'g' to var_name
	strcat(var_name,"g");	
	for(i = 1; i < var_no; i += pow(2,lp_task)-1, lp_task++)
		offset = i;
	// Append lp_task to var_name
	itoa(lp_task, &var_name[strlen(var_name)]);
	// Append opening brackets '({' to var_name
	strcat(var_name,"({");	
	// Genereting hp task set
	for(hp_task = 1; hp_task < this_task; hp_task++)
	{
		int jump = pow(2, hp_task-1);
		if(((var_no - offset) / jump) % 2 == 1){
			char temp[10];
			if(var_name[strlen(var_name)-1] != '{'){
				strcat(var_name, ",");
			}
			sprintf(temp, "T%d", hp_task);
			strcat(var_name, temp);
		}
	}	
	// Append closing brackets '})' to var_name
	strcat(var_name,"})");
}

void itoa(int i, char s[10]){
	int j = 0;
	do{
		s[j++] = '0' + i % 10;
	}while((i /= 10) != 0);
	s[j] = '\0';
	strrev(s);
}

void strrev(char * s){
	int len = strlen(s), j = 0;
	while(j < len/2){
		char temp = s[j];
		s[j] = s[len-1-j];
		s[len-1-j] = temp;
		j++;
	}
}