#include<headders/lexer.h>
#include<string.h>


void print_help(const char* program_name) {
    printf("This is an interpreter program\n");
    printf("Program interprets code in a provided [file].\n");
    printf("If no [file] is provided the stdin is used.\n");
    printf("Useage: %s [-h|--help] [file]\n",program_name);
    printf("Options:\n");
    printf("\t -h, --help:\tDisplays this message\n");
}

typedef struct {
    FILE* file;
} setup_t;


int parse_args(setup_t *config,int argc, const char *argv[]) {
    if (argc > 2) {
        fprintf(stderr,"Invalid number of parameters!\n");
        print_help(argv[0]);
        return -1;
    }

    if (argc == 1) {
        config->file = stdin;
        return 1;
    }

    if(!strcmp(argv[1],"-h") || !strcmp(argv[1],"--help")) {
        print_help(argv[0]);
        return 0;
    }

    if (argv[1][0] == '-' ) {
        fprintf(stderr,"Invalid option!\n");
        print_help(argv[0]);
        return -1;
    }

    config->file = fopen(argv[1],"r");
    return 1;
}


int main(int argc, const char* argv[]) {
    setup_t config;
    int ret = parse_args(&config,argc,argv);

    if (ret <= 0) {
        return ret;
    }

    if (config.file  == NULL) {
        fprintf(stderr,"Failed to open %s\n",argv[1]);
        return -2;
    }

    LexerContext ctx;
    if(!lexer_init(&ctx,config.file)) {
        fprintf(stderr,"Failed to initialize lexer\n");
        return -3;
    }

    if(config.file != stdin) {
        fclose(config.file);
    }

    lexer_free(&ctx);
    return 0;

}