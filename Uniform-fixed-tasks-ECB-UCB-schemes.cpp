#include <cstdlib>
#include <iostream>
#include<math.h>
#include<set>

using namespace std;

// BASE CONFIG
// #define NUM_TASKS 15
// #define CACHE_SIZE 256
// #define RF 1
// double BRT = 8;

#define NUM_TASKS 10
#define CACHE_SIZE 256

#define NUM_TASK_SETS 1000

// Some form of Reuse Factor for Cache
#define RF 0.8

#define Total_ECBs_CU 10

// In MicroSecs
double BRT = 8;

// Number of Insns per Micro Second
double NIPMS = 0.04;

// to scale Task Periods
float multFactor;

double Gamma_i_j_ECB_Only[NUM_TASKS][NUM_TASKS];
double Gamma_i_j_UCB_Only[NUM_TASKS][NUM_TASKS];
double Gamma_i_j_UCB_Union[NUM_TASKS][NUM_TASKS];
double Gamma_i_j_ECB_Union[NUM_TASKS][NUM_TASKS];

double Gamma_i_j_Staschulat[NUM_TASKS][NUM_TASKS];
double Gamma_i_j_ECB_Union_Multiset[NUM_TASKS][NUM_TASKS];
double Gamma_i_j_UCB_Union_Multiset[NUM_TASKS][NUM_TASKS];

double Gamma_i_j_Staschulat_PRE[NUM_TASKS][NUM_TASKS];
double Gamma_i_j_ECB_Union_Multiset_PRE[NUM_TASKS][NUM_TASKS];
double Gamma_i_j_UCB_Union_Multiset_PRE[NUM_TASKS][NUM_TASKS];

double Gamma_i_j_PRE_MAX[NUM_TASKS][NUM_TASKS];

long Num_Displaced_Blocks_ECB_Only[NUM_TASKS];
long Num_Displaced_Blocks_UCB_Only[NUM_TASKS];
long Num_Displaced_Blocks_UCB_Union[NUM_TASKS];
long Num_Displaced_Blocks_ECB_Union[NUM_TASKS];

long Num_Displaced_Blocks_Staschulat[NUM_TASKS];
long Num_Displaced_Blocks_ECB_Union_Multiset[NUM_TASKS];
long Num_Displaced_Blocks_UCB_Union_Multiset[NUM_TASKS];
long Num_Displaced_Blocks_ECB_UCB_Union_Multiset_Combined[NUM_TASKS];

long Num_Displaced_Blocks_Staschulat_PRE[NUM_TASKS];
long Num_Displaced_Blocks_ECB_Union_Multiset_PRE[NUM_TASKS];
long Num_Displaced_Blocks_UCB_Union_Multiset_PRE[NUM_TASKS];
long Num_Displaced_Blocks_ECB_UCB_Union_Multiset_Combined_PRE[NUM_TASKS];

long Num_Displaced_Blocks_PRE_MAX[NUM_TASKS];

/* Input */

// Exec times Malardalen
//double Cycles[NUM_TASKS] = {445, 504, 1252, 1351, 6573, 13449, 17088, 22146, 29160, 39962, 43319, 214076, 290782, 742585, 1567222};
double Cycles[15] = {445, 504, 1252, 1351, 6573, 13449, 17088, 22146, 29160, 39962, 43319, 214076, 290782, 742585, 1567222};

double C[NUM_TASKS];

// double C[NUM_TASKS]; 

long   D[NUM_TASKS]; 
long   T[NUM_TASKS]; 
double taskSetUtil; 

double Response[NUM_TASKS] = {0, 0, 0, 0};

double Response_NO_PREEMPT[NUM_TASKS];
double Response_ECB_Only[NUM_TASKS], Response_UCB_Only[NUM_TASKS], Response_UCB_Union[NUM_TASKS], Response_ECB_Union[NUM_TASKS];
double Response_Staschulat[NUM_TASKS];
double Response_ECB_Union_Multiset[NUM_TASKS], Response_UCB_Union_Multiset[NUM_TASKS], Response_ECB_UCB_Union_Multiset_Combined[NUM_TASKS]; 

double Response_Staschulat_PRE[NUM_TASKS];
double Response_ECB_Union_Multiset_PRE[NUM_TASKS], Response_UCB_Union_Multiset_PRE[NUM_TASKS], Response_ECB_UCB_Union_Multiset_Combined_PRE[NUM_TASKS]; 
double Response_PRE_MAX[NUM_TASKS];

int        Cost[NUM_TASKS] = {2, 4, 6, 8};
int Useful_cost[NUM_TASKS] = {1, 2, 3, 4};

std::set<int> TASK_ECB[NUM_TASKS], TASK_UCB[NUM_TASKS];
int ECB_TASK_ARRAY[NUM_TASKS][CACHE_SIZE]; 
     
     /*int ECB_TASK_0[]= {1, 2};
     int ECB_TASK_1[] = {1, 2, 3, 4};
     int ECB_TASK_2[] = {1, 2, 3, 4};
     int ECB_TASK_3[] = {1, 2, 3, 4};
     */
     
     /*int UCB_TASK_0[] = {1, 2};
     int UCB_TASK_1[] = {3, 4};
     int UCB_TASK_2[] = {1, 2, 3};
     int UCB_TASK_3[] = {2, 3, 4};
     */

     // Malardalen benchmark
     //int SIZE_ECB_TASK[NUM_TASKS] = {35, 79, 24, 24, 41, 817, 151, 170, 105, 477, 64, 484, 144, 100, 62};
     //int SIZE_UCB_TASK[NUM_TASKS] = { 5,  9,  4,  5, 10,   4,  15,   15,  9,  14, 13,  14,  14,  23, 35};
     
     
     /*int SIZE_ECB_TASK[NUM_TASKS] = {35, 79, 24, 24, 41, 817, 151, 170, 105, 477, 64, 484, 144, 100, 62 }; 
     int SIZE_UCB_TASK[NUM_TASKS] = { 5,  9,  4,  5, 10,   4,  15,  15,   9,  14, 13,  14,  14,  23, 35 }; 
     */
     
     //int SIZE_ECB_TASK[15] = {35, 79, 24, 24, 41, 817, 151, 170, 105, 477, 64, 484, 144, 100, 62 }; 
     //int SIZE_UCB_TASK[15] = { 5,  9,  4,  5, 10,   4,  15,  15,   9,  14, 13,  14,  14,  23, 35 };
     
     int SIZE_ECB_TASK[NUM_TASKS]; 
     int SIZE_UCB_TASK[NUM_TASKS]; 
     
enum METHOD_NAMES { NO_PREEMPT, 
                    ECB_ONLY,
                    UCB_ONLY, 
                    UCB_UNION,
                    ECB_UNION, 
                    ECB_UNION_MULTISET,
                    UCB_UNION_MULTISET,
                    ECB_UCB_UNION_MULTISET_COMBINED,
                    ECB_UNION_MULTISET_PRE,
                    UCB_UNION_MULTISET_PRE,
                    ECB_UCB_UNION_MULTISET_COMBINED_PRE,          
                    STASCHULAT, 
                    STASCHULAT_PRE,
                    PRE_MAX,
                    NUM_METHODS
                 };
/* Output */

//float Response[NUM_TASKS] = {2, 5, 9, 20};

long PRE_ij_min[NUM_TASKS][NUM_TASKS], 
     PRE_ij_max[NUM_TASKS][NUM_TASKS];
    
long PRE_min[NUM_TASKS], PRE_max[NUM_TASKS];

int Num_Executed_Tasks[NUM_METHODS];

FILE* fp;

bool BDU_ONLY;
FILE* BDU_fp;

/* Auxilliary Functions */
void clear_Response()
{
     for(int i = 0; i < NUM_TASKS; i++)
     {
             Response[i] = 0;
     }
}


long INTR_max(int i, int j)
{
    return (long) ceil( Response[j] / T[i]);
}

long INTR_min(int i, int j)
{
    return (long) floor( Response[j] / T[i]);
}

void Reset_PRE_min()
{
     int i, j;
     
     for(i=0; i < NUM_TASKS; i++)
     {
              for(j=0; j < NUM_TASKS; j++)
              {
                       PRE_ij_min[i][j] = 0;
              }
              PRE_min[i] = 0;
     }
        
}


void Reset_PRE_max()
{
     int i, j;
     
     for(i=0; i < NUM_TASKS; i++)
     {
              for(j=0; j < NUM_TASKS; j++)
              {
                       PRE_ij_max[i][j] = 0;
              }
              PRE_max[i] = 0;  
     }
      
}


void SetPRE_min(int i)
{
    int j; 
    
    
    long acc = 0;
    
    for(j=0; j < i; j++)
    {
            acc += PRE_ij_min[j][i];
    }

    PRE_min[i] = acc;    

}


void  SetPRE_max(int i)
{
    int j; 
    
    
    long acc = 0;
    
    for(j=0; j < i; j++)
    {
            acc += PRE_ij_max[j][i];
    }

    PRE_max[i] = acc;    

}


long calculate_pre_max_ij( int i, int j)
{
    int k;
    long acc, ret_val;
    
    acc = 0; 
    
    for ( k = i+1; k < j; k++)
    {
        acc += PRE_ij_min[i][k] * INTR_min( k, j);   
    }
    
    ret_val = INTR_max(i, j) - acc;
    
    if (ret_val < 0)
       ret_val = 0;
    
    return ret_val;
    
}


long calculate_pre_min_ij( int i, int j)
{
    int k;
    long acc, ret_val;
    
    acc = 0; 
    
    for ( k = i+1; k < j; k++)
    {
        acc += PRE_ij_max[i][k] * INTR_max( k, j);   
    }
    
    ret_val = INTR_min(i, j) - acc;
    
    if (ret_val < 0)
       ret_val = 0;
    
    return ret_val;
    
}


/* Cost[k] here means 
   - blocks of Task-k, 
      - that may be useful to other tasks, hence their use by task-k will replace blocks of other task in execution
      - OR that will be useful to tasks of priority lower than k (and higher than task-i under consideration)
*/
     
void ResponseMethod1()
{
     int i, j, k;
     float acc;
     
     PRE_ij_min[0][0] = 0;
     PRE_ij_max[0][0] = 0;
     Response[0] = C[0];
     
     for (j = 1; j < NUM_TASKS; j++) 
     {
         int flag = 1; int num_iter = 0;
         
         Response[j] = C[j];
         
         do 
         {
            for (i = 0; i < j; i++)
            {
                 int pre_max_ij, pre_min_ij;
                 
                 float max_cost, min_cost;
             
                 pre_max_ij = calculate_pre_max_ij (i, j);
                 pre_min_ij = calculate_pre_min_ij (i, j); 
             
                 PRE_ij_max[i][j] = pre_max_ij;
                 PRE_ij_min[i][j] = pre_min_ij;
                 
                 printf("The pre_max_ij for i=%d j=%d is %d \n\n", i, j, pre_max_ij);
             }
         
             SetPRE_max(j);
             SetPRE_min(j);
             
             /* Use different algos to calculate costs here */
             /*max_cost = PRE_max[j] * Cost[j];
             min_cost = PRE_min[j] * Cost[j];
             */
             
            acc = C[j];
             
             for ( k = 0; k < j; k++)
             {
                 acc += INTR_max(k, j) * (C[k] + Cost[k]);
             }
             
             if (acc <=  Response[j])
                flag = 0;
             else
             {
                 printf("acc =%f, response = %f, going again \n\n", acc, Response[j]);
             }
             
             Response[j] = acc;
             
             num_iter++;
             
         } while (flag);
         
         printf("For task %d, number of iterations are %d, acc=%f, response = %f \n\n", j, num_iter, acc, Response[j]);   
         
     } /* for-j */
}

/* By using PRE_j as the number of pre-emptions faced by task-j.
 * Here, cost[j] determines the cost of one pre-emption of task-j
 * that is, nothing but the number of maximum useful blocks at any execution point in task-j
 */
void ResponseMethod2()
{
     int i, j, k;
     float acc, pre_acc;
     
     PRE_ij_min[0][0] = 0;
     PRE_ij_max[0][0] = 0;
     Response[0] = C[0];
     
     for (j = 1; j < NUM_TASKS; j++) 
     {
         int flag = 1; int num_iter = 0;
         
         Response[j] = C[j];
         
         do 
         {
            for (i = 0; i < j; i++)
            {
                 int pre_max_ij, pre_min_ij;
                 
                 float max_cost, min_cost;
             
                 pre_max_ij = calculate_pre_max_ij (i, j);
                 pre_min_ij = calculate_pre_min_ij (i, j); 
             
                 PRE_ij_max[i][j] = pre_max_ij;
                 PRE_ij_min[i][j] = pre_min_ij;
                 
                 printf("The pre_max_ij for i=%d j=%d is %d \n\n", i, j, pre_max_ij);
             }
         
             SetPRE_max(j);
             SetPRE_min(j);
             
             /* Use different algos to calculate costs here */
             /*max_cost = PRE_max[j] * Cost[j];
             min_cost = PRE_min[j] * Cost[j];
             */
             
            acc = C[j];
             
             for ( k = 0; k < j; k++)
             {
                 acc += INTR_max(k, j) * C[k] ;
             }
             
             pre_acc = 0;
             
             pre_acc += PRE_max[j] * Useful_cost[j];
             
             
             acc = acc + pre_acc;
             
             if (acc <=  Response[j])
                flag = 0;
             else
             {
                 printf("acc =%f, PRE_max[%d]= %d, pre_acc = %f, response = %f, going again \n\n", acc, j, PRE_max[j], pre_acc, Response[j]);
             }
             
             Response[j] = acc;
             
             num_iter++;
             
         } while (flag);
         
         printf("For task %d, number of iterations are %d, acc=%f, response = %f, pre_max[%d]=%d \n\n", j, num_iter, acc, Response[j], j, PRE_max[j]);
         
     }
}



void LeeMethod1()
{



}


void quickCalculate()
{
     int i, j;
     
     for (j = 0; j < NUM_TASKS; j++)
     {
            for (i = 0; i < j; i++)
            {
                 int pre_max_ij, pre_min_ij;
                 
                 float max_cost, min_cost;
             
                 pre_max_ij = calculate_pre_max_ij (i, j);
                 pre_min_ij = calculate_pre_min_ij (i, j); 
             
                 PRE_ij_max[i][j] = pre_max_ij;
                 PRE_ij_min[i][j] = pre_min_ij;
                 
                 printf("The pre_max_ij[%d][%d] =  %d \n\n", i, j, pre_max_ij);
                 printf("The pre_min_ij[%d][%d] =  %d \n\n", i, j, pre_min_ij);
             }
         
             SetPRE_max(j);
             SetPRE_min(j);
             
             printf("PRE_max_%d = %d \n\n", j, PRE_max[j]);
             printf("PRE_min_%d = %d \n\n", j, PRE_min[j]);
         }
}



void Set_Union(std::set<int> & Set1, std::set<int> & Set2, std::set<int> & Set3)
{
     
  std::set<int>::iterator it1, it2, it3;
  
  for(it1=Set1.begin(); it1!=Set1.end(); ++it1)
  {
       Set3.insert(*it1);
  } 
  
  for(it2=Set2.begin(); it2!=Set2.end(); ++it2)
  {
       Set3.insert(*it2);
  }  
     
     
}

