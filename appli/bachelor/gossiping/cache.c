#include "cache.h"
#include <string.h>
#include "network.h"

data_cache cache;

/**
 * Initialize the cache
 * @param me [description]
 */
void init_cache(uint16_t me) {
	// Prefill cache
	memset(cache.value, 0, CACHE_SIZE);
	//cache.value = {0};
	cache.sender = me;
	cache.source = me;
	cache.empty = 1;
}

/**
 * Checks if the cache is empty
 * @return 1 if the cache is empty
 */
uint8_t cache_empty() {
	return cache.empty;
}

/**
 * Returns the size of the cache structure
 * @return [description]
 */
size_t cache_memory_footprint() {
	return sizeof(data_cache);
}

/**
 * Set the cache
 * @param data   Data
 * @param len    Length of data
 * @param sender Sender of data
 * @param source Source of data
 */
void set_cache(void* data, size_t len, uint16_t sender, uint16_t source) {
	memcpy(&cache.value, data, len);
	cache.sender = sender;
	cache.source = source;
	cache.empty = 0;
}

/**
 * Set only a segment of the cache
 * @param data   The Segment
 * @param sender Sender
 * @param source Source
 */
void set_cache_segment(cache_segment* data, uint16_t sender, uint16_t source) {
	cache.sender = sender;
	cache.source = source;
	cache.empty = 0;
	memcpy(&cache.value + data->start, data->value, data->len);
	
}

/**
 * Returns Pointer to the Cache value
 */
uint8_t* get_cache_value() {
	return cache.value;
}

/**
 * Returns pointer to the cache
 */
data_cache* get_cache() {
	return &cache;
}

/**
 * Send a cache segment to another node
 * @param  round_num The gossip round
 * @param  addr      The receiver
 * @param  type      Message type, either MSG_PUSH or MSG_PULL
 * @param  cseg      the cache segment
 * @return           -1 on error, 0 otherwise
 */
uint8_t send_cache_segment(uint16_t round_num, uint16_t addr, uint8_t type, cache_segment* cseg) {
	static gossip_message msg;
	int i;

	msg.type = type;
	msg.round = round_num;

	// Cache segment is too large!
	if (cseg->len > MAX_SEGMENT_SIZE) {
		return -1;
	}

	msg.source = cache.source;
	msg.value[0] = cseg->start;
	msg.value[1] = cseg->len;

	for (i = 0; i < 4; ++i)
	{
		msg.value[2+i] = cache.value[cseg->start + i];
	}

	send_package_uuid(addr, &msg, sizeof(gossip_message));

	return 0;
}