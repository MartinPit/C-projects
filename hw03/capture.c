#include <inttypes.h>
#include <stdlib.h>
#include <string.h>

#include "capture.h"

#define UNUSED(x) ((void) x);
#define MAX_VAL 4294967295

void destroy_node(struct node_t *node);
int add_node(struct pcap_context *, struct capture_t *);
bool add_copy_node(struct capture_t *, struct packet_t *);
bool init_copy_capture(const struct capture_t *, struct capture_t *);
uint32_t create_uint32_t(uint8_t *);

struct flow_t
{
    struct capture_t *capture;
    struct flow_t *next;
    uint8_t src[4];
    uint8_t dst[4];
};

struct linked_flow
{
    struct flow_t *first;
    struct flow_t *last;
};

int load_capture(struct capture_t *capture, const char *filename)
{
    struct pcap_context context;

    if (init_context(&context, filename) == PCAP_LOAD_ERROR) {
        return -1;
    }

    capture->header = malloc(sizeof(struct pcap_header_t));
    if (capture->header == NULL) {
        destroy_context(&context);
        return -1;
    }

    if (load_header(&context, capture->header) == PCAP_LOAD_ERROR) {
        destroy_context(&context);
        free(capture->header);
        return -1;
    }

    capture->first = NULL;
    capture->last = NULL;

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
    free(capture->header);

    struct node_t *node = capture->first;

    while (node != NULL) {
        destroy_packet(node->packet);
        struct node_t *temp_node = node->next;
        destroy_node(node);
        node = temp_node;
    }

    capture->first = NULL;
    capture->last = NULL;
}

const struct pcap_header_t *get_header(const struct capture_t *const capture)
{
    return capture->header;
}
struct packet_t *get_packet(
        const struct capture_t *const capture,
        size_t index)
{
    if (index >= packet_count(capture)) {
        return NULL;
    }

    struct node_t *node = capture->first;
    size_t count = 0;

    while (count != index) {
        count++;
        node = node->next;
    }

    return node->packet;
}

size_t packet_count(const struct capture_t *const capture)
{
    size_t length = 0;

    struct node_t *node = capture->first;

    while (node != NULL) {
        length++;
        node = node->next;
    }

    return length;
}

size_t data_transfered(const struct capture_t *const capture)
{
    size_t amount_of_data = 0;

    struct node_t *node = capture->first;

    while (node != NULL) {
        amount_of_data += node->packet->packet_header->orig_len;
        node = node->next;
    }

    return amount_of_data;
}

