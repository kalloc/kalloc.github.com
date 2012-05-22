/*
 * main.c
 *
 *  Created on: 23.01.2010
 *      Author: nick
 */

#include "include.h";

struct config {
    char *cmdline;
    char *strHost;
    char *strPort;
    char *path;
    int port;
    int level;
} config = {"", "0.0.0.0", "8888", "log/%d.%02d.%02d_%02d.%02d.log", 8888, 0};

void inline terminate(char *err) {
    printf("\n\t\t--=:[ ssDNSTester help ]:=--\n\n"
            " запускать так %s --ip 0.0.0.0 --port 8888\n\n"
            " --ip - это айпи на котором будет приниматься запросы\n"
            " --level - уровень дебага\n"
            " --port - port на который будут приходить запрос\n\n", config.cmdline);
    if (err) {
        printf("При запуске произошла ошибка — %s\n\n", err);
        exit(-1);
    }
    exit(0);

}

inline int getLogLevel() {
    return config.level;
}

inline int getLogPath() {
    return config.path;
}

inline char *getHostString() {
    return config.strHost;
}

inline char *getPortString() {
    return config.strPort;
}

int getPort() {
    if (!config.port) config.port = atoi(config.strPort);
    return config.port;
}

int main(int argc, char **argv) {
    evutil_make_listen_socket_reuseable(1);
    

    config.cmdline = argv[0];
    const char* short_options = "hi::p::L::";

    const struct option long_options[] = {
        { "help", no_argument, NULL, 'h'},
        { "ip", required_argument, NULL, 'i'},
        { "port", required_argument, NULL, 'p'},
        { "level", required_argument, NULL, 'L'},
        { NULL, 0, NULL, 0}
    };

    int rez;
    int option_index;

    while ((rez = getopt_long(argc, argv, short_options, long_options,
            &option_index)) != -1) {

        switch (rez) {
            case 'h':
                help();
                break;
            case 'i':
                if (optarg != NULL)
                    config.strHost = optarg;
                break;
            case 'p':
                if (optarg != NULL)
                    config.strPort = optarg;
                break;
            case 'L':
                if (optarg != NULL)
                    config.level = atoi(optarg);
                break;
        };
    };
    if (!config.strHost or !config.strPort) {
        if (!config.strHost and config.strPort) {
            die("Не указан или не валиден ip");
        } else if (config.strHost and !config.strPort) {
            die("Не указан Port");
        }
        help();
    }
    printf("Listen On %s:%s\n", config.strHost, config.strPort);
    initDig();
    initHttp();

    return 0;
}
