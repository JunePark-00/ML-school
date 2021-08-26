#pragma warning(disable:4996)
#define _CRT_SECURE_NO_WARNINGS 
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define INSTANCES 414
#define MAX_LEN 100

#define ABS(x) (((x) < 0) ? -(x) : (x))
#define MIN(a,b) (((a)<(b)) ? (a):(b))
#define MAX(a,b) (((a)>(b)) ? (a):(b))

double th0, th3;

typedef struct Data {

    double distance;
    double unitPrice;
    char x_axis[MAX_LEN];
    char y_axis[MAX_LEN];

}Data;

Data* dataList;

double LinearHypothesis(const double x3) { // 선형 가설 모델

    return th0 + th3 * x3;

}

double mse() { // cost function (Mean Squared Error)

    double error_sum = 0.0;

    for (int i = 0; i < INSTANCES; ++i) {

        const double error = dataList[i].unitPrice - LinearHypothesis(dataList[i].distance);

        error_sum += error * error;

    }

    return error_sum / (double)INSTANCES * 0.5;

}

void updateParameter(const double learning_rate) { // 파라미터 값을 업데이트 해주는 함수
    double dth0 = 0.0, dth3 = 0.0;

    for (int i = 0; i < INSTANCES; ++i) {
        const double hypo_pr = LinearHypothesis(dataList[i].distance) - dataList[i].unitPrice;

        const double dh_dth0 = 1.0;
        const double dh_dth3 = dataList[i].distance;

        dth0 += hypo_pr * dh_dth0;
        dth3 += hypo_pr * dh_dth3;
    }
    th0 -= learning_rate * dth0 / INSTANCES;
    th3 -= learning_rate * dth3 / INSTANCES;

}


int main() {
    dataList = (Data*)malloc(sizeof(Data) * INSTANCES);

    FILE* file;
    file = fopen("X3.txt", "r"); // x3값에 해당하는 자료들
    for (int i = 0; i < INSTANCES; ++i) {
        fgets(dataList[i].x_axis, MAX_LEN, file);
        dataList[i].distance = atof(dataList[i].x_axis);
    }

    FILE* file2;
    file2 = fopen("price.txt", "r"); // x값에 따른, 집의 단위 가격 자료들
    for (int i = 0; i < INSTANCES; ++i) {
        fgets(dataList[i].y_axis, MAX_LEN, file2);
        dataList[i].unitPrice = atof(dataList[i].y_axis);
    }

    fclose(file);
    fclose(file2);

    // 인풋 데이터의 최댓값과 최솟값을 찾는 과정
    double distance_min = (double)1e8, distance_max = -(double)1e8;  // 1e8: 표준 과학 개념. y축의 전체 축척 비율을 나타냄
    double price_min = (double)1e8, price_max = -(double)1e8;

    for (int i = 0; i < INSTANCES; ++i) {
        distance_min = MIN(distance_min, dataList[i].distance);
        distance_max = MAX(distance_max, dataList[i].distance);

        price_min = MIN(price_min, dataList[i].unitPrice);
        price_max = MAX(price_max, dataList[i].unitPrice);
    }

    // 거리와 집값의 범위를 비슷하게 바꿔줌(0에서 1사이로)
    double distance_scale = 1.0 / (distance_max - distance_min);
    double price_scale = 1.0 / (price_max - price_min);
    for (int i = 0; i < INSTANCES; ++i) {
        dataList[i].distance -= distance_min;
        dataList[i].unitPrice -= price_min;
        dataList[i].distance *= distance_scale;
        dataList[i].unitPrice *= price_scale;
    }

    // 가설 초기화
    th0 = 0.0;
    th3 = 0.0;

    // gradient descent

    double MSE_prev = 1e8;
    double th0_prev = th0;
    double th3_prev = th3;
    double final_th0 = 0.0, final_th3 = 0.0;

    int i;
    for (i = 0; i < 450; ++i) {
        updateParameter(1.0);

        const double MSE = mse();
        const double dMSE = MSE * (price_max - price_min) * (price_max - price_min);
        const double dth0 = (th0 - th3 * distance_min * distance_scale + price_min * price_scale) / price_scale;
        const double dth3 = th3 * distance_scale / price_scale;

        if (ABS(MSE_prev) <= ABS(MSE)) { // 오차가 다시 커지면 최적점을 찾은 것으로 간주하고 실행을 중단함
            
            th0 = th0_prev;
            th3 = th3_prev;

            break;
        }

        // 변수 백업
        MSE_prev = MSE;
        th0_prev = th0;
        th3_prev = th3;

        printf("J(th0: %lf, th3: %lf) = %lf(오차값)\n", dth0, dth3, dMSE);
        final_th0 = dth0;
        final_th3 = dth3;
    }
    printf("\n적절한 theta0값: %lf\n적절한 theta3값: %lf\n", final_th0, final_th3);

    printf("절차반복 수행 횟수: %d \n", i); // 절차반복 수행 횟수

    free(dataList);

    return 0;
}

