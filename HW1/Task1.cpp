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

double LinearHypothesis(const double x3) { // ���� ���� ��

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

void updateParameter(const double learning_rate) { // �Ķ���� ���� ������Ʈ ���ִ� �Լ�
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
    file = fopen("X3.txt", "r"); // x3���� �ش��ϴ� �ڷ��
    for (int i = 0; i < INSTANCES; ++i) {
        fgets(dataList[i].x_axis, MAX_LEN, file);
        dataList[i].distance = atof(dataList[i].x_axis);
    }

    FILE* file2;
    file2 = fopen("price.txt", "r"); // x���� ����, ���� ���� ���� �ڷ��
    for (int i = 0; i < INSTANCES; ++i) {
        fgets(dataList[i].y_axis, MAX_LEN, file2);
        dataList[i].unitPrice = atof(dataList[i].y_axis);
    }

    fclose(file);
    fclose(file2);

    // ��ǲ �������� �ִ񰪰� �ּڰ��� ã�� ����
    double distance_min = (double)1e8, distance_max = -(double)1e8;  // 1e8: ǥ�� ���� ����. y���� ��ü ��ô ������ ��Ÿ��
    double price_min = (double)1e8, price_max = -(double)1e8;

    for (int i = 0; i < INSTANCES; ++i) {
        distance_min = MIN(distance_min, dataList[i].distance);
        distance_max = MAX(distance_max, dataList[i].distance);

        price_min = MIN(price_min, dataList[i].unitPrice);
        price_max = MAX(price_max, dataList[i].unitPrice);
    }

    // �Ÿ��� ������ ������ ����ϰ� �ٲ���(0���� 1���̷�)
    double distance_scale = 1.0 / (distance_max - distance_min);
    double price_scale = 1.0 / (price_max - price_min);
    for (int i = 0; i < INSTANCES; ++i) {
        dataList[i].distance -= distance_min;
        dataList[i].unitPrice -= price_min;
        dataList[i].distance *= distance_scale;
        dataList[i].unitPrice *= price_scale;
    }

    // ���� �ʱ�ȭ
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

        if (ABS(MSE_prev) <= ABS(MSE)) { // ������ �ٽ� Ŀ���� �������� ã�� ������ �����ϰ� ������ �ߴ���
            
            th0 = th0_prev;
            th3 = th3_prev;

            break;
        }

        // ���� ���
        MSE_prev = MSE;
        th0_prev = th0;
        th3_prev = th3;

        printf("J(th0: %lf, th3: %lf) = %lf(������)\n", dth0, dth3, dMSE);
        final_th0 = dth0;
        final_th3 = dth3;
    }
    printf("\n������ theta0��: %lf\n������ theta3��: %lf\n", final_th0, final_th3);

    printf("�����ݺ� ���� Ƚ��: %d \n", i); // �����ݺ� ���� Ƚ��

    free(dataList);

    return 0;
}

