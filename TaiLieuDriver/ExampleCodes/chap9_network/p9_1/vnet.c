#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/netdevice.h>

struct net_device *vnet;

int vnet_open(struct net_device *dev)
{
	printk("vnet_open called\n");
	return 0;
}

int vnet_release(struct net_device *dev)
{
	printk("vnet_release called\n");
	return 0;
}

int vnet_xmit(struct sk_buff *skb, struct net_device *dev)
{
	printk("vnet_xmit called...\n");
	return 0;
}
int vnet_init(struct net_device *dev)
{
	printk("vnet device initialized\n");
	return 0;
};

const struct net_device_ops vnetdev_ops = {
	.ndo_init = vnet_init,
	.ndo_open = vnet_open,
	.ndo_stop = vnet_release,
	.ndo_start_xmit = vnet_xmit,
};

static void virtual_setup(struct net_device *dev)
{
	dev->netdev_ops = &vnetdev_ops;
}

int vnet_init_module(void)
{
	int result;
	vnet = alloc_netdev(0, "vnet%d", NET_NAME_USER, virtual_setup);
	if((result = register_netdev(vnet)))
	{
		printk("vnet: Error %d initalizing card ...", result);
		return result;
	}
	return 0;
}

void vnet_exit_module (void)
{
	unregister_netdev (vnet);
	printk ("vnet driver exited\n");
}

module_init(vnet_init_module);
module_exit(vnet_exit_module);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("CNTT - KMA");
MODULE_DESCRIPTION("Example about simple driver for a virtual net device");

