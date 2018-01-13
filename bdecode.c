#include <ctype.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#define TAB_STRING     "  "
#define XML_BYTESTRING "BYTESTRING"
#define XML_INTEGER    "INTEGER"
#define XML_LIST       "LIST"
#define XML_PAIR       "PAIR"
#define XML_DICTIONARY "DICTIONARY"
#define XML_LENGTH     "length"
#define XML_BDECODE    "BDECODE"

FILE *input = NULL;

int mygetchar () {
        int r;
        if ( input == NULL ) {
                r = getchar();
        }
        else {
                r = fgetc( input );
        }
        return r;
}

void tabber(int t){
        while(t-->0) printf(TAB_STRING);
}

void xmlprint(const int c){
        if ( c == '&' ) {
                printf("&amp;");
        } else if ( c == '<' ) {
                printf("&lt;");
        } else if ( c == '>' ) {
                printf("&gt;");
        } else if ( c == '"' ) {
                printf("&quot;");
        } else if ( c == '\'' ) {
                printf("&apos;");
        } else {
                if (isgraph(c)) {
                        putchar(c);
                } else {
                        printf("&#%d;",c);
                }
        }
}

int bdecode(const int tab){
        int c;
        int i;
        while ((c = mygetchar()) != EOF) {
                if (isdigit(c)){
                        tabber(tab); printf("<%s %s=\"",XML_BYTESTRING,XML_LENGTH);
                        i = 0;
                        unsigned int len = 0;
                        while (isdigit(c)) {
                                len = 10 * len + ( c - '0' );
                                putchar (c);
                                c = mygetchar();
                        }
                        printf ("\">");
                        int j;
                        for (j=0;j<len;j++) {
                                c = mygetchar();
                                xmlprint (c);
                        }
                        printf("</%s>\n",XML_BYTESTRING);
                        return 1;
                }else if( c == 'i'){
                        i = 0;
                        tabber(tab); printf("<%s>",XML_INTEGER);
                        c = mygetchar();
                        if(c == '-'){
                                printf("-");
                                c = mygetchar();
                        }
                        while (isdigit(c)){
                                putchar (c);
                                c = mygetchar();
                        }
                        printf ("</%s>\n",XML_INTEGER);
                        return 1;
                }else if(c == 'l'){
                        tabber(tab); printf("<%s>\n",XML_LIST);
                        while( bdecode(tab+1) == 1 );
                        tabber(tab); printf("</%s>\n",XML_LIST);
                        return 1;
                }else if( c == 'd'){
                        i = 1;
                        tabber(tab); printf("<%s>\n",XML_DICTIONARY);
                        while(i == 1){
                                tabber(tab+1); printf("<%s>\n",XML_PAIR);
                                i = bdecode(tab+2);
                                if ( i != 0 ) i = bdecode(tab+2);
                                tabber(tab+1); printf("</%s>\n",XML_PAIR);
                        }
                        tabber(tab); printf("</%s>\n",XML_DICTIONARY);
                        return 1;
                }else if( c == 'e' ){
                        return 0;
                }
                printf("Error: Wrong bencoded data!\n");
                return 0;
        }
}

int main(int argc, char *argv[]){
        int n = 1;
        int o = 1;
        int f = 0;
        int r = 0;
        if ( argc>1 ) {
                int j;
                for (j=1;j<argc;j++) {
                        if ( strcmp ( argv[j], "-h" ) == 0 ) {
                                printf ("Usage: %s [-n] [-o] [<file>]\n", argv[0]);
                                printf ("-n     do not print the standard xml header\n");
                                printf ("-o     do not print the root tag.\n");
                                printf ("<file> bencoded file (usually torrent file)\n\n");
                                printf ("Note:\n");
                                printf ("If you choose not to print the root tag then\n");
                                printf ("also the xml header will be omitted even if\n");
                                printf ("you do not specify the -n switch!\n");
                                printf ("If no file is specified the stdin will be read\n");
                                return 0;
                        }
                        if ( strcmp ( argv[j], "-n" ) == 0 ) {
                                n = 0;
                        }
                        else if ( strcmp ( argv[j], "-o" ) == 0 ) {
                                o = 0;
                        }
                        else if ( argv[j][0] != '-' ) {
                                input = fopen ( argv[j], "rb" );
                                if ( input == NULL ) {
                                        printf("Unable to open file %s\n",argv[j]);
                                        return 1;
                                }
                                f = 1;
                        }
                }
        }
        if (( n == 1 ) && ( o == 1 )) {
                printf ("<?xml version=\"1.0\" encoding=\"ISO-8859-1\" ?>\n");
                printf ("<?xml-stylesheet type=\"text/xsl\" ?>\n");
        }
        if ( o == 1 ) {
                printf ("<%s>\n",XML_BDECODE);
                bdecode(1);
                printf ("</%s>\n",XML_BDECODE);
        }
        else {
                bdecode(0);
        }
        if ( f == 1 ) {
                r = fclose ( input );
                if ( r != 0 ) {
                        r = 1;
                }
        }
        return r;
}
