#include <linux/init.h>
#include <linux/module.h>
#include <linux/types.h>
#include <net/sock.h>
#include <linux/netlink.h>

#define NETLINK_TEST	30
#define MSG_LEN		125
#define USER_PORT	100

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Linggang Zeng");
MODULE_DESCRIPTION("netlink module");

struct sock *nlsk = NULL;
extern struct net init_net;

int send_usrmsg(char *pbuf, uint16_t len)
{
	struct sk_buff *nl_skb;
	struct nlmsghdr *nlh;

	int ret;

	/* 创建sk_buff 空间 */
	nl_skb = nlmsg_new(len, GFP_ATOMIC);
	if (!nl_skb) {
		printk("netlink alloc failure\n");
		return -1;
	}

	/* 设置netlink消息头部 */
	nlh = nlmsg_put(nl_skb, 0, 0, NETLINK_TEST, len, 0);
	if (nlh == NULL) {
		printk("nlmsg_put failaure \n");
		nlmsg_free(nl_skb);
		return -1;
	}

	/* 拷贝数据发送 */
	memcpy(nlmsg_data(nlh), pbuf, len);
	ret = netlink_unicast(nlsk, nl_skb, USER_PORT, MSG_DONTWAIT);

	return ret;
}

static void netlink_rcv_msg(struct sk_buff *skb)
{
	struct nlmsghdr *nlh = NULL;
	char *umsg = NULL;
	char *kmsg = "Test netlink from kernel to user.";

	if (skb->len >= nlmsg_total_size(0)) {
		nlh = nlmsg_hdr(skb);
		umsg = NLMSG_DATA(nlh);
		if (umsg) {
			printk("kernel recv from user: %s\n", umsg);
			send_usrmsg(kmsg, strlen(kmsg));
		}
	}
}

struct netlink_kernel_cfg cfg = {
	.input = netlink_rcv_msg, /* set recv callback */
};

int netlink_module_init(void)
{
	/* create netlink socket */
	nlsk = (struct sock *)netlink_kernel_create(&init_net, NETLINK_TEST, &cfg);
	if (nlsk == NULL) {
		printk("netlink_kernel_create error!\n");
		return -1;
	}
	printk("netlink_module_init\n");

	return 0;
}

void netlink_module_exit(void)
{
	if (nlsk) {
		netlink_kernel_release(nlsk); /* release ..*/
		nlsk = NULL;
	}
	printk("netlink_module_exit\n");
}

module_init(netlink_module_init);
module_exit(netlink_module_exit);