void Set_Intersect(std::set<int> & Set1, std::set<int> & Set2, std::set<int> & Set3)
{
  std::set<int>::iterator it1, it2;
  
  for(it1=Set1.begin(); it1!=Set1.end(); ++it1)
  {
       it2 = Set2.find(*it1);
       
       if(it2 != Set2.end())
              Set3.insert(*it1);
  } 
     
}

void MultiSet_Intersect(std::multiset<int> & Set1, std::multiset<int> & Set2, std::multiset<int> & Set3)
{
     std::multiset<int>::iterator it1, it2;
     
     for(it1=Set1.begin(); it1!=Set1.end(); ++it1)
     {
       it2 = Set2.find(*it1);
       
       if(it2 != Set2.end())
       {       Set3.insert(*it1);
               Set2.erase(it2);
       }
     } 
}

int MultiSet_MOD(std::multiset<int> & Set1)
{
    return Set1.size();
}


int SET_MOD(std::set<int> & Set1)
{
    return Set1.size();
}



void print_SET(std::set<int> & Set1)
{
     std::set<int>::iterator it;
     
     for(it=Set1.begin(); it!= Set1.end(); ++it)
     {
         std::cout << ' ' << *it; 
         fprintf(fp, "%d ", *it);
     }
     
     std::cout << '\n';
     fprintf(fp, "\n");
     
}


int TestMultiSet()
{
  int i;  
  std::multiset<int> mymultiset;
  std::multiset<int>::iterator it;
  
  std::set<int> mySet, yourSet, ourSet;
  std::set<int>::iterator itSet;

  // set some initial values:
  for (int i=1; i<=5; i++) mymultiset.insert(i*10);  // 10 20 30 40 50

  it=mymultiset.insert(25);

  it=mymultiset.insert (it,27);    // max efficiency inserting
  it=mymultiset.insert (it,29);    // max efficiency inserting
  it=mymultiset.insert (it,24);    // no max efficiency inserting (24<29)

  int myints[]= {5,10,15};
  mymultiset.insert (myints,myints+3);


  for (int i=1; i<=5; i++) mymultiset.insert(i*10); 
  
  
  std::cout << "mymultiset contains:";
  for (it=mymultiset.begin(); it!=mymultiset.end(); ++it)
    std::cout << ' ' << *it;
  std::cout << '\n';
  
  
  cout << "Size of multiset is" << mymultiset.size() << endl ; 
  
  for (it=mymultiset.begin(); it!=mymultiset.end(); ++it)
  {
      mySet.insert(*it);
  }
  
  
  cout << "MySet is " << endl;
  for(itSet=mySet.begin(); itSet!=mySet.end(); ++itSet)
  {
      std::cout << ' ' << *itSet;
  }
  std::cout << '\n' << '\n';
  cout << "Size of MySet is" << mySet.size() << endl ; 
  
                  
  for(i=1; i<10; i++)
  {

     yourSet.insert(i*20);

  }   
  
  cout << "YourSet is " << endl;
  for(itSet=yourSet.begin(); itSet!=yourSet.end(); ++itSet)
  {
      std::cout << ' ' << *itSet;
  }
  std::cout << '\n' << '\n';
  cout << "Size of YourSet is" << yourSet.size() << endl ; 
     
  Set_Union(mySet, yourSet, ourSet);
  
  cout << "Union OurSet is " << endl;
  for(itSet=ourSet.begin(); itSet!=ourSet.end(); ++itSet)
  {
      std::cout << ' ' << *itSet;
  }
  std::cout << '\n' << '\n';
  cout << "Size of OurSet is" << ourSet.size() << endl ; 
  
  ourSet.clear();
  Set_Intersect(mySet, yourSet, ourSet);
  
  cout << "Intersection OurSet is " << endl;
  for(itSet=ourSet.begin(); itSet!=ourSet.end(); ++itSet)
  {
      std::cout << ' ' << *itSet;
  }
  std::cout << '\n' << '\n';
  cout << "Size of OurSet is" << SET_MOD(ourSet) << endl ;
  
  return 0;

}

void Response_time_NO_PREEMPT()
{
     int i, j, k;
     float acc;
     int flag;
     int LAST_TASK = NUM_TASKS-1;
     
     static bool BDU_found = false;
     
     bool sched = true;
     
     if (BDU_found == true)
     {
        fprintf(fp, "************** BreakDown Utilization already found for NO PREEMPT*********************\n \n");
        cout << "************** BreakDown Utilization already found for NO PREEMPT *********************" << endl << endl;
        
        if (BDU_ONLY)
           return;
     }
            
     clear_Response();
     
     fprintf(fp, "************** NO PREEMPT*********************\n \n");
     cout << "************** NO PREEMPT *********************" << endl << endl;
     
     Response[0] = C[0];
     Response_NO_PREEMPT[0] = Response[0];
     
     for (i = 1; (i < NUM_TASKS) && (sched==true); i++) 
     {
         flag = 1; int num_iter = 0;
         
         Response_NO_PREEMPT[i] = Response[i] = C[i];
         
         do 
         {
            
            acc = C[i];
            
             for ( j = 0; j < i; j++)
             {
                 acc += INTR_max(j, i) * C[j];               
             }
             
             
             if (acc <=  Response_NO_PREEMPT[i])
                flag = 0;
             else
             {
                 Response_NO_PREEMPT[i] = Response[i] = acc;
                 
                 if (Response[i] > D[i])
                 {
                     flag = 0;
                     sched = false;
                     LAST_TASK = i;
                 }         
             }
             
             num_iter++;
             
         } while (flag);
         
         printf("For task %d, number of iterations are %d, acc=%f, response = %f \n\n", i, num_iter, acc, Response_NO_PREEMPT[i]);  
         fprintf(fp, "For task %d, number of iterations are %d, acc=%f, response = %f \n\n", i, num_iter, acc, Response_NO_PREEMPT[i]);    
         
     } /* for-i */
     
     flag = 1;
     for(i = 0; i <= LAST_TASK; i++)
     {
           if ( Response[i] > D[i])
           {
                flag = 0;
                sched = false;
                printf("TASK %d NOT schedulable at TASKSET_UTIL = %f Response = %f Deadline = %ld \n", i, taskSetUtil, Response[i], D[i]);
                fprintf(fp, "TASK %d NOT schedulable at TASKSET_UTIL = %f Response = %f Deadline = %ld \n", i, taskSetUtil, Response[i], D[i]);
           }
           else
           {
               printf("TASK %d  IS schedulable at TASKSET_UTIL = %f Response = %f Deadline = %ld \n", i, taskSetUtil, Response[i], D[i]);
               fprintf(fp, "TASK %d IS schedulable at TASKSET_UTIL = %f Response = %f Deadline = %ld \n", i, taskSetUtil, Response[i], D[i]);
           }
     }
     if(sched ==false)
     {
             printf("\nTASKSET is NOT schedulable under NO PREEMPT at TASKSET_UTIL = %f \n", taskSetUtil);
             fprintf(fp, "\nTASKSET is NOT schedulable under NO PREEMPT at TASKSET_UTIL = %f \n", taskSetUtil);       
     }
     else
     {
             printf("\nTASKSET IS schedulable under NO PREEMPT at TASKSET_UTIL = %f \n", taskSetUtil);
             fprintf(fp, "\nTASKSET IS schedulable under NO PREEMPT at TASKSET_UTIL = %f \n", taskSetUtil);   
             
             Num_Executed_Tasks[NO_PREEMPT]++;
             /*if(BDU_found == false)
             {
                 BDU_found = true;
                 fprintf(BDU_fp, "\t\t NO PREEMPT \t\t\t\t %f \t\t %f \n\n", taskSetUtil, multFactor);
             }*/    
     }


     cout << "************** NO PREEMPT Ends *********************" << endl << endl;    
     fprintf(fp, "************* NO PREEMPT Ends ********************* \n\n");
     
}



void Response_time_ECB_Only()
{
     int i, j, k;
     double acc;
     int flag;
     int LAST_TASK = NUM_TASKS-1;
     
     static bool BDU_found = false;
     
     bool sched = true; 


     if (BDU_found == true)
     {
        fprintf(fp, "************** BreakDown Utilization already found for ECB Only*********************\n \n");
        cout << "************** BreakDown Utilization already found for ECB Only *********************" << endl << endl;
        
        if (BDU_ONLY)
           return;
     }
     
     clear_Response();
     
     fprintf(fp, "************** ECB Only *********************\n \n");
     cout << "************** ECB Only *********************" << endl << endl;
     
     Response_ECB_Only[0] = Response[0] = C[0];
     
     for (i = 1; (i < NUM_TASKS) && (sched == true); i++) 
     {
         flag = 1; int num_iter = 0;
         
         Response_ECB_Only[i] = Response[i] = C[i];
         
         do 
         {
            
            acc = C[i];
            
            Num_Displaced_Blocks_ECB_Only[i] = 0;
            
            for(int k = 0; k < NUM_TASKS; k++)
            {
                    Gamma_i_j_ECB_Only[k][i] = 0;
            }
             
             for ( j = 0; j < i; j++)
             {
                 acc += INTR_max(j, i) * (C[j] + (BRT * SET_MOD(TASK_ECB[j])) );
                 
                 Num_Displaced_Blocks_ECB_Only[i] +=  INTR_max(j, i) * SET_MOD(TASK_ECB[j]);
                 Gamma_i_j_ECB_Only[j][i] = INTR_max(j, i) * BRT * SET_MOD(TASK_ECB[j]); 
                 
                 /*cout << "INTR_MAX is " << INTR_max(k, j) << endl;
                 
                 cout << "C[k] is " << C[k] << endl; 
                 cout << "BRT is " << BRT << endl;
                 cout << "SET_MOD is " << SET_MOD(TASK_ECB[k]) << endl;
                 */
                 
                 // cout << "Adding " << INTR_max(k, j) * (C[k] + (BRT * SET_MOD(TASK_ECB[k])) ) << endl;
                 
             }
             
             
             if (acc <=  Response[i])
                flag = 0;
             else
             {
                 Response_ECB_Only[i] = Response[i] = acc;
                
                if (Response[i] > D[i])
                {
                    flag = 0;
                    sched = false;
                    LAST_TASK = i;
                }
             }
             
             num_iter++;
             
         } while (flag);
         
         printf("For task %d, number of iterations are %d, acc=%f, response = %f Num_Displaced_Blocks = %ld \n\n", i, num_iter, acc, Response_ECB_Only[i], Num_Displaced_Blocks_ECB_Only[i]);  
         fprintf(fp, "For task %d, number of iterations are %d, acc=%f, response = %f Num_Displaced_Blocks = %ld \n\n", i, num_iter, acc, Response_ECB_Only[i], Num_Displaced_Blocks_ECB_Only[i]);    
         
     } /* for-i */
     
     flag = 1;
     for(i = 0; i <= LAST_TASK; i++)
     {
           if ( Response[i] > D[i])
           {
                flag = 0;
                sched = false;
                printf("TASK %d NOT schedulable at TASKSET_UTIL = %f Response = %f Deadline = %ld \n", i, taskSetUtil, Response[i], D[i]);
                fprintf(fp, "TASK %d NOT schedulable at TASKSET_UTIL = %f Response = %f Deadline = %ld \n", i, taskSetUtil, Response[i], D[i]);
           }
           else
           {
               printf("TASK %d IS schedulable at TASKSET_UTIL = %f Response = %f Deadline = %ld \n", i, taskSetUtil, Response[i], D[i]);
               fprintf(fp, "TASK %d IS schedulable at TASKSET_UTIL = %f Response = %f Deadline = %ld \n", i, taskSetUtil, Response[i], D[i]);
           }
     }
     if(sched == false)
     {
             printf("\nTASKSET NOT schedulable under ECB Only at TASKSET_UTIL = %f \n", taskSetUtil);
             fprintf(fp, "\nTASKSET is NOT schedulable under ECB Only at TASKSET_UTIL = %f \n", taskSetUtil);       
     }
     else
     {
             printf("\nTASKSET IS schedulable under ECB Only at TASKSET_UTIL = %f \n", taskSetUtil);
             fprintf(fp, "\nTASKSET IS schedulable under ECB Only at TASKSET_UTIL = %f \n", taskSetUtil);
             
             Num_Executed_Tasks[ECB_ONLY]++;
             
             /*if(BDU_found == false)
             {
                 BDU_found = true;
                 fprintf(BDU_fp, "\t\t ECB Only \t\t\t\t %f \t\t %f \n\n", taskSetUtil, multFactor);
             }*/
     }
     

     cout << "************** ECB Only Ends *********************" << endl << endl;    
     fprintf(fp, "************* ECB Only Ends ********************* \n\n");
     
}


void Response_time_UCB_Only()
{
     int i, j, k;
     double acc;
     int flag;
     int LAST_TASK = NUM_TASKS - 1;
     
     static bool BDU_found = false;
     
     bool sched = true;
     
     if (BDU_found == true)
     {
        fprintf(fp, "************** BreakDown Utilization already found for UCB Only*********************\n \n");
        cout << "************** BreakDown Utilization already found for UCB Only*********************" << endl << endl;
        
        if (BDU_ONLY)
           return;
     }
     
     clear_Response();
     
     fprintf(fp, "************** UCB Only *********************\n \n");
     cout << "************** UCB Only *********************" << endl << endl;
     
     Response_UCB_Only[0] = Response[0] = C[0];
     
     for (i = 1; (i < NUM_TASKS) && (sched == true); i++) 
     {
         flag = 1; int num_iter = 0;
         
         Response_UCB_Only[i] = Response[i] = C[i];
         
         do 
         {
            
            acc = C[i];
            
            Num_Displaced_Blocks_UCB_Only[i] = 0;
            
            for(int k = 0; k < NUM_TASKS; k++)
            {
                    Gamma_i_j_UCB_Only[k][i] = 0;
            }
             
             for ( j = 0; j < i; j++)
             {
                 int Num_Blocks = 0;
                 
                 for (int k = j+1; k <=  i; k++)
                 {
                     if ( Num_Blocks < SET_MOD(TASK_UCB[k]) )
                        Num_Blocks = SET_MOD(TASK_UCB[k]) ;
                 } 
                 
                 acc += INTR_max(j, i) * (C[j] + (BRT * Num_Blocks) );
                 
                 Num_Displaced_Blocks_UCB_Only[i] +=  INTR_max(j, i) * Num_Blocks ;
                 Gamma_i_j_UCB_Only[j][i] = INTR_max(j, i) * BRT * Num_Blocks ; 
                 
             }
             
             
             if (acc <=  Response[i])
                flag = 0;
             else
             {
                 Response_UCB_Only[i] = Response[i] = acc;
                 if (Response [i] > D[i])
                 {
                     flag = 0;
                     sched = false;
                     LAST_TASK = i;
                 }
                 // printf("acc =%f, response = %f, going again \n\n", acc, Response[j]);
             }
             
             num_iter++;
             
         } while (flag);
         
         printf("For task %d, number of iterations are %d, acc=%f, response = %f Num_Displaced_Blocks = %ld \n\n", i, num_iter, acc, Response_UCB_Only[i], Num_Displaced_Blocks_UCB_Only[i]); 
         fprintf(fp, "For task %d, number of iterations are %d, acc=%f, response = %f Num_Displaced_Blocks = %ld \n\n", i, num_iter, acc, Response_UCB_Only[i], Num_Displaced_Blocks_UCB_Only[i]);     
         
     } /* for-i */
     
     flag = 1;
     for(i = 0; i <= LAST_TASK; i++)
     {
           if ( Response[i] > D[i])
           {
                flag = 0;
                sched = false;
                printf("TASK %d NOT schedulable at TASKSET_UTIL = %f Response = %f Deadline = %ld \n", i, taskSetUtil, Response[i], D[i]);
                fprintf(fp, "TASK %d NOT schedulable at TASKSET_UTIL = %f Response = %f Deadline = %ld \n", i, taskSetUtil, Response[i], D[i]);
           }
           else
           {
               printf("TASK %d IS schedulable at TASKSET_UTIL = %f Response = %f Deadline = %ld \n", i, taskSetUtil, Response[i], D[i]);
               fprintf(fp, "TASK %d IS schedulable at TASKSET_UTIL = %f Response = %f Deadline = %ld \n", i, taskSetUtil, Response[i], D[i]);
           }
     }
     if(sched == false)
     {
             printf("\nTASKSET is NOT schedulable under UCB Only at TASKSET_UTIL = %f \n", taskSetUtil);
             fprintf(fp, "\nTASKSET is NOT schedulable under UCB Only at TASKSET_UTIL = %f \n", taskSetUtil);       
     }
     else
     {
             printf("\nTASKSET IS schedulable under UCB Only at TASKSET_UTIL = %f \n", taskSetUtil);
             fprintf(fp, "\nTASKSET IS schedulable under UCB Only at TASKSET_UTIL = %f \n", taskSetUtil);
             
             Num_Executed_Tasks[UCB_ONLY]++;
             
             /*if(BDU_found == false)
             {
                 BDU_found = true;
                 fprintf(BDU_fp, "\t\t UCB Only \t\t\t\t %f \t\t %f \n\n", taskSetUtil, multFactor);
             }*/
     }
     
     cout << "************** UCB Only Ends *********************" << endl << endl;
     fprintf(fp, "************* UCB Only Ends ********************* \n\n");
     
     
}



