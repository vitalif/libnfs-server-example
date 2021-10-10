#include <stdlib.h>
#include <string.h>
#include "nfs-service.h"

static void fill_example_fsattr(struct fattr3 *attr)
{
    struct timespec now;
    clock_gettime(CLOCK_REALTIME, &now);
    attr->type = NF3DIR;
    attr->mode = 0755;
    attr->nlink = 1;
    attr->uid = 0;
    attr->gid = 0;
    attr->size = 4096;
    attr->used = 4096;
    attr->rdev = (specdata3){ 0, 0 };
    attr->fsid = 1;
    attr->fileid = 1;
    attr->atime.seconds = now.tv_sec;
    attr->atime.nseconds = now.tv_nsec;
    attr->mtime = attr->atime;
    attr->ctime = attr->atime;
}

static int nfs3_null_proc(struct rpc_context *rpc, struct rpc_msg *call)
{
    rpc_send_reply(rpc, call, NULL, (zdrproc_t)zdr_void, 0);
    return 0;
}

static int nfs3_getattr_proc(struct rpc_context *rpc, struct rpc_msg *call)
{
    GETATTR3args *args = call->body.cbody.args;
    GETATTR3res reply;
    if (args->object.data.data_len != 10)
    {
        // Example error
        reply.status = NFS3ERR_PERM;
    }
    else
    {
        // Fill info
        reply.status = NFS3_OK;
        fill_example_fsattr(&reply.GETATTR3res_u.resok.obj_attributes);
    }
    rpc_send_reply(rpc, call, &reply, (zdrproc_t)zdr_GETATTR3res, sizeof(GETATTR3res));
    return 0;
}

static int nfs3_setattr_proc(struct rpc_context *rpc, struct rpc_msg *call)
{
    SETATTR3args *args = call->body.cbody.args;
    SETATTR3res reply;
    
    rpc_send_reply(rpc, call, &reply, (zdrproc_t)zdr_SETATTR3res, sizeof(SETATTR3res));
    return 0;
}

static int nfs3_lookup_proc(struct rpc_context *rpc, struct rpc_msg *call)
{
    LOOKUP3args *args = call->body.cbody.args;
    LOOKUP3res reply;
    
    rpc_send_reply(rpc, call, &reply, (zdrproc_t)zdr_LOOKUP3res, sizeof(LOOKUP3res));
    return 0;
}

static int nfs3_access_proc(struct rpc_context *rpc, struct rpc_msg *call)
{
    ACCESS3args *args = call->body.cbody.args;
    ACCESS3res reply;
    
    rpc_send_reply(rpc, call, &reply, (zdrproc_t)zdr_ACCESS3res, sizeof(ACCESS3res));
    return 0;
}

static int nfs3_readlink_proc(struct rpc_context *rpc, struct rpc_msg *call)
{
    READLINK3args *args = call->body.cbody.args;
    READLINK3res reply;
    
    rpc_send_reply(rpc, call, &reply, (zdrproc_t)zdr_READLINK3res, sizeof(READLINK3res));
    return 0;
}

static int nfs3_read_proc(struct rpc_context *rpc, struct rpc_msg *call)
{
    READ3args *args = call->body.cbody.args;
    READ3res reply;
    
    rpc_send_reply(rpc, call, &reply, (zdrproc_t)zdr_READ3res, sizeof(READ3res));
    return 0;
}

static int nfs3_write_proc(struct rpc_context *rpc, struct rpc_msg *call)
{
    WRITE3args *args = call->body.cbody.args;
    WRITE3res reply;
    
    rpc_send_reply(rpc, call, &reply, (zdrproc_t)zdr_WRITE3res, sizeof(WRITE3res));
    return 0;
}

static int nfs3_create_proc(struct rpc_context *rpc, struct rpc_msg *call)
{
    CREATE3args *args = call->body.cbody.args;
    CREATE3res reply;
    
    rpc_send_reply(rpc, call, &reply, (zdrproc_t)zdr_CREATE3res, sizeof(CREATE3res));
    return 0;
}

static int nfs3_mkdir_proc(struct rpc_context *rpc, struct rpc_msg *call)
{
    MKDIR3args *args = call->body.cbody.args;
    MKDIR3res reply;
    
    rpc_send_reply(rpc, call, &reply, (zdrproc_t)zdr_MKDIR3res, sizeof(MKDIR3res));
    return 0;
}

