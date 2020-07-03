/*
 * File:  mod_readaddr.c
 * Autor: Felix Arnold
 * Aim:   Interface to TDC module
 */

#include <linux/module.h> /* Needed by all modules */
#include <linux/io.h>
#include <linux/litex.h>
#include <linux/uaccess.h>

#include <linux/fs.h>
#include <linux/cdev.h>

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Felix Arnold <felix.arnold@gmail.com>");
MODULE_DESCRIPTION("Simple linux kernel module");

#define MY_MODULE_NAME "mod_tdc"

static int my_open(struct inode *inode, struct file *filp);
static int my_release(struct inode *inode, struct file *filp);
static ssize_t my_read(struct file *filp, char *buf, size_t count, loff_t *f_pos);
static ssize_t my_write(struct file *filp, const char *buf, size_t count, loff_t *f_pos);

static struct file_operations my_fops = {
  .owner = THIS_MODULE,
  .read = my_read,
  .write = my_write,
  .open = my_open,
  .release = my_release,
};

int major;

static char mydata[100];

#define TDC_MEMORY_ADDRESS 0xf0005000

typedef struct {
  volatile unsigned int test;
  volatile unsigned int dummy01;
  volatile unsigned int dummy02;
  volatile unsigned int dummy03;
  volatile unsigned int counter;
  volatile unsigned int dummy11;
  volatile unsigned int dummy12;
  volatile unsigned int dummy13; 
  volatile unsigned int control;
  
} mask_regs_t;

mask_regs_t *regs;

static int __init module_start(void)
{
  printk(MY_MODULE_NAME ": Loading tdc module V1...\n");
  printk(MY_MODULE_NAME ": Hello universe!\n");

  // get pointer to memory address of tdc
  regs = (mask_regs_t *) ioremap_nocache(TDC_MEMORY_ADDRESS, 8);

  // some test output
  printk(MY_MODULE_NAME ": read from test reg 0 ->         %d\n", regs->test);
  printk(MY_MODULE_NAME ": read from test reg 1 ->         %d\n", regs->dummy01);
  printk(MY_MODULE_NAME ": read from test reg as uint32 -> %d\n", litex_get_reg((void *) &(regs->test), 4));

  major = register_chrdev(0, MY_MODULE_NAME, &my_fops);
  if (major < 0) {
	printk(MY_MODULE_NAME " error, cannot register the character device\n");
	return major;
  }
  
  printk(MY_MODULE_NAME ": driver module is ready to be used.");
  
  return 0;
}

static void __exit module_end(void) {
  unregister_chrdev(major, "mod_hrtimer_dev");
  printk(MY_MODULE_NAME ": see you around.\n");
}


static int my_open(struct inode *inode, struct file *filp) {
	return 0;	// SUCCESS zurueckmelden
}

static int my_release(struct inode *inode, struct file *filp) {
	return 0;	// SUCCESS zurückmelden
}

static ssize_t my_read(struct file *filp, char *buf, size_t count, loff_t *f_pos) {
        // construct text to sent to user
	int len=0;
	len += sprintf(mydata+len, "Measured time: ");
	int number_of_cycles = litex_get_reg((void *) &(regs->counter), 4);
	len += sprintf(mydata+len, "%d clock cyles, ", number_of_cycles  );
	len += sprintf(mydata+len, "this corresponds to %d ms\n", number_of_cycles/75000);
	__copy_to_user (buf, mydata, strnlen(mydata,sizeof(mydata))); 	// Daten aus Kernel- in Userspace kopieren	
	return len;	// Zurueckmelden wieviele Bytes effektiv geliefert werden
}

static ssize_t my_write(struct file *filp, const char *buf, size_t count, loff_t *f_pos) {
	if (sizeof(mydata)<count) 	// mydata[] Platz fuer gelieferte Daten?
		count = sizeof(mydata); 	// wenn nicht entsprehchend begrenzen!
	__copy_from_user(mydata, buf, count);	// Daten aus User- in Kernel-space kopieren

	mydata[count-1]=0; //create null terminated string and remove \n character at the end

	// parse commands from user
	if (strcmp(mydata, "start")==0) {
	   printk(MY_MODULE_NAME ": Start pulse received\n");
	   litex_set_reg((void *) &(regs->control), 4, 1); // bit 0 is the start pulse
	   litex_set_reg((void *) &(regs->control), 4, 0);	     
	}
	else if (strcmp(mydata, "stop")==0) {
	   printk(MY_MODULE_NAME ": Stop pulse received\n");
	   litex_set_reg((void *) &(regs->control), 4, 2); // bit 1 is the stop pulse
	   litex_set_reg((void *) &(regs->control), 4, 0);	     
	}
	else {
	  printk(MY_MODULE_NAME ": Unknown command\n");
	}
	mydata[0] = 0; //clear string
	
	return count;	// Zurückmelden wieviele Bytes effektiv konsumiert wurden
}


module_init(module_start);
module_exit(module_end);
