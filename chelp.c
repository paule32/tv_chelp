#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>

#define false 0
#define true  1

typedef uint8_t   BYTE;
typedef uint16_t  WORD;
typedef uint32_t DWORD;

int  hlpFileVersion;       // for future use: version
int  hlpLanguage;          // language for help file
char hlpFileCompressed[2]; // help file is compressed?

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
    int pos = 0;

    hlpFileCompressed[0] = 'n';
    hlpFileCompressed[1] = 0x0;

    // open source file in read-mode ...
    FILE *src = fopen("help.txt","r");
    if (!src) {
        fprintf(stderr,"help.txt can't be opened.\n");
        fflush (stderr);
        return 1;
    }

    // temp file for index names storage ...
    FILE *tmp1 = fopen("tmp1.dat","wb");
    if (!tmp1) {
        fprintf(stderr,"tmp1.dat can't be openend.\n");
        fflush (stderr);
        fclose(src);
        return 1;
    }

    // temp file for index names storage ...
    FILE *tmp2 = fopen("tmp2.dat","wb");
    if (!tmp2) {
        fprintf(stderr,"tmp2.dat can't be openend.\n");
        fflush (stderr);
        fclose(tmp1);
        fclose(src );
        return 1;
    }

    // help header content ...
    FILE *tmp3 = fopen("tmp3.dat","wb");
    if (!tmp3) {
        fprintf(stderr,"tmp3.dat can't be openend.\n");
        fflush (stderr);
        fclose(tmp1);
        fclose(tmp2);
        fclose(src );
        return 1;
    }

    // help header content index len ...
    FILE *tmp4 = fopen("tmp4.dat","wb");
    if (!tmp4) {
        fprintf(stderr,"tmp4.dat can't be openend.\n");
        fflush (stderr);
        fclose(tmp1);
        fclose(tmp2);
        fclose(tmp3);
        fclose(src );
        return 1;
    }

    // first line is always the version
    res = fscanf(src,"version: 0x%x\n",         &hlpFileVersion  );
    res = fscanf(src,"hlplang: %d\n"  ,         &hlpLanguage     );
    res = fscanf(src,"packed: %1s\n"  , (char*) &hlpFileCompressed);
    res = fscanf(src,"topics: %d\n"   ,         &topic_count);

    fprintf( stdout, "version: 0x%x\n",         hlpFileVersion    );
    fprintf( stdout, "hlplang: %d\n"  ,         hlpLanguage       );
    fprintf( stdout, "packed:  %1s\n" , (char*) hlpFileCompressed );
    fprintf( stdout, "topics:  %d\n"  ,         topic_count       );
    fflush ( stdout );

    // header
    fwrite( &hlpFileVersion   , sizeof(int), 1, tmp2 );
    fwrite( &hlpLanguage      , sizeof(int), 1, tmp2 );
    fwrite( &hlpFileCompressed,           2, 1, tmp2 );
    fwrite( &topic_count      , sizeof(int), 1, tmp2 );

    // header length ...
    pos = ((3   * sizeof(int)) + 2);
    fwrite( &pos, sizeof(int), 1, tmp2 );

    int endmark = 0;
    topic_size  = 0;

    while (1) {
        if (++endmark > topic_count)
        break;

        res = fscanf(src,"[begin 0x%x ", &topic_number);
        res = fscanf(src,"%1023s ]\n"  ,  topic_text);

        // cut: first & last ...
        char *p = topic_text;
        p++;
        p[strlen(p)-1] = 0x00;
        uint8_t po = strlen(p)+ 1;

        fwrite(&po,1,1, tmp1 );     // length of string (topic)
        fwrite(p, po,1, tmp1 );     // the string itself

        len = 0;

        // topic content text ...
        while (1) {
            res  = fscanf(src,"%1023s\n", topic_text);
            len += strlen(topic_text);

            if (topic_text[0] == '['
            &&  topic_text[1] == 'e'
            &&  topic_text[2] == 'n'
            &&  topic_text[3] == 'd'
            &&  topic_text[4] == ']')
            {
                len -= 5;
                break;
            }
            fwrite(topic_text,strlen(topic_text),1,tmp3);
        }   fwrite(&len,sizeof(len),1,tmp4);
    }

    fclose(src );

    fclose(tmp1);
    fclose(tmp2);
    fclose(tmp3);
    fclose(tmp4);

    res = system("./merge.sh");
    return 0;
}

