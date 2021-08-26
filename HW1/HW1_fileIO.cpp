#pragma warning(disable:4996)
#define _CRT_SECURE_NO_WARNINGS 
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define INSTANCES 414
#define MAX_LEN 100

double th0, th3;

typedef struct Data{

    double distance;
    double unitPrice;
    char x_axis[100];
    char y_axis[100];

}Data;

Data* dataList;

double LinearHypothesis(const double x3){
    
    return th0 + th3 * x3;

}

double mse(){

    double error_sum = 0.0;

    for(int i = 0; i < INSTANCES; ++i){

        const double error = dataList[i].unitPrice - LinearHypothesis(dataList[i].distance);

        error_sum += error * error;

    }

    return error_sum / (double)INSTANCES * 0.5;

}


int main() {
    dataList = (Data*)malloc(sizeof(Data) * INSTANCES);
    printf("============distance============\n");
    FILE* file;
    file = fopen("X3.txt", "r");
    for (int i = 0; i < INSTANCES; ++i) {
        fgets(dataList[i].x_axis, MAX_LEN, file);
        dataList[i].distance = atof(dataList[i].x_axis);
        printf("%lf\n", dataList[i].distance);
    }
    printf("============price============\n");
    FILE* file2;
    file2 = fopen("price.txt", "r");
    for (int i = 0; i < INSTANCES; ++i) {
        fgets(dataList[i].y_axis, MAX_LEN, file2);
        dataList[i].unitPrice = atof(dataList[i].y_axis);
        printf("%lf\n", dataList[i].unitPrice);
    }

    fclose(file);
    fclose(file2);
    free(dataList);

    return 0;
}