static int nfs3_symlink_proc(struct rpc_context *rpc, struct rpc_msg *call)
{
    SYMLINK3args *args = call->body.cbody.args;
    SYMLINK3res reply;
    
    rpc_send_reply(rpc, call, &reply, (zdrproc_t)zdr_SYMLINK3res, sizeof(SYMLINK3res));
    return 0;
}

static int nfs3_mknod_proc(struct rpc_context *rpc, struct rpc_msg *call)
{
    MKNOD3args *args = call->body.cbody.args;
    MKNOD3res reply;
    
    rpc_send_reply(rpc, call, &reply, (zdrproc_t)zdr_MKNOD3res, sizeof(MKNOD3res));
    return 0;
}

static int nfs3_remove_proc(struct rpc_context *rpc, struct rpc_msg *call)
{
    REMOVE3args *args = call->body.cbody.args;
    REMOVE3res reply;
    
    rpc_send_reply(rpc, call, &reply, (zdrproc_t)zdr_REMOVE3res, sizeof(REMOVE3res));
    return 0;
}

static int nfs3_rmdir_proc(struct rpc_context *rpc, struct rpc_msg *call)
{
    RMDIR3args *args = call->body.cbody.args;
    RMDIR3res reply;
    
    rpc_send_reply(rpc, call, &reply, (zdrproc_t)zdr_RMDIR3res, sizeof(RMDIR3res));
    return 0;
}

static int nfs3_rename_proc(struct rpc_context *rpc, struct rpc_msg *call)
{
    RENAME3args *args = call->body.cbody.args;
    RENAME3res reply;
    
    rpc_send_reply(rpc, call, &reply, (zdrproc_t)zdr_RENAME3res, sizeof(RENAME3res));
    return 0;
}

static int nfs3_link_proc(struct rpc_context *rpc, struct rpc_msg *call)
{
    LINK3args *args = call->body.cbody.args;
    LINK3res reply;
    
    rpc_send_reply(rpc, call, &reply, (zdrproc_t)zdr_LINK3res, sizeof(LINK3res));
    return 0;
}

static int nfs3_readdir_proc(struct rpc_context *rpc, struct rpc_msg *call)
{
    READDIR3args *args = call->body.cbody.args;
    READDIR3res reply;
    
    rpc_send_reply(rpc, call, &reply, (zdrproc_t)zdr_READDIR3res, sizeof(READDIR3res));
    return 0;
}

static int nfs3_readdirplus_proc(struct rpc_context *rpc, struct rpc_msg *call)
{
    READDIRPLUS3args *args = call->body.cbody.args;
    READDIRPLUS3res reply;
    
    rpc_send_reply(rpc, call, &reply, (zdrproc_t)zdr_READDIRPLUS3res, sizeof(READDIRPLUS3res));
    return 0;
}

static int nfs3_fsstat_proc(struct rpc_context *rpc, struct rpc_msg *call)
{
    FSSTAT3args *args = call->body.cbody.args;
    FSSTAT3res reply;
    
    rpc_send_reply(rpc, call, &reply, (zdrproc_t)zdr_FSSTAT3res, sizeof(FSSTAT3res));
    return 0;
}

static int nfs3_fsinfo_proc(struct rpc_context *rpc, struct rpc_msg *call)
{
    FSINFO3args *args = call->body.cbody.args;
    FSINFO3res reply;
    if (args->fsroot.data.data_len != 10)
    {
        // Example error
        reply.status = NFS3ERR_INVAL;
    }
    else
    {
        // Fill info
        reply.status = NFS3_OK;
        reply.FSINFO3res_u.resok.obj_attributes.attributes_follow = TRUE;
        fill_example_fsattr(&reply.FSINFO3res_u.resok.obj_attributes.post_op_attr_u.attributes);
        reply.FSINFO3res_u.resok.rtmax = 128*1024*1024;
        reply.FSINFO3res_u.resok.rtpref = 128*1024*1024;
        reply.FSINFO3res_u.resok.rtmult = 4096;
        reply.FSINFO3res_u.resok.wtmax = 128*1024*1024;
        reply.FSINFO3res_u.resok.wtpref = 128*1024*1024;
        reply.FSINFO3res_u.resok.wtmult = 4096;
        reply.FSINFO3res_u.resok.dtpref = 128;
        reply.FSINFO3res_u.resok.maxfilesize = 0x7fffffffffffffff;
        reply.FSINFO3res_u.resok.time_delta.seconds = 1;
        reply.FSINFO3res_u.resok.time_delta.nseconds = 0;
        reply.FSINFO3res_u.resok.properties = FSF3_SYMLINK | FSF3_HOMOGENEOUS;
    }
    rpc_send_reply(rpc, call, &reply, (zdrproc_t)zdr_FSINFO3res, sizeof(FSINFO3res));
    return 0;
}

