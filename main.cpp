#include "COW_library.h"

int main() {
    VersionedFile file("testfile.txt");
    file.create();
    file.open();
    file.close();
    return 0;
}