#ifndef BITMAP_H
#define BITMAP_H

#define bitmap_bytes(a, b)  ((((a) * (b)) % (sizeof(unsigned int) * 8)) ? \
                            (((a) * (b)) / (sizeof(unsigned int) * 8)) + 1 : \
                            (((a) * (b)) / (sizeof(unsigned int) * 8)))

void set(unsigned int *bitmap, unsigned long n)
{
    bitmap += (n / sizeof(unsigned int));
    n = n % sizeof(unsigned int);
    *bitmap |= (1 << n);
}

void unset(unsigned int *bitmap, unsigned long n)
{
    bitmap += (n / sizeof(unsigned int));
    n = n % sizeof(unsigned int);
    *bitmap &= ~(1 << n);
}

int test(unsigned int *bitmap, unsigned long n)
{
    bitmap += (n / sizeof(unsigned int));
    n = n % sizeof(unsigned int);
    return *bitmap & (1 << n);
}

#endif
