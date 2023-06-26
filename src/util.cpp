#include "util.hpp"
#if defined(_WIN32)
#include <windows.h>
#else
#include <unistd.h>
#endif

// Returns a malloc-allocated string 
char *string_duplicate(const char *string) {
	if (string == nullptr) {
		return nullptr;
	}
	size_t count = strlen(string)+1;
	char *new_string = (char*)malloc(count);

	memcpy(new_string, string, count);
	return new_string;
}