void Response_time_UCB_Union()
{
     int i, j, k;
     double acc;
     int flag;
     int LAST_TASK = NUM_TASKS - 1;
     
     std::set<int> workingSet1, workingSet2;
     
     static bool BDU_found = false;
     
     bool sched = true;
     
     if (BDU_found == true)
     {
        fprintf(fp, "************** BreakDown Utilization already found for UCB Union*********************\n \n");
        cout << "************** BreakDown Utilization already found for UCB Union *********************" << endl << endl;
        
        if (BDU_ONLY)
           return;
     }
     
     
     clear_Response();
     
     fprintf(fp, "************** UCB Union ********************* \n\n" );
     cout << "************** UCB Union *********************" << endl << endl;
     
     Response_UCB_Union[0] = Response[0] = C[0];
     
     for (i = 1; (i < NUM_TASKS) && (sched==true); i++) 
     {
         flag = 1; int num_iter = 0;
         
         Response_UCB_Union[i] = Response[i] = C[i];
         
         do 
         {
            
            acc = C[i];
            
            Num_Displaced_Blocks_UCB_Union[i] = 0;
            
            for(int k = 0; k < NUM_TASKS; k++)
            {
                    Gamma_i_j_UCB_Union[k][i] = 0;
            }
             
             
             for ( j = 0; j < i; j++)
             {
                 int Num_Blocks = 0;
                 workingSet1.clear();
                 workingSet2.clear();
                 
                 // calculate Union UCB_aff_k_j
                 for (int k = j+1; k <=  i; k++)
                 {
                     std::set<int>::iterator it;
  
                     for(it=TASK_UCB[k].begin(); it!=TASK_UCB[k].end(); ++it)
                     {
                          workingSet1.insert(*it);
                     } 
                 }    
                 Set_Intersect(workingSet1, TASK_ECB[j], workingSet2);
                 Num_Blocks = SET_MOD(workingSet2);
                 
                 acc += INTR_max(j, i) * (C[j] + (BRT * Num_Blocks) );
                 
                 Num_Displaced_Blocks_UCB_Union[i] +=  INTR_max(j, i) * Num_Blocks ;
                 Gamma_i_j_UCB_Union[j][i] = INTR_max(j, i) * BRT * Num_Blocks ; 
                 
                 /*cout << "INTR_MAX is " << INTR_max(k, j) << endl;
                 
                 cout << "C[k] is " << C[k] << endl; 
                 cout << "BRT is " << BRT << endl;
                 cout << "SET_MOD is " << SET_MOD(TASK_ECB[k]) << endl;
                 */
                 
                 // cout << "Adding " << INTR_max(k, j) * (C[k] + (BRT * SET_MOD(TASK_ECB[k])) ) << endl;
                 
             }
             
             
             if (acc <=  Response[i])
                flag = 0;
             else
             {
                 Response_UCB_Union[i] = Response[i] = acc;
                 
                 if (Response[i] > D[i])
                 {
                     flag = 0; 
                     sched = false;
                     LAST_TASK = i;
                 }
                 // printf("acc =%f, response = %f, going again \n\n", acc, Response[j]);
             }
             
             num_iter++;
             
         } while (flag);
         
         printf("For task %d, number of iterations are %d, acc=%f, response = %f Num_Displaced_Blocks = %ld \n\n", i, num_iter, acc, Response_UCB_Union[i], Num_Displaced_Blocks_UCB_Union[i]); 
         fprintf(fp, "For task %d, number of iterations are %d, acc=%f, response = %f Num_Displaced_Blocks = %ld \n\n", i, num_iter, acc, Response_UCB_Union[i], Num_Displaced_Blocks_UCB_Union[i]);     
         
     } /* for-i */
     
     flag = 1;
     for(i = 0; i <= LAST_TASK; i++)
     {
           if ( Response[i] > D[i])
           {
                flag = 0;
                sched = false;
                printf("TASK %d NOT schedulable at TASKSET_UTIL = %f Response = %f Deadline = %ld \n", i, taskSetUtil, Response[i], D[i]);
                fprintf(fp, "TASK %d NOT schedulable at TASKSET_UTIL = %f Response = %f Deadline = %ld \n", i, taskSetUtil, Response[i], D[i]);
           }
           else
           {
               printf("TASK %d IS schedulable at TASKSET_UTIL = %f Response = %f Deadline = %ld \n", i, taskSetUtil, Response[i], D[i]);
               fprintf(fp, "TASK %d IS schedulable at TASKSET_UTIL = %f Response = %f Deadline = %ld \n", i, taskSetUtil, Response[i], D[i]);
           }
     }
     if(sched == false)
     {
             printf("\nTASKSET is NOT schedulable under UCB Union at TASKSET_UTIL = %f \n", taskSetUtil);
             fprintf(fp, "\nTASKSET is NOT schedulable under UCB Union at TASKSET_UTIL = %f \n", taskSetUtil);       
     }
     else
     {
             printf("\nTASKSET IS  schedulable under UCB Union at TASKSET_UTIL = %f \n", taskSetUtil);
             fprintf(fp, "\nTASKSET IS  schedulable under UCB Union at TASKSET_UTIL = %f \n", taskSetUtil); 
             
             Num_Executed_Tasks[UCB_UNION]++;
             
             /*if(BDU_found == false)
             {
                 BDU_found = true;
                 fprintf(BDU_fp, "\t\t UCB Union \t\t\t\t %f \t\t %f \n\n", taskSetUtil, multFactor);
             }*/
     }
     
     cout << "************** UCB Union Ends *********************" << endl << endl;
     fprintf(fp, "************** UCB Union Ends ********************* \n\n");
     
}

void Response_time_ECB_Union()
{
     int i, j, k;
     double acc;
     int flag;
     int LAST_TASK = NUM_TASKS - 1;
     
     std::set<int> workingSet1, workingSet2;
     
     static bool BDU_found = false;
     
     bool sched = true;
     
     if (BDU_found == true)
     {
        fprintf(fp, "************** BreakDown Utilization already found for ECB Union*********************\n \n");
        cout << "************** BreakDown Utilization already found for ECB Union *********************" << endl << endl;
        
        if (BDU_ONLY)
           return;
     }
     
     clear_Response();
     
     fprintf(fp, "************** ECB Union ********************* \n\n"); 
     cout << "************** ECB Union *********************" << endl << endl;
     
     Response_ECB_Union[0] = Response[0] = C[0];
     
     for (i = 1; (i < NUM_TASKS) && (sched == true); i++) 
     {
         flag = 1; int num_iter = 0;
         
         Response_ECB_Union[i] = Response[i] = C[i];
         
         do 
         {
            
            acc = C[i];
            
            Num_Displaced_Blocks_ECB_Union[i] = 0;
            
            for(int k = 0; k < NUM_TASKS; k++)
            {
                    Gamma_i_j_ECB_Union[k][i] = 0;
            }
             
             
             for ( j = 0; j < i; j++)
             {
                 int Num_Blocks = 0;
                 workingSet1.clear();
                 workingSet2.clear();
                 
                 // calculate Union_ECB
                 for (int h = 0; h <=  j; h++)
                 {
                     std::set<int>::iterator it;
  
                     for(it=TASK_ECB[h].begin(); it!=TASK_ECB[h].end(); ++it)
                     {
                          workingSet1.insert(*it);
                     } 
                 }    
                 
                 for (int k = j+1; k <= i; k++)
                 {
                     workingSet2.clear();
                     Set_Intersect(workingSet1, TASK_UCB[k], workingSet2);
                     if (Num_Blocks < SET_MOD(workingSet2))
                        Num_Blocks = SET_MOD(workingSet2);
                 }
                 
                 acc += INTR_max(j, i) * (C[j] + (BRT * Num_Blocks) );
                 
                 Num_Displaced_Blocks_ECB_Union[i] +=  INTR_max(j, i) * Num_Blocks ;
                 Gamma_i_j_ECB_Union[j][i] = INTR_max(j, i) * BRT * Num_Blocks ; 
             }
             
             if (acc <=  Response[i])
                flag = 0;
             else
             {
                 Response_ECB_Union[i] = Response[i] = acc;
                 
                 if (Response[i] > D[i])
                 {
                     flag = 0;
                     sched = false;
                     LAST_TASK = i;
                 }
                 // printf("acc =%f, response = %f, going again \n\n", acc, Response[j]);
             }
             
             num_iter++;
             
         } while (flag);
         
         printf("For task %d, number of iterations are %d, acc=%f, response = %f Num_Displaced_Blocks = %ld \n\n", i, num_iter, acc, Response_ECB_Union[i], Num_Displaced_Blocks_ECB_Union[i]); 
         fprintf(fp, "For task %d, number of iterations are %d, acc=%f, response = %f Num_Displaced_Blocks = %ld \n\n", i, num_iter, acc, Response_ECB_Union[i], Num_Displaced_Blocks_ECB_Union[i]);     
         
     } /* for-i */
     
     flag = 1;
     for(i = 0; i <= LAST_TASK; i++)
     {
           if ( Response[i] > D[i])
           {
                flag = 0;
                sched = false;
                printf("TASK %d NOT schedulable at TASKSET_UTIL = %f Response = %f Deadline = %ld \n", i, taskSetUtil, Response[i], D[i]);
                fprintf(fp, "TASK %d NOT schedulable at TASKSET_UTIL = %f Response = %f Deadline = %ld \n", i, taskSetUtil, Response[i], D[i]);
           }
           else
           {
               printf("TASK %d IS schedulable at TASKSET_UTIL = %f Response = %f Deadline = %ld \n", i, taskSetUtil, Response[i], D[i]);
               fprintf(fp, "TASK %d IS schedulable at TASKSET_UTIL = %f Response = %f Deadline = %ld \n", i, taskSetUtil, Response[i], D[i]);
           }
     }
     if(sched == false)
     {
             printf("\nTASKSET is NOT schedulable under ECB Union at TASKSET_UTIL = %f \n", taskSetUtil);
             fprintf(fp, "\nTASKSET is NOT schedulable under ECB Union at TASKSET_UTIL = %f \n", taskSetUtil);       
     }
     else
     {
             printf("\nTASKSET IS  schedulable under ECB Union at TASKSET_UTIL = %f \n", taskSetUtil);
             fprintf(fp, "\nTASKSET IS  schedulable under ECB Union at TASKSET_UTIL = %f \n", taskSetUtil); 
             
             Num_Executed_Tasks[ECB_UNION]++;
             
             /*if(BDU_found == false)
             {
                 BDU_found = true;
                 fprintf(BDU_fp, "\t\t ECB Union \t\t\t\t %f \t\t %f \n\n", taskSetUtil, multFactor);
             }*/
     }
     
     
     cout << "************** ECB Union Ends *********************" << endl << endl;
     fprintf(fp, "************** ECB Union Ends ********************* \n\n");
     
}

