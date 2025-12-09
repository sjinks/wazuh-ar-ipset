#ifndef D8B11629_EFE6_4657_98C8_59680C36DC49
#define D8B11629_EFE6_4657_98C8_59680C36DC49

struct ipset;

struct ipset* ipset_initialize();
void ipset_finalize(struct ipset* h);
[[gnu::nonnull(1, 2, 3)]] int ipset_add_ip(struct ipset* h, const char* set_name, const char* ip);
[[gnu::nonnull(1, 2, 3)]] int ipset_del_ip(struct ipset* h, const char* set_name, const char* ip);

#endif /* D8B11629_EFE6_4657_98C8_59680C36DC49 */
