#include "capture.h"
#include <stdlib.h>

#define UNUSED(x) ((void) x);
void destroy_node(struct node_t* node);
struct node_t* initialize_node();

int load_capture(struct capture_t *capture, const char *filename)
{
    struct pcap_context context;
    capture -> length = 0;

    if (init_context(&context, filename) == PCAP_LOAD_ERROR) {
        destroy_context(&context);
        return -1;
    }

    capture -> header = malloc(sizeof(struct pcap_header_t));
    if (load_header(&context, capture -> header) == PCAP_LOAD_ERROR) {
        destroy_context(&context);
        destroy_capture(capture);
        return -1;
    }

    struct node_t *node = initialize_node();
    if (node == NULL) {
        destroy_context(&context);
        destroy_capture(capture);
        destroy_node(node);
        return -1;
    }

    int status = load_packet(&context, node -> packet);
    if (status == PCAP_LOAD_ERROR) {
        destroy_packet(node -> packet);
        destroy_context(&context);
        destroy_capture(capture);
        destroy_node(node);
        return -1;
    }

    capture -> length++;
    if (status == PCAP_FILE_END) {
        destroy_context(&context);
        destroy_node(node);
        return 0;
    }

    capture -> first = node;
    capture -> last = node;

    node = initialize_node();
    if (node == NULL) {
        destroy_context(&context);
        destroy_capture(capture);
        destroy_node(node);
        return -1;
    }
    status = load_packet(&context, node -> packet);

    while (status == PCAP_SUCCESS) {
        (capture -> last) -> next = node;
        capture -> last = node;

        node = initialize_node();
        if (node == NULL) {
        destroy_context(&context);
        destroy_capture(capture);
        destroy_node(node);
        return -1;
        }
        capture -> length++;
        status = load_packet(&context, node -> packet);

    }

    destroy_context(&context);

    if (status == PCAP_LOAD_ERROR) {
        destroy_capture(capture);
        return -1;
    }

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
        capture -> length--;
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
    return capture -> length;
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
    UNUSED(original);
    UNUSED(filtered);
    UNUSED(protocol);
    return -1;
}

int filter_larger_than(
        const struct capture_t *const original,
        struct capture_t *filtered,
        uint32_t size)
{
    UNUSED(original);
    UNUSED(filtered);
    UNUSED(size);
    return -1;
}

int filter_from_to(
        const struct capture_t *const original,
        struct capture_t *filtered,
        uint8_t source_ip[4],
        uint8_t destination_ip[4])
{
    UNUSED(original);
    UNUSED(filtered);
    UNUSED(source_ip);
    UNUSED(destination_ip);
    return -1;
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
