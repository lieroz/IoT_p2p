#include "node.h"

int main(int argc, const char *argv[])
{
    if (argc < 2)
    {
        return 1;
    }

    node::start(argv[1]);
    return 0;
}
