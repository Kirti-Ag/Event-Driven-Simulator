
/* Name: 20CS60R14_1d.c
      Creator: Kirti Agarwal (kirtiag@kgpian.iitkgp.ac.in)
      Date: Oct 4, 2020
      Description: In this Program, we check for all possible sequence of facilities in Package 1 and Package 6.
                    We select the sequence for which  average total time spent by all patients(for 10 days) in the 
                    hospital is minimum.
*/
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>

struct MinHeap
{
    int packagetype;
    int lastfacilityused;
    int registcomptime;
    char eventtype;
    int timestamp;
};
typedef struct MinHeap MinHeap;
struct Facility
{
    int time;
    int state;
    int meanST;
    int utiltime;
    int closingtime;
};
typedef struct Facility Facility;
struct Package
{
    int nooffac;
    int registStart;
    int registEnd;
    int meanAT;
    int usercount;
    int totalusertimespent;
};
typedef struct Package Package;
struct Node
{
    MinHeap data;
    struct Node *next;
};
typedef struct Node Node;
struct FacilityQueueDet
{
    Node *Front;
    Node *Rear;
};
typedef struct FacilityQueueDet FacilityQueueDet;
enum FacilityName
{
    REGISTRATION,
    ECG,
    ECHOCARDIOGRAM,
    TREADMILL,
    MRI,
    CT
};
MinHeap EventHeap[1000];
int n = 0;
Facility facility[6];
Package package[7];
FacilityQueueDet facilityqueuedet[6];
int P1[6][3];
int P2[2][2];
int totalusertime = 0;
int totalusercount = 0;
int DeltaCalculator(int);
double RandomNumberGenerator();
void InsertMinHeap(MinHeap);
void Copy2MinHeapNodes(int, int);
void CopyConstantToMinHeap(MinHeap, int);
MinHeap DelMinHeap();
void Adjust(int);
void BuildMinHeap();
void CreateNewArrival(int, int);
void enQueue(int, MinHeap);
void Service(int, MinHeap);
void deQueue(int);
void Initialization(char **);
void CreateFirstArrival(int, int);
int max(int, int);
void NewDay();
const char *getFacilityName(enum FacilityName);

//function to store different permutations of packages in 2-D array
void StorePermutation(int arr[], int size, int *count)
{
    int i, j;
    for (i = 0; i < size; i++)
    {
        if (size == 3)
            P1[*count][i] = arr[i];
        if (size == 2)
            P2[*count][i] = arr[i];        
    }
    *count = *count + 1;
}

//function to swap the variables
void swap(int *a, int *b)
{
    int temp;
    temp = *a;
    *a = *b;
    *b = temp;
}

//permutation function
void permutation(int *arr, int start, int end, int *count)
{
    if (start == end)
    {
        StorePermutation(arr, end + 1, count);
        return;
    }
    int i;
    for (i = start; i <= end; i++)
    {        
        swap((arr + i), (arr + start));
        permutation(arr, start + 1, end, count);
        swap((arr + i), (arr + start));
    }
}
void Initialization(char **argv)
{
    //initialize facility queue
    int i;
    for (i = 0; i < 6; i++)
    {
        facilityqueuedet[i].Front = NULL;
        facilityqueuedet[i].Rear = NULL;
        facility[i].time = 0;
        facility[i].state = 0;
        facility[i].utiltime = 0;
        facility[i].closingtime = 0;
    }
    for (i = 1; i <= 6; i++)
    {
        package[i].totalusertimespent = 0;
        package[i].usercount = 0;
    }
   
    facility[0].meanST = 5;
    facility[1].meanST = 10;
    facility[2].meanST = 20;
    facility[3].meanST = 40;
    facility[4].meanST = 40;
    facility[5].meanST = 30;
    //Package details Initialization
     package[1].meanAT = atoi(argv[1]);
    package[2].meanAT = atoi(argv[2]);
    package[3].meanAT = atoi(argv[2]);
    package[4].meanAT = atoi(argv[3]);
    package[5].meanAT = atoi(argv[4]);
    package[6].meanAT = atoi(argv[5]);
    package[1].nooffac = 3;
    package[1].registStart = 360;
    package[1].registEnd = 660;
    package[2].nooffac = 1;
    package[2].registStart = 480;
    package[2].registEnd = 900;
    package[3].nooffac = 1;
    package[3].registStart = 480;
    package[3].registEnd = 900;
    package[4].nooffac = 1;
    package[4].registStart = 360;
    package[4].registEnd = 1020;
    package[5].nooffac = 1;
    package[5].registStart = 360;
    package[5].registEnd = 900;
    package[6].nooffac = 2;
    package[6].registStart = 360;
    package[6].registEnd = 900;
    int num1[3] = {1, 2, 3};
    int pcount = 0;
    permutation(num1, 0, 2, &pcount);   
    pcount = 0;
    permutation(num1, 0, 1, &pcount);
    
}

