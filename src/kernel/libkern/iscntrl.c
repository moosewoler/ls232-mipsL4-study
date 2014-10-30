/** iscntrl(c) returns true if c is delete or control character */
int iscntrl (int c)
{
    if (c == 0x7f)
	return (1);
    if (c < ' ')
	return (1);
    return (0);
}