static int nfs3_pathconf_proc(struct rpc_context *rpc, struct rpc_msg *call)
{
    PATHCONF3args *args = call->body.cbody.args;
    PATHCONF3res reply;
    if (args->object.data.data_len != 10)
    {
        // Example error
        reply.status = NFS3ERR_INVAL;
    }
    else
    {
        // Fill info
        reply.status = NFS3_OK;
        reply.PATHCONF3res_u.resok.obj_attributes.attributes_follow = FALSE;
        reply.PATHCONF3res_u.resok.linkmax = 0;
        reply.PATHCONF3res_u.resok.name_max = 255;
        reply.PATHCONF3res_u.resok.no_trunc = TRUE;
        reply.PATHCONF3res_u.resok.chown_restricted = FALSE;
        reply.PATHCONF3res_u.resok.case_insensitive = FALSE;
        reply.PATHCONF3res_u.resok.case_preserving = TRUE;
    }
    rpc_send_reply(rpc, call, &reply, (zdrproc_t)zdr_PATHCONF3res, sizeof(PATHCONF3res));
    return 0;
}

static int nfs3_commit_proc(struct rpc_context *rpc, struct rpc_msg *call)
{
    COMMIT3args *args = call->body.cbody.args;
    COMMIT3res reply;
    
    rpc_send_reply(rpc, call, &reply, (zdrproc_t)zdr_COMMIT3res, sizeof(COMMIT3res));
    return 0;
}

static int mount3_mnt_proc(struct rpc_context *rpc, struct rpc_msg *call)
{
    dirpath *arg = call->body.cbody.args;
    int flavor = AUTH_NONE;
    mountres3 reply;
    reply.fhs_status = MNT3_OK;
    reply.mountres3_u.mountinfo.fhandle.fhandle3_len = 10;
    reply.mountres3_u.mountinfo.fhandle.fhandle3_val = "roothandle";
    reply.mountres3_u.mountinfo.auth_flavors.auth_flavors_len = 1;
    reply.mountres3_u.mountinfo.auth_flavors.auth_flavors_val = &flavor;
    rpc_send_reply(rpc, call, &reply, (zdrproc_t)zdr_mountres3, sizeof(mountres3));
    return 0;
}

static int mount3_dump_proc(struct rpc_context *rpc, struct rpc_msg *call)
{
    mountlist reply;
    reply = (struct mountbody*)malloc(sizeof(struct mountbody));
    reply->ml_hostname = "10.0.2.15";
    reply->ml_directory = "/test";
    reply->ml_next = NULL;
    rpc_send_reply(rpc, call, NULL, (zdrproc_t)zdr_mountlist, sizeof(mountlist));
    free(reply);
    return 0;
}

static int mount3_umnt_proc(struct rpc_context *rpc, struct rpc_msg *call)
{
    dirpath *arg = call->body.cbody.args;
    // do nothing
    rpc_send_reply(rpc, call, NULL, (zdrproc_t)zdr_void, 0);
    return 0;
}

static int mount3_umntall_proc(struct rpc_context *rpc, struct rpc_msg *call)
{
    // do nothing
    rpc_send_reply(rpc, call, NULL, (zdrproc_t)zdr_void, 0);
    return 0;
}

static int mount3_export_proc(struct rpc_context *rpc, struct rpc_msg *call)
{
    exports reply;
    reply = (struct exportnode*)malloc(sizeof(struct exportnode) + sizeof(struct groupnode));
    reply->ex_dir = "/test";
    reply->ex_groups = (struct groupnode*)(reply+1);
    reply->ex_groups->gr_name = "10.0.2.15";
    reply->ex_groups->gr_next = NULL;
    reply->ex_next = NULL;
    rpc_send_reply(rpc, call, &reply, (zdrproc_t)zdr_exports, sizeof(exports));
    free(reply);
    return 0;
}