void Response_time_Staschulat()
{
     int i, j, k;
     double acc;
     int flag;
     int LAST_TASK = NUM_TASKS-1;
     
     std::set<int> workingSet1, workingSet2;
     
     static bool BDU_found = false;
     
     bool sched = true;
     
     if (BDU_found == true)
     {
        fprintf(fp, "************** BreakDown Utilization already found for Staschulat*********************\n \n");
        cout << "************** BreakDown Utilization already found for Staschulat *********************" << endl << endl;
        
        if (BDU_ONLY)
           return;
     }
     
     clear_Response();
     
     fprintf(fp, "************** Staschulat ********************* \n\n");
     cout << "************** Staschulat *********************" << endl << endl;
     
     Response_Staschulat[0] = Response[0] = C[0];
     
     for (i = 1; (i < NUM_TASKS) && (sched == true); i++) 
     {
         flag = 1; int num_iter = 0;
         
         Response_Staschulat[i] = Response[i] = C[i];
         
         do 
         { 
            int q; 
            std::multiset<int> M;
            std::multiset<int>::iterator it;
            
            acc = C[i];
            
            Num_Displaced_Blocks_Staschulat[i] = 0;
            
            for(k = 0; k < NUM_TASKS; k++)
            {
                    Gamma_i_j_Staschulat[k][i] = 0;
            }
             
             
             for ( j = 0; j < i; j++)
             {
                 
                 q=0;
                 for(k=j; k < i; k++)
                 {
                          q += INTR_max(k, i);
                 }
                 
                 M.clear();
                 for(k=j+1; k <= i; k++)
                 {
                     int EkRi = INTR_max(k, i);
                     workingSet1.clear();
                     it = M.begin();
                     for(int s=0; s< EkRi; s++)
                     {
                             int n = INTR_max(j, k);
                             Set_Intersect(TASK_UCB[k], TASK_ECB[j], workingSet1);
                             int val = SET_MOD(workingSet1);
                             for(int t = 0; t < n; t++)
                             {
                                     it = M.insert(val);
                             }
                     }
                 }
                 
                 int Num_Blocks = 0;
                 for(int l=0; l < q; l++)
                 {
                          int val = 0;
                          std::multiset<int>::iterator max_it = M.begin();
                           
                          for(it = M.begin(); it != M.end(); ++it)
                          {
                                 if (val < *it)
                                 {
                                    val = *it;
                                    max_it = it; 
                                 }                                         
                          }
                          M.erase(max_it);
                          
                          Num_Blocks += val;
                 }
                 
                 acc += (INTR_max(j, i) * C[j]) + (BRT * Num_Blocks);
                 
                 Num_Displaced_Blocks_Staschulat[i] +=  Num_Blocks ;
                 Gamma_i_j_Staschulat[j][i] = BRT * Num_Blocks ; 
                 
             }
             
             
             if (acc <=  Response[i])
                flag = 0;
             else
             {
                 Response_Staschulat[i] = Response[i] = acc;
                 
                 if (Response[i] > D[i]) 
                 {
                     flag = 0;
                     sched = false;
                     LAST_TASK = i;
                 // printf("acc =%f, response = %f, going again \n\n", acc, Response[j]);
                 }
             }
             
             num_iter++;
             
         } while (flag);
         
         printf("For task %d, number of iterations are %d, acc=%f, response = %f Num_Displaced_Blocks = %ld \n\n", i, num_iter, acc, Response_Staschulat[i], Num_Displaced_Blocks_Staschulat[i]);
         fprintf(fp, "For task %d, number of iterations are %d, acc=%f, response = %f Num_Displaced_Blocks = %ld \n\n", i, num_iter, acc, Response_Staschulat[i], Num_Displaced_Blocks_Staschulat[i]);      
         
     } /* for-i */
     
     flag = 1;
     for(i = 0; i <= LAST_TASK; i++)
     {
           if ( Response[i] > D[i])
           {
                flag = 0;
                sched = false;
                printf("TASK %d NOT schedulable at TASKSET_UTIL = %f Response = %f Deadline = %ld \n", i, taskSetUtil, Response[i], D[i]);
                fprintf(fp, "TASK %d NOT schedulable at TASKSET_UTIL = %f Response = %f Deadline = %ld \n", i, taskSetUtil, Response[i], D[i]);
           }
           else
           {
               printf("TASK %d IS schedulable at TASKSET_UTIL = %f Response = %f Deadline = %ld \n", i, taskSetUtil, Response[i], D[i]);
               fprintf(fp, "TASK %d IS schedulable at TASKSET_UTIL = %f Response = %f Deadline = %ld \n", i, taskSetUtil, Response[i], D[i]);
           }
     }
     if(sched == false)
     {
             printf("\nTASKSET NOT schedulable under STASCHULAT at TASKSET_UTIL = %f \n", taskSetUtil);
             fprintf(fp, "\nTASKSET is NOT schedulable under STASCHULAT at TASKSET_UTIL = %f \n", taskSetUtil);       
     }
     else
     {
             printf("\nTASKSET IS  schedulable under STASCHULAT at TASKSET_UTIL = %f \n", taskSetUtil);
             fprintf(fp, "\nTASKSET IS schedulable under STASCHULAT at TASKSET_UTIL = %f \n", taskSetUtil); 
             
             Num_Executed_Tasks[STASCHULAT]++;
             
             /*if(BDU_found == false)
             {
                 BDU_found = true;
                 fprintf(BDU_fp, "\t\t Staschulat \t\t\t\t %f \t\t %f \n\n", taskSetUtil, multFactor);
             }*/
     }
     
     cout << "************** Staschulat Ends *********************" << endl << endl;
     fprintf(fp, "************** Staschulat Ends ********************* \n\n");
     
     
}

void Response_time_ECB_Union_Multiset(bool display=true)
{
     int i, j, k;
     double acc;
     int flag;
     int LAST_TASK = NUM_TASKS - 1;
     
     std::set<int> workingSet1, workingSet2;
     
     static bool BDU_found = false;
     
     bool sched = true;

     if (BDU_found == true)
     {
        fprintf(fp, "************** BreakDown Utilization already found for ECB Union Multiset*********************\n \n");
        cout << "************** BreakDown Utilization already found for ECB Union Multiset *********************" << endl << endl;
        
        if (BDU_ONLY)
           return;
     }
     
     clear_Response();
     
     if (display)
     {
                 fprintf(fp, "************** ECB Union Multiset ********************* \n\n");
                 cout << "************** ECB Union Multiset *********************" << endl << endl;
     }
     
     Response_ECB_Union_Multiset[0] = Response[0] = C[0];
     
     for (i = 1; (i < NUM_TASKS) && (sched == true); i++) 
     {
         flag = 1; int num_iter = 0;
         
         Response_ECB_Union_Multiset[i] = Response[i] = C[i];
         
         do 
         { 
            int EjRi, EjRk, EkRi; 
            std::multiset<int> M;
            std::multiset<int>::iterator it;
            
            acc = C[i];
            
            Num_Displaced_Blocks_ECB_Union_Multiset[i] = 0;
            
            for(k = 0; k < NUM_TASKS; k++)
            {
                    Gamma_i_j_ECB_Union_Multiset[k][i] = 0;
            }
             
             
             for ( j = 0; j < i; j++)
             {
                 
                 EjRi = INTR_max(j, i);
                 
                 M.clear();
                 for(k=j+1; k <= i; k++)  // for-k-affinity
                 {
                     EjRk = INTR_max(j, k);
                     EkRi = INTR_max(k, i);
                     
                     workingSet1.clear();
                     it = M.begin();
                     
                     int numRepeat = EjRk * EkRi ;
                     for(int s=0; s < numRepeat; s++)
                     {
                             int val = 0;
                             workingSet1.clear();
                             workingSet2.clear();
                 
                             // calculate Union_ECB
                             for (int h = 0; h <=  j; h++)
                             {
                                 std::set<int>::iterator it;
  
                                 for(it=TASK_ECB[h].begin(); it!=TASK_ECB[h].end(); ++it)
                                 {
                                      workingSet1.insert(*it);
                                 } 
                             }    
                 
                             workingSet2.clear();
                             Set_Intersect(workingSet1, TASK_UCB[k], workingSet2);
                             val = SET_MOD(workingSet2);
                             M.insert(val);                       
                     } // for EjRk*EkRi
                 } // for-k-affinity
                 
                 int Num_Blocks = 0;
                 for(int l=0; l < EjRi; l++)
                 {
                          int val = 0;
                          std::multiset<int>::iterator max_it = M.begin();
                           
                          for(it = M.begin(); it != M.end(); ++it)
                          {
                                 if (val < *it)
                                 {
                                    val = *it;
                                    max_it = it; 
                                 }                                         
                          }
                          M.erase(max_it);
                          
                          Num_Blocks += val;
                 }
                 
                 acc += (INTR_max(j, i) * C[j]) + (BRT * Num_Blocks);
                 
                 Num_Displaced_Blocks_ECB_Union_Multiset[i] +=  Num_Blocks ;
                 Gamma_i_j_ECB_Union_Multiset[j][i] = BRT * Num_Blocks ; 
                                  
             }
             
             if (acc <=  Response[i])
                flag = 0;
             else
             {
                 Response_ECB_Union_Multiset[i] = Response[i] = acc;
                 
                 if (Response[i] > D[i])
                 {
                    flag = 0;
                    sched = false;
                    LAST_TASK = i;
                 }
                 // printf("acc =%f, response = %f, going again \n\n", acc, Response[j]);
             }
              
             num_iter++;
             
         } while (flag);
         
         if(display)
         {
                    printf("For task %d, number of iterations are %d, acc=%f, response = %f Num_Displaced_Blocks = %ld \n\n", i, num_iter, acc, Response_ECB_Union_Multiset[i], Num_Displaced_Blocks_ECB_Union_Multiset[i]); 
                    fprintf(fp, "For task %d, number of iterations are %d, acc=%f, response = %f Num_Displaced_Blocks = %ld \n\n", i, num_iter, acc, Response_ECB_Union_Multiset[i], Num_Displaced_Blocks_ECB_Union_Multiset[i]);     
         }
         
     } /* for-i */
     
     flag = 1;
     for(i = 0; i <= LAST_TASK; i++)
     {
           if ( Response[i] > D[i] || Response[i] == 0)
           {
                flag = 0;
                sched = false;
                
                if(display)
                {
                    printf("TASK %d NOT schedulable at TASKSET_UTIL = %f Response = %f Deadline = %ld \n", i, taskSetUtil, Response[i], D[i]);
                    fprintf(fp, "TASK %d NOT schedulable at TASKSET_UTIL = %f Response = %f Deadline = %ld \n", i, taskSetUtil, Response[i], D[i]);
                }
           }
           else
           {
               if(display)
               {
                    printf("TASK %d IS schedulable at TASKSET_UTIL = %f Response = %f Deadline = %ld \n", i, taskSetUtil, Response[i], D[i]);
                    fprintf(fp, "TASK %d IS schedulable at TASKSET_UTIL = %f Response = %f Deadline = %ld \n", i, taskSetUtil, Response[i], D[i]);
               }
           }
     }
     
     if (display)
     {
        if(sched == false)
        {
             printf("\nTASKSET is NOT schedulable under ECB-Union-MULTISET at TASKSET_UTIL = %f \n", taskSetUtil);
             fprintf(fp, "\nTASKSET is NOT schedulable under ECB-Union-MULTISET at TASKSET_UTIL = %f \n", taskSetUtil);       
        }
        else
        {
             printf("\nTASKSET IS schedulable under ECB-Union-MULTISET at TASKSET_UTIL = %f \n", taskSetUtil);
             fprintf(fp, "\nTASKSET IS  schedulable under-ECB-Union MULTISET at TASKSET_UTIL = %f \n", taskSetUtil); 
             
             Num_Executed_Tasks[ECB_UNION_MULTISET]++;
             
             /*if(BDU_found == false)
             {
                 BDU_found = true;
                 fprintf(BDU_fp, "\t\t ECB Union Multiset \t\t\t\t %f \t\t %f \n\n", taskSetUtil, multFactor);
             }*/
        }
        
        fprintf(fp, "************** ECB Union Multiset Ends ********************* \n\n" ); 
        cout << "************** ECB Union Multiset Ends *********************" << endl << endl;
     }
     
}

void Response_time_UCB_Union_Multiset(bool display=true)
{
     int i, j, k;
     double acc;
     int Num_Blocks;
     int flag;
     int LAST_TASK = NUM_TASKS - 1;
     
     std::set<int> workingSet1, workingSet2;
     std::set<int>::iterator it;
     
     static bool BDU_found = false;
     
     bool sched = true;
     
     if (BDU_found == true)
     {
        fprintf(fp, "************** BreakDown Utilization already found for UCB Union Multiset*********************\n \n");
        cout << "************** BreakDown Utilization already found for UCB Union Multiset *********************" << endl << endl;
        
        if (BDU_ONLY)
           return;
     }
     
     clear_Response();
     
     if (display)
     {    
          fprintf(fp, "************** UCB Union Multiset ********************* \n\n");
          cout << "************** UCB Union Multiset *********************" << endl << endl;
     }
     
     Response_UCB_Union_Multiset[0] = Response[0] = C[0];
     
     for (i = 1; (i < NUM_TASKS) && (sched == true); i++) 
     {
         flag = 1; int num_iter = 0;
         
         Response_UCB_Union_Multiset[i] = Response[i] = C[i];
         
         do 
         { 
            int EjRi, EjRk, EkRi; 
            std::multiset<int> M, M_ucb_ij, M_ecb_j;
            std::multiset<int>::iterator it1, it2;
            
            acc = C[i];
            
            Num_Displaced_Blocks_UCB_Union_Multiset[i] = 0;
            
            for(k = 0; k < NUM_TASKS; k++)
            {
                    Gamma_i_j_UCB_Union_Multiset[k][i] = 0;
            }
             
             
             for ( j = 0; j < i; j++)
             {
                 
                 EjRi = INTR_max(j, i);
                 
                 M_ucb_ij.clear();
                 for(k=j+1; k <= i; k++)  // for-k-affinity
                 {
                     EjRk = INTR_max(j, k);
                     EkRi = INTR_max(k, i);
                     
                     it1 = M_ucb_ij.begin();
                     
                     int numRepeat = EjRk * EkRi ;
                     for(int s=0; s < numRepeat; s++)
                     {
                 
                             for(it=TASK_UCB[k].begin(); it!=TASK_UCB[k].end(); ++it)
                             {
                                   M_ucb_ij.insert(*it);
                             }                        
                     } // for EjRk*EkRi
                 } // for-k-affinity
                 
                 M_ecb_j.clear();
                 for(int s=0; s < EjRi; s++)
                 {
                         for(it=TASK_ECB[j].begin(); it!=TASK_ECB[j].end(); ++it)
                         {
                                   M_ecb_j.insert(*it);
                         } 
                 }
                 
                 M.clear();
                 MultiSet_Intersect(M_ucb_ij, M_ecb_j, M);
                 
                 Num_Blocks = MultiSet_MOD(M);
                 
                 acc += (INTR_max(j, i) * C[j]) + (BRT * Num_Blocks);
                 
                 Num_Displaced_Blocks_UCB_Union_Multiset[i] +=  Num_Blocks ;
                 Gamma_i_j_UCB_Union_Multiset[j][i] = BRT * Num_Blocks ; 
                                
             }
             
             if (acc <=  Response[i])
                flag = 0;
             else
             {
                 Response_UCB_Union_Multiset[i] = Response[i] = acc;
                 
                 if(Response[i] > D[i])
                 {
                     flag = 0;
                     sched = false;
                     LAST_TASK = i;           
                 }
                 // printf("acc =%f, response = %f, going again \n\n", acc, Response[j]);
             }
              
             num_iter++;
             
         } while (flag);
             
         if (display)
         {
            printf("For task %d, number of iterations are %d, acc=%f, response = %f Num_Displaced_Blocks = %ld \n\n", i, num_iter, acc, Response_UCB_Union_Multiset[i], Num_Displaced_Blocks_UCB_Union_Multiset[i]); 
            fprintf(fp, "For task %d, number of iterations are %d, acc=%f, response = %f Num_Displaced_Blocks = %ld \n\n", i, num_iter, acc, Response_UCB_Union_Multiset[i], Num_Displaced_Blocks_UCB_Union_Multiset[i]);     
         }
         
     } /* for-i */
     
     flag = 1;
     for(i = 0; i <= LAST_TASK; i++)
     {
           if ( Response[i] > D[i] || Response[i] == 0)
           {
                flag = 0;
                sched = false;
                
                if(display)
                {
                   printf("TASK %d NOT schedulable at TASKSET_UTIL = %f Response = %f Deadline = %ld \n", i, taskSetUtil, Response[i], D[i]);
                   fprintf(fp, "TASK %d NOT schedulable at TASKSET_UTIL = %f Response = %f Deadline = %ld \n", i, taskSetUtil, Response[i], D[i]);
                }
           }
           else
           {
               if(display)
               {
                   printf("TASK %d IS schedulable at TASKSET_UTIL = %f Response = %f Deadline = %ld \n", i, taskSetUtil, Response[i], D[i]);
                   fprintf(fp, "TASK %d IS schedulable at TASKSET_UTIL = %f Response = %f Deadline = %ld \n", i, taskSetUtil, Response[i], D[i]);
               }
           }
     }
     
     if (display)
     {
        if(sched == false)
        {
             printf("\nTASKSET NOT schedulable under UCB-Union-MULTISET at TASKSET_UTIL = %f \n", taskSetUtil);
             fprintf(fp, "\nTASKSET NOT schedulable under UCB-Union-MULTISET at TASKSET_UTIL = %f \n", taskSetUtil);       
        }
        else
        {
             printf("\nTASKSET IS schedulable under UCB-Union-MULTISET at TASKSET_UTIL = %f \n", taskSetUtil);
             fprintf(fp, "\nTASKSET IS schedulable under-UCB-Union MULTISET at TASKSET_UTIL = %f \n", taskSetUtil); 
             
             Num_Executed_Tasks[UCB_UNION_MULTISET]++;
             
             /*if(BDU_found == false)
             {
                 BDU_found = true;
                 fprintf(BDU_fp, "\t\t UCB Union Multiset \t\t\t\t %f \t\t %f \n\n", taskSetUtil, multFactor);
             }*/
        }
        
        fprintf(fp, "************** UCB Union Multiset Ends ********************* \n\n" ); 
        cout << "************** UCB Union Multiset Ends *********************" << endl << endl;
     }
     
}

