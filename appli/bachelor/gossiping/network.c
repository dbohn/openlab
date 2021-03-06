#include "network.h"
#include <string.h>

#include "mac_csma.h"
#include "phy.h"
#include "event.h"
#include "soft_timer.h"

#include "iotlab_uid_num_hashtable.h"

#include "feedback.h"

// Type of the broadcast HELLO Message
#define MSG_HELLO 	  	1
// The add reply
#define MSG_HELLO_ACK   2

#define ADDR_BROADCAST 0xFFFF

#define CHANNEL 15

// The maximum package size is available in
// the constant PHY_MAX_TX_LENGTH.
// This constant does not take the CSMA Mac Layer
// into account thus we subtract that size, it adds to a package
#define MAXPKGSIZE PHY_MAX_TX_LENGTH - 4

//static soft_timer_t topology_alarm[1];

// define the array of all neighbours and initialize it with a 0 first-element
// to allocate the memory
uint16_t neighbours[MAXNEIGHBOURS] = {0};
uint32_t neighboursCount = 0;

uint32_t number_of_neighbours() {
	return neighboursCount;
}

uint16_t* get_neighbours() {
	return neighbours;
}

uint16_t uuid_of_neighbour(unsigned int index) {
	return neighbours[index];
}

//uint8_t helo_counter = 0;


struct msg_send
{
    int try;
    uint16_t addr;
    uint8_t pkt[MAXPKGSIZE];
    size_t length;
};

/**
 * Method tries to send package NUM_TRIES times.
 * Extracted from distributed_algorithm IoT-Lab example
 * @param arg msg_send struct with data
 */
static void do_send(handler_arg_t arg)
{
    struct msg_send *send_cfg = (struct msg_send *)arg;
    int ret;

    ret = mac_csma_data_send(send_cfg->addr, send_cfg->pkt, send_cfg->length);
    if (ret != 0) {
        DEBUG_MSG("SENT;%04x;%u;\n", send_cfg->addr, send_cfg->try);
    } else {
    	ERROR("SEND-ERR;%04x;%u;\n", send_cfg->addr, send_cfg->try);
        if (send_cfg->try++ < NUM_TRIES)
            event_post(EVENT_QUEUE_APPLI, do_send, arg);
    }

}

/**
 * This method prepares a message, that will be sent
 * @param addr The address of the receiver
 * @param packet The data to be sent
 * @param length The length of the data to be sent
 */
static void send(uint16_t addr, void *packet, size_t length)
{
    static struct msg_send send_cfg;
    send_cfg.try = 0;
    send_cfg.addr = addr;
    send_cfg.length = length;
    memcpy(&send_cfg.pkt, packet, length);

    event_post(EVENT_QUEUE_APPLI, do_send, &send_cfg);
}

/**
 * Sends a packet to the peer_id-th peer of this node
 * @param peer_id The index of the neighbourhood array
 * @param packet  Pointer to the package
 * @param length  Length of the package
 */
void send_package(int peer_id, void *packet, size_t length)
{
	// Do not allow to send to unexisting peer!
	if (peer_id >= neighboursCount) {
		return;
	}

	uint16_t address = neighbours[peer_id];

	send(address, packet, length);
}

/**
 * Send a package to the peer given by the UUID
 * @param uuid   The UUID of the peer
 * @param packet Pointer to the package
 * @param length Length of the package
 */
void send_package_uuid(uint16_t uuid, void *packet, size_t length) {
	send(uuid, packet, length);
}

/**
 * Resets the list of neighbours and initializes radio module
 * @param channel            Radio channel
 * @param transmission_power Transmission power
 */
void reset_neighbours(uint32_t channel, uint32_t transmission_power) {
	// Remove any existing neighbours
	memset(neighbours, 0, sizeof(neighbours));
	neighboursCount = 0;

	// Initialize radio communication for lookup
	mac_csma_init(channel, transmission_power);

	uint8_t pkg = 0;
	send(ADDR_BROADCAST, &pkg, 1);
}

