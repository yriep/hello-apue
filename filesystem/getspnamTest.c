#include <stdio.h>
#include <shadow.h>
#include <errno.h>
#include <stdlib.h>
#include <string.h>

int main() {
    const char *username = "yriep";

    errno = 0;
    struct spwd *sp = getspnam(username);

    printf("返回值: %p\n", (void*)sp);
    printf("errno: %d\n", errno);
    perror("getspnam 结果");

    if (sp == NULL) {
        if (errno == 0) {
            printf("➡️ 用户 '%s' 不存在\n", username);
        } else if (errno == EACCES) {
            printf("➡️ 权限不足！请用 sudo 运行\n");
        } else {
            printf("➡️ 系统错误: %s\n", strerror(errno));
        }
        return 1;
    }

    printf("✅ 用户 '%s' 的密码哈希: %s\n", username, sp->sp_pwdp);
    return 0;
}