void Response_time_ECB_UCB_Union_Multiset_Combined()
{
     int flag;
     int LAST_TASK = NUM_TASKS - 1;
     
     static bool BDU_found = false;
     
     bool sched = true;
     
     if (BDU_found == true)
     {
        fprintf(fp, "************** BreakDown Utilization already found for ECB-UCB-Union-Multiset-Combined*********************\n \n");
        cout << "************** BreakDown Utilization already found for ECB Union Multiset-Combined*********************" << endl << endl;
        
        if (BDU_ONLY)
           return;
     }
     
     // Response_time_ECB_Union_Multiset(false);
     // Response_time_UCB_Union_Multiset(false);
     
     printf("************** ECB UCB Union Multiset Combined ********************* \n\n" );
     fprintf(fp, "************** ECB UCB Union Multiset Combined ********************* \n\n" );
     for (int i = 0; (i < NUM_TASKS) && (sched == true); i++)
     {
         if ( Response_ECB_Union_Multiset[i] < Response_UCB_Union_Multiset[i] && Response_ECB_Union_Multiset[i] != 0)
         {
             Response_ECB_UCB_Union_Multiset_Combined[i] = Response[i] = Response_ECB_Union_Multiset[i];
             Num_Displaced_Blocks_ECB_UCB_Union_Multiset_Combined[i] = Num_Displaced_Blocks_ECB_Union_Multiset[i];
         }
         else
         {
             Response_ECB_UCB_Union_Multiset_Combined[i] = Response[i] = Response_UCB_Union_Multiset[i];
             Num_Displaced_Blocks_ECB_UCB_Union_Multiset_Combined[i] = Num_Displaced_Blocks_UCB_Union_Multiset[i];
         }   
         
          if ( Response[i] > D[i] || Response[i] == 0)
           {
                sched = false;
                LAST_TASK = i;
           } 
         
         printf("For task %d, response = %f Num_Displaced_Blocks = %d \n\n", i, Response_ECB_UCB_Union_Multiset_Combined[i], Num_Displaced_Blocks_ECB_UCB_Union_Multiset_Combined[i]);
         fprintf(fp, "For task %d, response = %f Num_Displaced_Blocks = %d \n\n", i, Response_ECB_UCB_Union_Multiset_Combined[i], Num_Displaced_Blocks_ECB_UCB_Union_Multiset_Combined[i]);     
     }
     
     flag = 1;
     for(int i = 0; i <= LAST_TASK; i++)
     {
           if ( Response[i] > D[i] || Response[i] == 0)
           {
                flag = 0;
                sched = false;
                printf("TASK %d NOT schedulable at TASKSET_UTIL = %f Response = %f Deadline = %ld \n", i, taskSetUtil, Response[i], D[i]);
                fprintf(fp, "TASK %d NOT schedulable at TASKSET_UTIL = %f Response = %f Deadline = %ld \n", i, taskSetUtil, Response[i], D[i]);
           }
           else
           {
               printf("TASK %d IS schedulable at TASKSET_UTIL = %f Response = %f Deadline = %ld \n", i, taskSetUtil, Response[i], D[i]);
               fprintf(fp, "TASK %d IS schedulable at TASKSET_UTIL = %f Response = %f Deadline = %ld \n", i, taskSetUtil, Response[i], D[i]);
           }
     }
     if(sched==false)
     {
             printf("\nTASKSET is NOT schedulable under ECB-UCB-Union-MULTISET at TASKSET_UTIL = %f \n", taskSetUtil);
             fprintf(fp, "\nTASKSET is NOT schedulable under ECB-UCB-Union-MULTISET at TASKSET_UTIL = %f \n", taskSetUtil);       
     }
     else
     {
             printf("\nTASKSET IS  schedulable under ECB-UCB-Union-MULTISET at TASKSET_UTIL = %f \n", taskSetUtil);
             fprintf(fp, "\nTASKSET IS schedulable under-ECB-UCB-Union MULTISET at TASKSET_UTIL = %f \n", taskSetUtil);
             
             Num_Executed_Tasks[ECB_UCB_UNION_MULTISET_COMBINED]++; 
             
             /*if(BDU_found == false)
             {
                 BDU_found = true;
                 fprintf(BDU_fp, "\t\t ECB UCB Union Multiset Combined \t\t\t\t %f \t\t %f \n\n", taskSetUtil, multFactor);
             }*/
     }
     
     printf("************** ECB UCB Union Multiset Combined Ends ********************* \n\n" );
     fprintf(fp, "************** ECB UCB Union Multiset Combined Ends ********************* \n\n" );
     
}


void Response_time_Staschulat_PRE()
{
     int i, j, k;
     double acc;
     int flag;
     int LAST_TASK = NUM_TASKS - 1;
     
     std::set<int> workingSet1, workingSet2;
     
     static bool BDU_found = false;

     bool sched = true;
          
     if (BDU_found == true)
     {
        fprintf(fp, "************** BreakDown Utilization already found for Staschulat with PRE*********************\n \n");
        cout << "************** BreakDown Utilization already found for Staschulat with PRE *********************" << endl << endl;
        
        if (BDU_ONLY)
           return;
     }
     
     clear_Response();
     
     fprintf(fp, "************** Staschulat with PRE ********************* \n\n");
     printf("************** Staschulat with PRE ********************* \n\n");
     
     
     Response_Staschulat_PRE[0] = Response[0] = C[0];
     
     Reset_PRE_min();
     Reset_PRE_max();
     
     for (i = 1; (i < NUM_TASKS) && (sched == true); i++) 
     {
         flag = 1; int num_iter = 0;
         
         Response_Staschulat_PRE[i] = Response[i] = C[i];
         
         do 
         { 
            int q; 
            std::multiset<int> M;
            std::multiset<int>::iterator it;
            
            acc = C[i];
            
            Num_Displaced_Blocks_Staschulat_PRE[i] = 0;
            
            for(k = 0; k < NUM_TASKS; k++)
            {
                    Gamma_i_j_Staschulat_PRE[k][i] = 0;
            }
            
            // calculate PRE values
            for (j = 0; j < i; j++)
            {
                 int pre_max_ij, pre_min_ij;
                 
                 float max_cost, min_cost;
             
                 pre_max_ij = calculate_pre_max_ij (j, i);
                 pre_min_ij = calculate_pre_min_ij (j, i); 
             
                 PRE_ij_max[j][i] = pre_max_ij;
                 PRE_ij_min[j][i] = pre_min_ij;
                 
             }
             SetPRE_max(i);
             SetPRE_min(i);
             
             // calculate for higher priority tasks
             for ( j = 0; j < i; j++)
             {
                 
                 q=0;
                 for(k=j; k < i; k++)
                 {
                          q += PRE_ij_max[k][i];
                 }
                 
                 M.clear();
                 for(k=j+1; k <= i; k++)
                 {
                     int EkRi = INTR_max(k, i);
                     workingSet1.clear();
                     it = M.begin();
                     for(int s=0; s< EkRi; s++)
                     {
                             int n = PRE_ij_max[j][k];
                             Set_Intersect(TASK_UCB[k], TASK_ECB[j], workingSet1);
                             int val = SET_MOD(workingSet1);
                             for(int t = 0; t < n; t++)
                             {
                                     it = M.insert(val);
                             }
                     }
                 }
                 
                 int Num_Blocks = 0;
                 for(int l=0; l < q; l++)
                 {
                          int val = 0;
                          std::multiset<int>::iterator max_it = M.begin();
                           
                          for(it = M.begin(); it != M.end(); ++it)
                          {
                                 if (val < *it)
                                 {
                                    val = *it;
                                    max_it = it; 
                                 }                                         
                          }
                          M.erase(max_it);
                          
                          Num_Blocks += val;
                 }
                 
                 acc += (INTR_max(j, i) * C[j]) + (BRT * Num_Blocks);
                 
                 Num_Displaced_Blocks_Staschulat_PRE[i] +=  Num_Blocks ;
                 Gamma_i_j_Staschulat_PRE[j][i] = BRT * Num_Blocks ; 
                 
             }
             
             
             if (acc <=  Response[i])
                flag = 0;
             else
             {
                 Response_Staschulat_PRE[i] = Response[i] = acc;
                 
                 if (Response[i] > D[i])
                 {
                    flag = 0;
                    sched = false;
                    LAST_TASK = i;
                 }
                 // printf("acc =%f, response = %f, going again \n\n", acc, Response[j]);
             }
             
             num_iter++;
             
         } while (flag);
         
         printf("For task %d, number of iterations are %d, acc=%f, response = %f Num_Displaced_Blocks = %ld \n\n", i, num_iter, acc, Response_Staschulat_PRE[i], Num_Displaced_Blocks_Staschulat_PRE[i]);
         fprintf(fp, "For task %d, number of iterations are %d, acc=%f, response = %f Num_Displaced_Blocks = %ld \n\n", i, num_iter, acc, Response_Staschulat_PRE[i], Num_Displaced_Blocks_Staschulat_PRE[i]);      
         
     } // for-i 
     
     
     flag = 1;
     for(i = 0; i <= LAST_TASK; i++)
     {
           if ( Response[i] > D[i])
           {
                flag = 0;
                sched = false;
                printf("TASK %d NOT schedulable at TASKSET_UTIL = %f Response = %f Deadline = %ld \n", i, taskSetUtil, Response[i], D[i]);
                fprintf(fp, "TASK %d NOT schedulable at TASKSET_UTIL = %f Response = %f Deadline = %ld \n", i, taskSetUtil, Response[i], D[i]);
           }
           else
           {
               printf("TASK %d IS schedulable at TASKSET_UTIL = %f Response = %f Deadline = %ld \n", i, taskSetUtil, Response[i], D[i]);
               fprintf(fp, "TASK %d IS schedulable at TASKSET_UTIL = %f Response = %f Deadline = %ld \n", i, taskSetUtil, Response[i], D[i]);
           }
     }
     if(sched == false)
     {
             printf("\nTASKSET  NOT schedulable under Staschulat-with-PRE at TASKSET_UTIL = %f \n", taskSetUtil);
             fprintf(fp, "\nTASKSET  NOT schedulable under Staschulat-with-PRE at TASKSET_UTIL = %f \n", taskSetUtil);       
     }
     else
     {
             printf("\nTASKSET IS schedulable under Staschulat-with-PRE at TASKSET_UTIL = %f \n", taskSetUtil);
             fprintf(fp, "\nTASKSET IS schedulable under Staschulat-with-PRE at TASKSET_UTIL = %f \n", taskSetUtil); 
             
             Num_Executed_Tasks[STASCHULAT_PRE]++; 
             
             /*if(BDU_found == false)
             {
                 BDU_found = true;
                 fprintf(BDU_fp, "\t\t Staschulat with PRE \t\t\t\t %f \t\t %f \n\n", taskSetUtil, multFactor);
             }*/
     }
     
     
     cout << "************** Staschulat with PRE Ends *********************" << endl << endl;
     fprintf(fp,"************** Staschulat with PRE Ends ********************* \n\n");
     
}

