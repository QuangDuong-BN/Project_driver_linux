#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/netdevice.h>
#include <linux/ip.h>
#include <linux/icmp.h>

struct net_device *vnet;

/* tinh checksum cho buffer gom size phan tu, moi phan tu dai 16 bit */
static uint16_t checksum(uint16_t *buffer, uint32_t size)
{
	unsigned long cksum=0;
	while(size >1)
	{
		cksum +=*buffer++;
		size -=sizeof(unsigned short);
	}

	if(size)
	{
		cksum += *(unsigned char*)buffer;
	}
	cksum = (cksum >> 16) + (cksum & 0xffff);
	cksum += (cksum >>16);
	return (uint16_t)(~cksum);
}

/* cap nhat lai ICMP header. Vi du:
 * - ICMP type
 * - ICMP checksum
 */
static void update_icmph(char *buffer, uint32_t size)
{
	struct icmphdr *icmph = (struct icmphdr*)buffer;
	icmph->type = ICMP_ECHOREPLY;
	icmph->checksum = 0;
	checksum((uint16_t*)buffer, size);
}

/* cap nhat lai IP header. Vi du:
 * - dia chi ip nguon va dich
 * - IP checksum
 */
static void update_iph(struct iphdr *iph)
{
	uint32_t tmp = iph->saddr;
	iph->saddr = iph->daddr;
	iph->daddr = tmp;
	//Ta khong can tinh lai checksum vi checksum offload
}

/* chuan bi ban tin tra loi */
static void prepare_icmp_response(char *data, unsigned int data_len)
{
	struct iphdr *iph = (struct iphdr *)data;
	size_t iph_len = sizeof(struct iphdr);

	update_iph(iph);

	update_icmph(data + iph_len, data_len - iph_len);
}

/* cap nhat tong so goi tin TX/RX, tong so byte TX/RX */
static void update_statistic(struct sk_buff *skb, struct net_device *dev)
{
	// Thua nhan rang gui va nhan deu thanh cong
	++(dev->stats.rx_packets);
	++(dev->stats.tx_packets);
	dev->stats.rx_bytes += skb->len;
	dev->stats.tx_bytes += skb->len;
}

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

netdev_tx_t vnet_xmit(struct sk_buff *skb, struct net_device *dev)
{
	printk("vnet_xmit called...\n");

	//danh dau thoi diem gui goi tin di mem_start
	//dev->trans_start = jiffies;
	dev->mem_start = jiffies;

	//chuan bi ban tin phan hoi
	prepare_icmp_response(skb->data, skb->len);

	//thong bao cho kernel rang co mot goi tin den
	netif_rx(skb);

	//cap nhat cac so lieu thong ke
	update_statistic(skb, dev);

	return NETDEV_TX_OK;
}

static void vnet_tx_timeout(struct net_device *netdev)
{
	printk("vnet_tx_timeout called\n");
}

static struct net_device_stats *vnet_get_stats(struct net_device *netdev)
{
	printk("vnet_get_stats called\n");
	return &netdev->stats;
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
	.ndo_tx_timeout = vnet_tx_timeout,
	.ndo_get_stats = vnet_get_stats,
};

static void virtual_setup(struct net_device *dev)
{
	dev->netdev_ops = &vnetdev_ops;
	dev->watchdog_timeo = HZ;
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
MODULE_DESCRIPTION("Minh hoa network driver");

