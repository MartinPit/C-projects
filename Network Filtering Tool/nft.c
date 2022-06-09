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

    struct capture_t capture;

    struct capture_t filtered;

    struct capture_t final;

    if (load_capture(&capture, file) == -1) {
        fprintf(stderr, "Failed to load capture.\n");
        return false;
    }

    if (filter_from_mask(&capture, &filtered, src_ip, *src_mask) == -1) {
        destroy_capture(&capture);
        fprintf(stderr, "Failed to filter capture.\n");
        return false;
    }

    if (filter_to_mask(&filtered, &final, dst_ip, *dst_mask) == -1) {
        destroy_capture(&capture);
        destroy_capture(&filtered);
        fprintf(stderr, "Failed to filter capture.\n");
        return false;
    }

    int result = -1;
    if (!strcmp(mode, "flowstats")) {
        result = print_flow_stats(&final);
    } else {
        result = print_longest_flow(&final);
    }

    destroy_capture(&capture);
    destroy_capture(&filtered);
    destroy_capture(&final);

    if (result == -1) {
        return false;
    }

    return true;
}

int main(int argc, char *argv[])
{
    if (argc != 5) {
        fprintf(stderr, "Invalid amount of arguments.\n");
        return EXIT_FAILURE;
    }

    if (strcmp(argv[4], "flowstats") && strcmp(argv[4], "longestflow")) {
        fprintf(stderr, "Invalid statistic option.\n");
        return EXIT_FAILURE;
    }

    uint8_t src_ip[4] = { 0 };
    uint8_t dst_ip[4] = { 0 };
    uint8_t src_mask = 0;
    uint8_t dst_mask = 0;

    int result = parse_input(argv[2], src_ip, &src_mask);

    if (result == -1) {
        fprintf(stderr, "Ip/mask parsing error.\n");
        return EXIT_FAILURE;
    }

    if (result == 1) {
        fprintf(stderr, "Invalid ip/mask format.\n");
        return EXIT_FAILURE;
    }

    result = parse_input(argv[3], dst_ip, &dst_mask);

    if (result == -1) {
        fprintf(stderr, "Ip/mask parsing error.\n");
        return EXIT_FAILURE;
    }

    if (result == 1) {
        fprintf(stderr, "Invalid ip/mask format.\n");
        return EXIT_FAILURE;
    }

    if (src_mask > 32 || dst_mask > 32) {
        fprintf(stderr, "Invalid amount of mask bits.\n");
        return EXIT_FAILURE;
    }

    if (!nft(argv[1], src_ip, dst_ip, &src_mask, &dst_mask, argv[4])) {
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}
