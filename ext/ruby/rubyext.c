//
// rnmsg /|\ https://github.com/localhost/rnmsg
//           a Ruby binding for nanomsg (http://nanomsg.org)
//
// Copyright (c) 2014 Alex Brem
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:

// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.

// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
// THE SOFTWARE.
//

#include <ruby.h>
#include <ruby/thread.h>

#include <stdio.h>
#include <string.h>

// core
#include <nanomsg/nn.h>

// protocols
#include <nanomsg/pair.h>
#include <nanomsg/reqrep.h>
#include <nanomsg/pubsub.h>
#include <nanomsg/survey.h>
#include <nanomsg/pipeline.h>
#include <nanomsg/bus.h>

// transports
#include <nanomsg/inproc.h>
#include <nanomsg/ipc.h>
#include <nanomsg/tcp.h>

static VALUE mRnmsg, cSocket;

typedef struct {
    int fd;
} Socket;

static void rb_socket_free(Socket *S) {
    nn_shutdown(S->fd, 0);
    xfree(S);
}

VALUE rb_socket_alloc(VALUE klass) {
    Socket *S = ALLOC(Socket);
    S->fd = -1;
    return Data_Wrap_Struct(klass, NULL, rb_socket_free, S);
}

VALUE rb_socket_initialize(VALUE self, VALUE domain, VALUE protocol) {
    Socket *S;
    Data_Get_Struct(self, Socket, S);

    if (S)
        S->fd = nn_socket(NUM2INT(domain), NUM2INT(protocol));

    return Qnil;
}

VALUE rb_socket_bind(VALUE self, VALUE addr) {
    Socket *S;
    Data_Get_Struct(self, Socket, S);

    if (!S || S->fd == -1)
        return Qnil;

    const char *addr_c = StringValueCStr(addr);
    const int endpoint_id = nn_bind(S->fd, addr_c);
    return (endpoint_id == -1) ? Qnil : INT2NUM(endpoint_id);
}

VALUE rb_socket_poll(VALUE self, VALUE mask, VALUE timeout) {
    Socket *S;
    Data_Get_Struct(self, Socket, S);

    if (!S || S->fd == -1)
        return Qnil;

    if (timeout == Qnil)
        timeout = INT2NUM(0);

    struct nn_pollfd pfd;
    pfd.fd = S->fd;
    pfd.events = NUM2INT(mask);

    const int res = nn_poll(&pfd, 1, NUM2INT(timeout));
    if (res == -1) // error
        return Qnil;
    else if (res == 0) // timeout
        return Qfalse;

    return (pfd.revents & NUM2INT(mask)) ? Qtrue : Qfalse;
}

VALUE rb_socket_send_msg(VALUE self, VALUE obj, VALUE mask) {
    Socket *S;
    Data_Get_Struct(self, Socket, S);

    if (!S || S->fd == -1)
        return Qnil;

    const int msg_bytes = RSTRING_LEN(obj);
    const char *data = RSTRING_PTR(obj);

    void *msg = nn_allocmsg(msg_bytes, 0);
    const int nbytes = nn_send(S->fd, &msg, NN_MSG, NN_DONTWAIT);
    if (nbytes < 0)
        return Qnil;

    return (nbytes == msg_bytes) ? Qtrue : Qfalse;
}

VALUE rb_socket_recv_msg(VALUE self) {
    Socket *S;
    Data_Get_Struct(self, Socket, S);

    if (!S || S->fd == -1)
        return Qnil;

    void *buffer;
    const int nbytes = nn_recv(S->fd, &buffer, NN_MSG, NN_DONTWAIT);
    if (nbytes < 0)
        return Qnil;

    VALUE rs = rb_tainted_str_new(buffer, nbytes);
    nn_freemsg(buffer);

    return rs;
}

VALUE rb_socket_close(VALUE self) {
    Socket *S;
    Data_Get_Struct(self, Socket, S);

    if (S && S->fd != -1) {
        nn_close(S->fd);
        S->fd = -1;
    }

    return self;
}

VALUE rb_socket_shutdown(VALUE self, VALUE how) {
    Socket *S;
    Data_Get_Struct(self, Socket, S);

    if (S && S->fd != -1) {
        nn_shutdown(S->fd, NUM2INT(how));
    }

    return self;
}

VALUE rb_socket_get_fd(VALUE self) {
    Socket *S;
    Data_Get_Struct(self, Socket, S);
    return rb_int_new(S->fd);
}

VALUE rb_socket_get_sysfd(VALUE self) {
    Socket *S;
    Data_Get_Struct(self, Socket, S);

    int optval;
    size_t optval_len = sizeof(int);
    const int res = nn_getsockopt(S->fd, NN_SOL_SOCKET, NN_RCVFD, (void *)&optval, &optval_len);
    return INT2NUM((res == 0) ? optval : -1);
}

VALUE rb_socket_term(VALUE klass) {
    nn_term();
    return klass;
}

VALUE rb_socket_device(VALUE klass, VALUE so1, VALUE so2) {
    Socket *S1;
    Data_Get_Struct(so1, Socket, S1);

    Socket tmp = { .fd = -1 };
    Socket *S2 = &tmp;

    if (so2 != Qnil) // FIXME
        Data_Get_Struct(so2, Socket, S2);

    return nn_device(S1->fd, S2->fd);
}

void Init_rnmsg() {
    mRnmsg = rb_define_module("Nanomsg");

    cSocket = rb_define_class_under(mRnmsg, "Socket", rb_cObject);
    rb_define_alloc_func(cSocket, rb_socket_alloc);
    rb_define_method(cSocket, "initialize", rb_socket_initialize, 2);
    rb_define_method(cSocket, "bind", rb_socket_bind, 1);
    rb_define_method(cSocket, "poll", rb_socket_poll, 2);
    rb_define_method(cSocket, "send_msg", rb_socket_send_msg, 1);
    rb_define_method(cSocket, "recv_msg", rb_socket_recv_msg, 0);
    rb_define_method(cSocket, "close", rb_socket_close, 0);
    rb_define_method(cSocket, "shutdown", rb_socket_shutdown, 1);

    rb_define_method(cSocket, "get_fd", rb_socket_get_fd, 0);
    rb_define_method(cSocket, "get_sysfd", rb_socket_get_sysfd, 0);

    rb_define_singleton_method(cSocket, "device", rb_socket_device, 2);
    rb_define_singleton_method(cSocket, "term", rb_socket_term, 0);

    for (int i = 0, value;; ++i) {
        const char *name = nn_symbol(i, &value);
        if (!name)
            break;
        if (strncmp("NN_", name, 3) == 0 || strncmp("AF_", name, 3) == 0)
            rb_const_set(mRnmsg, rb_intern(name), INT2NUM(value));
    }

    // nanomsg events (missing from nn_symbol)
    rb_const_set(mRnmsg, rb_intern("NN_POLLIN"), INT2NUM(NN_POLLIN));
    rb_const_set(mRnmsg, rb_intern("NN_POLLOUT"), INT2NUM(NN_POLLOUT));
}
