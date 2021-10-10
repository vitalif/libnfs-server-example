const rpc = [
    'GETATTR',
    'SETATTR',
    'LOOKUP',
    'ACCESS',
    'READLINK',
    'READ',
    'WRITE',
    'CREATE',
    'MKDIR',
    'SYMLINK',
    'MKNOD',
    'REMOVE',
    'RMDIR',
    'RENAME',
    'LINK',
    'READDIR',
    'READDIRPLUS',
    'FSSTAT',
    'FSINFO',
    'PATHCONF',
    'COMMIT',
];

const len = rpc.reduce((a, c) => a < c.length ? c.length : a, 0);
let t = '';
let s = '';
for (const f of rpc)
{
    let pad = '';
    for (let i = f.length; i < len; i++)
        pad += ' ';
    t += `    {NFS3_${f}, ${pad}nfs3_${f.toLowerCase()}_proc, ${pad}(zdrproc_t)zdr_${f}3args, ${pad}sizeof(${f}3args)},\n`;
    s += `static int nfs3_${f.toLowerCase()}_proc(struct rpc_context *rpc, struct rpc_msg *call)
{
    ${f}3args *args = call->body.cbody.args;
    ${f}3res reply;
    
    rpc_send_reply(rpc, call, &reply, (zdrproc_t)zdr_${f}3res, sizeof(${f}3res));
    return 0;
}

`;
}

t = `struct service_proc nfs3_pt[] = {\n${t}};\n`;
console.log(t);
console.log(s);
