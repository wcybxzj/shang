#define TAIL_REPAIR_TIME_DEFAULT 0

enum protocol {
    ascii_prot = 3, /* arbitrary value. */
    binary_prot,
    negotiating_prot /* Discovering the protocol */
};