//Re-Initializes queues, time and state of facilities
void NewDay()
{
    n = 0;
    for (int i = 0; i < 6; i++)
    {
        facilityqueuedet[i].Front = NULL;
        facilityqueuedet[i].Rear = NULL;
        facility[i].time = 0;
        facility[i].state = 0;
    }
}
/* 
*      InsertMinHeap()- Inserts event nodes into the minheap
*/
void InsertMinHeap(MinHeap node)
{
    n++;
    int i = n;
    int item = node.timestamp;
    while (i > 1 && EventHeap[i / 2].timestamp > item)
    {
        EventHeap[i] = EventHeap[i / 2];
        i = i / 2;
    }
    EventHeap[i] = node;
}

/* 
*   DelMinHeap()- Delete the node with minimum timestamp from minHeap
*/
MinHeap DelMinHeap()
{
    MinHeap Node = EventHeap[1];
    EventHeap[1] = EventHeap[n];
    n--;
    Adjust(1);
    return (Node);
}
/* 
*   Adjust()- Adjusts the newly inserted node in heap to its correct position
*/
void Adjust(int i)
{
    MinHeap Node = EventHeap[i];
    int item = EventHeap[i].timestamp;
    int j = i * 2;
    while (j <= n)
    {
        if (j < n && EventHeap[j].timestamp > EventHeap[j + 1].timestamp)
            j++;
        if (EventHeap[j].timestamp >= item)
            break;
        EventHeap[j / 2] = EventHeap[j];
        j = j * 2;
    }
    EventHeap[j / 2] = Node;
}
/*
*   RandomNumberGenerator()- Generating Random number in the range [0.1,0.7] and returning.
*/
double RandomNumberGenerator()
{
    double value = 0, num;
    do
    {
        num = (double)rand() / (double)((unsigned)RAND_MAX + 1);
    } while (num > 0.7 || num < 0.1);
    return num;
}

/*
*   DeltaCalculator()- Calculate and Return the Delta value for the generated random number.
*/

int DeltaCalculator(int mean)
{
    int t = (int)(-1) * ((log(RandomNumberGenerator())) * mean);
    return t;
}

/*
*   CreateFirstArrival()- Creates 1st arrival events for the packagetype arg and inserts it into minheap.
*/
void CreateFirstArrival(int packagetype, int meanAT)
{
    MinHeap temp;
    temp.packagetype = packagetype;
    temp.lastfacilityused = -1;
    temp.eventtype = 'A';
    temp.registcomptime = -1;
    temp.timestamp = package[packagetype].registStart + DeltaCalculator(meanAT);
    InsertMinHeap(temp);
}

/*
* CreateNewArrival()- Creates subsequent arrival event for packagetype arg and inserts into minheap
*/
void CreateNewArrival(int packagetype, int timestamp)
{

    int delta;
    if (packagetype == 4)
    {
        if (timestamp >= 360 && timestamp <= 720)
            delta = DeltaCalculator(package[packagetype].meanAT);
        if (timestamp > 720 && timestamp <= package[packagetype].registEnd)
            delta = DeltaCalculator(package[packagetype].meanAT);
    }
    else
        delta = DeltaCalculator(package[packagetype].meanAT);
    MinHeap Node;
    Node.packagetype = packagetype;
    Node.lastfacilityused = -1;
    Node.eventtype = 'A';
    Node.registcomptime = -1;
    Node.timestamp = timestamp + delta;

    if (Node.timestamp >= package[packagetype].registStart && Node.timestamp <= package[packagetype].registEnd)
        InsertMinHeap(Node);
}

