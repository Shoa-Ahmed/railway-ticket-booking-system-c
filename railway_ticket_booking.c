#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define TOTAL_SEATS 5
#define MAX_TRAINS 3

typedef struct Passenger
{
    char name[50];
    int age;
    int seatNumber;
    char status[15]; // CONFIRMED or WAITLISTED
    struct Passenger* next;
} Passenger;

typedef struct Queue
{
    Passenger* front;
    Passenger* rear;
} Queue;

typedef struct Train {
    int trainID;
    char name[50];
    char source[50];
    char destination[50];
    int totalSeats;
    Passenger* confirmedList;
    Queue waitlistQueue;
    int bookedSeats;
} Train;

Train trains[MAX_TRAINS];


Passenger* createPassenger(char name[], int age, int seatNumber, char status[])
{
    Passenger* p = (Passenger*)malloc(sizeof(Passenger));
    strcpy(p->name, name);
    p->age = age;
    p->seatNumber = seatNumber;
    strcpy(p->status, status);
    p->next = NULL;
    return p;
}

// Queue Operations

void enqueue(Queue* q, Passenger* p)
{
    if (q->rear == NULL)
    {
        q->front = q->rear = p;
    }
    else
    {
        q->rear->next = p;
        q->rear = p;
    }
}


Passenger* dequeue(Queue* q)
{
    if (q->front == NULL) return NULL;

    Passenger* temp = q->front;
    q->front = q->front->next;
    if (q->front == NULL) q->rear = NULL;
    temp->next = NULL;
    return temp;
}

Passenger* addToConfirmedList(Passenger* head, Passenger* p)
{
    if (head == NULL) return p;

    Passenger* temp = head;
    while (temp->next != NULL)
        temp = temp->next;
    temp->next = p;
    return head;
}

// Booking Operations

void bookTicket(int trainIndex, char name[], int age)
{
    Train* t = &trains[trainIndex];

    if (t->bookedSeats < t->totalSeats)
    {
        t->bookedSeats++;
        Passenger* newP = createPassenger(name, age, t->bookedSeats, "CONFIRMED");
        t->confirmedList = addToConfirmedList(t->confirmedList, newP);
        printf("\nTicket booked on %s. %s got seat %d.\n", t->name, name, newP->seatNumber);
    }
    else
    {
        Passenger* waitP = createPassenger(name, age, -1, "WAITLISTED");
        enqueue(&t->waitlistQueue, waitP);
        printf("\nNo seats left on %s. %s added to waitlist.\n", t->name, name);
    }
}

void cancelTicket(int trainIndex, char name[])
{
    Train* t = &trains[trainIndex];
    Passenger *temp = t->confirmedList, *prev = NULL;

    while (temp != NULL && strcmp(temp->name, name) != 0)
    {
        prev = temp;
        temp = temp->next;
    }

    if (temp == NULL)
    {
        printf("\nPassenger not found in confirmed list.\n");
        return;
    }

    int freedSeat = temp->seatNumber;

    printf("\nTicket for %s (Seat %d) cancelled.\n", temp->name, temp->seatNumber);
    t->bookedSeats--;

    if (prev == NULL)
        t->confirmedList = temp->next;
    else
        prev->next = temp->next;

    free(temp);

    if (t->waitlistQueue.front != NULL)
    {
        Passenger* fromWaitlist = dequeue(&t->waitlistQueue);
        t->bookedSeats++;
        fromWaitlist->seatNumber = freedSeat;
        strcpy(fromWaitlist->status, "CONFIRMED");
        t->confirmedList = addToConfirmedList(t->confirmedList, fromWaitlist);
        printf("%s from waitlist is now CONFIRMED with Seat %d.\n", fromWaitlist->name, fromWaitlist->seatNumber);
    }
}

// Display Functions

void displayBookings(Passenger* confirmedList, Queue* waitlistQueue)
{
    Passenger* temp;

    printf("\nConfirmed Passengers:\n");
    if (confirmedList == NULL)
        printf("No confirmed bookings.\n");
    else
    {
        temp = confirmedList;
        while (temp != NULL)
        {
            printf("%s (Age: %d) - Seat %d [%s]\n", temp->name, temp->age, temp->seatNumber, temp->status);
            temp = temp->next;
        }
    }
    printf("\nWaitlisted Passengers:\n");
    if (waitlistQueue->front == NULL)
        printf("No passengers in waitlist.\n");
    else
    {
        temp = waitlistQueue->front;
        while (temp != NULL)
        {
            printf("%s (Age: %d) [%s]\n", temp->name, temp->age, temp->status);
            temp = temp->next;
        }
    }
}

// Train Initialization

void initializeTrains()
{
    trains[0] = (Train){101, "Express A", "Kolkata", "Shimla", TOTAL_SEATS, NULL, {NULL, NULL}, 0};
    trains[1] = (Train){102, "Express B", "Pune", "Kolkata", TOTAL_SEATS, NULL, {NULL, NULL}, 0};
    trains[2] = (Train){103, "Express C", "Hyderabad", "Delhi", TOTAL_SEATS, NULL, {NULL, NULL}, 0};
}

int chooseTrain()
{
    printf("\nAvailable Trains:\n");
    for (int i = 0; i < MAX_TRAINS; i++)
    {
        printf("%d. %s (%s to %s)\n", i + 1, trains[i].name, trains[i].source, trains[i].destination);
    }
    int choice;
    printf("Select a train (1-%d): ", MAX_TRAINS);
    scanf("%d", &choice);
    return choice - 1;
}


int main() {
    int choice;
    char name[50];
    int age;

    initializeTrains();

    do {
        printf("\nRailway Ticket Booking System");
        printf("\n1. Book Ticket");
        printf("\n2. Cancel Ticket");
        printf("\n3. Display Bookings");
        printf("\n4. Exit");
        printf("\nEnter choice: ");
        scanf("%d", &choice);
        switch (choice) {
            case 1:
            {
                int trainIndex = chooseTrain();
                if (trainIndex < 0 || trainIndex >= MAX_TRAINS)
                {
                    printf("Invalid train selection.\n");
                    break;
                }
                printf("Enter Passenger Name: ");
                scanf(" %[^\n]", name);
                printf("Enter Age: ");
                scanf("%d", &age);
                bookTicket(trainIndex, name, age);
                break;
            }

            case 2:
            {
                int trainIndex = chooseTrain();
                if (trainIndex < 0 || trainIndex >= MAX_TRAINS)
                    {
                    printf("Invalid train selection.\n");
                    break;
                    }
                printf("Enter Passenger Name to Cancel: ");
                scanf(" %[^\n]", name);
                cancelTicket(trainIndex, name);
                break;
            }

            case 3:
                for (int i = 0; i < MAX_TRAINS; i++)
                {
                    printf("\n%s (%s to %s):\n", trains[i].name, trains[i].source, trains[i].destination);
                    displayBookings(trains[i].confirmedList, &trains[i].waitlistQueue);
                }
                break;

            case 4:
                printf("\nExiting System.\n");
                break;

            default:
                printf("\nInvalid choice. Try again.\n");
        }
    } while (choice != 4);

    return 0;
}






