#include "header.h"

int splitter(char* line, char* fields[], int max) // gets rid of strtok errors when line is "x, y,, z, a"
{
	int count = 0;
	char* p = line;
	for (char* q = line; *q != '\0'; q++)
	{
		if (*q == '\n')
		{
			*q = '\0';
			break;
		}
	}

	fields[count++] = p;

	while (*p != '\0' && count < max)
	{

		if (*p == ',')
		{
			*p = '\0';
			fields[count++] = p + 1;
		}
		p++;
	}

	while (count < max)
	{
		fields[count++] = "";
	}

	return count;
}

int getHour(char* minuteStr)
{
	return atoi(minuteStr);
}