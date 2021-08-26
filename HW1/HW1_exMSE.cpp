#pragma warning(disable:4996)
#define _CRT_SECURE_NO_WARNINGS 
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define INSTANCES 5
#define MAX_LEN 100

double th0, th3;

typedef struct Data {

    double distance;
    double unitPrice;
    char x_axis[100];
    char y_axis[100];

}Data;

Data* dataList;

double LinearHypothesis(const double x3) {

    return th0 + th3 * x3;

}

double mse() {

    double error_sum = 0.0;

    for (int i = 0; i < INSTANCES; ++i) {

        const double error = dataList[i].unitPrice - LinearHypothesis(dataList[i].distance);

        error_sum += error * error;

    }

    return error_sum / (double)INSTANCES * 0.5;

}


int main()

{  
    dataList = (Data*)malloc(sizeof(Data) * INSTANCES);

    dataList[0].distance = 84.87882;
    dataList[0].unitPrice = 37.9;

    dataList[1].distance = 306.5947;    	
    dataList[1].unitPrice = 42.2;

    dataList[2].distance = 561.9845;
    dataList[2].unitPrice = 47.3;

    dataList[3].distance = 561.9845;
    dataList[3].unitPrice = 54.8;

    dataList[4].distance = 390.5684;
    dataList[4].unitPrice = 43.1;

    th0 = 0.0;
    th3 = 0.0;

    printf("Mean squared error: %.1f.\n", mse());

    free(dataList);

    return 0;
}