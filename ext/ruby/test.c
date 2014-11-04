#include <stdio.h>

#include <nanomsg/nn.h>
#include <nanomsg/pipeline.h>
#include <nanomsg/tcp.h>

int main(int argc, char **argv) {
    int fd = nn_socket(AF_SP, NN_PULL);
    int endpoint_id = nn_bind(fd, "tcp://127.0.0.1:4000");

    for (;;) {
        puts("LOOP");

        struct nn_pollfd pfd;
        pfd.fd = fd;
        pfd.events = NN_POLLIN;
        int res = nn_poll(&pfd, 1, 2000);
        if (res < 0) {
            puts("Error: nn_poll");
            break;
        } else if (res == 0)
            continue;

        if (pfd.revents & NN_POLLIN) {
            // void *msg = nn_allocmsg(512, 0);
            void *buffer;
            // void *ptr = &buffer;

            int nbytes = nn_recv(fd, &buffer, NN_MSG, 0); // NN_DONTWAIT
            if (nbytes < 0) {
                puts("Error: nn_recv");
                break;
            }

            // struct nn_cmsghdr *hdr = NN_CMSG_FIRSTHDR(msg);
            // unsigned char *data = NN_CMSG_DATA(hdr);

            printf("--\n%i:%s\n--\n", nbytes, (char *)buffer);
            nn_freemsg(buffer);
        }

    }

    return 0;
}
