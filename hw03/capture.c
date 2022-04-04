#include "capture.h"
#include <stdlib.h>
#include <string.h>

#define UNUSED(x) ((void) x);

void destroy_node(struct node_t* node);
int add_node(struct pcap_context *context, struct capture_t *capture);
bool add_copy_node(struct capture_t *capture, struct packet_t *packet);

int load_capture(struct capture_t *capture, const char *filename)
{
    struct pcap_context context;

    if (init_context(&context, filename) == PCAP_LOAD_ERROR) {
        destroy_context(&context);
        return -1;
    }

    capture -> header = malloc(sizeof(struct pcap_header_t));
    if (capture -> header == NULL) {
        destroy_context(&context);
        return -1;
    }

    if (load_header(&context, capture -> header) == PCAP_LOAD_ERROR) {
        destroy_context(&context);
        destroy_capture(capture);
        return -1;
    }

    capture -> first = NULL;
    capture -> last = NULL;

    int status = add_node(&context, capture);
    while (status == PCAP_SUCCESS) {
        status = add_node(&context, capture);
    }

    if (status == PCAP_LOAD_ERROR) {
        destroy_capture(capture);
        destroy_context(&context);
        return -1;
    }

    destroy_context(&context);
    return 0;
}

void destroy_capture(struct capture_t *capture)
{   
    free(capture -> header);
    capture -> header = NULL;

    struct node_t *node = capture -> first;

    while (node != NULL) {
        destroy_packet(node -> packet);
        struct node_t *temp_node = node -> next;
        destroy_node(node);
        node = temp_node;
    }

    capture -> first = NULL;
    capture -> last = NULL;
}

const struct pcap_header_t *get_header(const struct capture_t *const capture)
{
    return capture -> header;
}
struct packet_t *get_packet(
        const struct capture_t *const capture,
        size_t index)
{
    if (index == 0) {
        return capture -> first -> packet;
    }

    struct node_t *node = capture -> first;
    size_t count = 0;

    while (count != index) {
        count++;
        node = node -> next;
    }

    return node -> packet;
}

size_t packet_count(const struct capture_t *const capture)
{
    size_t length = 0;

    struct node_t *node = capture -> first;

    while(node != NULL) {
        length++;
        node = node -> next;
    }

    return length;
}

size_t data_transfered(const struct capture_t *const capture)
{
    size_t amount_of_data = 0;

    struct node_t *node = capture -> first;

    while (node != NULL) {
        amount_of_data += node -> packet -> packet_header -> orig_len;
        node = node -> next;
    }

    return amount_of_data;
}

int filter_protocol(
        const struct capture_t *const original,
        struct capture_t *filtered,
        uint8_t protocol)
{
    filtered -> header = malloc(sizeof(struct pcap_header_t));
    if (filtered -> header == NULL) {
        return -1;
    }

    memcpy(filtered -> header, original -> header, sizeof(struct pcap_header_t));

    filtered -> first = NULL;
    filtered -> last = NULL;

    for (size_t i = 0; i < packet_count(original); i++) {
        if (get_packet(original, i) -> ip_header -> protocol == protocol) {
            if (!add_copy_node(filtered, get_packet(original, i))) {
                destroy_capture(filtered);
                return -1;
            }
        }
    }
    return 0;
}

int filter_larger_than(
        const struct capture_t *const original,
        struct capture_t *filtered,
        uint32_t size)
{
    filtered -> header = malloc(sizeof(struct pcap_header_t));
    if (filtered -> header == NULL) {
        return -1;
    }

    memcpy(filtered -> header, original -> header, sizeof(struct pcap_header_t));

    filtered -> first = NULL;
    filtered -> last = NULL;

    for (size_t i = 0; i < packet_count(original); i++) {
        if (get_packet(original, i) -> packet_header -> orig_len >= size) {
            if (!add_copy_node(filtered, get_packet(original, i))) {
                destroy_capture(filtered);
                return -1;
            }
        }
    }
    return 0;
}

int filter_from_to(
        const struct capture_t *const original,
        struct capture_t *filtered,
        uint8_t source_ip[4],
        uint8_t destination_ip[4])
{
    filtered -> header = malloc(sizeof(struct pcap_header_t));
    if (filtered -> header == NULL) {
        return -1;
    }

    memcpy(filtered -> header, original -> header, sizeof(struct pcap_header_t));

    filtered -> first = NULL;
    filtered -> last = NULL;

    for (size_t i = 0; i < packet_count(original); i++) {
        
        bool approved = true;
        uint8_t *src = get_packet(original, i) -> ip_header -> src_addr;
        uint8_t *dest = get_packet(original, i) -> ip_header -> dst_addr;

        for (int j = 0; j < 4; j++) {
            if (src[j] != source_ip[j] || dest[j] != destination_ip[j]) {
                approved = false;
                break;
            }
        }

        if (approved) {
            if (!add_copy_node(filtered, get_packet(original, i))) {
                destroy_capture(filtered);
                return -1;
            }
        }
    }
    return 0;
}

int filter_from_mask(
        const struct capture_t *const original,
        struct capture_t *filtered,
        uint8_t network_prefix[4],
        uint8_t mask_length)
{
    UNUSED(original);
    UNUSED(filtered);
    UNUSED(network_prefix);
    UNUSED(mask_length);
    return -1;
}

int filter_to_mask(
        const struct capture_t *const original,
        struct capture_t *filtered,
        uint8_t network_prefix[4],
        uint8_t mask_length)
{
    UNUSED(original);
    UNUSED(filtered);
    UNUSED(network_prefix);
    UNUSED(mask_length);
    return -1;
}

int print_flow_stats(const struct capture_t *const capture)
{
    UNUSED(capture);
    return -1;
}

int print_longest_flow(const struct capture_t *const capture)
{
    UNUSED(capture);
    return -1;
}

struct node_t* initialize_node() 
{
    struct node_t *node = malloc(sizeof(struct node_t));

    if (node == NULL) {
        return NULL;
    }

    node -> packet = malloc(sizeof(struct packet_t));

    if (node -> packet == NULL) {
        free(node);
        return NULL;
    }

    node -> next = NULL;

    return node;
}

void destroy_node(struct node_t* node)
{   
    free(node -> packet);
    node -> packet = NULL;
    node -> next = NULL;

    free(node);
}

int add_node(struct pcap_context *context, struct capture_t *capture) {

    struct node_t *node = initialize_node();
    if (node == NULL) {
        return PCAP_LOAD_ERROR;
    }

    int status = load_packet(context, node -> packet);
    if (status == PCAP_FILE_END) {
        destroy_node(node);
        return PCAP_FILE_END;
    }

    if (status == PCAP_LOAD_ERROR) {
        destroy_node(node);
        return PCAP_LOAD_ERROR;
    }

    if (capture -> first == NULL) {
        capture -> first = node;
    } else {
        capture -> last -> next = node;
    }

    capture -> last = node;

    return PCAP_SUCCESS;
}

bool add_copy_node(struct capture_t *capture, struct packet_t *packet) {
    struct node_t *node = initialize_node();
    if (node == NULL) {
        return false;
    }

    if (copy_packet(packet, node -> packet) == PCAP_LOAD_ERROR) {
        destroy_node(node);
        return false;
    }

    if (capture -> first == NULL) {
        capture -> first = node;
    } else {
        capture -> last -> next = node;
    }

    capture -> last = node;

    return true;
}
