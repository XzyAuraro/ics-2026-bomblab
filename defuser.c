#include "defuser.h"

#include <inttypes.h>
#include <stdio.h>

#define MAX_FIELDS 8

/*
 * 把 values 中的 count 个整数写成一行“空格分隔的无符号十进制整数”，存入 out。
 * 成功返回 0；count 为 0（还没有实现）、snprintf 出错或结果被截断时返回非零。
 */
static int write_answer(char *out, size_t cap, const uint32_t *values, size_t count) {
    size_t used = 0;
    for (size_t i = 0; i < count; ++i) {
        int size = snprintf(out + used, cap - used, "%s%" PRIu32, i ? " " : "", values[i]);
        if (size < 0 || (size_t)size >= cap - used) return 1;
        used += (size_t)size;
    }
    return count == 0;
}

int solve_phase_1(uint64_t challenge, char *out, size_t cap) {
    uint32_t values[MAX_FIELDS] = {0};
    size_t count = 1;

    const uint32_t high = (uint32_t)(challenge >> 32);
    const uint32_t middle = (uint32_t)(challenge >> 16);
    const uint32_t low = (uint32_t)challenge;
    const uint32_t step1 = (high * UINT32_C(3) + middle) ^ UINT32_C(0xf5a6);
    const uint32_t step0 = (high ^ UINT32_C(0x572e)) + low * UINT32_C(4);
    const uint32_t difference = step0 - step1;
    values[0] = difference * UINT32_C(3) + UINT32_C(0x1565);

    return write_answer(out, cap, values, count);
}

int solve_phase_2(uint64_t challenge, char *out, size_t cap) {
    uint32_t values[MAX_FIELDS] = {0};
    size_t count = 6;

    values[0] = (uint32_t)((challenge >> 18) & UINT64_C(0x3f))+ UINT32_C(12);
    for (size_t i = 1; i < count; ++i) {
        const unsigned int shift = 19U + 4U * (unsigned int)(i - 1);
        const uint32_t part =(uint32_t)((challenge >> shift) & UINT64_C(0x3f));
        values[i] = (values[i - 1] + part)^ (UINT32_C(7) * (uint32_t)i);
    }
    return write_answer(out, cap, values, count);
}

int solve_phase_3(uint64_t challenge, char *out, size_t cap) {
    uint32_t values[MAX_FIELDS] = {0};
    size_t count = 2;
    const uint32_t x = (uint32_t)(challenge >> 3) ^ (uint32_t)(challenge >> 32);
    values[0] = (uint32_t)(challenge & UINT64_C(7));
    switch (values[0]) {
        case 0:
            values[1] = x * UINT32_C(3) + UINT32_C(0x6a69);
            break;
        case 1:
            values[1] = (x * UINT32_C(4)) ^ UINT32_C(0xebd9);
            break;
        case 2:
            values[1] = (x ^ UINT32_C(0x726a)) + (x >> 2);
            break;
        case 3:
            values[1] = x * UINT32_C(5) + UINT32_C(0x3b3ad);
            break;
        case 4:
            values[1] = x * UINT32_C(5) + UINT32_C(0x1d4e);
            break;
        case 5:
            values[1] = (x ^ UINT32_C(0x607)) + (x >> 1);
            break;
        case 6:
            values[1] = (x * UINT32_C(2)) ^ UINT32_C(0xe506);
            break;
        case 7:
            values[1] = x * UINT32_C(5) + UINT32_C(0x16445);
            break;
        default:
            return 1;
    }
    return write_answer(out, cap, values, count);
}

int solve_phase_4(uint64_t challenge, char *out, size_t cap) {
    uint32_t values[MAX_FIELDS] = {0};
    size_t count = 1;

    const uint32_t target = (uint32_t)(challenge & UINT64_C(0x7f));
    const uint32_t salt = (uint32_t)(challenge >> 7)^ (uint32_t)(challenge >> 32);
    uint32_t pending[8] = {0};
    size_t depth = 0;
    uint32_t low = 0;
    uint32_t high = 127;
    uint32_t answer = 0;
    for (;;) {
        const uint32_t mid = low + ((high - low) >> 1);
        if (target == mid) {
            answer = (mid + UINT32_C(0x87)) ^ salt;
            break;
        }
        if (target < mid) {
            pending[depth++] = (mid + salt) ^ UINT32_C(1);
            high = mid - 1;
        } else {
            pending[depth++] = (mid + salt) ^ UINT32_C(0x1c);
            low = mid + 1;
        }
    }
    while (depth > 0) {
        answer = (answer * UINT32_C(3)) ^ pending[--depth];
    }
    values[0] = answer;
    return write_answer(out, cap, values, count);
}

