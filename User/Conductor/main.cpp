extern "C" void testprint(char*);

static char* PrintInt(char *bob, int value)
{
    *bob = '0' + (value % 10);
    if (value != 0) {
        value /= 10;
        if (value != 0)
            return PrintInt(bob - 1, value);
    }
    return bob;
}

extern "C" void sysmain(void)
{
    char buf[10];
    int x = 0;

bob:
    buf[sizeof(buf) - 1] = '\0';
    
    testprint("Userspace taunt! ");
    testprint(PrintInt(buf - 1, x++));
    testprint("\n");
    for (int i = 0; i <100000; i++)
        for (int j = 0; j <1000; j++);
    goto bob;
}
