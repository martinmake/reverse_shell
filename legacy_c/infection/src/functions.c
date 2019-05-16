#include <stdio.h>
#include <stddef.h>
#include <string.h>
#include <inttypes.h>

void read_to_eof(char* const _s, size_t size, FILE* fr)
{
	char *s = _s;
	char c;

	while ((c = getc(fr)) != EOF && (s - _s) < (long int) size)
		*s++ = c;
}

uint8_t is_str_command(const char* str, const char* command)
{
	size_t command_len = strlen(command);

	for (size_t i = 0; i < command_len; i++)
		if (str[i] != command[i])
			return 0;

	return 1;
}