/**
 * Send HELLO-broadcast message
 */
void lookup_neighbours() {

	// Procedure:
	// 1. Each node broadcasts a HELLO-message
	// 2. Upon reception of a broadcast message,
	// register the sender node as a neighbour

	uint8_t pkg = MSG_HELLO;
	send(ADDR_BROADCAST, &pkg, 1);
}

// 
/**
 * This method prints all currently known neighbours
 * Attention: This is not protected against race conditions
 * So use at your own risk!
 */
void print_neighbours() {
	int i;
	MESSAGE("NGB;%u", neighboursCount);
	for (i = 0; i < neighboursCount; i++) {
		#if defined(LONG_NEIGHBOUR_LOG) && LONG_NEIGHBOUR_LOG == 1
		struct node my_node = node_from_uid(neighbours[i]);

		if (my_node.num != 0) {
        	char *node_str;

	        switch (my_node.type) {
	        case M3:
	            node_str = "m3";
	            break;
	        case A8:
	            node_str = "a8";
	            break;
	        default:
	            node_str = "invalid";
	            break;
        	}
        	
        	printf(";%04x(%s-%u)", neighbours[i], node_str, my_node.num);
	    } else {
	        printf(";%04x", neighbours[i]);
	    }
	    #else
	    	printf(";%04x", neighbours[i]);
	    #endif
	}
	printf("\n");
}

/**
 * This method handles a received HELLO message
 * @param src_addr Sender of the HELLO
 * @param rssi     Signal strength of the received message
 */
static void handleReceivedHello(uint16_t src_addr, int8_t rssi) {
	// Check if received rssi above threshold
	// thus the signal strength is high enough
	if (rssi > RSSI_THRESHOLD) {
		// We assume, that we will not loose any nodes
		// i.e. all nodes will be running until the end of the experiment
		// As we are filling up the list of nodes from the left,
		// we can conclude, that as soon as we find the first 0 value,
		// we know, that we did not know about the node before.
		int i = 0;
		for (i = 0; i < MAXNEIGHBOURS; ++i)
		{
			if (neighbours[i] == 0) {
				MESSAGE("ADD;%04x;\n", src_addr);
				neighbours[i] = src_addr;
				neighboursCount++;

				// Send acknowledgement
				uint8_t pkg = MSG_HELLO_ACK;
				send(src_addr, &pkg, 1);
				break;
			}

			// We have registered the node already!
			if (neighbours[i] == src_addr) {
				break;
			}
		}
	} else {
		MESSAGE("REJECT;%04x;%d;\n", src_addr, rssi);
	}
}

/**
 * Handle received ACK package
 */
static void handleHelloAck(uint16_t src_addr, int8_t rssi) {
	int i = 0;
	for (i = 0; i < MAXNEIGHBOURS; ++i) {
		if (neighbours[i] == src_addr) {
			MESSAGE("KNOWN;%04x;\n", src_addr);
			return;
		}
		if (neighbours[i] == 0) {
			MESSAGE("ACK;%04x;\n", src_addr);
			neighbours[i] = src_addr;
			neighboursCount++;
			return;
		}
	}
}

/**
 * Message reception handler
 * @param src_addr Sender
 * @param data     Received data
 * @param length   Length of the received data
 * @param rssi     Signal strengh
 * @param lqi      Link Quality Indicator
 */
void network_csma_data_received(uint16_t src_addr, const uint8_t *data,
				     uint8_t length, int8_t rssi, uint8_t lqi) {
	// The first byte of the received message is always the message type.
	// Thus fetch it and look at it!

	uint8_t type = data[0];

	//printf("Message received!\n");
	
	DEBUG_MSG("INCOMING;%u;\n", type);

	switch (type) {
		case (MSG_HELLO):
			handleReceivedHello(src_addr, rssi);
			break;
		case (MSG_HELLO_ACK):
			handleHelloAck(src_addr, rssi);
			break;
		default:
			break;
	}
}