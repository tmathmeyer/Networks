#include <stdio.h>
#include <stdlib.h>
#include "project3.h"

extern int TraceLevel;
extern float clocktime;

struct distance_table {
  int costs[MAX_NODES][MAX_NODES];
};
struct distance_table dt4;
struct NeighborCosts   *neighbor4;

/* students to write the following two routines, and maybe some others */

void rtinit4() {
    int i,j;
    for(i = 0; i < MAX_NODES; i++)
    {
        for(j = 0; j < MAX_NODES; j++)
        {
            dt4.costs[i][j] = 999;
        }
    }
    neighbor4 = getNeighborCosts(4);

    for(i = 0; i < neighbor4->NodesInNetwork; i++)
    {
        dt4.costs[i][i] = neighbor4->NodeCosts[i];
    }




    

    if (TraceLevel >= 0)
    {
        printf("At time t=%i, rtinit4() was called.\n", clocktime);
    }



    for(i = 0; i < neighbor4->NodesInNetwork; i++)
    {
        struct RoutePacket *generated = (struct RoutePacket *) malloc(sizeof(struct RoutePacket));
        generated->sourceid = 4;
        generated->destid = i;
        int i;
        for(i = 0; i<MAX_NODES; i++)
        {
            generated->mincost[i] = getMinFrom(i, dt4);
        }
        toLayer2(*generated);
    }

    printdt1(4, neighbor4, &dt4);
    printf("\n\n\n");
}


void rtupdate4( struct RoutePacket *rcvdpkt ) {
    if (TraceLevel >= 0)
    {
        printf("At time t=%f, rtupdate4() was called.\n", clocktime);
    }


    int source = rcvdpkt->sourceid;
    int updates = 0;

    //printf("%i\n", source);
    int i;
    for(i = 0; i < MAX_NODES; i++)
    {
        updates += setMin(&(dt4.costs[i][source]), rcvdpkt->mincost[i] + dt4.costs[source][source]);
        //printf("%i ", rcvdpkt->mincost[i]);
    }
    //printf("\n");
    printdt0(4, neighbor4, &dt4);

    if (updates)
    {
        for(i = 0; i < neighbor4->NodesInNetwork; i++)
        {
            struct RoutePacket *generated = (struct RoutePacket *) malloc(sizeof(struct RoutePacket));
            generated->sourceid = 0;
            generated->destid = i;
            int i;
            for(i = 0; i<MAX_NODES; i++)
            {
                generated->mincost[i] = getMinFrom(i, dt4);
            }
            toLayer2(*generated);
        }
    }
}

void finalprint4()
{
    printdt0(4, neighbor4, dt4);
}


/////////////////////////////////////////////////////////////////////
//  printdt
//  This routine is being supplied to you.  It is the same code in
//  each node and is tailored based on the input arguments.
//  Required arguments:
//  MyNodeNumber:  This routine assumes that you know your node
//                 number and supply it when making this call.
//  struct NeighborCosts *neighbor:  A pointer to the structure 
//                 that's supplied via a call to getNeighborCosts().
//                 It tells this print routine the configuration
//                 of nodes surrounding the node we're working on.
//  struct distance_table *dtptr: This is the running record of the
//                 current costs as seen by this node.  It is 
//                 constantly updated as the node gets new
//                 messages from other nodes.
/////////////////////////////////////////////////////////////////////
void printdt4( int MyNodeNumber, struct NeighborCosts *neighbor, 
		struct distance_table *dtptr ) {
    int       i, j;
    int       TotalNodes = neighbor->NodesInNetwork;     // Total nodes in network
    int       NumberOfNeighbors = 0;                     // How many neighbors
    int       Neighbors[MAX_NODES];                      // Who are the neighbors

    // Determine our neighbors 
    for ( i = 0; i < TotalNodes; i++ )  {
        if (( neighbor->NodeCosts[i] != INFINITY ) && i != MyNodeNumber )  {
            Neighbors[NumberOfNeighbors] = i;
            NumberOfNeighbors++;
        }
    }
    // Print the header
    printf("                via     \n");
    printf("   D%d |", MyNodeNumber );
    for ( i = 0; i < NumberOfNeighbors; i++ )
        printf("     %d", Neighbors[i]);
    printf("\n");
    printf("  ----|-------------------------------\n");

    // For each node, print the cost by travelling thru each of our neighbors
    for ( i = 0; i < TotalNodes; i++ )   {
        if ( i != MyNodeNumber )  {
            printf("dest %d|", i );
            for ( j = 0; j < NumberOfNeighbors; j++ )  {
                    printf( "  %4d", dtptr->costs[i][Neighbors[j]] );
            }
            printf("\n");
        }
    }
    printf("\n");
}    // End of printdt4

