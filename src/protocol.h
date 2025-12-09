#ifndef E843AD49_4201_4CAF_A3FA_E6BEC9083A8C
#define E843AD49_4201_4CAF_A3FA_E6BEC9083A8C

#define OS_SUCCESS   0
#define OS_FAILURE  -1
#define MAX_BUF_SIZE 65536

[[gnu::returns_nonnull]] struct cJSON* read_and_parse();
[[gnu::nonnull(1)]] void send_control_message(const char* srcip);

#endif /* E843AD49_4201_4CAF_A3FA_E6BEC9083A8C */
