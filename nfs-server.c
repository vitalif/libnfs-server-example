#define _FILE_OFFSET_BITS 64
#ifndef _GNU_SOURCE
#define _GNU_SOURCE
#endif

#include <poll.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <time.h>

#include <event2/event.h>

#include "nfs-service.h"

struct event_base *base;

struct server
{
    struct rpc_context *rpc;
    struct event *read_event;
    struct event *write_event;
};

struct mapping
{
    struct mapping *next;
    u_int prog;
    u_int vers;
    int port;
    char *netid;
    char *addr;
    char *owner;
};
struct mapping *map;

void free_map_item(struct mapping *item)
{
    free(item->netid);
    free(item->addr);
    free(item->owner);
    free(item);
}

static void free_server(struct server *server)
{
    if (server->rpc)
    {
        rpc_disconnect(server->rpc, NULL);
        rpc_destroy_context(server->rpc);
    }
    if (server->read_event)
    {
        event_free(server->read_event);
    }
    if (server->write_event)
    {
        event_free(server->write_event);
    }
    free(server);
}

/*
 * Based on the state of libnfs and its context, update libevent
 * accordingly regarding which events we are interested in.
 */
static void update_events(struct rpc_context *rpc, struct event *read_event, struct event *write_event)
{
    int events = rpc_which_events(rpc);
    if (read_event)
    {
        if (events & POLLIN)
            event_add(read_event, NULL);
        else
            event_del(read_event);
    }
    if (write_event)
    {
        if (events & POLLOUT)
            event_add(write_event, NULL);
        else
            event_del(write_event);
    }
}

/*
 * Add a registration for program,version,netid.
 */
int pmap_register(int prog, int vers, char *netid, char *addr, char *owner)
{
    struct mapping *item;
    char *str;
    int count = 0;

    item = malloc(sizeof(struct mapping));
    item->prog  = prog;
    item->vers  = vers;
    item->netid = netid;
    item->addr  = addr;
    item->owner = owner;

    /* The port are the last two dotted decimal fields in the address */
    for (str = item->addr + strlen(item->addr) - 1; str >= item->addr; str--)
    {
        if (*str != '.')
        {
            if (*str < '0' || *str > '9')
                break;
            continue;
        }
        count++;
        if (count == 2)
        {
            int high, low;
            sscanf(str, ".%d.%d", &high, &low);
            item->port = high * 256 + low;
            break;
        }
    }

    item->next = map;
    map = item;
}

/*
 * Find and return a registration matching program,version,netid.
 */
struct mapping *map_lookup(int prog, int vers, char *netid)
{
    struct mapping *tmp;
    for (tmp = map; tmp; tmp = tmp->next)
    {
        if (tmp->prog != prog)
            continue;
        if (tmp->vers != vers)
            continue;
        if (strcmp(tmp->netid, netid))
            continue;
        return tmp;
    }
    return NULL;
}

/*
 * Remove a registration from our map or registrations.
 */
void map_remove(int prog, int vers, char *netid)
{
    struct mapping *prev = NULL;
    struct mapping *tmp;
    for (tmp = map; tmp; prev = tmp, tmp = tmp->next)
    {
        if (tmp->prog != prog)
            continue;
        if (tmp->vers != vers)
            continue;
        if (strcmp(tmp->netid, netid))
            continue;
        break;
    }
    if (tmp == NULL)
        return;
    if (prev)
        prev->next = tmp->next;
    else
        map = tmp->next;
    free_map_item(tmp);
    return;
}

/*
 * The NULL procedure. All protocols/versions must provide a NULL procedure
 * as index 0.
 * It is used by clients, and rpcinfo, to "ping" a service and verify that
 * the service is available and that it does support the indicated version.
 */
static int pmap2_null_proc(struct rpc_context *rpc, struct rpc_msg *call)
{
    rpc_send_reply(rpc, call, NULL, (zdrproc_t)zdr_void, 0);
    return 0;
}

/*
 * v2 GETPORT.
 * This is the lookup function for portmapper version 2.
 * A client provides program, version and protocol (tcp or udp)
 * and portmapper returns which port that service is available on,
 * (or 0 if no such program is registered.)
 */
