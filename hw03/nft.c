#include <inttypes.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "capture.h"

int parse_input(char *argv, uint8_t ip[4], uint8_t *mask)
{
    int result = sscanf(argv, "%" SCNu8 ".%" SCNu8 ".%" SCNu8 ".%" SCNu8 "/%" SCNu8, &ip[0], &ip[1], &ip[2], &ip[3], mask);

    if (result == EOF) {
        return -1;
    }

    if (result != 5) {
        return 1;
    }

    return 0;
}

bool nft(char *file, uint8_t src_ip[4], uint8_t dst_ip[4], uint8_t *src_mask, uint8_t *dst_mask, char *mode)
{   
    if (access(file, R_OK)) {
        fprintf(stderr, "File doesn't exist.\n");
        return false;
    }

    struct capture_t *capture = malloc(sizeof(struct capture_t));
    if (capture == NULL) {
        fprintf(stderr, "Failed to allocate memory.\n");
        return false;
    }

    struct capture_t *filtered = malloc(sizeof(struct capture_t));
    if (capture == NULL) {
        free(capture);
        fprintf(stderr, "Failed to allocate memory.\n");
        return false;
    }

    struct capture_t *final = malloc(sizeof(struct capture_t));
    if (capture == NULL) {
        free(capture);
        free(filtered);
        fprintf(stderr, "Failed to allocate memory.\n");
        return false;
    }

    if (load_capture(capture, file) == -1) {
        free(capture);
        free(filtered);
        free(final);
        fprintf(stderr, "Failed to load capture.\n");
        return false;
    }

    if (filter_from_mask(capture, filtered, src_ip, *src_mask) == -1) {
        destroy_capture(capture);
        free(capture);
        free(filtered);
        free(final);
        fprintf(stderr, "Failed to filter capture.\n");
        return false;
    }

    if (filter_to_mask(filtered, final, dst_ip, *dst_mask) == -1) {
        destroy_capture(capture);
        destroy_capture(filtered);
        free(capture);
        free(filtered);
        free(final);
        fprintf(stderr, "Failed to filter capture.\n");
        return false;
    }

    size_t count = packet_count(filtered); 
    count = packet_count(final);
    int result = -1;
    if (!strcmp(mode, "flowstats")) {
        result = print_flow_stats(final);
    } else {
        result = print_longest_flow(final);
    }

    destroy_capture(capture);
    destroy_capture(filtered);
    destroy_capture(final);
    free(capture);
    free(filtered);
    free(final);

    if (result == -1) {
        return false;
    }

    return true;
}

int main(int argc, char *argv[])
{
    if (argc != 5) {
        fwrite("Invalid amount of arguments.\n", 29, 1, stderr);
        return EXIT_FAILURE;
    }

    if (strcmp(argv[4], "flowstats") && strcmp(argv[4], "longestflow")) {
        fwrite("Invalid statistic option.\n", 26, 1, stderr);
        return EXIT_FAILURE;
    }

    uint8_t src_ip[4] = { 0 };
    uint8_t dst_ip[4] = { 0 };
    uint8_t src_mask = 0;
    uint8_t dst_mask = 0;

    int result = parse_input(argv[2], src_ip, &src_mask);

    if (result == -1) {
        fwrite("Ip/mask parsing error.\n", 23, 1, stderr);
        return EXIT_FAILURE;
    }

    if (result == 1) {
        fwrite("Invalid ip/mask format.\n", 24, 1, stderr);
        return EXIT_FAILURE;
    }

    result = parse_input(argv[3], dst_ip, &dst_mask);

    if (result == -1) {
        fwrite("Ip/mask parsing error.\n", 23, 1, stderr);
        return EXIT_FAILURE;
    }

    if (result == 1) {
        fwrite("Invalid ip/mask format.\n", 24, 1, stderr);
        return EXIT_FAILURE;
    }

    if (src_mask > 32 || dst_mask > 32) {
        fwrite("Invalid amount of mask bits.\n", 29, 1, stderr);
        return EXIT_FAILURE;
    }

    if (!nft(argv[1], src_ip, dst_ip, &src_mask, &dst_mask, argv[4])) {
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}