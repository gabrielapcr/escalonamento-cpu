#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#define STRING_SIZE 24

typedef struct {
    char name[STRING_SIZE];
    int period;
    int burst;
    int have_task;
    int quant_exec;
    int flag_holding;
    int quant_missing;
    int lost_deadlines;
    int complete_execution;
    int killed;
} task;

void bubbleSort(task Tasks[], int line);

int main(int argc, char* argv[])
{
    FILE *inputFile, *outputFile;
    inputFile = fopen(argv[1], "r");
    outputFile = fopen("rate_gpcr.out","w");

    int count = 0, aux = 0, line = 0, time_unit, idle = 0;
    int burst, period, name[STRING_SIZE];

    //read part for count
    fscanf(inputFile, "%d", &time_unit);
	while (fscanf(inputFile, "%s %d %d", name, &period, &burst)!= EOF)
    {
		line++;
	}
	task Tasks[line];
    fclose(inputFile);

	//open file again to save information on struct
    inputFile = fopen(argv[1], "r");

    //ERROR VERIFICATION
    if(argc == 2)
    {
        inputFile = fopen(argv[1],"r");
        if(inputFile == NULL)
        {
            //print and forced program completion 
            printf("file can't be open\n");
            return 0;
        }
        // if(argv[1] != "input.txt")
        // {
        //     //print and forced program completion 
        //     printf("wrong input file formatting\n");
        //     return 0;
        // }
    }
    else
    {
        printf("invalid number of arguments\n");
        return 0;
    }
    int aux2 = 0;
    //DATA READING
    fscanf(inputFile, "%d", &time_unit);
    for(int i = 0; fscanf(inputFile, "%s %d %d", Tasks[i].name, &Tasks[i].period, &Tasks[i].burst)!= EOF; i++)
    {
        Tasks[i].quant_exec = 0;
        Tasks[i].have_task = 0;
        Tasks[i].flag_holding = 0;
        Tasks[i].quant_missing = Tasks[i].burst;
        Tasks[i].lost_deadlines = 0;
        Tasks[i].complete_execution = 0;
        Tasks[i].killed = 0;
        // if(Tasks[i].name != NULL && Tasks[i].period != "  " && Tasks[i].burst != "  ") 
        // {
        //     aux2 += 3;
        // }
    }
    // if(aux2 != line * 3)
    // {
    //     printf("wrong number of input\n");
    //     return 1;
    // }
    bubbleSort(Tasks,line);

    //LÓGICA ABAIXO TODO:

    fprintf(outputFile,"EXECUTION BY RATE\n");
    
    for (int i = 0; i < time_unit; i++){
        int atual_task = -9;
        // RECEBENDO NOVAS TASKS
        for(int j = 0; j < line; j++)
        {
            //verificando se é divisivel 
            if((i % (Tasks[j].period)) == 0)
            {
                Tasks[j].have_task++;
                //verificando se tem lost por duas unidades
                if(Tasks[j].have_task == 2)
                {
                    if ((Tasks[j].flag_holding) == 0){
                        Tasks[j].lost_deadlines++;
                        if (Tasks[j].quant_exec != 0){
                            fprintf(outputFile,"[%s] for %d units - L\n",Tasks[j].name, (Tasks[j].quant_exec));
                        }
                    } 
                    else if ((Tasks[j].flag_holding) == 1){
                        Tasks[j].lost_deadlines++;
                        if ((Tasks[j].burst - Tasks[j].quant_exec) != 0){
                            fprintf(outputFile,"[%s] for %d units - L\n",Tasks[j].name, (Tasks[j].burst - Tasks[j].quant_exec));
                        }
                    }
                    Tasks[j].have_task = 1;
                    Tasks[j].quant_exec = 0;
                }
            }
        }
        // verificação de prioridade das tasks 
        for(int j = 0; j < line; j++)
        {
            if(Tasks[j].have_task == 1)
            {
                Tasks[j].quant_exec++;
                //executa ate o deadline dela e decrementa a prioridade da task
                if(Tasks[j].quant_exec == Tasks[j].burst)
                {
                    Tasks[j].have_task = 0;
                    if (Tasks[j].flag_holding == 1){
                        if ((Tasks[j].burst - Tasks[j].quant_exec) == 0){
                            Tasks[j].complete_execution++;
                            fprintf(outputFile,"[%s] for %d units - F\n",Tasks[j].name, Tasks[j].quant_missing);
                        }
                        else {
                            Tasks[j].complete_execution++;
                            fprintf(outputFile,"[%s] for %d units - F\n",Tasks[j].name, (Tasks[j].burst - Tasks[j].quant_exec));
                        }
                    }
                    else if (Tasks[j].flag_holding == 0){
                        Tasks[j].complete_execution++;
                        fprintf(outputFile,"[%s] for %d units - F\n",Tasks[j].name, Tasks[j].quant_exec);
                    }
                    Tasks[j].quant_exec = 0;
                }
                atual_task = j;
                break;
            }

            if(j == (line-1))
            {
                atual_task = -1;
                aux = -1;
                idle++;
            }
           
        }
        // verificando se trocou de task 
        if (aux != atual_task)
        {
            if (atual_task != -1 && aux != -1)
            {
                if (Tasks[atual_task].have_task != 0 && Tasks[aux].quant_exec != 0){
                    fprintf(outputFile,"[%s] for %d units - H\n",Tasks[aux].name, Tasks[aux].quant_exec);
                    Tasks[aux].quant_missing = (Tasks[aux].burst - Tasks[aux].quant_exec);
                    Tasks[aux].flag_holding = 1;
                }
            }
            else 
            {
                fprintf(outputFile,"idle for %d units\n",idle);
                idle = 0;
            }
            aux = atual_task;
        }
    }

    for (int i = 0;i < line; i++){
        if ((time_unit % Tasks[i].period) == 0){
            Tasks[i].have_task++;
        }

        if ((Tasks[i].have_task) == 1){
            Tasks[i].killed = 1;
            if (Tasks[i].quant_exec != 0){
                fprintf(outputFile,"[%s] for %d units - K\n",Tasks[aux].name, Tasks[aux].quant_exec);
            }
        }
    
    }

    if (idle != 0){
        fprintf(outputFile,"idle for %d units\n",idle);
    }

    fprintf(outputFile,"\n");
    fprintf(outputFile,"LOST DEADLINES\n");
    for (int i = 0;i < line; i++){
        fprintf(outputFile,"[%s] %d\n",Tasks[i].name,Tasks[i].lost_deadlines);
    }
    fprintf(outputFile,"\n");
    fprintf(outputFile,"COMPLETE EXECUTION\n");
    for (int i = 0;i < line; i++){
        fprintf(outputFile,"[%s] %d\n",Tasks[i].name,Tasks[i].complete_execution);
    }
    fprintf(outputFile,"\n");
    fprintf(outputFile,"KILLED\n");
    for (int i = 0;i < line; i++){
        fprintf(outputFile,"[%s] %d\n",Tasks[i].name,Tasks[i].killed);
    }

    fclose(outputFile);
    fclose(inputFile);

    return 0;
}

void bubbleSort(task Tasks[STRING_SIZE], int line)
{
    int i, continua, aux, end = line;
    do
    {
        continua = 0;
        int j;
        task first;
        for(i = 0; i < end-1; i++)
        {
            for(j = 0;j <(end - 1 -i); j++)
            {
                if (Tasks[j].period > Tasks[j+1].period)
                {
                    first = Tasks[j];
                    Tasks[j] = Tasks[j+1];
                    Tasks[j+1] = first;
                    continua = j;
                }
            }
        }
        end--;
    }while(continua != 0); 
}