static int pmap2_getport_proc(struct rpc_context *rpc, struct rpc_msg *call)
{
    PMAP2GETPORTargs *args = call->body.cbody.args;
    struct mapping *tmp;
    char *netid;
    uint32_t port = 0;
    if (args->prot == IPPROTO_TCP)
        netid = "tcp";
    else
        netid = "udp";
    tmp = map_lookup(args->prog, args->vers, netid);
    if (tmp)
        port = tmp->port;
    rpc_send_reply(rpc, call, &port, (zdrproc_t)zdr_uint32_t, sizeof(uint32_t));
    return 0;
}

/*
 * v2 DUMP.
 * This RPC returns a list of all endpoints that are registered with
 * portmapper.
 */
static int pmap2_dump_proc(struct rpc_context *rpc, struct rpc_msg *call)
{
    PMAP2DUMPres reply;
    struct mapping *tmp;

    reply.list = NULL;
    for (tmp = map; tmp; tmp = tmp->next)
    {
        struct pmap2_mapping_list *tmp_list;
        int proto;

        /* pmap2 only support ipv4 */
        if (!strcmp(tmp->netid, "tcp"))
            proto = IPPROTO_TCP;
        else if (!strcmp(tmp->netid, "udp"))
            proto = IPPROTO_UDP;
        else
            continue;

        tmp_list = malloc(sizeof(struct pmap2_mapping_list));
        tmp_list->map.prog  = tmp->prog;
        tmp_list->map.vers  = tmp->vers;
        tmp_list->map.prot  = proto;
        tmp_list->map.port  = tmp->port;

        tmp_list->next = reply.list;
        reply.list = tmp_list;
    }

    rpc_send_reply(rpc, call, &reply, (zdrproc_t)zdr_PMAP2DUMPres, sizeof(PMAP2DUMPres));

    while (reply.list)
    {
        struct pmap2_mapping_list *tmp_list = reply.list->next;
        free(reply.list);
        reply.list = tmp_list;
    }

    return 0;
}

/*
 * v2 SET
 * This procedure is used to register and endpoint with portmapper.
 */
static int pmap2_set_proc(struct rpc_context *rpc, struct rpc_msg *call)
{
    PMAP2GETPORTargs *args = call->body.cbody.args;
    char *prot;
    char *addr;
    uint32_t response = 1;

    if (args->prot == IPPROTO_TCP)
        prot = "tcp";
    else
        prot = "udp";

    /* Don't update if we already have a mapping */
    if (map_lookup(args->prog, args->vers, prot))
    {
        response = 0;
        rpc_send_reply(rpc, call, &response, (zdrproc_t)zdr_uint32_t, sizeof(uint32_t));
        return 0;
    }

    asprintf(&addr, "0.0.0.0.%d.%d", args->port >> 8, args->port & 0xff);
    pmap_register(args->prog, args->vers, strdup(prot), addr, strdup("<unknown>"));

    rpc_send_reply(rpc, call, &response, (zdrproc_t)zdr_uint32_t, sizeof(uint32_t));
    return 0;
}

/*
 * v2 UNSET
 * This procedure is used to remove a registration from portmappers
 * list of endpoints.
 */
static int pmap2_unset_proc(struct rpc_context *rpc, struct rpc_msg *call)
{
    PMAP2GETPORTargs *args = call->body.cbody.args;
    char *prot;
    char *addr;
    uint32_t response = 1;
    if (args->prot == IPPROTO_TCP)
        prot = "tcp";
    else
        prot = "udp";
    map_remove(args->prog, args->vers, prot);
    rpc_send_reply(rpc, call, &response, (zdrproc_t)zdr_uint32_t, sizeof(uint32_t));
    return 0;
}

/*
 * Service table for portmapper v2.
 *
 * Service management is table driven in libnfsand this is the table
 * that defines which procedures we implement for portmapper v2.
 * If clients try to connect to the not-yet-implemented procedures here
 * libnfs will automatically respond with an RPC layer error that flags
 * PROCEDURE UNAVAILABLE.
 *
 * This table contains the procedure number, the callback function to implement
 * this procedure, the unmarshalling function that libnfs should use to unppack
 * the client payload as well as its size.
 *
 * Version 2 does not support ipv6 so this version of portmapper is
 * not too commonly used any more.
 */
