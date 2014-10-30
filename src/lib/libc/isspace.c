/** isspace(c) returns 1 if c == tab newline or space */
int isspace (int c)
{
    switch (c) {
    case ' ':
    case '\t':
    case '\n':
    case '\r':
    case '\f':
    case '\v':
	return (1);
    }
    return (0);
}
