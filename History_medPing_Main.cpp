/*--------------------------------------------------------------
Programmer:	Amy Saad

Summary: This program collects randomized vital signs and
then deletes a chosen record

----------------------------------------------------------------*/

/*--------------------------------------------------------------
INPUT: user inputs a number of seconds

OUTPUT: returns vital records

---------------------------------------------------------------*/

#include "medPing.h"

#include <cassert>
#include <chrono>
#include <thread>

//-----------------------------------------------------------------------
#include <ctime>

// WINDOWS users: uncomment next line
// #include <windows.h>

const long MAX_HISTORY  = 5;	// can store upto (last) MAX_HISTORY sets of vital signs

const long MAX_WAIT_SEC = 4;	// will random pause from 1 to MAX_WAIT_SEC

//========================================
struct oneVitalHistoryRecord
{
    long    nSecs;            //time in seconds
    double	bodyTemp_F;       //temp
    short	pulseRate;        //pulse
    short   glucose;          //glucose level
    short   systolic;         //blood pressure in two values
    short   diastolic;
    short   respRate;

};
//========================================
medPing mp;
// function prototypes for keeping track of HISTORY
// (see bottom of this file where you'll write the definitions)

void AddHistoryRecord(long nSecs, double newTemp, short pulseRate, short glu, short sys,
                      short dia, oneVitalHistoryRecord vitalHistory[ ], long& hmr,
                      short respRate);


const long NOT_FOUND = -1;  // used to indicate failure on linear search

long FindVitalRecord(long nSecs, const oneVitalHistoryRecord vitalHistory[ ], long hmr);

void DeleteHistoryRecord(short recordIndex, oneVitalHistoryRecord vitalHistory[ ], long& hmr);

void printAllVitalRecords(medPing& mP, const oneVitalHistoryRecord vitalHistory[ ], long hmr);
// end function prototypes

//-----------------------------------------------------------------------
//end of AddHistoryRecord function
//create a medPing object (mP object has global file scope)
medPing mP;	//mp is object, medping is class

//--------------
// medPing_Main \
//----------------------------------------------------------------------
// called from iPhone runSim-tab
int medPing_Main()
{
    // print a message to the cell phone
    mP.CELL_PrintF("Hello medPing patient ...\n\n");
    //no cout statements ^
    //======= DATA STRUCTURE ======================================
    // to hold patient's history of vital signs
    oneVitalHistoryRecord	vitalHistory[MAX_HISTORY];

    // hmr (how many really) vital signs  0 <= hmr < MAX_HISTORY
    long hmr = 0;
    //=============================================================

    RandGen		randGenerator;	// i need a random number generator
    time_t		start, now;		// keep track of time in simulation


    // ask user at CELL to input length of simulation
    mP.CELL_PrintF("How many SECONDS would you like to simulate?\n");
    double simulationTime = mP.CELL_fetchReal(); //
    time(&start);

    // simulation loop ....
    time(&now);
    while ( difftime(now,start) < simulationTime )		// while still time to simulate ...
    {
        long waitThisLongSEC = randGenerator.RandInt(1, MAX_WAIT_SEC);  //generates random number
        mP.CELL_PrintF("\n--------------------------\n");
        mP.CELL_PrintF("\n[PAUSE ... (%d seconds) ]\n", waitThisLongSEC);
        std::this_thread::sleep_for(std::chrono::milliseconds(waitThisLongSEC*1000));
        // Sleep(waitThisLongSEC);	// ZZzzzz.....
        // WINDOWS uses the function called: Sleep( milliseconds )
        // Sleep(waitThisLongSEC*1000);	// ZZzzzz.....

        // check our watch ...
        long nSecs = time(&now);

        // fetch vital signs from the medPing chip HERE (use mP object)
        double newTemp;
        newTemp = mP.getBodyTemperature_F();

        short pulseRate;
        pulseRate = mP.getPulseRate_BPM();			// 60 - 100 beats per minute

        short	glu;
        glu = mP.getGlucoseLevel_mgdL();

        short sys;
        short dia;
        mP.getBloodPressure_mmHg(sys, dia);

        short respRate;
        respRate = mP.getRespirationRate_BPM();

        AddHistoryRecord(nSecs, newTemp, pulseRate, glu, sys,
                         dia, vitalHistory, hmr,respRate);

        printAllVitalRecords(mP, vitalHistory, hmr);

    } // while still more to simulate ...

    mP.CELL_PrintF("\n\nSIMULATION OVER.\n\n");

    mP.CELL_PrintF("DELETE a record; Enter a RAW time: ");
    long inputIndex;
    cin >> inputIndex;

    short recordIndex;
    recordIndex = FindVitalRecord(inputIndex, vitalHistory, hmr);

    DeleteHistoryRecord(recordIndex, vitalHistory, hmr);

    printAllVitalRecords(mP, vitalHistory, hmr);

    mP.CELL_PrintF("\n\nDONE.\n");

    return 0;

} // end medPing_Main()

