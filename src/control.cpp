#include <lwip/sockets.h>  

void setup_tcp() {
    /* create recv buffer */
    char rx_buffer[BUFSIZ] = {0};

    /* create socket */
    int sock = socket(PF_INET, SOCK_STREAM, 0);
    //if (sock < 0) ESP_LOGE("No se pudo crear el socket");
    int opt = 1;
    /* set socket options (listen socket) */
    setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));
    struct sockaddr_in sa;
    sa.sin_addr.s_addr = htonl(INADDR_ANY);
    sa.sin_family = AF_INET;
    sa.sin_port = htons(6666);

    /* bind socket */
    int err = bind(sock, (struct sockaddr *)&sa, sizeof(sa));
    //if (err != 0) ESP_LOGE("No se pudo vincular el socket");
    /* start listening in the socket */
    err = listen(sock, 1);

    for(;;) {
        int len = 0;
        do {
            len = recv(sock, rx_buffer, sizeof(rx_buffer) - 1, 0);
            rx_buffer[len] = 0;
            //ESP_LOGI(rx_buffer);
        } while (len > 0);
    }

}