#include "node.h"

int main(int argc, const char *argv[])
{
    if (argc < 2)
    {
        return 1;
    }

    Node(5000).start(argv[1]);
    return 0;
}