int filter_protocol(
        const struct capture_t *const original,
        struct capture_t *filtered,
        uint8_t protocol)
{
    if (!init_copy_capture(original, filtered)) {
        return -1;
    }

    size_t count = packet_count(original);
    for (size_t i = 0; i < count; i++) {
        if (get_packet(original, i)->ip_header->protocol == protocol) {
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
    if (!init_copy_capture(original, filtered)) {
        return -1;
    }

    size_t count = packet_count(original);
    for (size_t i = 0; i < count; i++) {
        if (get_packet(original, i)->packet_header->orig_len >= size) {
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
    if (!init_copy_capture(original, filtered)) {
        return -1;
    }

    size_t count = packet_count(original);
    for (size_t i = 0; i < count; i++) {
        bool approved = true;
        uint8_t *src = get_packet(original, i)->ip_header->src_addr;
        uint8_t *dest = get_packet(original, i)->ip_header->dst_addr;

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
    if (!init_copy_capture(original, filtered)) {
        return -1;
    }

    uint32_t mask = create_mask(mask_length);

    size_t count = packet_count(original);
    for (size_t i = 0; i < count; i++) {
        uint32_t address = create_uint32_t(get_packet(original, i)->ip_header->src_addr);
        uint32_t prefix = create_uint32_t(network_prefix);

        if ((prefix & mask) == (address & mask)) {
            if (!add_copy_node(filtered, get_packet(original, i))) {
                destroy_capture(filtered);
                return -1;
            }
        }
    }

    return 0;
}

int filter_to_mask(
        const struct capture_t *const original,
        struct capture_t *filtered,
        uint8_t network_prefix[4],
        uint8_t mask_length)
{
    if (!init_copy_capture(original, filtered)) {
        return -1;
    }

    uint32_t mask = create_mask(mask_length);

    size_t count = packet_count(original);
    for (size_t i = 0; i < count; i++) {
        uint32_t address = create_uint32_t(get_packet(original, i)->ip_header->dst_addr);
        uint32_t prefix = create_uint32_t(network_prefix);

        if ((prefix & mask) == (address & mask)) {
            if (!add_copy_node(filtered, get_packet(original, i))) {
                destroy_capture(filtered);
                return -1;
            }
        }
    }

    return 0;
}

void print_ip(uint8_t *ip)
{
    printf("%u", ip[0]);
    for (int i = 1; i < 4; i++) {
        printf(".%u", ip[i]);
    }
}

bool add_flow(const struct capture_t *const capture, struct linked_flow *flows, uint8_t src[], uint8_t dst[])
{
    struct flow_t *flow = malloc(sizeof(*flow));
    if (flow == NULL) {
        fprintf(stderr, "Failed to allocate memory.\n");
        return false;
    }

    flow->next = NULL;

    flow->capture = malloc(sizeof(struct capture_t));
    if (flow->capture == NULL) {
        free(flow);
        fprintf(stderr, "Failed to allocate memory.\n");
        return false;
    }

    if (filter_from_to(capture, flow->capture, src, dst) == -1) {
        free(flow->capture);
        free(flow);
        return false;
    }

    for (int i = 0; i < 4; i++) {
        flow->src[i] = src[i];
        flow->dst[i] = dst[i];
    }

    if (flows->first == NULL) {
        flows->first = flow;
    } else {
        flows->last->next = flow;
    }

    flows->last = flow;

    return true;
}

bool flow_present(struct linked_flow *flows, uint8_t src[], uint8_t dst[])
{
    struct flow_t *flow = flows->first;

    while (flow != NULL) {
        bool check = true;
        for (int i = 0; i < 4; i++) {
            if ((flow->src)[i] != src[i]) {
                check = false;
                break;
            }

            if ((flow->dst)[i] != dst[i]) {
                check = false;
                break;
            }
        }

        if (check) {
            return true;
        }

        flow = flow->next;
    }

    return false;
}

bool init_linked_flow(const struct capture_t *const capture, struct linked_flow *flows)
{   
    size_t count = packet_count(capture);
    for (size_t i = 0; i < count; i++) {
        uint8_t *src = get_packet(capture, i)->ip_header->src_addr;
        uint8_t *dst = get_packet(capture, i)->ip_header->dst_addr;

        if (flow_present(flows, src, dst)) {
            continue;
        }

        if (!add_flow(capture, flows, src, dst)) {
            return false;
        }
    }

    return true;
}

void destroy_linked_flow(struct linked_flow *flows)
{
    struct flow_t *flow = flows->first;

    while (flow != NULL) {
        struct flow_t *temp_flow = flow->next;
        destroy_capture(flow->capture);
        free(flow->capture);
        free(flow);
        flow = temp_flow;
    }
}

int print_flow_stats(const struct capture_t *const capture)
{
    struct linked_flow *flows = malloc(sizeof(*flows));
    if (flows == NULL) {
        fwrite("Failed to allocate memory.\n", 27, 1, stderr);
        return -1;
    }
    flows->first = NULL;
    flows->last = NULL;

    if (!init_linked_flow(capture, flows)) {
        free(flows);
        fwrite("Failed to allocate memory.\n", 28, 1, stderr);
        return -1;
    }

    struct flow_t *flow = flows->first;
    while (flow != NULL) {
        print_ip(flow->src);
        printf(" -> ");
        print_ip(flow->dst);
        printf(" : %zu\n", packet_count(flow->capture));

        flow = flow->next;
    }

    destroy_linked_flow(flows);
    free(flows);

    return 0;
}

struct flow_t *find_longest_flow(const struct linked_flow *const flows)
{
    struct flow_t *longest = flows->first;
    uint32_t longest_sec = longest->capture->last->packet->packet_header->ts_sec - longest->capture->first->packet->packet_header->ts_sec;

    uint32_t longest_usec = longest->capture->last->packet->packet_header->ts_usec - longest->capture->first->packet->packet_header->ts_usec;

    struct flow_t *flow = flows->first->next;

    while (flow != NULL) {
        uint32_t temp_sec = flow->capture->last->packet->packet_header->ts_sec - flow->capture->first->packet->packet_header->ts_sec;

        uint32_t temp_usec = flow->capture->last->packet->packet_header->ts_usec - flow->capture->first->packet->packet_header->ts_usec;

        if (temp_sec > longest_sec) {
            longest = flow;
            longest_sec = temp_sec;
            longest_usec = temp_usec;
        }

        if (temp_sec == longest_sec && temp_usec > longest_usec) {
            longest = flow;
            longest_sec = temp_sec;
            longest_usec = temp_usec;
        }

        flow = flow->next;
    }
    return longest;
}

int print_longest_flow(const struct capture_t *const capture)
{
    if (capture->first == NULL) {
        fwrite("Given capture cannot be empty.\n", 31, 1, stderr);
        return -1;
    }

    struct linked_flow *flows = malloc(sizeof(*flows));
    if (flows == NULL) {
        fwrite("Failed to allocate memory.\n", 27, 1, stderr);
        return -1;
    }
    flows->first = NULL;
    flows->last = NULL;

    if (!init_linked_flow(capture, flows)) {
        free(flows);
        fwrite("Failed to allocate memory.\n", 27, 1, stderr);
        return -1;
    }

    struct flow_t *longest = find_longest_flow(flows);

    struct packet_header_t *start_hd = longest->capture->first->packet->packet_header;
    struct packet_header_t *end_hd = longest->capture->last->packet->packet_header;

    print_ip(longest->src);
    printf(" -> ");
    print_ip(longest->dst);
    printf(" : %" PRIu32 ":%" PRIu32, start_hd->ts_sec, start_hd->ts_usec);
    printf(" - %" PRIu32 ":%" PRIu32 "\n", end_hd->ts_sec, end_hd->ts_usec);

    destroy_linked_flow(flows);
    free(flows);

    return 0;
}

struct node_t *initialize_node()
{
    struct node_t *node = malloc(sizeof(struct node_t));

    if (node == NULL) {
        return NULL;
    }

    node->packet = malloc(sizeof(struct packet_t));

    if (node->packet == NULL) {
        free(node);
        return NULL;
    }

    node->next = NULL;

    return node;
}

void destroy_node(struct node_t *node)
{
    free(node->packet);
    node->packet = NULL;
    node->next = NULL;

    free(node);
}

int add_node(struct pcap_context *context, struct capture_t *capture)
{
    struct node_t *node = initialize_node();
    if (node == NULL) {
        return PCAP_LOAD_ERROR;
    }

    int status = load_packet(context, node->packet);
    if (status == PCAP_FILE_END) {
        destroy_node(node);
        return PCAP_FILE_END;
    }

    if (status == PCAP_LOAD_ERROR) {
        destroy_node(node);
        return PCAP_LOAD_ERROR;
    }

    if (capture->first == NULL) {
        capture->first = node;
    } else {
        capture->last->next = node;
    }

    capture->last = node;

    return PCAP_SUCCESS;
}

bool add_copy_node(struct capture_t *capture, struct packet_t *packet)
{
    struct node_t *node = initialize_node();
    if (node == NULL) {
        return false;
    }

    if (copy_packet(packet, node->packet) == PCAP_LOAD_ERROR) {
        destroy_node(node);

        return false;
    }

    if (capture->first == NULL) {
        capture->first = node;
    } else {
        capture->last->next = node;
    }

    capture->last = node;

    return true;
}

uint32_t create_mask(uint8_t mask_length)
{
    if (mask_length == 32) {
        return MAX_VAL;
    }
    int to_move = 31;
    uint32_t mask = 0;

    while (mask_length != 0) {
        mask += 1;
        mask <<= 1;
        to_move--;
        mask_length--;
    }
    return mask <<= to_move;
}

bool init_copy_capture(const struct capture_t *const original, struct capture_t *copy)
{
    copy->header = malloc(sizeof(struct pcap_header_t));
    if (copy->header == NULL) {
        return false;
    }

    memcpy(copy->header, original->header, sizeof(struct pcap_header_t));

    copy->first = NULL;
    copy->last = NULL;

    return true;
}

uint32_t create_uint32_t(uint8_t *address)
{
    return (address[0] << 24) | (address[1] << 16) |
            (address[2] << 8) | address[3];
}