struct service_proc pmap2_pt[] = {
    {PMAP2_NULL, pmap2_null_proc, (zdrproc_t)zdr_void, 0},
    {PMAP2_SET, pmap2_set_proc, (zdrproc_t)zdr_PMAP2SETargs, sizeof(PMAP2SETargs)},
    {PMAP2_UNSET, pmap2_unset_proc, (zdrproc_t)zdr_PMAP2UNSETargs, sizeof(PMAP2UNSETargs)},
    {PMAP2_GETPORT, pmap2_getport_proc, (zdrproc_t)zdr_PMAP2GETPORTargs, sizeof(PMAP2GETPORTargs)},
    {PMAP2_DUMP, pmap2_dump_proc, (zdrproc_t)zdr_void, 0},
    //{PMAP2_CALLIT, pmap2_...},
};

/*
 * The NULL procedure. All protocols/versions must provide a NULL procedure
 * as index 0.
 * It is used by clients, and rpcinfo, to "ping" a service and verify that
 * the service is available and that it does support the indicated version.
 */
static int pmap3_null_proc(struct rpc_context *rpc, struct rpc_msg *call)
{
    rpc_send_reply(rpc, call, NULL, (zdrproc_t)zdr_void, 0);
    return 0;
}

/*
 * v3 DUMP.
 * This RPC returns a list of all endpoints that are registered with
 * portmapper.
 */
static int pmap3_dump_proc(struct rpc_context *rpc, struct rpc_msg *call)
{
    PMAP3DUMPres reply;
    struct mapping *tmp;
    reply.list = NULL;
    for (tmp = map; tmp; tmp = tmp->next)
    {
        struct pmap3_mapping_list *tmp_list;

        tmp_list = malloc(sizeof(struct pmap3_mapping_list));
        tmp_list->map.prog  = tmp->prog;
        tmp_list->map.vers  = tmp->vers;
        tmp_list->map.netid = tmp->netid;
        tmp_list->map.addr  = tmp->addr;
        tmp_list->map.owner = tmp->owner;

        tmp_list->next = reply.list;
        reply.list = tmp_list;
    }

    rpc_send_reply(rpc, call, &reply, (zdrproc_t)zdr_PMAP3DUMPres, sizeof(PMAP3DUMPres));

    while (reply.list)
    {
        struct pmap3_mapping_list *tmp_list = reply.list->next;
        free(reply.list);
        reply.list = tmp_list;
    }

    return 0;
}

/*
 * Service table for portmapper v3.
 *
 * Service management is table driven in libnfs and this is the table
 * that defines which procedures we implement for portmapper v3.
 * If clients try to connect to the not-yet-implemented procedures here
 * libnfs will automatically respond with an RPC layer error that flags
 * PROCEDURE UNAVAILABLE.
 *
 * This table contains the procedure number, the callback function to implement
 * this procedure, the unmarshalling function that libnfs should use to unppack
 * the client payload as well as its size.
 */
struct service_proc pmap3_pt[] = {
    {PMAP3_NULL, pmap3_null_proc, (zdrproc_t)zdr_void, 0},
    //{PMAP3_SET, pmap3_...},
    //{PMAP3_UNSET, pmap3_...},
    //{PMAP3_GETADDR, pmap3_...},
    {PMAP3_DUMP, pmap3_dump_proc, (zdrproc_t)zdr_void, 0},
    //{PMAP3_CALLIT, pmap3_...},
    //{PMAP3_GETTIME, pmap3_...},
    //{PMAP3_UADDR2TADDR, pmap3_...},
    //{PMAP3_TADDR2UADDR, pmap3_...},
};

// Handle incoming event
static void server_io(evutil_socket_t fd, short events, void *private_data)
{
    struct server *server = private_data;
    int revents = 0;
    if (events & EV_READ)
        revents |= POLLIN;
    if (events & EV_WRITE)
        revents |= POLLOUT;
    // Let libnfs process the event
    if (rpc_service(server->rpc, revents) < 0)
    {
        free_server(server);
        return;
    }
    // Update which events we are interested in
    update_events(server->rpc, server->read_event, server->write_event);
}

