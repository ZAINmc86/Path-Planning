#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define path_size 16
#define population_size 90
#define iteration 100000
#define size (path_size - 2)
#define weight_l 2
#define weight_t 2
#define weight_f 3

const char blank_space = ' ';
const char obstacle = 254;
const char initial = 158;
const char destination = 158;
const char step = 46;

void rand_gen();
void turns(int rand_gene[population_size][size]);
void path_planning(int cpy_gene[population_size][path_size]);
void copy_pop(int rand_gene[population_size][size], int cpy_gene[population_size][path_size]);
void Bubble_sorting(int rand_gene[population_size][size], float fitest[population_size]);
void mutation_fn(int rand_gene[population_size][size]);
void cross_over_fn(int rand_gene[population_size][size]);
void display_sol(int arena_define[path_size][path_size]);
void maxval();
void fitness_fn();
int check_sol(float fitest[population_size]);

char arena[path_size][path_size];
int rand_gene[population_size][size];
int cpy_gene[population_size][path_size];
int path_turns[population_size];
int path_length[population_size];
int inf_step[population_size];
int direction_bit[population_size], orientation[population_size];
int min[3], max[3];
float fitest[population_size];
int solved_index = 0;

int arena_define[path_size][path_size] =
    {
        {0, 0, 0, 0, 1, 0, 1, 0, 1, 0, 0, 0, 0, 0, 1, 0},
        {0, 0, 0, 0, 1, 0, 1, 0, 1, 0, 0, 0, 0, 0, 1, 0},
        {0, 0, 0, 0, 1, 1, 1, 0, 1, 0, 0, 0, 0, 0, 1, 0},
        {0, 0, 1, 0, 1, 1, 1, 0, 1, 0, 0, 1, 0, 0, 1, 0},
        {0, 0, 1, 0, 0, 1, 1, 0, 1, 0, 0, 1, 0, 0, 0, 0},
        {0, 0, 1, 0, 0, 1, 0, 0, 0, 0, 0, 1, 0, 1, 0, 0},
        {1, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 1, 0, 0},
        {1, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0},
        {1, 0, 0, 0, 0, 0, 0, 1, 0, 1, 0, 0, 0, 1, 0, 1},
        {1, 0, 0, 0, 0, 0, 0, 1, 0, 1, 0, 0, 0, 0, 0, 1},
        {0, 0, 0, 1, 0, 0, 1, 1, 0, 1, 0, 0, 1, 0, 0, 1},
        {0, 0, 0, 1, 0, 0, 1, 1, 0, 0, 0, 0, 1, 0, 0, 1},
        {0, 1, 0, 1, 0, 0, 1, 1, 0, 0, 1, 0, 1, 0, 0, 0},
        {0, 1, 0, 1, 0, 0, 1, 0, 0, 0, 1, 0, 1, 0, 0, 0},
        {0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 1, 0, 1, 0, 0, 0},
        {0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0}};

int main()
{
    int check = 0, i = 0;
    rand_gen();

    while (i < iteration && check == 0)
    {
        turns(rand_gene);
        copy_pop(rand_gene, cpy_gene);
        path_planning(cpy_gene);
        maxval();
        fitness_fn();
        check = check_sol(fitest);
        if (check == 1)
        {
            break;
        }
        Bubble_sorting(rand_gene, fitest);
        cross_over_fn(rand_gene);
        mutation_fn(rand_gene);

        printf("%d,%d,%d,%0.2f", path_turns[0], path_length[0], inf_step[0], fitest[0]);
        printf("\n");

        i++;
    }

    if (check == 1)
    {
        printf("\ngene = %d.\n\n", i);
        display_sol(arena_define);
    }
    else
    {
        printf("No solution found.");
    }

    return 0;
}
// Fun
void rand_gen()
{
    srand(time(NULL));

    for (int i = 0; i < population_size; i++)
    {
        for (int j = 0; j < size; j++)
        {
            rand_gene[i][j] = (rand() % path_size);
            direction_bit[i] = (rand() % 2);
            orientation[i] = (rand() % 2);
        }
    }
}

void copy_pop(int rand_gene[population_size][size], int cpy_gene[population_size][path_size])
{
    for (int i = 0; i < population_size; i++)
    {
        cpy_gene[i][0] = 0;
        cpy_gene[i][path_size - 1] = (path_size - 1);

        for (int j = 0; j < path_size - 2; j++)
        {
            cpy_gene[i][j + 1] = rand_gene[i][j];
        }
    }
}

