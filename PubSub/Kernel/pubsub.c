#include <linux/module.h>
#include <net/sock.h> 
#include <linux/netlink.h>
#include <linux/skbuff.h> 
#define NETLINK_USER 31

#include <linux/string.h>
#include <linux/list.h>
#include <linux/slab.h>


struct sock *nl_sk = NULL; //Netlink socket, communicates betwen kernel and userspace
int pid;

//Implementing a PID linked list
struct linkedList {
	int pid;
	struct list_head list;
};

/*
struct linkedList publishers = {
	0, LIST_HEAD_INIT(publishers.link)
};

struct linkedList subscribers = {
	0, LIST_HEAD_INIT(subscribers.link)
};
*/

//Start a linked list called subscrbers
LIST_HEAD(subscribers);




int pidSub[2] = {0, 0};
int pidPub = 0;
int i = 0;


void removeChar1(char *str) {
    memmove(str, str+1, strlen(str));
}

//The following function reads the parameter called "skb", which is the socket buffer
static void hello_nl_recv_msg(struct sk_buff *skb) {

    struct nlmsghdr *nlh; //Netlink Message Header
	struct sk_buff *skb_out; //Pointer to the socket buffer
	int msg_size;
	char *msg = "Hello from kernel";//This is the message we send back to the user
	int res;
	int found = 1;

	printk(KERN_INFO "Entering: %s\n", __FUNCTION__);

		//I removed this so we can adjust the string that will go back to the user
    	//msg_size = strlen(msg);

    	nlh = (struct nlmsghdr *)skb->data; //This is the data we recieve from the publisher
    	printk(KERN_INFO "Netlink received msg payload:%s\n", (char *)nlmsg_data(nlh));
		//printk(KERN_INFO "Payload size is: %d\n", strlen( (char *)nlmsg_data(nlh)));
	//msg =nlmsg_data(nlh);

	pid = nlh->nlmsg_pid; /*pid of publishing process */
		printk(KERN_INFO "Netlink received from PID:%d\n", pid);


	//User Added Code
	//When we have recieved a message, check if the first character is a s
		//If it is an s
			//Traverse the subscribers linkedlist looking for the same PID. 
			//If we have found a process of the same PID, the message was probably a real message. Pass through to p.
			//If we have not found the same PID, we have a new subscriber. 
				//Append the PID of the new subscriber to a new node at the end of the subscribers linked list
		//If it is any other character, it should be published to all subscribers (Not including the one who sent it)
			//Starting from the head of the subscribers chain
			//If it does not match the PID of our publisher PID, then send the message to that PID
			//Repeat for all items in the linked list

			//Respond to the original PID with a success message


	//Copy the message over to a character array called recievedMsg
	char recievedMsg[strlen( (char *)nlmsg_data(nlh) )];
	strcpy((char *)recievedMsg, (char *)nlmsg_data(nlh));
	printk(KERN_INFO "value: %s\n", recievedMsg);
	struct linkedList *lstPtr; 
	struct linkedList *tmp;

	//If the first letter is an s
	if(recievedMsg[0] == 's') {
		printk("s operation: %d", pid);
		
		found = 0;

		//Traverse the list trying to see if we found the PID. If we have found the PID,
		//Then set the flag found = 1
		list_for_each_entry(lstPtr, &subscribers, list) {
			printk("List currently - PID: %d", lstPtr->pid);
			if(lstPtr -> pid == pid) {
				printk("Found matching PID: %d", lstPtr -> pid);
				found = 1;
			} else {
				printk("Nonmatching PID: %d", lstPtr -> pid);
			}
		}
	} else if (recievedMsg[0] == 'd') {
		//Traverse the list to output
		list_for_each_entry(lstPtr, &subscribers, list) {
			printk("List currently - PID: %d", lstPtr->pid);
		}
		printk("-----------------------");
	}

	//If we have not found anything, simply add the PID to the tail of the Linked list.
	//If we have found it, then broadcast the message
	if (found == 0){
		tmp = kmalloc(sizeof(struct linkedList), GFP_KERNEL);
		tmp->pid = pid;
		list_add_tail(&tmp->list, &subscribers);
		printk(KERN_INFO "Added pid: %d to the linked list", pid);
	} else {

		//Broadcast the message

		//Default back to found = 0
		found = 0;

		//Copy the message over to the msg buffer
		msg = recievedMsg;
		msg_size = strlen(msg);

		//Iterate through each entry in the linked list
		list_for_each_entry(lstPtr, &subscribers, list) {

			skb_out = nlmsg_new(msg_size, 0);
		if (!skb_out) {
			printk(KERN_ERR "Failed to allocate new skb\n");
				return;
		}
		nlh = nlmsg_put(skb_out, 0, 0, NLMSG_DONE, msg_size, 0);
		NETLINK_CB(skb_out).dst_group = 0; // not in mcast group 
		strncpy(nlmsg_data(nlh), msg, msg_size);

			//Announce to dmesg that we're going to broadcast
			printk("Broadcasting to PID: %d", lstPtr->pid);

			if (nlmsg_unicast(nl_sk, skb_out, lstPtr->pid) < 0) {
				printk(KERN_INFO "Error while sending message\n");
			} else {
				printk(KERN_INFO "Msg sent");
			}
		}
	}


	// msg_size = strlen(msg);

	// skb_out = nlmsg_new(msg_size, 0);
	// if (!skb_out) {
	// 	printk(KERN_ERR "Failed to allocate new skb\n");
	// 		return;
	// }
	// nlh = nlmsg_put(skb_out, 0, 0, NLMSG_DONE, msg_size, 0);
	// NETLINK_CB(skb_out).dst_group = 0; /* not in mcast group */
	// strncpy(nlmsg_data(nlh), msg, msg_size);

	// //nl_sk is the struct sock * returned by netlink_kernel_create
	// //skb_out is a buffer that contains the message 
	// //pid is the pid of the process to which the message should be sent to
	// if (nlmsg_unicast(nl_sk, skb_out, pid) < 0) {
	// 	printk(KERN_INFO "Error while sending bak to user\n");
	// } else {
	// 	printk(KERN_INFO "Msg sent");
	// }
	
}

