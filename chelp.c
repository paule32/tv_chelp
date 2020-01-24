#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>

#define false 0
#define true  1

typedef uint8_t   BYTE;
typedef uint16_t  WORD;
typedef uint32_t DWORD;

int hlpFileVersion;    // for future use: version
int hlpFileCompressed; // help file is compressed?

int  topic_count  ;     // # of topic's
int  topic_number ;     // #num index of topic size
int  topic_index  ;     // index      of topic
int  topic_size   ;     // name    of topic
char topic_text[1024];  // content of topic

int main(int argc, char **argv)
{
    int res = 0;
    int len = 0;
    int chr = 0;

    hlpFileVersion    = 0x1000;   // current version
    hlpFileCompressed =    '0';

    // open source file in read-mode ...
    FILE *src = fopen("help.txt","r");
    if (!src) {
        fprintf(stderr,"help.txt can't be opened.\n");
        fflush (stderr);
        return 1;
    }

    // open destination file in write-binary-mode ...
    FILE *dst = fopen("help.hlp","wb");
    if (!dst) {
        fprintf(stderr,"help.hlp can't be opened.\n");
        fflush (stderr);
        fclose(src);
        return 1;
    }

    // first line is always the version
    res = fscanf(src,"version: 0x%x\n", &hlpFileVersion  );
    res = fscanf(src,"packed: %d\n"   , &hlpFileCompressed);
    res = fscanf(src,"topics: %d\n"   , &topic_count);

    fprintf(stdout, "version: 0x%x\n", hlpFileVersion );
    fprintf(stdout, "packed:  %c\n"  , hlpFileCompressed);
    fprintf(stdout, "topics:  %d\n"  , topic_count);
    fflush (stdout);

    fwrite( &hlpFileVersion   , sizeof(int ), 1, dst );
    fwrite( &hlpFileCompressed, sizeof(char), 1, dst );
    fwrite( &topic_count      , sizeof(int ), 1, dst );

    int endmark = 0;
    topic_size  = 0;

    while (1) {
        if (++endmark > 1)
        break;

        res = fscanf(src,"[begin 0x%x 0x%x]\n",
        &topic_number,
        &topic_index );
        topic_size = 0;

        fprintf(stdout,"[begin 0x%x 0x%x]\n",
        topic_number,
        topic_index );

        while (1) {
            res = fscanf(src,"%1023s\n", topic_text);
            len = strlen(topic_text);
            topic_size += len;

            if (topic_text[0] == '['
            &&  topic_text[1] == 'e'
            &&  topic_text[2] == 'n'
            &&  topic_text[3] == 'd'
            &&  topic_text[4] == ']') {
                fflush(stdout);
                break;
            }

            fprintf(stdout,"%s", topic_text);
        }
    }

    fclose(src);
    fclose(dst);

    return 0;
}