void path_planning(int cpy_gene[population_size][path_size])
{                   // for column first .
    int length = 0; // 0 means column wise
    int k = 0, decide;
    int infeasible_step = 0;

    for (int i = 0; i < population_size; i++)
    {
        decide = ((direction_bit[i] % 2) ^ (orientation[i] % 2)) == 0 ? 0 : 1;
        
        if ((check_sol(fitest) == 1) && (i == solved_index))
        {
            if ((decide != 0))
            {
                printf("(0 , 0) ");
            }
        }
        for (int j = 0; j < path_size - 1; j++)
        {
            if (cpy_gene[i][j + 1] > cpy_gene[i][j])
            {
                while (k != (cpy_gene[i][j + 1] + 1))
                {
                    if (orientation[i] % 2 == 0)
                    {
                        if ((check_sol(fitest) == 1) && (i == solved_index))
                        {
                            printf("(%d , %d) ", k, j + decide);
                            arena[k][j + decide] = step;
                        }
                        if (arena_define[k][j + decide] == 1)
                        {
                            infeasible_step++;
                        }
                    }
                    else
                    {
                        if ((check_sol(fitest) == 1) && (i == solved_index))
                        {
                            printf("(%d , %d) ", j + decide, k);
                            arena[j + decide][k] = step;
                        }

                        if (arena_define[j + decide][k] == 1)
                        {
                            infeasible_step++;
                        }
                    }
                    k++;
                    length++;
                }
                k--; // to assign the previous value.
            }
            else
            {
                while (k != (cpy_gene[i][j + 1] - 1))
                {
                    if (orientation[i] % 2 == 0)
                    {
                        if ((check_sol(fitest) == 1) && (i == solved_index))
                        {
                            printf("(%d , %d) ", k, j + decide);
                            arena[k][j + decide] = step;
                        }

                        if (arena_define[k][j + decide] == 1)
                        {
                            infeasible_step++;
                        }
                    }
                    else
                    {
                        if ((check_sol(fitest) == 1) && (i == solved_index))
                        {
                            printf("(%d , %d) ", j + decide, k);
                            arena[j + decide][k] = step;
                        }
                        if (arena_define[j + decide][k] == 1)
                        {
                            infeasible_step++;
                        }
                    }
                    k--;
                    length++;
                }
                k++;
            }
        }
        if ((check_sol(fitest) == 1) && (i == solved_index))
        {
            if ((decide == 0))
            {
                printf("(%d , %d)", path_size - 1, path_size - 1);
            }
        }

        path_length[i] = length;
        length = 0;
        inf_step[i] = infeasible_step;
        infeasible_step = 0;
        k = 0;
    }
}

void maxval()
{

    min[0] = path_turns[0];
    max[0] = path_turns[0];

    for (int i = 0; i < population_size; i++)
    {
        if (path_turns[i] < min[0])
        {
            min[0] = path_turns[i];
        }
        if (path_turns[i] > max[0])
        {
            max[0] = path_turns[i];
        }
    }

    min[1] = path_length[0];
    max[1] = path_length[0];

    for (int i = 0; i < population_size; i++)
    {
        if (path_length[i] < min[1])
        {
            min[1] = path_length[i];
        }
        if (path_length[i] > max[1])
        {
            max[1] = path_length[i];
        }
    }

    min[2] = inf_step[0];
    max[2] = inf_step[0];

    for (int i = 0; i < population_size; i++)
    {
        if (inf_step[i] < min[2])
        {
            min[2] = inf_step[i];
        }
        if (inf_step[i] > max[2])
        {
            max[2] = inf_step[i];
        }
    }
}

void fitness_fn()
{
    float fit_t, fit_l, fit_f;
    int S_min = 0;

    for (int i = 0; i < population_size; i++)
    {
        fit_t = 1 - (float)(path_turns[i] - min[0]) / (max[0] - min[0]);
        fit_l = 1 - (float)(path_length[i] - min[1]) / (max[1] - min[1]);
        fit_f = 1 - (float)(inf_step[i] - S_min) / (max[2] - S_min);

        fitest[i] = (100 * weight_f * fit_f) * (float)((weight_l * fit_l) + (weight_t * fit_t)) / (weight_l + weight_t);
    }
}

