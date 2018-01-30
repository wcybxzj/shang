#include "server.h"
#include "cluster.h"
#include "endianconv.h"

#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <sys/file.h>
#include <math.h>

/* -----------------------------------------------------------------------------
 * Key space handling
 * -------------------------------------------------------------------------- */
/* We have 16384 hash slots. The hash slot of a given key is obtained
 * as the least significant 14 bits of the crc16 of the key.
 *
 * However if the key contains the {...} pattern, only the part between
 * { and } is hashed. This may be useful in the future to force certain
 * keys to be in the same node (assuming no resharding is in progress). */
// 我们有16384个哈希槽，获得给定key的哈希槽作为密钥的crc16的最低有效14比特
// 计算给定key应该被分配到哪个槽，如果key包含 {...} ，那么只对{}中的字符串计算哈希值
unsigned int keyHashSlot(char *key, int keylen) {
	return 0;
}



/* -----------------------------------------------------------------------------
 * Cluster functions related to serving / redirecting clients
 * -------------------------------------------------------------------------- */

/* The ASKING command is required after a -ASK redirection.
 * The client should issue ASKING before to actually send the command to
 * the target instance. See the Redis Cluster specification for more
 * information. */
// 客户端接到-ASK命令后，需要发送ASKING命令
void askingCommand(client *c) {
    // 必须在集群模式下运行
    if (server.cluster_enabled == 0) {
        addReplyError(c,"This instance has cluster support disabled");
        return;
    }
    // 设置CLIENT_ASKING标识，表示转向到其他
    c->flags |= CLIENT_ASKING;
    addReply(c,shared.ok);
}


