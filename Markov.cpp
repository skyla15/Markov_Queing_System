#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <math.h>
#include <stdbool.h>
#define norm_rand()  (rand()/(RAND_MAX + 1.0))

#define SIM_TIME 200000.0

double expntl(double);
int min_departure(double arr[], int capacity);// 인덱스를 이용하여, 가장 빨리 떠나는 Customer를 가져오기 위한 함수
int idle_server(double arr[], int capacity);// 서버의 Capacity 확인
double Ta=200.0, Ts=100.0;


int main() {
    int n;
    // Ta : Time of Arrival
    // next_arrival_tiem : 지난시간 + Exponential(Ta)
    // next
    // Ts : Time of service
    double simulation_time=SIM_TIME;
    double next_arrival_time, next_departure_time, elapsed_time;
    double B, C, L, s, tb, lastEventTime, W, X, U;
    n=0; // # of Customer
    next_arrival_time=0.0;
    next_departure_time=simulation_time;
    elapsed_time=0.0;
    B=s=0.0;
    C=0;
    lastEventTime=elapsed_time;
    /*
    std::priority_queue<int, std::vector<int>, std::greater<int> > Td;
    */
    
    double lastBusyTime = 0.0;
    int arrayIndex = 0;
    int nextDepartIndex = 0;
    double custDepartures[10];
    // MM1과 다르게 서버에 있는 Customer들의 Departure Time은 각기 다름
    // 따라서 이들을 저장하여, 빨리 나가는 순서대로 뽑아내기 위하여 배열을 사용
    
    for(int i = 0 ; i < 10 ; i++)
        custDepartures[i] = SIM_TIME; // Customer Departure Time 초기화
    
/* Seed the random-number generator with current time so that the number will be different every time we run.*/
    srand((unsigned)time(NULL));
    
        while (elapsed_time < simulation_time)
        {
            if (next_arrival_time < next_departure_time )
            { /* event 1 : customer arrival */
                
                elapsed_time=next_arrival_time; // New Customer가 도착한 시간
                s = s + n*(elapsed_time-lastEventTime);
                // 각 프로세스가 도착한 시간을 lastEventTime, 현재 프로세스가 도착하는 시간을 기준으로(elapsed_time)으로
                // 그 시간 간격동안 있는 프로세스들의 수와 그 시간 간격을 더함으로써 서비스를 받고있는 n개의 프로세스가 서버에 있는 시간을 더함
                n++; // 시스템 내의 customer 추가
                lastEventTime = elapsed_time; // lastEventTime : 현재 Arrived Customer Time 등록
                next_arrival_time = elapsed_time+expntl(Ta); // Mean Interarrival Time을 Exponential하여 현재 시간에 더해서 구함
               
                if ( n <= 10)
                { // 서버가 커스터머를 받을 수 있다면
                    arrayIndex = idle_server(custDepartures, 10);
                    // 가능한 서버의 인덱스를 가져오고, 해당 인덱스에 현재 Customer의 Departure 시간 저장
                    custDepartures[arrayIndex] = elapsed_time + expntl(Ts);
                    // Customer가 떠나는 시간은 Mean Service Time의 Exponential함수를 통해 설정
                    
                    if ( n == 1)
                    { // 손님이 0명에서 1명이 될 경우, 인덱스에 들어있는 값을 현재 프로세스의 Departure Time으로 설정
                        nextDepartIndex = arrayIndex;
                        next_departure_time = custDepartures[nextDepartIndex];
                        lastBusyTime = elapsed_time; // 서버가 서비스를 시작하는 시간
                    }
                }
            }
            else
            { /* event 2 : customer departure */
                // 서비스를 받고 있는 Customer 의 Departure Time보다 늦는 Arrival TIme을 갖는다는 것은
                // 서비스를 받고 있는 손님의 갯수가 1명이 줄어든다는 의미로, Cumstomer Departure.
                elapsed_time = next_departure_time; // 이전 프로세스의 떠나는 시간까지를 현재 기준 시간으로 잡음
                s+= n*(elapsed_time-lastEventTime);
                // 이전프로세스가 떠나는 시간부터 이전 프로세스가 도착한(떠난)시간 간격 동안, 서비스를 받고 있는 손님의 수를 그 시간간격을 곱합으로써
                // 서버내에서 서비스를 받고 있는 총 손님들의 시간을 더함
                n--;
                // 손님 떠남
                lastEventTime = elapsed_time; // 현재 손님 떠난 시간으로 기록
                C++;
                // 서비스를 받고 떠난 손님의 수 증가
                
                custDepartures[nextDepartIndex] = simulation_time; // 나간 손님 시간이 저장된 인덱스 초기화
                
                if( n > 0 )
                {
                    if ( n >= 10)
                        // 손님이 10개 이상이라면, 그 다음 손님이 떠나기 위해서는, "이전에 온 손님들 중 한 명이 서비스를 다 받은 뒤에" 서비스를 받을 수 있음
                        // 따라서 이전 손님이 나간 현재시간 (elapsed_time)에 서비스를 받는 시간을 더함
                        // 이 시간을 custDepartures 배열의 초기화된 인덱스에 저장
                        custDepartures[nextDepartIndex] = elapsed_time + expntl(Ts);
                    
                    nextDepartIndex = min_departure(custDepartures, 10);
                    // 배열에 저장된 시간 중 가장 빠른 Departure Time을 갖는 인덱스를 가져옴
                    next_departure_time = custDepartures[nextDepartIndex];
                    // 가장 빠른 이 전 손님들 중의 departure_time을 다음 손님이 나가는 시간으로 가져옴
                }
                else // 서버에서 이전에 들어온 마지막 손님이 떠남
                {
                    next_departure_time = simulation_time; // 변수 초기화
                    B += elapsed_time - lastBusyTime;
                    // 서버에 손님이 1명이 들어올 때부터 나갈 때까지 서버는 사용중임.
                    // 따라서 손님이 1명이 들어왔을 때 부터 0명이 될 때까지는 서버가 사용중인 시간.
                }
            }
            printf("n : %d  lastBusyTime : %f   lastEventTime : %f\n", n, lastBusyTime, lastEventTime);
        }
         
    
    X = C / elapsed_time;
    printf("throughput = %f\n", X);
    // elapsed time = T
    
    U = B / elapsed_time;
    printf("utilization = %f\n", U);
    
    L = s / elapsed_time;
    printf("mean customer no. in store = %f\n", L);
    
    W = L/X;
    printf("mean \"Dwell\" time per customer = %f\n", W);
    
    printf("L : %f  W : %f \n", L, W);
    
    
}

