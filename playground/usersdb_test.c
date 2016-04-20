#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "usersdb.h"

#define USERDB_CVS_FILENAME "../db/users.csv"

/// loads the use database CSV file into RAM, while prepending the
/// size of the file in ASCII as new first line.
static char* load_complete_csv(const char* filename) {
    FILE* f = fopen(filename, "rb");
    if (!f) return NULL;
    if (fseek(f, 0, SEEK_END) != 0) {
        fclose(f);
        return NULL;
    }
    long size = ftell(f);
    fseek(f, 0, SEEK_SET);
    char* data = malloc(size + 256 + 256);
    if (data == NULL) {
        fclose(f);
        return NULL;
    }
    if (fread(data + 256, 1, size, f) != size) {
        free(data);
        fclose(f);
        return NULL;
    }
    fclose(f);
    data += 256;

    // make sure that the final line is terminated with a linefeed
    if (data[size-1] != '\n') { 
        data[size++] = '\n';
    }
    data[size] = '\0';

    // prepend filesize line
    char size_as_text[24];
    int stlen = sprintf(size_as_text, "%li", size);
    data -= stlen + 1;
    memcpy(data, size_as_text, stlen);
    data[stlen] = '\n';

    return data;
}

int main() {
    const char* userdb = load_complete_csv(USERDB_CVS_FILENAME);
    if (!userdb) {
        fputs("Error while loading " USERDB_CVS_FILENAME, stderr);
        return 1;
    }
    char text[256] = "";
    static const long lookup_ids[] = {
        2623666,
        2780011,
        2780099
    };
    const int count = sizeof(lookup_ids) / sizeof(lookup_ids[0]);
    for (int i = 0; i < count; ++i) {
        if (find_dmr_user(text, lookup_ids[i], userdb, sizeof text)) {
            printf("Entry for ID %li: \"%s\"\n", lookup_ids[i], text);
        } else {
            printf("Entry for ID %li NOT found!\n", lookup_ids[i]);
        }
    }
    return 0;
}

