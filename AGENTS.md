# Repository instructions

## Project

`firewall-ipset` is a short-lived, stateful Wazuh Active Response program written in C23. It runs on Linux, typically as root under `wazuh-execd`, and manages ipsets through libipset. CMake configures the build; libcjson provides JSON support. Favor correctness, security, and clear error handling over micro-optimization.

## Protocol and safety

- Treat stdin JSON as untrusted: validate its structure, values, lengths, and ranges before use. Use `read_and_parse()` for incoming messages and the existing JSON helpers for extraction.
- For `add`, send a `check_keys` message, wait for Wazuh's response, and add the IP only on `continue`. An `abort` succeeds without changing the set; an invalid response fails. For `delete`, remove the IP directly.
- Reserve stdout for JSON protocol messages. Use `send_control_message()` and `json_build_control_message()` for control messages; ensure each message is one newline-terminated line, flush it before waiting for a response, and handle output errors. Send diagnostics through `wazuh_log_message()`.
- Keep reads bounded by `MAX_BUF_SIZE` and reject overlong input. Avoid unbounded loops and unnecessary blocking. When logging external data, prevent control characters from forging log entries.
- Assume root privileges. Do not introduce process creation, shell commands, network I/O, background processes or threads, privilege changes, or arbitrary file I/O unless explicitly required by the task. Use narrowly scoped helpers and existing abstractions.
- Check allocations and relevant helper return values; handle errors explicitly and release owned resources. Use `OS_SUCCESS` and `OS_FAILURE` where appropriate. Prefer bounded operations and clear ownership over clever pointer manipulation.

## Code conventions and interfaces

- Follow the existing C style in `src/`, use descriptive `snake_case`, keep internal functions `static`, and avoid unrelated refactors. Do not introduce C++ or change public interfaces or move files unless requested.
- `main.c` coordinates initialization, parsing, dispatch, and cleanup. `commands.c` implements add/delete flows; `protocol.c` handles message I/O; `json.c`, `utils.c`, `ipset.c`, and `wazuh.c` own their respective helpers. Extend the relevant module and header when needed.
- Use `json_get_srcip()`, `json_get_command()`, and `utils_parse_command()` for alert and response fields. `utils_get_ip_family()` validates an address and returns 4, 6, or -1; `utils_get_list_name()` selects the IPv4 or IPv6 set from the two `parameters.extra_args` entries.
- Use `ipset_initialize()`, `ipset_add_ip()`, `ipset_del_ip()`, and `ipset_finalize()` for ipset operations. Check operation results. Initialize Wazuh logging before use and finalize resources on exit.
- Respect cJSON ownership: `json_build_control_message()` takes ownership of its keys array; its returned string belongs to the caller. Check the relevant implementation and library contract when changing failure paths.

## Build and validation

- Install CMake and development packages for `libipset` and `libcjson`, then run `cmake -S . -B build` and `cmake --build build -v`. Add new source files and dependencies to the root `CMakeLists.txt`.
- CI currently builds the project; it does not define an automated test suite. Add or run focused tests appropriate to behavioral changes, and report any validation that could not be performed.
- Runtime ipset checks require suitable Linux privileges and pre-existing sets; they can alter firewall state. The README describes a local protocol exercise, which does not fully emulate Wazuh. Do not treat a build alone as protocol or kernel integration validation.