double expntl(double x)
{   /* 'expntl' returns a psuedo-random variate from a negative exponential distribution with mean x */
    return(-x*log(norm_rand()));
}

/*******************************************************************************
*       min_departure(double arr[], int capacity)
********************************************************************************
* Function that return the index of the minimum departure time
* - Input: arr (array of departures)
* - Input: capacity (size of the array)
*******************************************************************************/

int min_departure(double arr[], int capacity)
{
    int index = 0;

    for (int i=1; i < capacity; i++)
    {
        if (arr[i] < arr[index])
            index = i;
    }
//    printf(" - INDEX = %d \n", index);
    return index;
}


/*******************************************************************************
*       idle_server(double arr[], int capacity)
********************************************************************************
* Function that return the index of an "idle server"
* It's used to determine to which position of the array we will save the
* departure time of the client (which server is serving the customer)
* - Input: arr (array of departures)
* - Input: capacity (size of the array)
*******************************************************************************/
int idle_server(double arr[], int capacity)
{
    int index = 0;
    bool idle = false;

    for (int i=1; (i < capacity && !idle); i++)
    {
        if (arr[i] == SIM_TIME)
        {
            index = i;
            idle = true;
        }
    }
//    printf(" - INDEX = %d \n", index);
    return index;
}



/*
 Referenced Code 
 https://github.com/lukaswals/queuing-simulators/blob/master/mmc.c
 */
