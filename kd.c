#include<stdio.h>
#include<math.h>
#include "../lib/lp_solve/lp_lib.h"
#include "taskstruct.h"

// Arrays to store nnp_max and nnp_min
int nnp_max[10][10];
int nnp_min[10][10];

// Array to store the worst case response times of tasks
double R[10];

double min(double a, double b, double c);
double solve_constraints(int this_task);
double inv_max(int hp_task, int lp_task);
double inv_min(int hp_task, int lp_task);
double calc_nnp_max(int hp_task, int lp_task);
double calc_nnp_min(int hp_task, int lp_task);
double PC(int this_task);
double sigma_tda(int this_task);
double wcrt(int this_task);
double get_f(int this_task, int hp_task, int lp_task);

void main(){
	int task_no;
	for(task_no = 1; task_no <= TOTAL_TASKS; task_no++){
		wcrt(task_no);
		printf("%d - %0.2f", task_no, R[task_no]);
		if(R[task_no] > task[task_no].d){
			printf(" NOT schedulable\n");
			break;
		}
		else printf(" Scheulabel\n");
		printf("=====================================================\n\n");
	}
}

// Returns worst case response time of task 'tssk_no'
double wcrt(int this_task){	
	double R_new;
	R_new = task[this_task].e;
	R[this_task] = 0;
	while(R_new != R[this_task]){
		printf("TASK_NO =  %d\n\n", this_task);
		R[this_task] = R_new;		
		R_new = task[this_task].e + sigma_tda(this_task) + PC(this_task);// Time demand equation
		printf("R_old = %lf, R_new = %lf\n\n",R[this_task], R_new );
		printf("-----------------------------------------------------\n\n");
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
	int hp_task;
	if (this_task >= 2){
		for(hp_task = this_task-1; hp_task >= 1; hp_task--){
			nnp_max[hp_task][this_task] = calc_nnp_max(hp_task, this_task);
			nnp_min[hp_task][this_task] = calc_nnp_min(hp_task, this_task);
		}
		// Define constraints
		return solve_constraints(this_task);
	}
	else return 0;
}

double solve_constraints(int this_task)
{
	lprec *lp;
	int numVar = 0, *var = NULL, ret = 0, i, j, k, var_count;
	double *coeff = NULL, lhs,rhs, obj;
	char col_name[10];

	/* Creating a model */
	for(i = 1;i < this_task; i++)
		numVar+=i;	
	lp = make_lp(0, numVar);
	if(lp == NULL)
		ret = 1; /* Couldn't construct a new model */
		
	if(ret == 0) {
		var_count = 1;
		for(i = 1 ; i < this_task; i++){
			for(j = i+1 ; j <= this_task; j++)
			{
				sprintf(col_name, "%dNNP%d_%d", this_task, i, j);
				set_col_name(lp, var_count, col_name);
				var_count++;			
			}
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
		/* --------------------adding EQN-D-------------------- */
		for(j = 2;j <= this_task;j++){
			var_count = 0;
			for(i = 1; i < j; i++){
				sprintf(col_name,"%dNNP%d_%d",this_task, i, j);
				var[var_count] = get_nameindex(lp, col_name, FALSE);
				coeff[var_count] = 1;
				var_count++;
			}

			lhs= 0;
			for(i = 1; i < j; i++)
				lhs+= nnp_min[i][j];
			lhs*= floor(R[this_task]/task[j].p);			
			
			rhs = 0;
			for(i = 1; i < j; i++)
				rhs += nnp_max[i][j];
			rhs *= ceil(R[this_task]/task[j].p);
			
			if(!add_constraintex(lp, var_count, coeff, var, GE, lhs))
				ret = 3;
			if(!add_constraintex(lp, var_count, coeff, var, LE, rhs))
				ret = 3;			
		}
	}
	
	if(ret == 0) {	
		/* --------------------adding EQN-E-------------------- */
		for(k = 2;k <= this_task;k++)
		{			
			var_count = 0;
			for(j = 2; j <= k; j++){
				for(i = 1; i < j; i++){
					sprintf(col_name,"%dNNP%d_%d",this_task, i, j);
					var[var_count] = get_nameindex(lp, col_name, FALSE);
					coeff[var_count] = 1;
					var_count++;
				}
			}
			
			rhs = 0;
			for(i = 1; i < k; i++)
				rhs += ceil(R[this_task]/task[i].p);
			if(!add_constraintex(lp, var_count, coeff, var, LE,rhs))
				ret = 3;
		}
	}
	
	if(ret == 0) {
		/* ------------------adding EQN-G & H------------------ */
		for(j = 2; j <= this_task ; j++){
			for(i = 1; i < j; i++){
				lhs= floor(R[this_task]/task[j].p) * nnp_min[i][j];
				sprintf(col_name,"%dNNP%d_%d",this_task, i, j);
				var[0] = get_nameindex(lp, col_name, FALSE);
				coeff[0] = 1;
				if(!add_constraintex(lp, 1, coeff, var, GE, lhs))
					ret = 3;
				
				rhs = min(ceil(R[this_task]/task[i].p), ceil(R[this_task]/task[j].p) * ceil(R[j]/task[i].p), ceil(R[this_task]/task[j].p) * nnp_max[i][j]);
				if(!add_constraintex(lp, 1, coeff, var, LE,rhs))
					ret = 3;
			}
		}
	}
	
	if(ret == 0) {
 		/* --------------------adding EQN-I-------------------- */
		for(i = 1; i < this_task; i++){
			var_count = 0;
			for(j = i+1; j <= this_task; j++){
				sprintf(col_name,"%dNNP%d_%d",this_task, i, j);
				var[var_count] = get_nameindex(lp, col_name, FALSE);
				coeff[var_count] = 1;
				var_count++;				
			}
			rhs = ceil(R[this_task]/task[i].p);
			if(!add_constraintex(lp, var_count, coeff, var, LE,rhs))
				ret = 3;
		}
	}
		
	set_add_rowmode(lp, FALSE);
	if(ret == 0) {
		/* -----------------set the objective----------------- */
		var_count = 0;
		for(i = 1 ; i < this_task; i++){
			for(j = i+1 ; j<= this_task; j++){
				sprintf(col_name,"%dNNP%d_%d",this_task, i, j);
				var[var_count] = get_nameindex(lp, col_name, FALSE);
				coeff[var_count] = get_f(this_task, i, j);
				var_count++;
			}			
		}
		if(!set_obj_fnex(lp, var_count, coeff, var))
			ret = 4;
		set_maxim(lp);
		write_LP(lp, stdout);
		set_verbose(lp, IMPORTANT);
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
		printf("\nVariable values:\n");
		get_variables(lp, coeff);
		printf("\n");
		for(j = 0; j < numVar; j++)
			printf("%s: %f\n", get_col_name(lp, j + 1), coeff[j]);		
		/* objective value */
		printf("\nObjective value: %f\n\n", obj);
	}
	printf("LP ERROR = %d\n\n", ret);
	
	/* free allocated memory */
	if(coeff != NULL)
		free(coeff);
	if(var != NULL)
		free(var);
	if(lp != NULL) 		
		delete_lp(lp);
	
	return ret == 0 ? obj : 0;
}

double inv_max(int hp_task, int lp_task){
	return ceil(R[lp_task] / task[hp_task].p);
}

double inv_min(int hp_task, int lp_task){
	return floor(R[lp_task] / task[hp_task].p);
}

double calc_nnp_max(int hp_task, int lp_task){
	int i;	
	double ret_val = inv_max(hp_task, lp_task);
	for(i = hp_task + 1;i < lp_task; i++)
		ret_val -= nnp_min[hp_task][i] * inv_min(i, lp_task);
	return ret_val > 0 ? ret_val : 0;
}

double calc_nnp_min(int hp_task, int lp_task){
	int i;
	double ret_val = inv_min(hp_task, lp_task);
	for(i = hp_task + 1;i < lp_task; i++)
		ret_val -= nnp_max[hp_task][i] * inv_max(i, lp_task);
	return ret_val > 0 ? ret_val : 0;
}
	
double min(double a, double b, double c){
	return a < b ? (a < c ? a : c) : (b < c ? b : c);
}

double get_f(int this_task, int hp_task, int lp_task){
	if(this_task == lp_task)
		// Ignore lp_task
		return f[this_task-2][(int)pow(2,hp_task-1)-1];
	else
		return f[this_task-2][(int)(pow(2,hp_task-1)+pow(2,lp_task-1)-1)];
}
