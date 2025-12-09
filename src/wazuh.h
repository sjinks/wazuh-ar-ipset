#ifndef C753DACF_D459_4B15_8863_ADDC0C1F5B4A
#define C753DACF_D459_4B15_8863_ADDC0C1F5B4A

void wazuh_initialize();
void wazuh_finalize();
[[gnu::format(printf, 1, 2), gnu::nonnull(1)]] void wazuh_log_message(const char* format, ...);

#endif /* C753DACF_D459_4B15_8863_ADDC0C1F5B4A */
