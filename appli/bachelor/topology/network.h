#ifndef GSP_NETWORK_H
#define GSP_NETWORK_H

#include <stdint.h>
#include <stddef.h>
#include "printf.h"
#include "iotlab_uid.h"

// Define an upper limit to the number of neighbours
// it is desirable that all neighbours fit into one package
// thus we make this number dependent on the MAXPKGSIZE
// we add two bytes to transmit a message type
// One adress is of type uint16_t
#define MAXNEIGHBOURS ((MAXPKGSIZE - 2)/ sizeof(uint16_t))

// Define a minimum signal quality
#define RSSI_THRESHOLD  -70

// How often the algorithm will try to spread the information
#define NUM_TRIES 5

void lookup_neighbours();

void print_neighbours();

/**
 * Returns the number of neighbours currently registered.
 * As there is no way to reliably detect when the neighbour lookup is finished,
 * there is no security that the number won't change after a call to this method.
 * @return The number of neighbours
 */
uint32_t number_of_neighbours();

/**
 * This returns a pointer to the list of all neighbours.
 * As there is no way to reliably detect when the neighbour lookup is finished,
 * there is no security that the list won't change after a call to this method.
 * 
 * @return Pointer to the neighbour list.
 */
uint16_t* get_neighbours();

/**
 * This function returns for a given peer id (i.e. index in local neighbours list)
 * the UUID that is used to identify a node in the network layer
 * @param  index [description]
 * @return       [description]
 */
uint16_t uuid_of_neighbour(unsigned int index);

void reset_neighbours(uint32_t channel, uint32_t transmission_power);

/**
 * Send a package to a peer
 * @param peer_id The id of the peer in the neighbours list
 * @param packet  The package itself
 * @param length  The length of the package
 */
void send_package(int peer_id, void *packet, size_t length);

void send_package_uuid(uint16_t uuid, void *packet, size_t length);

void network_csma_data_received(uint16_t src_addr, const uint8_t *data, uint8_t length, int8_t rssi, uint8_t lqi);

#endif