/*
*   minToTime()- converts minute to hour:min 
*/
int max(int a, int b)
{
    int result = (a > b) ? a : b;
    return result;
}

/*
*   enQueue()- Inserts patient into facility queue if facility's state is busy
*/
void enQueue(int facnum, MinHeap patient)
{
    Node *newnode = (Node *)malloc(sizeof(Node));
    if (newnode == NULL)
    {
        printf("\nOverflow: Memory Full");
        exit(1);
    }
    newnode->next = NULL;
    newnode->data = patient;
    if (facilityqueuedet[facnum].Front == NULL)
    {
        facilityqueuedet[facnum].Front = newnode;
        facilityqueuedet[facnum].Rear = newnode;
    }
    else
    {
        (facilityqueuedet[facnum].Rear)->next = newnode;
        facilityqueuedet[facnum].Rear = newnode;
    }
}

/*
*   deQueue()- Removes patient from the facility queue when another patient has finished using it.
*/
void deQueue(int facnum)
{

    if (facilityqueuedet[facnum].Front == NULL)
    {

        return;
        // exit(1);
    }
    MinHeap patient = (facilityqueuedet[facnum].Front)->data;
    Node *temp = facilityqueuedet[facnum].Front;
    if (facilityqueuedet[facnum].Front == facilityqueuedet[facnum].Rear)
    {
        facilityqueuedet[facnum].Front = NULL;
        facilityqueuedet[facnum].Rear = NULL;
    }
    else
        facilityqueuedet[facnum].Front = (facilityqueuedet[facnum].Front)->next;
    free(temp);
    temp = NULL;
    Service(facnum, patient);
}

/*
*   Service()- Service patient for the facility 'facnum' 
*/ 
void Service(int facnum, MinHeap patient)
{

    if (facility[facnum].state == 0)
    {
        facility[facnum].state = 1;
        int delta = DeltaCalculator(facility[facnum].meanST);
        facility[facnum].utiltime += delta;
        patient.eventtype = 'S';
        patient.timestamp = delta + max(facility[facnum].time, patient.timestamp);
        if (patient.timestamp <= 1800)
        {
            if (facnum == 0)
                patient.registcomptime = patient.timestamp;
            facility[facnum].time = patient.timestamp;
            patient.lastfacilityused = facnum;
            InsertMinHeap(patient);
        }
    }
    else
    {
        enQueue(facnum, patient);
    }
}

/*
*   EndOfPackage()- Executed at the end of package for a patient. Free the facility 'facnum' state and
                    dequeue if anyone waiting in it's queue. Increment usercount and timespent in the 
                    respective package.
*/
void EndOfPackage(int facnum, MinHeap patient)
{
    facility[facnum].state = 0;
    deQueue(facnum);
    // package[patient.packagetype].usercount++;
    // package[patient.packagetype].totalusertimespent += patient.timestamp - patient.registcomptime;
    totalusercount++;
    totalusertime += patient.timestamp - patient.registcomptime;
}