int solve_phase_5(uint64_t challenge, char *out, size_t cap) {
   uint32_t values[MAX_FIELDS] = {0};
   size_t count = 2;

    struct Phase5Node {
        const struct Phase5Node *next;
        uint32_t id;
        uint32_t bias;
    };
    static const struct Phase5Node nodes[12] = {
        {&nodes[2],  UINT32_C(0x062c), UINT32_C(0xa0cf)},
        {&nodes[7],  UINT32_C(0x0222), UINT32_C(0x9538)},
        {NULL,       UINT32_C(0x0b74), UINT32_C(0x2d67)},
        {&nodes[11], UINT32_C(0x043f), UINT32_C(0xb900)},
        {&nodes[1],  UINT32_C(0x083b), UINT32_C(0xa7d2)},
        {&nodes[9],  UINT32_C(0x001f), UINT32_C(0x9254)},
        {&nodes[10], UINT32_C(0x034d), UINT32_C(0xf9a5)},
        {&nodes[0],  UINT32_C(0x0730), UINT32_C(0x7b8c)},
        {&nodes[3],  UINT32_C(0x0133), UINT32_C(0x5aed)},
        {&nodes[4],  UINT32_C(0x0a7f), UINT32_C(0x2eea)},
        {&nodes[8],  UINT32_C(0x0542), UINT32_C(0x886f)},
        {&nodes[5],  UINT32_C(0x093b), UINT32_C(0x278f)}
    };
    static const struct Phase5Node *const heads[8] = {
        &nodes[6], &nodes[10], &nodes[8], &nodes[3],
        &nodes[11], &nodes[5], &nodes[9], &nodes[4]
    };
    const uint32_t steps =(uint32_t)((challenge >> 3) & UINT64_C(3)) + UINT32_C(1);
    const struct Phase5Node *node = heads[challenge & UINT64_C(7)];
    values[0] = (uint32_t)(challenge >> 6)^ (uint32_t)(challenge >> 32)^ UINT32_C(0x6b58);
    for (uint32_t i = 0; i < steps; ++i) {
        values[0] = ((values[0] + node->bias) * UINT32_C(5)) ^ node->id;
        values[1] = node->id;
        node = node->next;
    }
    return write_answer(out, cap, values, count);
}

int solve_phase_6(uint64_t challenge, char *out, size_t cap) {
    uint32_t values[MAX_FIELDS] = {0};
    size_t count = 1;

    const uint32_t selectors[2] = {
        (uint32_t)(challenge & UINT64_C(7)),
        (uint32_t)((challenge >> 3) & UINT64_C(7))
    };
    uint32_t x = (uint32_t)(challenge >> 6)^ (uint32_t)(challenge >> 32);
    for (size_t i = 0; i < 2; i++) {
        switch (selectors[i]) {
            case 0:
            case 5:
                x = (x + UINT32_C(0x4294)) ^ (x >> 2);
                break;
            case 1:
            case 7:
                x = (x ^ UINT32_C(0xa06e)) + x * UINT32_C(8);
                break;
            case 2:
                x = (x >> 5) | (x << 27);
                break;
            case 3:
                x = (x * UINT32_C(4) + UINT32_C(0x9d5d)) ^ (x * UINT32_C(5));
                break;
            case 4:
                x = ((x << 3) | (x >> 29)) ^ UINT32_C(0xa722);
                break;
            case 6:
                x = ((x >> 1) ^ x) + UINT32_C(0x3323);
                break;
            default:
                return 1;
        }

        if (i == 0) {
            x ^= UINT32_C(0xb483);
        }
    }
    values[0] = x + UINT32_C(0x69e9);
    return write_answer(out, cap, values, count);
}


int solve_secret(uint64_t challenge, uint64_t context, char *out, size_t cap) {
      const uint32_t a =(uint32_t)(context & UINT64_C(3));
    const uint32_t challenge_fold =(uint32_t)challenge ^ (uint32_t)(challenge >> 32);
    const uint32_t context_fold =(uint32_t)context ^ (uint32_t)(context >> 32);
    const uint32_t mix_input =challenge_fold ^ context_fold;
    const uint32_t mix_result =(mix_input * UINT32_C(3) + UINT32_C(0x152d)) ^ UINT32_C(0x3409);
    const uint32_t modifier =(a * UINT32_C(0x152d))^ (uint32_t)(context >> 16);
    const uint32_t b =mix_result ^ modifier;
    const uint32_t c =b ^ (context_fold + a + UINT32_C(0x3409) + (challenge_fold >> 3));

    int size = snprintf(out, cap, "AAAAAAAAAAAAAAAA" "AAAAAAAAAAAAAAAA" "OPEN%" PRIu32 " %" PRIu32 " %" PRIu32,
        a, b, c
    );
    if (size < 0 || (size_t)size >= cap) {
        return 1;
    }
    return 0;
}
