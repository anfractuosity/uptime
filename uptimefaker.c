#undef __KERNEL__
#define __KERNEL__

#undef MODULE
#define MODULE


#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/fs.h>
#include <linux/init.h>
#include <linux/proc_fs.h>
#include <linux/sched.h>
#include <linux/seq_file.h>
#include <linux/time.h>
#include <linux/kernel_stat.h>
#include <asm/cputime.h>
#include <linux/delay.h>
#include <linux/sched.h>
#include <linux/kthread.h>

static struct task_struct *thread1;
int thread_fn();


static int __init uptimefaker_init(void) {
	unsigned long flags;
	printk(KERN_INFO "UptimeFaker\n");

	printk("Jiffies before %lx\n", jiffies_64);
	char our_thread[8] = "thread1";
	printk(KERN_INFO "in init");
	thread1 = kthread_create(thread_fn, NULL, our_thread);
	if ((thread1)) {
		printk(KERN_INFO "in if");
		wake_up_process(thread1);
	}

	printk("Jiffies after %lx\n", jiffies_64);
	return 0;	
}


// Found from ......
static void disable_page_protection(void) {
	unsigned long value;
	asm volatile ("mov %%cr0,%0":"=r" (value));
	if (value & 0x00010000) {
		value &= ~0x00010000;
		asm volatile ("mov %0,%%cr0"::"r" (value));
	}
}

static int patchee(struct seq_file *m, void *v) {
	printk("PATCHEEEEEEEEEEEEEEE\n");
	seq_printf(m, "18738072.28 74817307.16\n");
	return 0;
}

void patchme(void *addr) {

	long val = &patchee;
	int i = 0;

	unsigned char ops[] =
	    { 0x48, 0xC7, 0xC0, 0x00, 0x1C, 0x1B, 0x81, 0xFF, 0xE0, 0x90, 0x90,
  0x90, 0x90 };

	for (i = 0; i < 4; i++) {
		ops[i + 3] = (unsigned char)((char *)(&val))[i];
		printk("Addr: %x\n", ops[i + 3]);
	}
	printk("FULL ADDR: %lx\n", patchee);
	unsigned char *c = (unsigned char *)addr;

	for (i = 0; i < 13; i++) {
		c[i] = ops[i];
	}

}

int thread_fn() {
	printk(KERN_INFO "In thread1\n-------------\n");
	disable_page_protection();
	patchme(0xffffffff811b1c00);
	printk(KERN_INFO "\n------------\n");

	msleep(2);
	schedule();
	return 0;
}

static void __exit uptimefaker_cleanup(void) {
	printk(KERN_INFO "Cleaning up module.\n");
}

MODULE_LICENSE("GPL");
module_init(uptimefaker_init);
module_exit(uptimefaker_cleanup);