/*
* getFacilityName()- convert integer value to facility name from enum definition
*/
const char *getFacilityName(enum FacilityName fac)
{
    switch (fac)
    {
    case ECG:
        return "ECG";
    case ECHOCARDIOGRAM:
        return "ECHOCARDIOGRAM";
    case TREADMILL:
        return "TREADMILL";
    case MRI:
        return "MRI";
    case CT:
        return "CT";
    }
}
int main(int argc, char **argv)
{
    if(argc<6){
        printf("Invalid Input");
        exit(0);
    }
    char *originalP1="123";
    char *originalP2="12";
    Initialization(argv);
    srand(time(0));

    int bestAvgTimeSpent = INT_MAX;
    int p1Sequence = -1;
    int p2Sequence = -1;
    for (int p1 = 0; p1 < 6; p1++)
    {
        for (int p2 = 0; p2 < 2; p2++)
        {

            for (int days = 1; days <= 10; days++)
            {
                NewDay();
                for (int i = 1; i <= 6; i++)
                    CreateFirstArrival(i, package[i].meanAT);
                //Until Event Heap Empty
                while (n > 0)
                {
                    MinHeap patient = DelMinHeap();
                    switch (patient.eventtype)
                    {
                    case 'A':
                        CreateNewArrival(patient.packagetype, patient.timestamp);
                        Service(0, patient);
                        break;

                    case 'S':
                        if (patient.lastfacilityused == 0)
                        {
                            facility[0].state = 0;
                            deQueue(0);
                        }
                        switch (patient.packagetype)
                        {
                        case 1:
                            if (patient.lastfacilityused == 0)
                            {
                                Service(P1[p1][0], patient);
                                break;
                            }
                            if (patient.lastfacilityused == P1[p1][0])
                            {
                                facility[P1[p1][0]].state = 0;
                                deQueue(P1[p1][0]);
                                Service(P1[p1][1], patient);
                                break;
                            }
                            if (patient.lastfacilityused == P1[p1][1])
                            {
                                facility[P1[p1][1]].state = 0;
                                deQueue(P1[p1][1]);
                                Service(P1[p1][2], patient);
                                break;
                            }
                            if (patient.lastfacilityused == P1[p1][2])
                            {
                                EndOfPackage(P1[p1][2], patient);
                                break;
                            }
                            break;
                        case 2:
                            if (patient.lastfacilityused == 4)
                            {
                                EndOfPackage(4, patient);
                                break;
                            }
                            Service(4, patient);
                            break;
                        case 3:
                            if (patient.lastfacilityused == 5)
                            {
                                EndOfPackage(5, patient);
                                break;
                            }
                            Service(5, patient);
                            break;
                        case 4:
                            if (patient.lastfacilityused == 1)
                            {
                                EndOfPackage(1, patient);
                                break;
                            }
                            Service(1, patient);
                            break;
                        case 5:
                            if (patient.lastfacilityused == 2)
                            {
                                EndOfPackage(2, patient);
                                break;
                            }
                            Service(2, patient);
                            break;
                        case 6:
                            if (patient.lastfacilityused == 0)
                            {
                                Service(P2[p2][0], patient);
                                break;
                            }
                            if (patient.lastfacilityused == P2[p2][0])
                            {
                                facility[P2[p2][0]].state = 0;
                                deQueue(P2[p2][0]);
                                Service(P2[p2][1], patient);
                                break;
                            }
                            if (patient.lastfacilityused == P2[p2][1])
                            {
                                EndOfPackage(P2[p2][1], patient);
                                break;
                            }
                            break;
                        default:
                            break;
                        }
                    default:
                        break;
                    }
                }
                for (int i = 1; i < 6; i++)
                    facility[i].closingtime += facility[i].time;
            } //end of 10 days
            float avgTime = totalusertime / totalusercount;
            if (avgTime < bestAvgTimeSpent)
            {
                bestAvgTimeSpent = avgTime;
                p1Sequence = p1;
                p2Sequence = p2;
            }
            totalusercount = 0;
            totalusertime = 0;
        } //perm of p2
    }     //perm of p1
    int i;
    int result=0;
    for(i=0;i<3;i++){
        if(P1[p1Sequence][i]!=atoi(originalP1+i)) result=1;
    }
    for(i=0;i<2;i++){
        if(P2[p2Sequence][i]!=atoi(originalP2+i)) result=1;
    }
    if(result==1) printf("\n Yes, we can improve average total time spent by all patients by changing the sequence of tests in the packages");
    else printf("\n No, we cannot improve average total time spent by all patients by changing the sequence of tests in the packages");
    printf("\n Best Sequence for Package 1 is: ");
    for (i = 0; i < 2; i++)
    {
        printf("%s -> ", getFacilityName(P1[p1Sequence][i]));
    }
    printf("%s ", getFacilityName(P1[p1Sequence][i]));
    printf("\n Best Sequence for Package 2 is: ");
    printf("%s -> ", getFacilityName(P2[p2Sequence][0]));
    printf("%s ", getFacilityName(P2[p2Sequence][1]));
}