// Accept a connection
static void do_accept(evutil_socket_t s, short events, void *private_data)
{
    struct sockaddr_storage ss;
    socklen_t len = sizeof(ss);
    struct server *server;
    int fd;

    server = malloc(sizeof(struct server));
    if (server == NULL)
        return;
    memset(server, 0, sizeof(*server));

    if ((fd = accept(s, (struct sockaddr *)&ss, &len)) < 0)
    {
        free_server(server);
        return;
    }
    evutil_make_socket_nonblocking(fd);

    server->rpc = rpc_init_server_context(fd);
    if (server->rpc == NULL)
    {
        close(fd);
        free_server(server);
        return;
    }

    // portmap
    rpc_register_service(server->rpc, PMAP_PROGRAM, PMAP_V2, pmap2_pt, sizeof(pmap2_pt) / sizeof(pmap2_pt[0]));
    rpc_register_service(server->rpc, PMAP_PROGRAM, PMAP_V3, pmap3_pt, sizeof(pmap3_pt) / sizeof(pmap3_pt[0]));

    // NFS
    rpc_register_service(server->rpc, NFS_PROGRAM, NFS_V3, nfs3_pt, sizeof(nfs3_pt) / sizeof(nfs3_pt[0]));
    rpc_register_service(server->rpc, MOUNT_PROGRAM, MOUNT_V3, nfs3_mount_pt, sizeof(nfs3_mount_pt) / sizeof(nfs3_mount_pt[0]));

    // read and write events
    server->read_event = event_new(base, fd, EV_READ|EV_PERSIST, server_io, server);
    server->write_event = event_new(base, fd, EV_WRITE|EV_PERSIST, server_io, server);
    update_events(server->rpc, server->read_event, server->write_event);
}

int main(int argc, char *argv[])
{
    struct sockaddr_in in;
    int one = 1;

    base = event_base_new();
    if (base == NULL)
    {
        printf("Failed create event context\n");
        exit(10);
    }

    in.sin_family = AF_INET;
    in.sin_port = htons(111);
    in.sin_addr.s_addr = htonl(INADDR_ANY);

    pmap_register(PMAP_PROGRAM, PMAP_V2, strdup("tcp"), strdup("0.0.0.0.0.111"), strdup("portmapper-service"));
    pmap_register(PMAP_PROGRAM, PMAP_V3, strdup("tcp"), strdup("0.0.0.0.0.111"), strdup("portmapper-service"));
    pmap_register(NFS_PROGRAM, NFS_V3, strdup("tcp"), strdup("0.0.0.0.0.2049"), strdup("nfs-server"));
    pmap_register(MOUNT_PROGRAM, MOUNT_V3, strdup("tcp"), strdup("0.0.0.0.0.2049"), strdup("rpc.mountd"));

    // Portmap socket
    int listen_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (listen_socket == -1)
    {
        printf("Failed to create listening socket\n");
        exit(10);
    }
    evutil_make_socket_nonblocking(listen_socket);
    setsockopt(listen_socket, SOL_SOCKET, SO_REUSEADDR, &one, sizeof(one));
    if (bind(listen_socket, (struct sockaddr *)&in, sizeof(in)) < 0)
    {
        printf("Failed to bind listening socket\n");
        exit(10);
    }
    if (listen(listen_socket, 16) < 0)
    {
        printf("failed to listen to socket\n");
        exit(10);
    }
    struct event *listen_event = event_new(base, listen_socket, EV_READ|EV_PERSIST, do_accept, NULL);
    event_add(listen_event, NULL);

    // NFS socket
    int nfs_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (nfs_socket == -1)
    {
        printf("Failed to create listening socket\n");
        exit(10);
    }
    evutil_make_socket_nonblocking(nfs_socket);
    setsockopt(nfs_socket, SOL_SOCKET, SO_REUSEADDR, &one, sizeof(one));
    in.sin_family = AF_INET;
    in.sin_port = htons(2049);
    in.sin_addr.s_addr = htonl(INADDR_ANY);
    if (bind(nfs_socket, (struct sockaddr *)&in, sizeof(in)) < 0)
    {
        printf("Failed to bind listening socket\n");
        exit(10);
    }
    if (listen(nfs_socket, 16) < 0)
    {
        printf("failed to listen to socket\n");
        exit(10);
    }
    listen_event = event_new(base, nfs_socket, EV_READ|EV_PERSIST, do_accept, NULL);
    event_add(listen_event, NULL);

    // Start the event loop
    event_base_dispatch(base);

    return 0;
}