void AddHistoryRecord(long nSecs, double newTemp, short pulseRate, short glu, short sys,
                      short dia, oneVitalHistoryRecord vitalHistory[ ], long& hmr,
                      short respRate){

    if (hmr == MAX_HISTORY){
        vitalHistory[hmr] = vitalHistory[hmr + 1];
    }
    else {
        vitalHistory[hmr].nSecs = nSecs;
        vitalHistory[hmr].bodyTemp_F = newTemp;
        vitalHistory[hmr].pulseRate = pulseRate;
        vitalHistory[hmr].glucose = glu;
        vitalHistory[hmr].systolic = sys;
        vitalHistory[hmr].diastolic = dia;
        vitalHistory[hmr].respRate = respRate;
        hmr++;
    }
}//---------------------------------------------------------------------------------------------
// Prints history (last set of) hmr vital signs (to medPing output)
// PRE: inputs seconds from user input
// POST: sends index to DeleteHistoryRecord
// SIDE EFFECTS:


long FindVitalRecord(long nSecs, const oneVitalHistoryRecord vitalHistory[ ], long hmr) {
    for (short i = 0; i < hmr; i++) {
        if (vitalHistory[i].nSecs == nSecs)
            return i;
    }
    return NOT_FOUND;
}
//---------------------------------------------------------------------------------------------
// Prints history (last set of) hmr vital signs (to medPing output)
// PRE: input seconds from user
// POST: sends index to the DeleteHistoryRecord function
// SIDE EFFECTS:
//--------

void DeleteHistoryRecord(short recordIndex, oneVitalHistoryRecord vitalHistory[ ], long& hmr) {
    for (short i = recordIndex; i < hmr; i++)
        vitalHistory[i] = vitalHistory[i + 1];
    hmr = hmr - 1;
}
// PRE: receives index from FindVitalHistory
// POST: deletes an element from array[index] and moves it the array
// to the right to the empty index
// SIDE EFFECTS:
//---------------------------------------------------------------------------------------------
void printAllVitalRecords(medPing& mP, const oneVitalHistoryRecord vitalHistory[ ], long hmr)
{
    if (hmr > 0)
    {
        mP.CELL_PrintF("\n---- Records (so far) ----");
        for(long i=0; i < hmr; i++)
        {
            mP.CELL_PrintF("\nRECORD [%02d]\n", i);
            mP.CELL_PrintF("RAW time:  %10ld\n", vitalHistory[i].nSecs);
            mP.CELL_PrintF("temp(F):    %4.1f\n", vitalHistory[i].bodyTemp_F);
            mP.CELL_PrintF("pulse(BPM): %4d\n",   vitalHistory[i].pulseRate);
            mP.CELL_PrintF("glucose level (mg/DL): %4d\n",   vitalHistory[i].glucose);
            mP.CELL_PrintF("systolic levels (mm/Hg): %4d\n",   vitalHistory[i].systolic);
            mP.CELL_PrintF("diastolic levels (mm/Hg): %4d\n",   vitalHistory[i].diastolic);
            mP.CELL_PrintF("respiration rate (BPM): %4d\n",   vitalHistory[i].respRate);
        } // for all records so far
        mP.CELL_PrintF("\n---- end History ----\n\n");

    } // if any records
    else
        mP.CELL_PrintF("\nNo History so far ...\n\n");

    //---------------------------------------------------------------------------------------------
// Prints history (last set of) hmr vital signs (to medPing output)
// PRE: takes mP, vitalHistory, and hmr and goes into if statement and for loop
// POST: prints a print statement stating the time, temp, pulse, levels, & rate
// SIDE EFFECTS:

} // printAllVitalRecords()

