#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main(int argc, char* argv[])
{
    if (argc < 2)
    {
        printf("Usage: ./mqtt_spammer times");
        return 1;
    }

    for (int i = 0; i < atoi(argv[1]); i++)
    {
        char cmd[100];
        snprintf(cmd, 100, "./test_pub_sub pub%d %d %d &", i, i*100 + 1, (i+1)*100);
        system(cmd);
    }

    return 0;
}