#include "mptcp_core.h"

extern void start_client();

extern void *clientThread(void *vargp);

/* utility functions */
extern char* concat(const char *s1, const char *s2);
extern char* intToChar(int num);
extern void chopPrefix(char *str);