void Response_time_ECB_Union_Multiset_PRE(bool display=true)
{
     int i, j, k;
     double acc;
     int flag;
     int LAST_TASK = NUM_TASKS - 1;
     
     std::set<int> workingSet1, workingSet2;
     
     static bool BDU_found = false;
     
     bool sched = true;
     
     if (BDU_found == true)
     {
        fprintf(fp, "************** BreakDown Utilization already found for ECB Union Multiset PRE*********************\n \n");
        cout << "************** BreakDown Utilization already found for ECB Union Multiset PRE*********************" << endl << endl;
        
        if (BDU_ONLY)
           return;
     }
     
     clear_Response();
     
     if (display)
     {
                 fprintf(fp, "************** ECB Union Multiset PRE********************* \n\n");
                 cout << "************** ECB Union Multiset PRE*********************" << endl << endl;
     }
     
     Response_ECB_Union_Multiset_PRE[0] = Response[0] = C[0];
     
     Reset_PRE_min();
     Reset_PRE_max();
     
     for (i = 1; (i < NUM_TASKS) && (sched == true); i++) 
     {
         flag = 1; int num_iter = 0;
         
         Response_ECB_Union_Multiset_PRE[i] = Response[i] = C[i];
         
         do 
         { 
            int EjRi, EjRk, EkRi; 
            std::multiset<int> M;
            std::multiset<int>::iterator it;
            
            acc = C[i];
            
            Num_Displaced_Blocks_ECB_Union_Multiset_PRE[i] = 0;
            
            for(k = 0; k < NUM_TASKS; k++)
            {
                    Gamma_i_j_ECB_Union_Multiset_PRE[k][i] = 0;
            }
            
            // calculate PRE values
            for (j = 0; j < i; j++)
            {
                 int pre_max_ij, pre_min_ij;
                 
                 float max_cost, min_cost;
             
                 pre_max_ij = calculate_pre_max_ij (j, i);
                 pre_min_ij = calculate_pre_min_ij (j, i); 
             
                 PRE_ij_max[j][i] = pre_max_ij;
                 PRE_ij_min[j][i] = pre_min_ij;
                 
             }
             SetPRE_max(i);
             SetPRE_min(i);
             
             
             for ( j = 0; j < i; j++)
             {
                 
                 EjRi = INTR_max(j, i);
                 
                 M.clear();
                 for(k=j+1; k <= i; k++)  // for-k-affinity
                 {
                     EjRk = INTR_max(j, k);
                     EkRi = INTR_max(k, i);
                     
                     workingSet1.clear();
                     it = M.begin();
                     
                     int numRepeat = PRE_ij_max[j][k] * EkRi ;
                     for(int s=0; s < numRepeat; s++)
                     {
                             int val = 0;
                             workingSet1.clear();
                             workingSet2.clear();
                 
                             // calculate Union_ECB
                             for (int h = 0; h <=  j; h++)
                             {
                                 std::set<int>::iterator it;
  
                                 for(it=TASK_ECB[h].begin(); it!=TASK_ECB[h].end(); ++it)
                                 {
                                      workingSet1.insert(*it);
                                 } 
                             }    
                 
                             workingSet2.clear();
                             Set_Intersect(workingSet1, TASK_UCB[k], workingSet2);
                             val = SET_MOD(workingSet2);
                             M.insert(val);                       
                     } // for EjRk*EkRi
                 } // for-k-affinity
                 
                 int Num_Blocks = 0;
                 for(int l=0; l < PRE_ij_max[j][i]; l++)
                 {
                          int val = 0;
                          std::multiset<int>::iterator max_it = M.begin();
                           
                          for(it = M.begin(); it != M.end(); ++it)
                          {
                                 if (val < *it)
                                 {
                                    val = *it;
                                    max_it = it; 
                                 }                                         
                          }
                          M.erase(max_it);
                          
                          Num_Blocks += val;
                 }
                 
                 acc += (INTR_max(j, i) * C[j]) + (BRT * Num_Blocks);
                 
                 Num_Displaced_Blocks_ECB_Union_Multiset_PRE[i] +=  Num_Blocks ;
                 Gamma_i_j_ECB_Union_Multiset_PRE[j][i] = BRT * Num_Blocks ; 
                                  
             }
             
             if (acc <=  Response[i])
                flag = 0;
             else
             {
                 Response_ECB_Union_Multiset_PRE[i] = Response[i] = acc;
                 
                 if (Response[i] > D[i])
                 {
                    flag = 0;
                    sched = false;
                    LAST_TASK = i;
                 }
                 // printf("acc =%f, response = %f, going again \n\n", acc, Response[j]);
             }
              
             num_iter++;
             
         } while (flag);
         
         if(display)
         {
                    printf("For task %d, number of iterations are %d, acc=%f, response = %f Num_Displaced_Blocks = %ld \n\n", i, num_iter, acc, Response_ECB_Union_Multiset_PRE[i], Num_Displaced_Blocks_ECB_Union_Multiset_PRE[i]); 
                    fprintf(fp, "For task %d, number of iterations are %d, acc=%f, response = %f Num_Displaced_Blocks = %ld \n\n", i, num_iter, acc, Response_ECB_Union_Multiset_PRE[i], Num_Displaced_Blocks_ECB_Union_Multiset_PRE[i]);     
         }
         
     } /* for-i */
     
     flag = 1;
     for(i = 0; i <= LAST_TASK; i++)
     {
           if ( Response[i] > D[i] || Response[i] == 0)
           {
                flag = 0;
                sched = false;
                if(display)
                {
                     printf("TASK %d NOT schedulable at TASKSET_UTIL = %f Response = %f Deadline = %ld \n", i, taskSetUtil, Response[i], D[i]);
                     fprintf(fp, "TASK %d NOT schedulable at TASKSET_UTIL = %f Response = %f Deadline = %ld \n", i, taskSetUtil, Response[i], D[i]);
                }
           }
           else
           {
               if(display)
               {
                    printf("TASK %d IS schedulable at TASKSET_UTIL = %f Response = %f Deadline = %ld \n", i, taskSetUtil, Response[i], D[i]);
                    fprintf(fp, "TASK %d IS schedulable at TASKSET_UTIL = %f Response = %f Deadline = %ld \n", i, taskSetUtil, Response[i], D[i]);
               }
           }
     }
     
     if(display)
     {
        if(sched == false)
        {
             printf("\nTASKSET NOT schedulable under ECB-Union-Multiset-PRE at TASKSET_UTIL = %f \n", taskSetUtil);
             fprintf(fp, "\nTASKSET NOT schedulable under ECB-Union-Multiset-PRE at TASKSET_UTIL = %f \n", taskSetUtil);       
        }
        else
        {
             printf("\nTASKSET IS schedulable under ECB-Union-Multiset-PRE at TASKSET_UTIL = %f \n", taskSetUtil);
             fprintf(fp, "\nTASKSET IS schedulable under ECB-Union-Multiset-PRE at TASKSET_UTIL = %f \n", taskSetUtil);
             
             Num_Executed_Tasks[ECB_UNION_MULTISET_PRE]++; 
             
             /*if(BDU_found == false)
             {
                 BDU_found = true;
                 fprintf(BDU_fp, "\t\t ECB Union Multiset PRE  \t\t\t\t %f \t\t %f \n\n", taskSetUtil, multFactor);
             }*/
        }
        
        fprintf(fp, "************** ECB Union Multiset PRE Ends ********************* \n\n" ); 
        cout << "************** ECB Union Multiset PRE Ends *********************" << endl << endl;
     }
}

void Response_time_UCB_Union_Multiset_PRE(bool display=true)
{
     int i, j, k;
     double acc;
     int Num_Blocks;
     int flag;
     int LAST_TASK = NUM_TASKS - 1;
     
     std::set<int> workingSet1, workingSet2;
     std::set<int>::iterator it;
     
     static bool BDU_found = false;
     
     bool sched = true;
     
     if (BDU_found == true)
     {
        fprintf(fp, "************** BreakDown Utilization already found for UCB Union Multiset PRE*********************\n \n");
        cout << "************** BreakDown Utilization already found for UCB Union Multiset PRE*********************" << endl << endl;
        
        if (BDU_ONLY)
           return;
     }
     
     clear_Response();
     
     if (display)
     {    
          fprintf(fp, "************** UCB Union Multiset PRE ********************* \n\n");
          cout << "************** UCB Union Multiset PRE *********************" << endl << endl;
     }
     
     Response_UCB_Union_Multiset_PRE[0] = Response[0] = C[0];
     
     Reset_PRE_min();
     Reset_PRE_max();
     
     for (i = 1; (i < NUM_TASKS) && (sched == true); i++) 
     {
         flag = 1; int num_iter = 0;
         
         Response_UCB_Union_Multiset_PRE[i] = Response[i] = C[i];
         
         do 
         { 
            int EjRi, EjRk, EkRi; 
            std::multiset<int> M, M_ucb_ij, M_ecb_j;
            std::multiset<int>::iterator it1, it2;
            
            acc = C[i];
            
            Num_Displaced_Blocks_UCB_Union_Multiset_PRE[i] = 0;
            
            for(k = 0; k < NUM_TASKS; k++)
            {
                    Gamma_i_j_UCB_Union_Multiset_PRE[k][i] = 0;
            }
            
            // calculate PRE values
            for (j = 0; j < i; j++)
            {
                 int pre_max_ij, pre_min_ij;
                 
                 float max_cost, min_cost;
             
                 pre_max_ij = calculate_pre_max_ij (j, i);
                 pre_min_ij = calculate_pre_min_ij (j, i); 
             
                 PRE_ij_max[j][i] = pre_max_ij;
                 PRE_ij_min[j][i] = pre_min_ij;
                 
             }
             SetPRE_max(i);
             SetPRE_min(i);
             
             
             for ( j = 0; j < i; j++)
             {
                 
                 EjRi = INTR_max(j, i);
                 
                 M_ucb_ij.clear();
                 for(k=j+1; k <= i; k++)  // for-k-affinity
                 {
                     EjRk = INTR_max(j, k);
                     EkRi = INTR_max(k, i);
                     
                     it1 = M_ucb_ij.begin();
                     
                     int numRepeat = PRE_ij_max[j][k] * EkRi ;
                     for(int s=0; s < numRepeat; s++)
                     {
                 
                             for(it=TASK_UCB[k].begin(); it!=TASK_UCB[k].end(); ++it)
                             {
                                   M_ucb_ij.insert(*it);
                             }                        
                     } // for EjRk*EkRi
                 } // for-k-affinity
                 
                 M_ecb_j.clear();
                 for(int s=0; s < PRE_ij_max[j][i]; s++)
                 {
                         for(it=TASK_ECB[j].begin(); it!=TASK_ECB[j].end(); ++it)
                         {
                                   M_ecb_j.insert(*it);
                         } 
                 }
                 
                 M.clear();
                 MultiSet_Intersect(M_ucb_ij, M_ecb_j, M);
                 
                 Num_Blocks = MultiSet_MOD(M);
                 
                 acc += (INTR_max(j, i) * C[j]) + (BRT * Num_Blocks);
                 
                 Num_Displaced_Blocks_UCB_Union_Multiset_PRE[i] +=  Num_Blocks ;
                 Gamma_i_j_UCB_Union_Multiset_PRE[j][i] = BRT * Num_Blocks ; 
                                
             }
             
             if (acc <=  Response[i])
                flag = 0;
             else
             {
                 Response_UCB_Union_Multiset_PRE[i] = Response[i] = acc;
                 
                 if (Response[i] > D[i])
                 {
                    flag = 0;
                    sched = false;
                    LAST_TASK = i;
                 }
                 // printf("acc =%f, response = %f, going again \n\n", acc, Response[j]);
             }
              
             num_iter++;
             
         } while (flag);
         
         if (display)
         {
            printf("For task %d, number of iterations are %d, acc=%f, response = %f Num_Displaced_Blocks = %ld \n\n", i, num_iter, acc, Response_UCB_Union_Multiset_PRE[i], Num_Displaced_Blocks_UCB_Union_Multiset_PRE[i]); 
            fprintf(fp, "For task %d, number of iterations are %d, acc=%f, response = %f Num_Displaced_Blocks = %ld \n\n", i, num_iter, acc, Response_UCB_Union_Multiset_PRE[i], Num_Displaced_Blocks_UCB_Union_Multiset_PRE[i]);     
         }
         
     } /* for-i */
     
     flag = 1;
     for(i = 0; i <= LAST_TASK; i++)
     {
           if ( Response[i] > D[i] || Response[i] == 0)
           {
                flag = 0;
                sched = false;
                
                if(display)
                {
                     printf("TASK %d NOT schedulable at TASKSET_UTIL = %f Response = %f Deadline = %ld \n", i, taskSetUtil, Response[i], D[i]);
                     fprintf(fp, "TASK %d NOT schedulable at TASKSET_UTIL = %f Response = %f Deadline = %ld \n", i, taskSetUtil, Response[i], D[i]);
                }
           }
           else
           {
               if(display)
               {
                     printf("TASK %d IS schedulable at TASKSET_UTIL = %f Response = %f Deadline = %ld \n", i, taskSetUtil, Response[i], D[i]);
                     fprintf(fp, "TASK %d IS schedulable at TASKSET_UTIL = %f Response = %f Deadline = %ld \n", i, taskSetUtil, Response[i], D[i]);
               }
           }
     }
     
     if(display)
     {
         if(sched == false)
         {
             printf("\nTASKSET  NOT schedulable under UCB-Union-Multiset-PRE at TASKSET_UTIL = %f \n", taskSetUtil);
             fprintf(fp, "\nTASKSET  NOT schedulable under UCB-Union-Multiset-PRE at TASKSET_UTIL = %f \n", taskSetUtil);     
         }
         else
         {
             printf("\nTASKSET IS schedulable under UCB-Union-Multiset-PRE at TASKSET_UTIL = %f \n", taskSetUtil);
             fprintf(fp, "\nTASKSET IS schedulable under UCB-Union-Multiset-PRE at TASKSET_UTIL = %f \n", taskSetUtil); 
             
             Num_Executed_Tasks[UCB_UNION_MULTISET_PRE]++;
             
             /*if(BDU_found == false)
             {
                 BDU_found = true;
                 fprintf(BDU_fp, "\t\t UCB-Union-Multiset-PRE  \t\t\t\t %f \t\t %f \n\n", taskSetUtil, multFactor);
             }*/  
             
         }
         
         fprintf(fp, "************** UCB Union Multiset PRE Ends ********************* \n\n" ); 
         cout << "************** UCB Union Multiset PRE Ends *********************" << endl << endl;
     }
     
}


void Response_time_ECB_UCB_Union_Multiset_Combined_PRE()
{
     int flag;
     int LAST_TASK = NUM_TASKS - 1;
     
     static bool BDU_found = false;
    
     bool sched = true;
      
     if (BDU_found == true)
     {
        fprintf(fp, "************** BreakDown Utilization already found for ECB UCB Union Multiset Combined PRE*********************\n \n");
        cout << "************** BreakDown Utilization already found for ECB UCB Union Multiset Combined PRE*********************" << endl << endl;
        
        if (BDU_ONLY)
           return;
     }
     
     // Response_time_ECB_Union_Multiset_PRE(false);
     // Response_time_UCB_Union_Multiset_PRE(false);
     
     printf("************** ECB UCB Union Multiset Combined PRE ********************* \n\n" );
     fprintf(fp, "************** ECB UCB Union Multiset Combined PRE ********************* \n\n" );
     for (int i = 0; (i < NUM_TASKS) && (sched == true); i++)
     {
         if ( Response_ECB_Union_Multiset_PRE[i] < Response_UCB_Union_Multiset_PRE[i] && Response_ECB_Union_Multiset_PRE[i] != 0)
         {
             Response_ECB_UCB_Union_Multiset_Combined_PRE[i] = Response[i] = Response_ECB_Union_Multiset_PRE[i];
             Num_Displaced_Blocks_ECB_UCB_Union_Multiset_Combined_PRE[i] = Num_Displaced_Blocks_ECB_Union_Multiset_PRE[i];
         }
         else
         {
             Response_ECB_UCB_Union_Multiset_Combined_PRE[i] = Response[i] = Response_UCB_Union_Multiset_PRE[i];
             Num_Displaced_Blocks_ECB_UCB_Union_Multiset_Combined_PRE[i] = Num_Displaced_Blocks_UCB_Union_Multiset_PRE[i];
         } 
         
         if ( Response[i] > D[i] || Response[i] == 0)
         {
                sched = false; 
                LAST_TASK = i;
         }  
         
         printf("For task %d, response = %f Num_Displaced_Blocks = %d \n\n", i, Response_ECB_UCB_Union_Multiset_Combined_PRE[i], Num_Displaced_Blocks_ECB_UCB_Union_Multiset_Combined_PRE[i]);
         fprintf(fp, "For task %d, response = %f Num_Displaced_Blocks = %d \n\n", i, Response_ECB_UCB_Union_Multiset_Combined_PRE[i], Num_Displaced_Blocks_ECB_UCB_Union_Multiset_Combined_PRE[i]);     
     }
     
     flag = 1;
     for(int i = 0; i <= LAST_TASK; i++)
     {
           if ( Response[i] > D[i] || Response[i] == 0)
           {
                flag = 0;
                sched = false;
                printf("TASK %d NOT schedulable at TASKSET_UTIL = %f Response = %f Deadline = %ld \n", i, taskSetUtil, Response[i], D[i]);
                fprintf(fp, "TASK %d NOT schedulable at TASKSET_UTIL = %f Response = %f Deadline = %ld \n", i, taskSetUtil, Response[i], D[i]);
           }
           else
           {
               printf("TASK %d IS schedulable at TASKSET_UTIL = %f Response = %f Deadline = %ld \n", i, taskSetUtil, Response[i], D[i]);
               fprintf(fp, "TASK %d IS schedulable at TASKSET_UTIL = %f Response = %f Deadline = %ld \n", i, taskSetUtil, Response[i], D[i]);
           }
     }
     if(sched == false)
     {
             printf("\nTASKSET NOT schedulable under ECB-UCB-Union-Multiset-PRE at TASKSET_UTIL = %f \n", taskSetUtil);
             fprintf(fp, "\nTASKSET  NOT schedulable under ECB-UCB-Union-Multiset-PRE at TASKSET_UTIL = %f \n", taskSetUtil);       
     }
     else
     {
             printf("\nTASKSET IS schedulable under ECB-UCB-Union-Multiset-PRE at TASKSET_UTIL = %f \n", taskSetUtil);
             fprintf(fp, "\nTASKSET IS schedulable under ECB-UCB-Union-Multiset-PRE at TASKSET_UTIL = %f \n", taskSetUtil); 
             
             Num_Executed_Tasks[ECB_UCB_UNION_MULTISET_COMBINED_PRE]++;
             
             /*if(BDU_found == false)
             {
                 BDU_found = true;
                 fprintf(BDU_fp, "\t\t ECB-UCB-Union-Multiset-Combined PRE  \t\t\t\t %f \t\t %f \n\n", taskSetUtil, multFactor);
             }*/
     }
     
     printf("************** ECB UCB Union Multiset Combined PRE Ends ********************* \n\n" );
     fprintf(fp, "************** ECB UCB Union Multiset Combined PRE Ends ********************* \n\n" );
     
}