static int __init hello_init(void)
{

    printk("Entering: %s\n", __FUNCTION__);
    struct netlink_kernel_cfg cfg = {
        .input = hello_nl_recv_msg,
    };

    nl_sk = netlink_kernel_create(&init_net, NETLINK_USER, &cfg);
    if (!nl_sk) {
        printk(KERN_ALERT "Error creating socket.\n");
        return -10;
    }

	//Lets add a node with an pid of 0 just to start off this linked list
	struct linkedList *tmp, *next;
	struct linkedList *ptr = NULL;


	tmp = kmalloc(sizeof(struct linkedList), GFP_KERNEL);
	tmp->pid = 0;
	list_add_tail(&tmp->list, &subscribers);

	//Adding another node to test
	tmp = kmalloc(sizeof(struct linkedList), GFP_KERNEL);
	tmp->pid = 1;
	list_add_tail(&tmp->list, &subscribers);

	//Adding another node to test
	tmp = kmalloc(sizeof(struct linkedList), GFP_KERNEL);
	tmp->pid = 2;
	list_add_tail(&tmp->list, &subscribers);

	list_for_each_entry(ptr, &subscribers, list) {
		printk("My List - PID: %d", ptr->pid);
	}

	list_for_each_entry_safe(tmp, next, &subscribers, list) {
		list_del(&tmp->list);
		kfree(tmp);
	}


    return 0;
}

static void __exit hello_exit(void)
{

    printk(KERN_INFO "exiting hello module\n");
    netlink_kernel_release(nl_sk);

	struct linkedList *tmp, *next;

	list_for_each_entry_safe(tmp, next, &subscribers, list) {
		list_del(&tmp->list);
		kfree(tmp);
	}
}

module_init(hello_init); module_exit(hello_exit);

MODULE_LICENSE("GPL");
