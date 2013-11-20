#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "project2.h"
 
/* ***************************************************************************
 ALTERNATING BIT AND GO-BACK-N NETWORK EMULATOR: VERSION 1.1  J.F.Kurose

   This code should be used for Project 3, unidirectional or bidirectional
   data transfer protocols from A to B and B to A.
   Network properties:
   - one way network delay averages five time units (longer if there
     are other messages in the channel for GBN), but can be larger
   - packets can be corrupted (either the header or the data portion)
     or lost, according to user-defined probabilities
   - packets may be delivered out of order.

   Compile as gcc -g project3.c student3.c -o p3
**********************************************************************/



/********* STUDENTS WRITE THE NEXT SEVEN ROUTINES *********/
/* 
 * The routines you will write are detailed below. As noted above, 
 * such procedures in real-life would be part of the operating system, 
 * and would be called by other procedures in the operating system.  
 * All these routines are in layer 4.
 */


int teds_awesome_checksum(char* data, int data_len)
{
	long chk = data_len ^ data[0];
	int i = 1;
	for(; i < data_len; i++)
	{
		chk <<= 1;
		chk ^= data[i];
	}
	int a = chk;
	int b = chk >> 32;
	return a^b;
}


int AREC_packet_number = 0;
int AREC_ack_number = 0;
int AREC_last_send_time = 0;

int BREC_packet_number = 0;
int BREC_acknack_number = 0;





/* 
 * A_output(message), where message is a structure of type msg, containing 
 * data to be sent to the B-side. This routine will be called whenever the 
 * upper layer at the sending side (A) has a message to send. It is the job 
 * of your protocol to insure that the data in such a message is delivered 
 * in-order, and correctly, to the receiving side upper layer.
 */
void A_output(struct msg message) {
	AREC_packet_number++;
	struct pkt packet;
	packet.seqnum = AREC_packet_number;
	packet.acknum = -1;
	packet.checksum = teds_awesome_checksum(message.data, MESSAGE_LENGTH);
	char holder[MESSAGE_LENGTH];
	strcpy(holder, message.data);
	strcpy(packet.payload, holder);


	//tolayer3(0, packet);
}

/*
 * Just like A_output, but residing on the B side.  USED only when the 
 * implementation is bi-directional.
 */
void B_output(struct msg message)  {
	puts("B_out");
}

/* 
 * A_input(packet), where packet is a structure of type pkt. This routine 
 * will be called whenever a packet sent from the B-side (i.e., as a result 
 * of a tolayer3() being done by a B-side procedure) arrives at the A-side. 
 * packet is the (possibly corrupted) packet sent from the B-side.
 */
void A_input(struct pkt packet) {
	if (teds_awesome_checksum(packet.payload, MESSAGE_LENGTH) == packet.checksum && packet.acknum == 1)
	{
		AREC_ack_number++;
		puts("proper!");
	}
	else
	{
		puts("bad");
	}
}

/*
 * A_timerinterrupt()  This routine will be called when A's timer expires 
 * (thus generating a timer interrupt). You'll probably want to use this 
 * routine to control the retransmission of packets. See starttimer() 
 * and stoptimer() in the writeup for how the timer is started and stopped.
 */
void A_timerinterrupt() {
	puts("A_ti");
}  

/* The following routine will be called once (only) before any other    */
/* entity A routines are called. You can use it to do any initialization */
void A_init() {
}


/* 
 * Note that with simplex transfer from A-to-B, there is no routine  B_output() 
 */

/*
 * B_input(packet),where packet is a structure of type pkt. This routine 
 * will be called whenever a packet sent from the A-side (i.e., as a result 
 * of a tolayer3() being done by a A-side procedure) arrives at the B-side. 
 * packet is the (possibly corrupted) packet sent from the A-side.
 */
void B_input(struct pkt packet) {

	if (teds_awesome_checksum(packet.payload, MESSAGE_LENGTH) == packet.checksum)
	{
		struct msg m;
		strcpy(m.data, packet.payload);
		tolayer5(1, m);
		packet.acknum=1;
		puts("success!");
		tolayer3(1, packet);
	}
	else
	{
		puts("failure");
		puts(packet.payload);

		packet.acknum=0;
		tolayer3(1, packet);
	}
	//tolayer5(1, message);
}

/*
 * B_timerinterrupt()  This routine will be called when B's timer expires 
 * (thus generating a timer interrupt). You'll probably want to use this 
 * routine to control the retransmission of packets. See starttimer() 
 * and stoptimer() in the writeup for how the timer is started and stopped.
 */
void  B_timerinterrupt() {
}

/* 
 * The following routine will be called once (only) before any other   
 * entity B routines are called. You can use it to do any initialization 
 */
void B_init() {
}

