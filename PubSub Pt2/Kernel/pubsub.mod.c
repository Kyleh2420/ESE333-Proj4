#include <linux/build-salt.h>
#include <linux/module.h>
#include <linux/vermagic.h>
#include <linux/compiler.h>

BUILD_SALT;

MODULE_INFO(vermagic, VERMAGIC_STRING);
MODULE_INFO(name, KBUILD_MODNAME);

__visible struct module __this_module
__section(.gnu.linkonce.this_module) = {
	.name = KBUILD_MODNAME,
	.init = init_module,
#ifdef CONFIG_MODULE_UNLOAD
	.exit = cleanup_module,
#endif
	.arch = MODULE_ARCH_INIT,
};

#ifdef CONFIG_RETPOLINE
MODULE_INFO(retpoline, "Y");
#endif

static const struct modversion_info ____versions[]
__used __section(__versions) = {
	{ 0xdd8f8694, "module_layout" },
	{ 0x3bfcb827, "netlink_kernel_release" },
	{ 0xa1407a51, "__netlink_kernel_create" },
	{ 0x30cb0399, "init_net" },
	{ 0xdecd0b29, "__stack_chk_fail" },
	{ 0xac5bc229, "netlink_unicast" },
	{ 0x9166fada, "strncpy" },
	{ 0xaf0ee0b5, "__nlmsg_put" },
	{ 0x71a7b444, "__alloc_skb" },
	{ 0x69acdf38, "memcpy" },
	{ 0xc5850110, "printk" },
	{ 0xb0e602eb, "memmove" },
	{ 0x754d539c, "strlen" },
	{ 0xbdfb6dbb, "__fentry__" },
};

MODULE_INFO(depends, "");


MODULE_INFO(srcversion, "1EEA5CA805D678D1C31D6C3");