struct service_proc nfs3_pt[22] = {
    {NFS3_NULL,        nfs3_null_proc,        (zdrproc_t)zdr_void,             0},
    {NFS3_GETATTR,     nfs3_getattr_proc,     (zdrproc_t)zdr_GETATTR3args,     sizeof(GETATTR3args)},
    {NFS3_SETATTR,     nfs3_setattr_proc,     (zdrproc_t)zdr_SETATTR3args,     sizeof(SETATTR3args)},
    {NFS3_LOOKUP,      nfs3_lookup_proc,      (zdrproc_t)zdr_LOOKUP3args,      sizeof(LOOKUP3args)},
    {NFS3_ACCESS,      nfs3_access_proc,      (zdrproc_t)zdr_ACCESS3args,      sizeof(ACCESS3args)},
    {NFS3_READLINK,    nfs3_readlink_proc,    (zdrproc_t)zdr_READLINK3args,    sizeof(READLINK3args)},
    {NFS3_READ,        nfs3_read_proc,        (zdrproc_t)zdr_READ3args,        sizeof(READ3args)},
    {NFS3_WRITE,       nfs3_write_proc,       (zdrproc_t)zdr_WRITE3args,       sizeof(WRITE3args)},
    {NFS3_CREATE,      nfs3_create_proc,      (zdrproc_t)zdr_CREATE3args,      sizeof(CREATE3args)},
    {NFS3_MKDIR,       nfs3_mkdir_proc,       (zdrproc_t)zdr_MKDIR3args,       sizeof(MKDIR3args)},
    {NFS3_SYMLINK,     nfs3_symlink_proc,     (zdrproc_t)zdr_SYMLINK3args,     sizeof(SYMLINK3args)},
    {NFS3_MKNOD,       nfs3_mknod_proc,       (zdrproc_t)zdr_MKNOD3args,       sizeof(MKNOD3args)},
    {NFS3_REMOVE,      nfs3_remove_proc,      (zdrproc_t)zdr_REMOVE3args,      sizeof(REMOVE3args)},
    {NFS3_RMDIR,       nfs3_rmdir_proc,       (zdrproc_t)zdr_RMDIR3args,       sizeof(RMDIR3args)},
    {NFS3_RENAME,      nfs3_rename_proc,      (zdrproc_t)zdr_RENAME3args,      sizeof(RENAME3args)},
    {NFS3_LINK,        nfs3_link_proc,        (zdrproc_t)zdr_LINK3args,        sizeof(LINK3args)},
    {NFS3_READDIR,     nfs3_readdir_proc,     (zdrproc_t)zdr_READDIR3args,     sizeof(READDIR3args)},
    {NFS3_READDIRPLUS, nfs3_readdirplus_proc, (zdrproc_t)zdr_READDIRPLUS3args, sizeof(READDIRPLUS3args)},
    {NFS3_FSSTAT,      nfs3_fsstat_proc,      (zdrproc_t)zdr_FSSTAT3args,      sizeof(FSSTAT3args)},
    {NFS3_FSINFO,      nfs3_fsinfo_proc,      (zdrproc_t)zdr_FSINFO3args,      sizeof(FSINFO3args)},
    {NFS3_PATHCONF,    nfs3_pathconf_proc,    (zdrproc_t)zdr_PATHCONF3args,    sizeof(PATHCONF3args)},
    {NFS3_COMMIT,      nfs3_commit_proc,      (zdrproc_t)zdr_COMMIT3args,      sizeof(COMMIT3args)},
};

struct service_proc nfs3_mount_pt[6] = {
    {MOUNT3_NULL,    nfs3_null_proc,      (zdrproc_t)zdr_void,    0},
    {MOUNT3_MNT,     mount3_mnt_proc,     (zdrproc_t)zdr_dirpath, sizeof(dirpath)},
    {MOUNT3_DUMP,    mount3_dump_proc,    (zdrproc_t)zdr_void,    0},
    {MOUNT3_UMNT,    mount3_umnt_proc,    (zdrproc_t)zdr_dirpath, sizeof(dirpath)},
    {MOUNT3_UMNTALL, mount3_umntall_proc, (zdrproc_t)zdr_void,    0},
    {MOUNT3_EXPORT,  mount3_export_proc,  (zdrproc_t)zdr_void,    0},
};