void Bubble_sorting(int rand_gene[population_size][size], float fitest[population_size])
{

    int temp = 0;
    int temp_array[size];

    for (int i = 0; i < population_size; i++)         // For all the population to be sorted according to their fitness
    {                                                 // so the parent selection(fittest) can be made.
        for (int j = i + 1; j < population_size; j++) // For comparing the populations,that is why size is not used.
        {                                             // we compare the values of the fitness of populations.
            if (fitest[j] > fitest[i])
            {
                // bubble sorting of the random population array
                // highest value would be at higher position
                // i-value of the fitness_fn[i] represent population row

                for (int k = 0; k < size; k++) // Follwing the steps of swapping the population.
                {
                    temp_array[k] = rand_gene[i][k]; // For specific population [i] just copying
                    // the whole values in temporary 1-D array.
                }
                for (int k = 0; k < size; k++)
                {
                    rand_gene[i][k] = rand_gene[j][k]; // Copying continues.
                }
                for (int k = 0; k < size; k++)
                {
                    rand_gene[j][k] = temp_array[k]; // Copying continues.
                }
                // swapping completed.

                // bubble sorting the fitness array.
                // highest value would be at higher position.
                temp = fitest[i]; // swaping the values of the fitness of the population also.
                fitest[i] = fitest[j];
                fitest[j] = temp;
            }
        }
    }
}

void mutation_fn(int rand_gene[population_size][size])
{
    srand(time(NULL));

    for (int i = 0; i < population_size; i = i + 1) // i= i + 2,for doing mutation_fn in one population leaving other.
    {                                               // For each population different mutaion index and mutaion value
        int index = rand() % size;                  // For changing the value of the fitness, expected to make that
        int val = rand() % size;                    // population more fitest by reducing its attack value.
        rand_gene[i][index] = val;
    }
}

void cross_over_fn(int rand_gene[population_size][size])
{ // Cross_over :By taking the fittest parent after selection then crossing their values,expected to make
    // more fittest offspring for the search of the ultimate solution.

    srand(time(NULL));
    // rand() % size + 1;// For random crossover point.
    int cross_point = rand() % (size - 1) + 1; // to reduce the range of the cross_point.
    // (size % 2 == 0) ? (size / 2) : ((size - 1) / 2); // crossover point as mid.
    int temp_arr[size - cross_point]; // For swaping the values from cross_over_fn point to total numbrer of queens

    int k = 0;

    for (int i = 0; i < population_size; i++) // For copying the First half of the population to the oter half.
    {                                         // i = representing the population number .
        for (int j = 0; j < size; j++)        // J = representing the  gene of that population.
        {
            rand_gene[k + (population_size / 2)][j] = rand_gene[i][j];
            //[k + (population_size / 2)]= represnting the  next half population.
        }
        k++;
    }
    // For swapping the next half to the cross_over_fn point with other population.
    for (int i = (population_size / 2); i < population_size; i = i + 2) // For interchanging the values of two population
    // then oter two population.
    {
        for (int j = 0; j < (size - cross_point); j++) //(n_Q - cross_point)= represts how many times this loop will
        {                                              // run for swapping the genes pf the populations.

            temp_arr[j] = rand_gene[i + 1][j + cross_point];
        }
        for (int j = 0; j < (size - cross_point); j++)
        {

            rand_gene[i + 1][j + cross_point] = rand_gene[i][j + cross_point];
        }
        for (int j = 0; j < (size - cross_point); j++)
        {

            rand_gene[i][j + cross_point] = temp_arr[j];
        }
    }
}

int check_sol(float fitest[population_size])
{
    // For checking the solution.
    for (int i = 0; i < population_size; i++)
    {
        if (fitest[i] == (100 * weight_f))
        {
            solved_index = i; // used for representing the board of the specific population.
            return 1;
        }
    }
    return 0;
}

void display_sol(int arena_define[path_size][path_size])
{
    printf("0 ");
    for (int i = 0; i < size; i++)
    {
        printf("%d ", rand_gene[solved_index][i]);
    }
    printf("%d\n", path_size - 1);

    for (int i = 0; i < path_size; i++)
    {
        for (int j = 0; j < path_size; j++)
        {
            if (arena_define[i][j] == 0)
            {
                arena[i][j] = ' ';
            }
            else if (arena_define[i][j] == 1)
            {
                arena[i][j] = obstacle;
            }
        }
    }

    printf("\n");
    path_planning(cpy_gene);
    printf("\b\n");
    printf("\n");
    printf("\n");
    printf("Direction = %d: ", direction_bit[solved_index]);
    printf("Orientation = %d: ", orientation[solved_index]);
    printf("No of turns = %d: ", path_turns[solved_index]);
    printf("Path_Length = %d: ", path_length[solved_index]);
    printf("Infesible _Steps = %d: ", inf_step[solved_index]);
    printf("fitness_fn = %0.2f", fitest[solved_index]);
    printf("\n");
    printf("\n");

    arena[0][0] = initial;
    arena[path_size - 1][path_size - 1] = destination;

    for (int i = 0; i < path_size; i++)
    {
        printf("\n|");
        for (int j = 0; j < path_size; j++)
        {
            printf("%c", arena[i][j]);
            printf("|");
        }
    }
    printf("\n");
}