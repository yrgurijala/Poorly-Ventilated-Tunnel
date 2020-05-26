//  Yashwanth Reddy Gurijala
//  COSC 3360 - Operating System Fundamentals
//  Due Date - 4/29/2020
//  
//  Description:
//   A car tunnel is so poorly ventilated that it has become
//   necessary to restrict:
//     1. The number of northbound cars in the tunnel,
//     2. The number of southbound cars in the tunnel,
//     3. The total number of cars in the tunnel.
//   This C++ code Simulates the enforcement of these 
//   restrictions using Pthread mutexes and condition variables
//
//  Yashwanth_Gurijala_Assignment3.cpp
//
//  Created by Yashwanth Reddy Gurijala on 4/29/2020.
//  Copyright © 2020 Yashwanth Reddy Gurijala. All rights reserved.
//

#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <string.h>
#include <signal.h>
#include <sys/types.h>
using namespace std;

static int maxCarsInTunnel;
static int maxNCarsInTunnel;
static int maxSCarsInTunnel;

static int currentCarsInTunnel;
static int currentNCarsInTunnel;
static int currentSCarsInTunnel;

static int totalCars;
static int totalNCars;
static int totalSCars;
static int totalCarsThatWaited;

static pthread_mutex_t mainAccess = PTHREAD_MUTEX_INITIALIZER;
static pthread_cond_t ok = PTHREAD_COND_INITIALIZER;

struct carDetails 
{
    int carNumber;
    int travelTime;
    bool didItWait;
};

void* northBound(void* car) 
{
    struct carDetails* carPointer = (struct carDetails*) car;
    int tunnelTime = carPointer->travelTime;
    int carNum = carPointer->carNumber;
    bool wait = carPointer->didItWait;

    pthread_mutex_lock(&mainAccess);
    cout << "Northbound car # " << carNum << " arrives at the tunnel. \n";

    while (currentCarsInTunnel >= maxCarsInTunnel || currentNCarsInTunnel >= maxNCarsInTunnel)
    {
        if (wait == false)
        {
            wait = true;
            totalCarsThatWaited++;
            cout << "-- Northbound car # " << carNum << " has to wait. \n";
        }

        pthread_cond_wait(&ok, &mainAccess);
    }

    currentNCarsInTunnel++;
    currentCarsInTunnel++;
    cout << "Northbound car # " << carNum << " enters the tunnel. \n";

    pthread_cond_signal(&ok);
    pthread_mutex_unlock(&mainAccess);

    sleep(tunnelTime);

    pthread_mutex_lock(&mainAccess);

    cout << "Northbound car # " << carNum << " exits the tunnel. \n";

    currentNCarsInTunnel--;
    currentCarsInTunnel--;

    pthread_cond_broadcast(&ok);
    pthread_mutex_unlock(&mainAccess);
}

void* southBound(void* car) 
{
    struct carDetails* carPointer = (struct carDetails*) car;
    int tunnelTime = carPointer->travelTime;
    int carNum = carPointer->carNumber;
    bool wait = carPointer->didItWait;

    pthread_mutex_lock(&mainAccess);
    cout << "Southbound car # " << carNum << " arrives at the tunnel. \n";

    while (currentCarsInTunnel >= maxCarsInTunnel || currentSCarsInTunnel >= maxSCarsInTunnel)
    {
        if (wait == false) 
        {
            wait = true;
            totalCarsThatWaited++;
            cout << "-- Southbound car # " << carNum << " has to wait. \n";
        }

        pthread_cond_wait(&ok, &mainAccess);
    }

    currentSCarsInTunnel++;
    currentCarsInTunnel++;
    cout << "Southbound car # " << carNum << " enters the tunnel. \n";

    pthread_cond_signal(&ok);
    pthread_mutex_unlock(&mainAccess);

    sleep(tunnelTime);

    pthread_mutex_lock(&mainAccess);
    cout << "Southbound car # " << carNum << " exits the tunnel. \n";

    currentSCarsInTunnel--;
    currentCarsInTunnel--;

    pthread_cond_broadcast(&ok);
    pthread_mutex_unlock(&mainAccess);
}

int main() 
{
    struct carDetails carsList;
    pthread_t tid[128];
    int counter = 0;

    cin >> maxCarsInTunnel;
    cin >> maxNCarsInTunnel;
    cin >> maxSCarsInTunnel;

    cout << "Maximum number of cars in the tunnel: " << maxCarsInTunnel << "\n";
    cout << "Maximum number of northbound cars: " << maxNCarsInTunnel << "\n";
    cout << "Maximum number of southbound cars: " << maxSCarsInTunnel << "\n";

    int arrivalTime;
    char direction;
    int traversalTime;

    while (cin) 
    {
        cin >> arrivalTime;
        cin >> direction;
        cin >> traversalTime;

        if (!cin)
            break;

        sleep(arrivalTime);

        if (direction == 'S' || direction == 's') 
        {
            totalSCars++;
            carsList.carNumber = totalSCars;
            carsList.travelTime = traversalTime;
            carsList.didItWait = false;
            pthread_create(&tid[counter], NULL, southBound, (void*)&carsList);
        }
        else 
        {
            totalNCars++;
            carsList.carNumber = totalNCars;
            carsList.travelTime = traversalTime;
            carsList.didItWait = false;
            pthread_create(&tid[counter], NULL, northBound, (void*)&carsList);
        }

        counter++;
    }

    for (int i = 0; i < counter; i++)
        pthread_join(tid[i], NULL);

    totalCars = totalNCars + totalSCars;

    cout << totalNCars << " northbound car(s) crossed the tunnel." << endl;
    cout << totalSCars << " southbound car(s) crossed the tunnel." << endl;
    cout << totalCarsThatWaited << " car(s) had to wait." << endl;

    return 0;
}
