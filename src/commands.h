#ifndef FC6C3578_362B_4EEA_9292_FDFE171FEBDC
#define FC6C3578_362B_4EEA_9292_FDFE171FEBDC

struct ipset;

[[gnu::nonnull(1, 2, 3)]] int process_add_command(struct ipset* handle, const char* ip, const char* set_name);
[[gnu::nonnull(1, 2, 3)]] int process_delete_command(struct ipset* handle, const char* ip, const char* set_name);

#endif /* FC6C3578_362B_4EEA_9292_FDFE171FEBDC */
