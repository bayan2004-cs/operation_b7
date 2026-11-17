#include "bitwise.h"

#include <string.h>

static void clear_out(char *out, size_t capacity)
{
    if (out != NULL && capacity > 0) {
        out[0] = '\0';
    }
}

static int is_hex_digit(char c)
{
    return (c >= '0' && c <= '9') || (c >= 'A' && c <= 'F') ||
           (c >= 'a' && c <= 'f');
}

int parse_base_number(const char *text, unsigned base, uint32_t *out)
{
    uint32_t value = 0;
    size_t i = 0;
    size_t digits = 0;

    if (text == NULL || out == NULL ||
        (base != 2 && base != 10 && base != 16)) {
        return 0;
    }
    if (text[0] == '0') {
        if (base == 2 && (text[1] == 'b' || text[1] == 'B')) {
            i = 2;
        } else if (base == 16 && (text[1] == 'x' || text[1] == 'X')) {
            i = 2;
        }
    }
    for (; text[i] != '\0'; ++i) {
        char c = text[i];
        unsigned digit;

        if (c >= '0' && c <= '9') {
            digit = (unsigned)(c - '0');
        } else if (c >= 'A' && c <= 'F') {
            digit = (unsigned)(c - 'A') + 10u;
        } else if (c >= 'a' && c <= 'f') {
            digit = (unsigned)(c - 'a') + 10u;
        } else {
            return 0;
        }
        if (digit >= base) {
            return 0;
        }
        if (value > (UINT32_MAX - digit) / base) {
            return 0;
        }
        value = value * base + digit;
        ++digits;
    }
    if (digits == 0) {
        return 0;
    }
    *out = value;
    return 1;
}

int format_binary(uint32_t value, unsigned width, char *out, size_t capacity)
{
    unsigned i;

    if (out == NULL || capacity == 0) {
        return 0;
    }
    out[0] = '\0';
    if (width < 1 || width > 32 || capacity < (size_t)width + 1) {
        return 0;
    }
    if (width < 32 && (value >> width) != 0) {
        return 0;
    }
    for (i = 0; i < width; ++i) {
        out[i] = ((value >> (width - 1 - i)) & 1u) ? '1' : '0';
    }
    out[width] = '\0';
    return 1;
}

int decode_twos_complement(uint32_t raw, unsigned bits, int32_t *out)
{
    if (out == NULL || bits < 2 || bits > 16 || (raw >> bits) != 0) {
        return 0;
    }
    if (raw & (1u << (bits - 1))) {
        *out = (int32_t)raw - (int32_t)(1u << bits);
    } else {
        *out = (int32_t)raw;
    }
    return 1;
}

int encode_twos_complement(int32_t value, unsigned bits, uint32_t *out)
{
    int32_t min;
    int32_t max;

    if (out == NULL || bits < 2 || bits > 16) {
        return 0;
    }
    min = -(int32_t)(1u << (bits - 1));
    max = (int32_t)(1u << (bits - 1)) - 1;
    if (value < min || value > max) {
        return 0;
    }
    if (value < 0) {
        *out = (uint32_t)(value + (int32_t)(1u << bits));
    } else {
        *out = (uint32_t)value;
    }
    return 1;
}

int unpack_packet(uint16_t word, PacketFields *out)
{
    if (out == NULL) {
        return 0;
    }
    out->type = (uint8_t)((word >> 12) & 0x0Fu);
    out->index = (uint8_t)((word >> 8) & 0x0Fu);
    out->payload = (uint8_t)(word & 0xFFu);
    return 1;
}

int pack_packet(const PacketFields *fields, uint16_t *out)
{
    if (fields == NULL || out == NULL || fields->type > 15 ||
        fields->index > 15) {
        return 0;
    }
    *out = (uint16_t)(((uint16_t)fields->type << 12) |
                      ((uint16_t)fields->index << 8) |
                      (uint16_t)fields->payload);
    return 1;
}