void Response_time_PRE_MAX()
{
     int i, j, k;
     double acc;
     int flag;
     int LAST_TASK = NUM_TASKS - 1;
     
     std::set<int> workingSet1, workingSet2;
     
     static bool BDU_found = false;
     
     bool sched = true;
     
     if (BDU_found == true)
     {
        fprintf(fp, "************** BreakDown Utilization already found for PRE MAX*********************\n \n");
        cout << "************** BreakDown Utilization already found for PRE MAX*********************" << endl << endl;
        
        if (BDU_ONLY)
           return;
     }
     
     clear_Response();
     
     fprintf(fp, "************** PRE MAX ********************* \n\n");
     printf("************** PRE MAx *********************\n\n"); 
     
     Response_PRE_MAX[0] = Response[0] = C[0];
     
     Reset_PRE_min();
     Reset_PRE_max();
     
     for (i = 1; (i < NUM_TASKS) && (sched == true); i++) 
     {
         flag = 1; int num_iter = 0;
         
         Response_PRE_MAX[i] = Response[i] = C[i];
         
         do 
         { 
            
            acc = C[i];
            
            Num_Displaced_Blocks_PRE_MAX[i] = 0;
            
            for(k = 0; k < NUM_TASKS; k++)
            {
                    Gamma_i_j_PRE_MAX[k][i] = 0;
            }
            
            // calculate PRE values
            for (j = 0; j < i; j++)
            {
                 int pre_max_ij, pre_min_ij;
                 
                 float max_cost, min_cost;
             
                 pre_max_ij = calculate_pre_max_ij (j, i);
                 pre_min_ij = calculate_pre_min_ij (j, i); 
             
                 PRE_ij_max[j][i] = pre_max_ij;
                 PRE_ij_min[j][i] = pre_min_ij;
                 
             }
             SetPRE_max(i);
             SetPRE_min(i);
             
             int Num_Blocks = 0;
             for ( j = 0; j <= i; j++)
             {
                 int blocks_j = 0; 
                 
                 for(k=0; k < j; k++)
                 {
                          workingSet1.clear();
                          workingSet2.clear();
                          Set_Union(TASK_ECB[k], TASK_ECB[j], workingSet1);
                          Set_Intersect(workingSet1, TASK_UCB[i], workingSet2);
                          blocks_j = PRE_ij_max[k][j] * INTR_max(j, i) * SET_MOD(workingSet2);
                          Num_Blocks += blocks_j;
                 }
                 if (j < i)
                 {
                       acc += (INTR_max(j, i) * C[j]);
                       Gamma_i_j_PRE_MAX[j][i] = BRT * blocks_j ;
                 }
             }   
                 
             acc += (BRT * Num_Blocks);
             Num_Displaced_Blocks_PRE_MAX[i] =  Num_Blocks ;
             
             if (acc <=  Response[i])
                flag = 0;
             else
             {
                 Response_PRE_MAX[i] = Response[i] = acc;
                 
                 if (Response[i] > D[i])
                 {
                     flag = 0;
                     sched = false;
                     LAST_TASK = i;
                 }
                 // printf("acc =%f, response = %f, going again \n\n", acc, Response[j]);
             }
              
             num_iter++;
             
         } while (flag);
         
         
         printf("For task %d, number of iterations are %d, acc=%f, response = %f Num_Displaced_Blocks = %ld \n\n", i, num_iter, acc, Response_PRE_MAX[i], Num_Displaced_Blocks_PRE_MAX[i]); 
         fprintf(fp, "For task %d, number of iterations are %d, acc=%f, response = %f Num_Displaced_Blocks = %ld \n\n", i, num_iter, acc, Response_PRE_MAX[i], Num_Displaced_Blocks_PRE_MAX[i]);     
         
         
     } /* for-i */
     
     flag = 1;
     for(i = 0; i < NUM_TASKS; i++)
     {
           if ( Response[i] > D[i])
           {
                flag = 0;
                sched = false;
                printf("TASK %d NOT schedulable at TASKSET_UTIL = %f Response = %f Deadline = %ld \n", i, taskSetUtil, Response[i], D[i]);
                fprintf(fp, "TASK %d NOT schedulable at TASKSET_UTIL = %f Response = %f Deadline = %ld \n", i, taskSetUtil, Response[i], D[i]);
           }
           else
           {
               printf("TASK %d IS schedulable at TASKSET_UTIL = %f Response = %f Deadline = %ld \n", i, taskSetUtil, Response[i], D[i]);
               fprintf(fp, "TASK %d IS schedulable at TASKSET_UTIL = %f Response = %f Deadline = %ld \n", i, taskSetUtil, Response[i], D[i]);
           }
     }
     if(sched == false)
     {
             printf("\nTASKSET NOT schedulable under PRE-MAX at TASKSET_UTIL = %f \n", taskSetUtil);
             fprintf(fp, "\nTASKSET is NOT schedulable under PRE-MAX at TASKSET_UTIL = %f \n", taskSetUtil);       
     }
     else
     {
             printf("\nTASKSET IS schedulable under PRE-MAX at TASKSET_UTIL = %f \n", taskSetUtil);
             fprintf(fp, "\nTASKSET IS schedulable under PRE-MAX at TASKSET_UTIL = %f \n", taskSetUtil);
             
             Num_Executed_Tasks[PRE_MAX]++; 
             
             /*if(BDU_found == false)
             {
                 BDU_found = true;
                 fprintf(BDU_fp, "\t\t PRE MAX  \t\t\t\t %f \t\t %f \n\n", taskSetUtil, multFactor);
             }*/
     }
     
     fprintf(fp, "************** PRE MAX Ends ********************* \n\n" ); 
     cout << "************** PRE MAX Ends *********************" << endl << endl;
     
}


void Clear_ALL()
{

     for(int i=0; i < NUM_TASKS; i++)
     {
         
              Num_Displaced_Blocks_ECB_Only[i] = 0;
              Num_Displaced_Blocks_UCB_Only[i] = 0;
              Num_Displaced_Blocks_UCB_Union[i] = 0;
              Num_Displaced_Blocks_ECB_Union[i] = 0;

              Num_Displaced_Blocks_Staschulat[i] = 0;
              Num_Displaced_Blocks_ECB_Union_Multiset[i] = 0;
              Num_Displaced_Blocks_UCB_Union_Multiset[i] = 0;
              Num_Displaced_Blocks_ECB_UCB_Union_Multiset_Combined[i] = 0;

              Num_Displaced_Blocks_Staschulat_PRE[i] = 0;
              Num_Displaced_Blocks_ECB_Union_Multiset_PRE[i] = 0;
              Num_Displaced_Blocks_UCB_Union_Multiset_PRE[i] = 0;
              Num_Displaced_Blocks_ECB_UCB_Union_Multiset_Combined_PRE[i] = 0;

              Num_Displaced_Blocks_PRE_MAX[i] = 0;

              Response[i] = 0;

              Response_NO_PREEMPT[i] = 0;
              Response_ECB_Only[i] = 0; 
              Response_UCB_Only[i] = 0; 
              Response_UCB_Union[i] = 0; 
              Response_ECB_Union[i] = 0;
              Response_Staschulat[i] = 0;
              Response_ECB_Union_Multiset[i] = 0; 
              Response_UCB_Union_Multiset[i] = 0; 
              Response_ECB_UCB_Union_Multiset_Combined[i] = 0; 

              Response_Staschulat_PRE[i] = 0;
              Response_ECB_Union_Multiset_PRE[i] = 0; 
              Response_UCB_Union_Multiset_PRE[i] = 0; 
              Response_ECB_UCB_Union_Multiset_Combined_PRE[i] = 0; 
              Response_PRE_MAX[i] = 0;

              PRE_min[i] = 0;
              PRE_max[i] = 0;
    
             
             
             for(int j = 0; j < NUM_TASKS; j++)
             {
                     Gamma_i_j_ECB_Only[i][j] = 0;
                     Gamma_i_j_UCB_Only[i][j] = 0;
                     Gamma_i_j_UCB_Union[i][j] = 0;
                     Gamma_i_j_ECB_Union[i][j] = 0;

                     Gamma_i_j_Staschulat[i][j] = 0;
                     Gamma_i_j_ECB_Union_Multiset[i][j] = 0;
                     Gamma_i_j_UCB_Union_Multiset[i][j] = 0;

                     Gamma_i_j_Staschulat_PRE[i][j] = 0;
                     Gamma_i_j_ECB_Union_Multiset_PRE[i][j] = 0;
                     Gamma_i_j_UCB_Union_Multiset_PRE[i][j] = 0;

                     Gamma_i_j_PRE_MAX[i][j] = 0;
                     
                     PRE_ij_min[i][j] = 0; 
                     PRE_ij_max[i][j] = 0;
             }
      }
}

void Read_ECBs()
{
     int i, j;
     std::set<int>::iterator it;
     
     for(j=0; j < NUM_TASKS; j++)
     {
              int count = SIZE_ECB_TASK[j];
              int newPos = 0;
              
              int blockToInsert = rand() % CACHE_SIZE;
              
              for(i = 0; i < count; i++)
              {
                    it = TASK_ECB[j].find(blockToInsert);
                    
                    if (it == TASK_ECB[j].end())
                    {
                       TASK_ECB[j].insert(blockToInsert);
                       
                       // assert(newPos < count)
                       ECB_TASK_ARRAY[j][newPos++] = blockToInsert;
                    }
                    
                    blockToInsert = (blockToInsert + 1) % CACHE_SIZE;
              }
     }
     

    
    cout << "Printing ECBs" << endl;
    
    for(int i = 0; i < NUM_TASKS; i++)
    {
            fprintf(fp, "ECB Set %d : \n", i);
            print_SET(TASK_ECB[i]);
            fprintf(fp, "\n");
    }
    
    cout << "Printing ECBs DONE" << endl;
    
                  
}


void Read_UCBs()
{
     int i, j;
     std::set<int>::iterator it;
     
     for(j=0; j < NUM_TASKS; j++)
     {
         int newUCBlock, newPos, count, numBlocks, numUCBs;
         
         count = 0;
         
         if (SIZE_ECB_TASK[j] <= CACHE_SIZE)
         {
             numBlocks = SIZE_ECB_TASK[j];
         }
         else
         {
             numBlocks = CACHE_SIZE;
         }
         
         if (SIZE_UCB_TASK[j] <= CACHE_SIZE)
         {
             numUCBs = SIZE_UCB_TASK[j];
         }
         else
         {
             numUCBs = CACHE_SIZE;
         }
         
         while(count < numUCBs)
         {
                     newPos = rand() % numBlocks;
                       
                     newUCBlock = ECB_TASK_ARRAY[j][newPos];
                     it = TASK_UCB[j].find(newUCBlock);
                     
                     if (it == TASK_UCB[j].end())
                     {
                            TASK_UCB[j].insert(newUCBlock);
                            count++;
                     }
         }
         
         printf("The UCBs for task %d is done\n", j);
         
     }// for-j
     
     
     
     cout << "Printing UCBs" << endl;
    
     for(int i = 0; i < NUM_TASKS; i++)
     {
             fprintf(fp, "UCB Set %d : \n", i);
             print_SET(TASK_UCB[i]);
             fprintf(fp, "\n");
     }
     
     cout << "Printing UCBs Done" << endl;
     
     
}


double printTaskInfo()
{
     double util = 0;
     
     printf("**********************************************************************************************\n");
     printf("******************************* PRINTING TASK INFO *******************************************\n\n");
     
     fprintf(fp, "**********************************************************************************************\n");
     fprintf(fp, "******************************* PRINTING TASK INFO *******************************************\n\n");
     
     for(int i = 0; i < NUM_TASKS; i++)
     {
             printf("TASK = %d   EXEC_TIME = %f  PERIOD = %d  DEADLINE = %d \n\n", i, C[i], T[i], D[i]);
             fprintf(fp, "TASK = %d   EXEC_TIME = %f  PERIOD = %d  DEADLINE = %d \n\n", i, C[i], T[i], D[i]);   
             
             util += ( C[i] / T[i]);              
     }
     
     printf("The Utilization of TASK_SET is %f \n", util);
      
     printf("******************************* PRINTING TASK INFO ENDS*******************************************\n\n");
     fprintf(fp, "******************************* PRINTING TASK INFO ENDS*******************************************\n\n");
     
     return util;
}

void Create_TASK_Info_Linear(float multFactor)
{
     for(int i = 0; i < NUM_TASKS; i++)
     {
             C[i] = Cycles[i] * NIPMS; 
             T[i] = (long) (C[i] * multFactor);
             D[i] = T[i];
     }
}

bool Create_TASK_Info_Geometric(float multF)
{
     bool feasible = true;
     int i;
     
     //int C_mult_factor = 2;
     int T_mult_factor = 2;
     
     float util = 1 / multF ; 
     
     float sum_util, control_util;
     
     T[0] = 4;
     C[0] = util * T[0];
     
     D[0] = T[0];
     
     sum_util = C[0] / T[0]; 
     
     for(i = 1; i < NUM_TASKS; i++)
     {
             T[i] = T[i-1] * T_mult_factor;
             //C[i] = C[i-1] * C_mult_factor;
             C[i] = util * T[i];
             D[i] = T[i];
             
             sum_util+= (C[i] / T[i]) ;
     }
     
     printf("TASKSET, util = %f sum_util = %f \n", util, sum_util);
     
     if (sum_util > 1.0)
     {
         printf("INFEASIBLE TASKSET, util = %f sum_util = %f \n", util, sum_util);
         feasible = false;
     }
     /*else
     {
         control_util = total_util - sum_util;
         T[i] = T[i-1] * T_mult_factor;
     
         C[i] = control_util * T[i];
         D[i] = T[i];
     }
     */
     
     return feasible;
     
}

// Call all methods
void CALL_METHODS()
{
        
        Response_time_NO_PREEMPT();
        
        Response_time_ECB_Only();
        
        Response_time_UCB_Only(); 
        Response_time_UCB_Union();
        
        Response_time_ECB_Union(); 
        
        Response_time_ECB_Union_Multiset_PRE();
        Response_time_UCB_Union_Multiset_PRE();
        Response_time_ECB_UCB_Union_Multiset_Combined_PRE();  
        
        Response_time_ECB_Union_Multiset();
        Response_time_UCB_Union_Multiset();
        Response_time_ECB_UCB_Union_Multiset_Combined();
        
        Response_time_PRE_MAX();
        
        Response_time_Staschulat_PRE();
        Response_time_Staschulat();
        
}


