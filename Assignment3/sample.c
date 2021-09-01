#include<stdio.h>
#include<math.h>
#include<stdlib.h>
#include<time.h>

double RandomNumberGenerator()
{
    double value = 0, num;
    do
    {
        num = (double)rand() / (double)((unsigned)RAND_MAX + 1);
        // value = (int)(num * 100 + .5);
    } while (num > 0.9 || num < 0.1);
    printf("\n R=%f  ", num);
    return num;
}
void DeltaCalculator(int mean)
{
    float ran=log(RandomNumberGenerator());
    int t = (int)(-1) * (ran * mean);
    printf(" Log(R)=%f",ran);
    printf(" Delta=%d", t);
    // return t;
}
int main(){
    printf("\n Taking Random Number in the range (0,1) .");
    printf("\n For Mean=40, Range of Values are:");
    srand(time(0));
    for(int i=0;i<10;i++)
        DeltaCalculator(40);
}