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


int teds_awesome_checksum(char *data, int data_len, int acknum, int seqnum)
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
	return a^b^acknum^seqnum;
}

int AREC_packet_number = 0;

struct ll {
	struct pkt* packet;
	struct ll* next;
};

struct ll* head = 0;

void enqueue_packet(struct pkt* packet)
{
	struct ll* new_head = malloc(sizeof(struct ll));
	new_head->packet = packet;
	new_head->next = head;
	head = new_head;
}

void clear_packets_before(int last_needed_seq_num)
{
	struct ll* top = head;
	while(top != 0)
	{
		if (top->packet->seqnum < last_needed_seq_num)
		{
			top->next = 0;
		}
		top = top->next;
	}
}

void write_all_packets_in_buffer()
{
	struct ll* top = head;
	while(top != 0)
	{
		tolayer3(0, *(top->packet));
		top = top->next;
	}
}

void write_eight_packets_in_buffer(int i)
{
	struct ll* top = head;
	while(top != 0)
	{
		if (top->packet->seqnum - i <= 7)
		{
			tolayer3(0, *(top->packet));
		}
		top = top->next;
	}
}























/* 
 * A_output(message), where message is a structure of type msg, containing 
 * data to be sent to the B-side. This routine will be called whenever the 
 * upper layer at the sending side (A) has a message to send. It is the job 
 * of your protocol to insure that the data in such a message is delivered 
 * in-order, and correctly, to the receiving side upper layer.
 */
void A_output(struct msg message) {
	struct pkt packet;
	packet.seqnum = AREC_packet_number;
	packet.acknum = -1;
	packet.checksum = teds_awesome_checksum(message.data, MESSAGE_LENGTH, -1, packet.seqnum);
	strncpy(packet.payload, message.data, MESSAGE_LENGTH);
	
	AREC_packet_number += 1;
	tolayer3(0, packet);

	struct pkt* packetQ = malloc(sizeof(struct pkt));
	memcpy(packetQ, &packet, sizeof(struct pkt));

	enqueue_packet(packetQ);
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
//	if (teds_awesome_checksum(packet.payload, MESSAGE_LENGTH) == packet.checksum && packet.acknum == 1)
//	{
//		AREC_ack_number++;
//	}
//	else
//	{
//	}

	int next_packet = packet.acknum;
	int npc = packet.checksum;

	
	if (npc == teds_awesome_checksum(packet.payload, MESSAGE_LENGTH, packet.acknum, packet.seqnum))
	{	
		printf("ACK ON:  %i\n-->last:  %i\n", next_packet, head->packet->seqnum);
		clear_packets_before(next_packet);
		write_eight_packets_in_buffer(next_packet);
	}

//	printf("ACK ON:  %i\nCKSM:  %i\n",next_packet, npc);
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
int next_req_pkt = 0;

void B_input(struct pkt packet) {
	char* got_pay = packet.payload;
	int len = MESSAGE_LENGTH;
	int got_ack = packet.acknum;
	int got_seq = packet.seqnum;
	if (teds_awesome_checksum(got_pay, len, got_ack, got_seq) == packet.checksum)
	{ //the packet is valid!!
		if (packet.seqnum > next_req_pkt)
		{
			struct pkt ack_dropped;
			strncpy(packet.payload, "                               ", MESSAGE_LENGTH);
			ack_dropped.acknum = next_req_pkt;
			ack_dropped.seqnum = next_req_pkt;
			ack_dropped.checksum = teds_awesome_checksum(ack_dropped.payload, MESSAGE_LENGTH, ack_dropped.acknum, ack_dropped.seqnum); 
			
			
			tolayer3(1, ack_dropped);
			return;
		}
		else if (packet.seqnum < next_req_pkt)
		{
			return; //you've already gotten this one
		}
		else
		{ //send the message back up and increase counter
			struct msg m;
			strncpy(m.data, packet.payload, MESSAGE_LENGTH);
			tolayer5(1, m);
			next_req_pkt += 1;
			return;
		}
	}
	else
	{ //the packet is a pile of shit
		struct msg m;
		struct pkt return_packet;

		strncpy(m.data, packet.payload, MESSAGE_LENGTH);
		strncpy(return_packet.payload, "                                      ", MESSAGE_LENGTH);

		return_packet.acknum = next_req_pkt;
		return_packet.seqnum = next_req_pkt;
		return_packet.checksum = teds_awesome_checksum(return_packet.payload, MESSAGE_LENGTH, return_packet.acknum, return_packet.seqnum);
		
		tolayer3(1, packet);
		//tolayer5(1, m);
	}
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