void Malardaren_benchmark()
{   
    fp = fopen("Results_Malardalen.txt", "w");
    fprintf(fp, "\t\t CACHE SIZE = %d\n", CACHE_SIZE);
    fprintf(fp, "\t\t BRT = %f\n", BRT);
    fprintf(fp, "\t\t BRT = %d\n", RF);
    fprintf(fp, "\t\t NUM_TASKS = %d\n\n", NUM_TASKS);
    
    BDU_ONLY = true;
    
    BDU_fp = fopen("BDU-Info.txt", "w");
    fprintf(BDU_fp, "\t\t CACHE SIZE = %d\n", CACHE_SIZE);
    fprintf(BDU_fp, "\t\t BRT = %f\n", BRT);
    fprintf(BDU_fp, "\t\t BRT = %d\n", RF);
    fprintf(BDU_fp, "\t\t NUM_TASKS = %d\n\n", NUM_TASKS);
    fprintf(BDU_fp, "\t\tSCHEME \t\t\t\t BREAKDOWN UTILIZATION \t\t MULTFACTOR\n\n");
    
    Read_ECBs();
    Read_UCBs();
    
    for(multFactor = NUM_TASKS+125; multFactor < NUM_TASKS + 135; multFactor=multFactor+1)
    {
        Clear_ALL();
    
        Create_TASK_Info_Linear(multFactor);
        taskSetUtil = printTaskInfo();
    
        CALL_METHODS();
    
        printf("Printing INFO for TASK_UTILIZATION = %f ENDS \n\n", taskSetUtil);
        printf("************************************************************************************************\n\n\n\n");
    
        fprintf(fp, "Printing INFO for TASK_UTILIZATION = %f ENDS \n", taskSetUtil);
        fprintf(fp, "*******************************************************************************************\n\n\n\n");
        fflush(fp);
    }
    
    
    fclose(fp);
    fclose(BDU_fp);
         
    getchar();    
    
}

void Geometric_execution_times_benchmark()
{
        bool feasible = true;
        
        float total_util = 1.0;
        
        BRT = 0.008;
        BDU_ONLY = true;
        
        fp = fopen("Results_Geometric_times.txt", "w");
        
        fprintf(fp, "\t\t CACHE SIZE = %d\n", CACHE_SIZE);
        fprintf(fp, "\t\t BRT = %f\n", BRT);
        fprintf(fp, "\t\t BRT = %d\n", RF);
        fprintf(fp, "\t\t NUM_TASKS = %d\n\n", NUM_TASKS);
        
        BDU_fp = fopen("BDU-Info.txt", "w");
        fprintf(BDU_fp, "\t\t CACHE SIZE = %d\n", CACHE_SIZE);
        fprintf(BDU_fp, "\t\t BRT = %f\n", BRT);
        fprintf(BDU_fp, "\t\t BRT = %d\n", RF);
        fprintf(BDU_fp, "\t\t NUM_TASKS = %d\n\n", NUM_TASKS);
        fprintf(BDU_fp, "\t\tSCHEME \t\t\t\t BREAKDOWN UTILIZATION \t\t MULTFACTOR\n\n");
    
        Read_ECBs();
        Read_UCBs();
    
        for(multFactor = NUM_TASKS+0.37; multFactor < NUM_TASKS + 0.38; multFactor=multFactor+0.001)
        {
           Clear_ALL();
    
           feasible = Create_TASK_Info_Geometric(multFactor);
           taskSetUtil = printTaskInfo();
        
           if (feasible) 
           {
              CALL_METHODS();
           }
    
           printf("Printing INFO for TASK_UTILIZATION = %f ENDS \n\n", taskSetUtil);
           printf("************************************************************************************************\n\n\n\n");
    
           fprintf(fp, "Printing INFO for TASK_UTILIZATION = %f ENDS \n", taskSetUtil);
           fprintf(fp, "*******************************************************************************************\n\n\n\n");
        
           fflush(fp);
        }
        
        fclose(fp);
        fclose(BDU_fp);
        
        getchar();
          
}


void UUniFast(float A[], int n, float sumUtil)
{
     int i; float nextSumUtil; float random_val; 
     
     printf("The total util is %f\n", sumUtil);
     
     srand((unsigned)time(NULL));
     for (i=1; i < n; i++)
     {
         random_val = ((double) rand() / (RAND_MAX + 1));
         
         nextSumUtil = sumUtil * random_val;
         A[i-1] = sumUtil - nextSumUtil;
         sumUtil = nextSumUtil;
     }
         
     A[i-1] = sumUtil;
     
     for (i = 0; i < n; i++)
     {
         printf("The util for element %d is %f\n", i, A[i]);
     }
}

void UUniSort(float A[], int n, float sumUtil)
{
     int i, j; float random_val; float temp;
     
     printf("The total util is %f\n", sumUtil);
     
     srand((unsigned)time(NULL));
     A[0] = 0.0;
     for (i=1; i < n; i++)
     {
         random_val = ((double) rand() / (RAND_MAX + 1));
         A[i] = sumUtil * random_val;
     }
     
     for (i = 0; i < n-1; i++)
     {
         for (j=0; j < n-i-1; j++)
         {
             if (A[j] > A[j+1])
             {
                      temp = A[j];
                      A[j] = A[j+1];
                      A[j+1] = temp;
             }
         }
     }         
     
     for (i = 1; i < n ; i++)
     {
         A[i-1] = A[i] - A[i-1];
     }
     
     A[n-1] = sumUtil - A[n-1];
         
     for(i=0; i < n; i++)
     {
         printf("The util for element %d is %f\n", i, A[i]);
     }
}

// 5 - 500 => 1 - 100 => 0 => log (100)
void LogUniformPeriods(float A[], int n, int minP, int maxP)
{
     int i, j; float temp;
     
     float logScaleFactor;
     
     logScaleFactor = log(maxP/minP);
     
     srand((unsigned)time(NULL));
     for(i=0; i < n; i++)
     {
              // generate random numbers between 0 and 1
                 A[i] = (double) rand()/RAND_MAX;
              // Now A[i] is between 0 to log(maxP/minP) to make it uniform log distribution
                 A[i] = logScaleFactor * A[i];
              // Now map it to the scale 1 to maxP/minP
                 A[i] = exp(A[i]);
              // Finally map it to minP to maxP
                 A[i] = A[i] * minP;
     }
     
     // Sort the periods
     for (i = 0; i < n-1; i++)
     {
         for (j=0; j < n-i-1; j++)
         {
             if (A[j] > A[j+1])
             {
                      temp = A[j];
                      A[j] = A[j+1];
                      A[j+1] = temp;
             }
         }
     } 
     
     for(i= 0; i < n; i++)
     {
            printf("The generated random number is %f\n", A[i]);
     }
}

void Set_SizeECBs_UUniFast()
{
     int i; 
     
     float values_uunifast[NUM_TASKS];
     
     UUniSort(values_uunifast, NUM_TASKS, 1.0);
     
     for(i=0; i < NUM_TASKS; i++)
     {
              SIZE_ECB_TASK[i] = (int) (values_uunifast[i] * Total_ECBs_CU * CACHE_SIZE);
              printf("The number of ECBs for task %d is %d \n", i, SIZE_ECB_TASK[i]);
     }
}

void Set_SizeUCBs_Uniform()
{
     int i;
     
     srand((unsigned)time(NULL));
     for(i=0; i < NUM_TASKS; i++)
     {
              SIZE_UCB_TASK[i] = (int) ( RF * SIZE_ECB_TASK[i] * ( (double) rand()/ RAND_MAX )) ;
              printf("The number of UCBs for task %d is %d \n", i, SIZE_UCB_TASK[i]);
     }
     
}

void CreateTask_Uniform_Distribution(float totalUtil, int minPeriod, int maxPeriod)
{
     int i;
     
     float utilsArray[NUM_TASKS], periodsArray[NUM_TASKS];
     
     UUniFast(utilsArray, NUM_TASKS, totalUtil);
     LogUniformPeriods(periodsArray, NUM_TASKS, minPeriod, maxPeriod);
     
     for(i=0; i < NUM_TASKS; i++)
     {
              T[i] = (long) periodsArray[i];
              C[i] = utilsArray[i] * T[i];
              D[i] = T[i];
     }
     
      for(i=0; i < NUM_TASKS; i++)
     {
              printf("The utilization of task %d is %f\n", i, utilsArray[i]);     
     }
     
     for(i=0; i < NUM_TASKS; i++)
     {
              printf("The period of task %d is %d\n", i, T[i]);
     }
     
     for(i=0; i < NUM_TASKS; i++)
     {
              printf("The execution time for task %d is %f\n", i, C[i]);         
     }
     
}

void Print_Task_Execution_Statistics()
{
     int i;
     
     for(i = 0; i < NUM_METHODS; i++)
     {
           switch(i)
           {
                    case 0 : printf("The number of schedulable tasks for NO_PREEMPT is %d \n", Num_Executed_Tasks[NO_PREEMPT]);
                             fprintf(fp, "The number of schedulable tasks for NO_PREEMPT is %d \n", Num_Executed_Tasks[NO_PREEMPT]);
                             break;
                    case 1 : printf("The number of schedulable tasks for ECB_ONLY is %d \n", Num_Executed_Tasks[ECB_ONLY]);
                             fprintf(fp, "The number of schedulable tasks for ECB_ONLY is %d \n", Num_Executed_Tasks[ECB_ONLY]);
                             break;
                    case 2 : printf("The number of schedulable tasks for UCB_ONLY is %d \n", Num_Executed_Tasks[UCB_ONLY]);
                             fprintf(fp, "The number of schedulable tasks for UCB_ONLY is %d \n", Num_Executed_Tasks[UCB_ONLY]);
                             break;
                    case 3 : printf("The number of schedulable tasks for UCB_UNION is %d \n", Num_Executed_Tasks[UCB_UNION]);
                             fprintf(fp, "The number of schedulable tasks for UCB_UNION is %d \n", Num_Executed_Tasks[UCB_UNION]);
                             break;
                    case 4 : printf("The number of schedulable tasks for ECB_UNION is %d \n", Num_Executed_Tasks[ECB_UNION]);
                             fprintf(fp, "The number of schedulable tasks for ECB_UNION is %d \n", Num_Executed_Tasks[ECB_UNION]);
                             break;
                    case 5 : printf("The number of schedulable tasks for ECB_UNION_MULTISET is %d \n", Num_Executed_Tasks[ECB_UNION_MULTISET]);
                             fprintf(fp, "The number of schedulable tasks for ECB_UNION_MULTISET is %d \n", Num_Executed_Tasks[ECB_UNION_MULTISET]);
                             break;
                    case 6 : printf("The number of schedulable tasks for UCB_UNION_MULTISET is %d \n", Num_Executed_Tasks[UCB_UNION_MULTISET]);
                             fprintf(fp, "The number of schedulable tasks for UCB_UNION_MULTISET is %d \n", Num_Executed_Tasks[UCB_UNION_MULTISET]);
                             break;
                    case 7 : printf("The number of schedulable tasks for ECB_UCB_UNION_MULTISET_COMBINED is %d \n", Num_Executed_Tasks[ECB_UCB_UNION_MULTISET_COMBINED]);
                             fprintf(fp, "The number of schedulable tasks for ECB_UCB_UNION_MULTISET_COMBINED is %d \n", Num_Executed_Tasks[ECB_UCB_UNION_MULTISET_COMBINED]);
                             break;
                    case 8 : printf("The number of schedulable tasks for ECB_UNION_MULTISET_PRE is %d \n", Num_Executed_Tasks[ECB_UNION_MULTISET_PRE]);
                             fprintf(fp, "The number of schedulable tasks for ECB_UNION_MULTISET_PRE is %d \n", Num_Executed_Tasks[ECB_UNION_MULTISET_PRE]);
                             break;
                    case 9 : printf("The number of schedulable tasks for UCB_UNION_MULTISET_PRE is %d \n", Num_Executed_Tasks[UCB_UNION_MULTISET_PRE]);
                             fprintf(fp, "The number of schedulable tasks for UCB_UNION_MULTISET_PRE is %d \n", Num_Executed_Tasks[UCB_UNION_MULTISET_PRE]);
                             break;
                    case 10 : printf("The number of schedulable tasks for ECB_UCB_UNION_MULTISET_COMBINED_PRE is %d \n", Num_Executed_Tasks[ECB_UCB_UNION_MULTISET_COMBINED_PRE]);
                             fprintf(fp, "The number of schedulable tasks for ECB_UCB_UNION_MULTISET_COMBINED_PRE is %d \n", Num_Executed_Tasks[ECB_UCB_UNION_MULTISET_COMBINED_PRE]);
                             break;
                    case 11 : printf("The number of schedulable tasks for STASCHULAT is %d \n", Num_Executed_Tasks[STASCHULAT]);
                             fprintf(fp, "The number of schedulable tasks for STASCHULAT is %d \n", Num_Executed_Tasks[STASCHULAT]);
                             break;
                    case 12 : printf("The number of schedulable tasks for STASCHULAT_PRE is %d \n", Num_Executed_Tasks[STASCHULAT_PRE]);
                             fprintf(fp, "The number of schedulable tasks for STASCHULAT_PRE is %d \n", Num_Executed_Tasks[STASCHULAT_PRE]);
                             break;
                    case 13 : printf("The number of schedulable tasks for PRE_MAX is %d \n", Num_Executed_Tasks[PRE_MAX]);
                             fprintf(fp, "The number of schedulable tasks for PRE_MAX is %d \n", Num_Executed_Tasks[PRE_MAX]);
                             break;
                    default : break;
                             
           }
     }
}

void Clear_Task_Execution_Statistics()
{
     int i;
     
     for( i = 0; i < NUM_METHODS; i++)
     {
          Num_Executed_Tasks[i] = 0;
     }
}


void Uniform_Distribution_Benchmark()
{
     float totalUtil = taskSetUtil = 0.05; 
    
     int minPeriod = 5;
     int maxPeriod = 500;
    
    
     BRT = 0.008;
     BDU_ONLY = false;
     
    fp = fopen("Results_Uniform Distribution", "w");
    fprintf(fp, "\t\t CACHE SIZE = %d\n", CACHE_SIZE);
    fprintf(fp, "\t\t BRT = %f\n", BRT);
    fprintf(fp, "\t\t RF = %d\n", RF);
    fprintf(fp, "\t\t NUM_TASKS = %d\n\n", NUM_TASKS);
     
     Set_SizeECBs_UUniFast();
     Set_SizeUCBs_Uniform();
     
     Read_ECBs();
     Read_UCBs();
      
     for (; totalUtil <= 1.0; totalUtil += 0.05)
     {
         taskSetUtil = totalUtil; 
         
         for(int i=1; i <= NUM_TASK_SETS; i++)
         {
              Clear_ALL();
              CreateTask_Uniform_Distribution(totalUtil, minPeriod, maxPeriod);
              printTaskInfo();
     
              CALL_METHODS();
          }
          printf("Printing statistics for %f utilization and %d NUM_TASK_SETS \n\n", totalUtil, NUM_TASK_SETS);
          fprintf(fp, "Printing statistics for %f utilization and %d NUM_TASK_SETS \n\n", totalUtil, NUM_TASK_SETS);  
          Print_Task_Execution_Statistics();
          printf("Printing statistics for %f utilization and %d NUM_TASK_SETS complete\n\n", totalUtil, NUM_TASK_SETS);
          fprintf(fp, "Printing statistics for %f utilization and %d NUM_TASK_SETS complete\n\n", totalUtil, NUM_TASK_SETS);
          
          Clear_Task_Execution_Statistics();
          
          fflush(fp);
     }
     fflush(fp);
     fclose(fp);
}

int main(int argc, char *argv[])
{
    
    //Malardaren_benchmark();
    //Geometric_execution_times_benchmark();
    
    Uniform_Distribution_Benchmark();
    
    getchar();
    
}


