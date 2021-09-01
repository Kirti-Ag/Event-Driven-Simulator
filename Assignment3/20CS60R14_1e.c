/* Name: Task1_e.c
      Creator: Kirti Agarwal (kirtiag@kgpian.iitkgp.ac.in)
      Date: Oct 4, 2020
      Description: In this Program, decision of which facility should be duplicated for maximum throughput is
                    taken.
                    Throughput is calculated, for each duplication case, as follows:
                    (Total number of patients serviced in 10 days)/(Total time for which hospital is operational for 10 days).
                    For whichever facility duplication, we get the maximum throughput, that is to be duplicated.
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
    int duplicate;
    int duplicatetime;
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

MinHeap EventHeap[1000];
int n = 0;
Facility facility[6];
Package package[7];
FacilityQueueDet facilityqueuedet[6];
int totalPatientCount=0;
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
void Service(int, MinHeap,int);
void deQueue(int,int);
void Initialization(char **);
void CreateFirstArrival(int, int,int *);
int max(int, int);
void NewDay();
void EndOfPackage(int , MinHeap ,int);

/*
* Initialization()- Initializing the Packages and Facility data for the given values and program logic
*/

void Initialization(char **argv)
{
    //initialize facility queue
    int i;
    for (i = 0; i < 6; i++)
    {
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
    package[1].meanAT = atoi(argv[1]);
    package[2].meanAT = atoi(argv[2]);
    package[3].meanAT = atoi(argv[2]);
    package[4].meanAT = atoi(argv[3]);
    package[5].meanAT = atoi(argv[4]);
    package[6].meanAT = atoi(argv[5]);
}
/*
*    NewDay()- Re-initialize facility queue, time and state in the beginning of each day
*/
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
*   InsertMinHeap()- Insert event node into the minheap
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
*   DelMinHeap()- Delete node with minimum timestamp from the heap
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
*   Adjust()- Adjusts the newly inserted node in the heap to its correct position
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
*   RandomNumberGenerator()- Generates random number in the range [0.1,0.7]
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
*   DeltaCalculator()- Calculates delta for the mean received in the argument. 
*                        Returns Delta value
*/
int DeltaCalculator(int mean)
{
    int t = (int)(-1) * ((log(RandomNumberGenerator())) * mean);
    return t;
}

/*
*   CreateFirstArrival()- Creates 1st arrival events for the packagetype arg and inserts it into minheap.
*/
void CreateFirstArrival(int packagetype, int meanAT , int *firstAT)
{
    MinHeap temp; 
    temp.packagetype = packagetype;
    temp.lastfacilityused = -1;
    temp.eventtype = 'A';
    temp.registcomptime = -1;
    temp.timestamp = package[packagetype].registStart + DeltaCalculator(meanAT);
    if(temp.timestamp<*firstAT){
        *firstAT= temp.timestamp;
    }
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
* max()- Return Max of the 2 input arg values.
*/
int max(int a, int b)
{
    int result = (a > b) ? a : b;
    return result;
}
int min(int a, int b)
{
    int result = (a < b) ? a : b;
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
void deQueue(int facnum, int dupfac)
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
    Service(facnum, patient, dupfac);
}

/*
*   Service()- Service patient for the facility 'facnum' 
*/
void Service(int facnum, MinHeap patient, int dupfac)
{

    if (facnum!=dupfac && facility[facnum].state == 0)
    {
        facility[facnum].state = 1;
        int delta = DeltaCalculator(facility[facnum].meanST);
        facility[facnum].utiltime += delta;
        patient.eventtype = 'S';
        patient.timestamp = delta + max(facility[facnum].time, patient.timestamp);
        if (patient.timestamp <= 1800)//accept service only if it can be finished before 6am next morning
        {
            if (facnum == 0)
                patient.registcomptime = patient.timestamp;
            facility[facnum].time = patient.timestamp;
            patient.lastfacilityused = facnum;
            InsertMinHeap(patient);
        }
    }
    //If this facility has a duplicate
    else if(facnum==dupfac && (facility[facnum].state==0 || facility[facnum].state==1)){
        facility[facnum].state ++;
        int delta = DeltaCalculator(facility[facnum].meanST);
        patient.eventtype = 'S';
        if(facility[facnum].time < facility[facnum].duplicatetime){
            patient.timestamp=delta + max(facility[facnum].time,patient.timestamp);
        }
        else{
            patient.timestamp=delta + max(facility[facnum].duplicatetime,patient.timestamp);
        }

        if(patient.timestamp<=1800){
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
void EndOfPackage(int facnum, MinHeap patient, int dupfac)
{
    facility[facnum].state = 0;
    deQueue(facnum, dupfac);
    package[patient.packagetype].usercount++;
    package[patient.packagetype].totalusertimespent += patient.timestamp - patient.registcomptime;
    totalPatientCount++;
}

int main(int argc, char **argv)
{
    if (argc < 6)
    {
        printf("Invalid Input");
        exit(0);
    }
    Initialization(argv);
    srand(time(0));
    //Checking throughput by duplicating each facility for 10 days one by one.
    double maxThroughput=INT_MIN;  int toBeDupFac=-1;
    for (int dupfac = 1; dupfac <= 5; dupfac++)
    {
        int totalOpenTime=0; totalPatientCount=0;
        for (int days = 1; days <= 10; days++)
        {
            int firstAT=INT_MAX; int lastClosing=INT_MIN;
            NewDay();
            //Create 1st arrivals for each package
            
            for (int i = 1; i <= 6; i++)
                CreateFirstArrival(i, package[i].meanAT, &firstAT);
            while (n > 0)
            {
                MinHeap patient = DelMinHeap();
                switch (patient.eventtype)
                {
                case 'A':
                    CreateNewArrival(patient.packagetype, patient.timestamp);
                    Service(0, patient,dupfac);
                    break;

                case 'S':
                    if (patient.lastfacilityused == 0)
                    {
                        facility[0].state = 0;
                        deQueue(0,dupfac);
                    }
                    switch (patient.packagetype)
                    {
                    case 1:
                        if (patient.lastfacilityused == 0)
                        {
                            Service(1, patient,dupfac);
                            break;
                        }
                        if (patient.lastfacilityused == 1)
                        {
                            facility[1].state = 0;
                            deQueue(1,dupfac);
                            Service(2, patient,dupfac);
                            break;
                        }
                        if (patient.lastfacilityused == 2)
                        {
                            facility[2].state = 0;
                            deQueue(2,dupfac);
                            Service(3, patient,dupfac);
                            break;
                        }
                        if (patient.lastfacilityused == 3)
                        {
                            EndOfPackage(3, patient,dupfac);
                            break;
                        }
                        break;
                    case 2:
                        if (patient.lastfacilityused == 4)
                        {
                            EndOfPackage(4, patient,dupfac);
                            break;
                        }
                        Service(4, patient,dupfac);
                        break;
                    case 3:
                        if (patient.lastfacilityused == 5)
                        {
                            EndOfPackage(5, patient,dupfac);
                            break;
                        }
                        Service(5, patient,dupfac);
                        break;
                    case 4:
                        if (patient.lastfacilityused == 1)
                        {
                            EndOfPackage(1, patient,dupfac);
                            break;
                        }
                        Service(1, patient,dupfac);
                        break;
                    case 5:
                        if (patient.lastfacilityused == 2)
                        {
                            EndOfPackage(2, patient,dupfac);
                            break;
                        }
                        Service(2, patient,dupfac);
                        break;
                    case 6:
                        if (patient.lastfacilityused == 0)
                        {
                            Service(1, patient,dupfac);
                            break;
                        }
                        if (patient.lastfacilityused == 1)
                        {
                            facility[1].state = 0;
                            deQueue(1,dupfac);
                            Service(2, patient,dupfac);
                            break;
                        }
                        if (patient.lastfacilityused == 2)
                        {
                            EndOfPackage(2, patient,dupfac);
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
            for (int i = 1; i < 6; i++){
                facility[i].closingtime+=facility[i].time;
                if(facility[i].time>lastClosing) lastClosing=facility[i].time;
            }
            // printf("\n\n Lastclosingtime:%d  First AT: %d \n",lastClosing,firstAT);
            totalOpenTime+=lastClosing-firstAT;
                // facility[i].closingtime += facility[i].time;

        } //end of 10 days
        // printf("\n Total no of patient in 10 days: %d \n TotalOpentime :%d", totalPatientCount,totalOpenTime);
        double throughput=((float)totalPatientCount)/((float)totalOpenTime);
        // printf("\n throughput: %f",throughput);
        if(throughput>maxThroughput){
            maxThroughput=throughput;
            toBeDupFac=dupfac;
        }
    }//end of dupfac loop
    printf("\n Facility%d is to be duplicated for maximum throughput of:%f",toBeDupFac,maxThroughput);
}
