#include <stdio.h>
#include "project3.h"

extern int TraceLevel;
extern float clocktime;

struct distance_table {
  int costs[MAX_NODES][MAX_NODES];
};
struct distance_table dt2;
struct NeighborCosts   *neighbor2;

/* students to write the following two routines, and maybe some others */

void rtinit2() {
    int i,j;
    for(i = 0; i < MAX_NODES; i++)
    {
        for(j = 0; j < MAX_NODES; j++)
        {
            dt2.costs[i][j] = 999;
        }
    }
    neighbor2 = getNeighborCosts(2);

    for(i = 0; i < neighbor2->NodesInNetwork; i++)
    {
        dt2.costs[i][i] = neighbor2->NodeCosts[i];
    }




    

    if (TraceLevel >= 0)
    {
        printf("At time t=%i, rtinit2() was called.\n", clocktime);
    }



    for(i = 0; i < neighbor2->NodesInNetwork; i++)
    {
        struct RoutePacket *generated = (struct RoutePacket *) malloc(sizeof(struct RoutePacket));
        generated->sourceid = 2;
        generated->destid = i;
        int i;
        for(i = 0; i<MAX_NODES; i++)
        {
            generated->mincost[i] = getMinFrom(i, dt2);
        }
        toLayer2(*generated);
    }

    printdt1(2, neighbor2, &dt2);
    printf("\n\n\n");
}


void rtupdate2( struct RoutePacket *rcvdpkt ) {
    if (TraceLevel >= 0)
    {
        printf("At time t=%f, rtupdate2() was called.\n", clocktime);
    }


    int source = rcvdpkt->sourceid;
    int updates = 0;

    //printf("%i\n", source);
    int i;
    for(i = 0; i < MAX_NODES; i++)
    {
        updates += setMin(&(dt2.costs[i][source]), rcvdpkt->mincost[i] + dt2.costs[source][source]);
        //printf("%i ", rcvdpkt->mincost[i]);
    }
    //printf("\n");
    printdt0(2, neighbor2, &dt2);

    if (updates)
    {
        for(i = 0; i < neighbor2->NodesInNetwork; i++)
        {
            struct RoutePacket *generated = (struct RoutePacket *) malloc(sizeof(struct RoutePacket));
            generated->sourceid = 2;
            generated->destid = i;
            int i;
            for(i = 0; i<MAX_NODES; i++)
            {
                generated->mincost[i] = getMinFrom(i, dt2);
            }
            toLayer2(*generated);
        }
    }
}

void finalprint2()
{
    printdt0(2, neighbor2, dt2);
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
void printdt2( int MyNodeNumber, struct NeighborCosts *neighbor, 
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
}    // End of printdt2