int decode_channel(const PacketFields *packets, size_t count,
                   uint8_t type, uint8_t key,
                   char *out, size_t capacity)
{
    uint8_t seen[16] = {0};
    uint8_t chars[16] = {0};
    size_t i;
    size_t n = 0;

    if (out == NULL || capacity == 0) {
        return 0;
    }
    out[0] = '\0';
    if (packets == NULL && count > 0) {
        return 0;
    }
    for (i = 0; i < count; ++i) {
        if (packets[i].type != type) {
            continue;
        }
        if (packets[i].index > 15 || seen[packets[i].index]) {
            return 0;
        }
        seen[packets[i].index] = 1;
        chars[packets[i].index] = (uint8_t)(key ^ packets[i].payload);
        ++n;
    }
    if (n < 2) {
        return 0;
    }
    for (i = 0; i < 16; ++i) {
        if ((i < n) != (seen[i] != 0)) {
            return 0;
        }
    }
    if (chars[n - 1] != 0 || capacity < n) {
        return 0;
    }
    for (i = 0; i + 1 < n; ++i) {
        if (chars[i] < 0x20 || chars[i] > 0x7E) {
            return 0;
        }
    }
    for (i = 0; i < n; ++i) {
        out[i] = (char)chars[i];
    }
    return 1;
}

int parse_meeting_record(const uint8_t raw[10], MeetingRecord *out)
{
    MeetingRecord rec;
    uint8_t checksum = 0;
    int32_t adjust;
    int32_t final_minute;
    size_t i;

    if (out != NULL) {
        memset(out, 0, sizeof(*out));
    }
    if (raw == NULL || out == NULL) {
        return 0;
    }
     for (i = 0; i < 9; ++i) {
        checksum ^= raw[i];
    }
    if (checksum != raw[9] || raw[7] != 0) {
        return 0;
    }
    rec.meeting_id = (uint16_t)((uint16_t)raw[0] | ((uint16_t)raw[1] << 8));
    rec.base_minute = (uint16_t)((uint16_t)raw[2] | ((uint16_t)raw[3] << 8));
    rec.location_code = (uint16_t)((uint16_t)raw[4] | ((uint16_t)raw[5] << 8));
    rec.flags = raw[6];
    if ((rec.flags & (uint8_t)~0x03u) != 0 || rec.flags == 0x03) {
        return 0;
    }
    if (rec.meeting_id == 0 || rec.location_code == 0 ||
        rec.base_minute > 1439) {
        return 0;
    }
    if (!decode_twos_complement(raw[8], 8, &adjust)) {
        return 0;
    }
    final_minute = (int32_t)rec.base_minute + adjust;
    if (final_minute < 0 || final_minute > 1439) {
        return 0;
    }
    rec.minute_adjust = (int16_t)adjust;
    rec.final_minute = (uint16_t)final_minute;
    *out = rec;
    return 1;
}

int find_confirmed_meeting(FILE *input, MeetingRecord *out)
{
    uint8_t raw[10];
    MeetingRecord candidate;
    MeetingRecord current;
    size_t matches = 0;
    size_t got;

    if (out != NULL) {
        memset(out, 0, sizeof(*out));
    }
    if (input == NULL || out == NULL) {
        return 0;
    }
    memset(&candidate, 0, sizeof(candidate));
    for (;;) {
        got = fread(raw, 1, sizeof(raw), input);
        if (got == 0) {
            break;
        }
        if (got != sizeof(raw)) {
            return 0;
        }
        if (parse_meeting_record(raw, &current) &&
            (current.flags & 0x01u) && !(current.flags & 0x02u)) {
            ++matches;
            candidate = current;
        }
    }
    if (ferror(input) || matches != 1) {
        return 0;
    }
    *out = candidate;
    return 1;
}

int resolve_location(const PacketFields *packets, size_t count,
                     uint8_t key, uint16_t location_code,
                     char *out, size_t capacity)
{
    char text[16];
    char hex[5];
    uint32_t code;
    size_t len;
    size_t i;

    clear_out(out, capacity);
    if (out == NULL || capacity == 0) {
        return 0;
    }
    if (!decode_channel(packets, count, 0x0E, key, text, sizeof(text))) {
        return 0;
    }
    len = strlen(text);
    if (len < 10 || strncmp(text, "LOC:", 4) != 0 || text[8] != '=') {
        return 0;
    }
    for (i = 0; i < 4; ++i) {
        if (!is_hex_digit(text[4 + i])) {
            return 0;
        }
        hex[i] = text[4 + i];
    }
    hex[4] = '\0';
    if (!parse_base_number(hex, 16, &code) || code != location_code) {
        return 0;
    }
    for (i = 9; i < len; ++i) {
        if (text[i] < 'A' || text[i] > 'Z') {
            return 0;
        }
    }
    if (capacity < len - 9 + 1) {
        return 0;
    }
    memcpy(out, text + 9, len - 9 + 1);
    return 1;
}