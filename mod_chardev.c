/*
 * File:  mod_readaddr.c
 * Autor: Felix Arnold
 * Aim:   read data from address
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

#define MY_MODULE_NAME "mod_chardev"

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

typedef struct {
  volatile unsigned int reg0;
  volatile unsigned int reg1;
} mask_regs_t;

mask_regs_t *regs;

static int __init module_start(void)
{
  printk(MY_MODULE_NAME ": Loading hello module...\n");
  printk(MY_MODULE_NAME ": Hello universe!\n");
  regs = (mask_regs_t *) ioremap_nocache(0xf0005000, 8);
  printk(MY_MODULE_NAME ": read data from reg0 is %d\n", regs->reg0);
  printk(MY_MODULE_NAME ": read data from reg1 is %d\n", regs->reg1);
  printk(MY_MODULE_NAME ": read data from reg0_l is %d\n", litex_get_reg((void *) &(regs->reg0), 4));

  major = register_chrdev(0, MY_MODULE_NAME, &my_fops);
  if (major < 0) {
	printk(MY_MODULE_NAME " error, cannot register the character device\n");
	return major;
  }
  
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
	if (strnlen(mydata,sizeof(mydata))<count) 	// mehr Daten angefordert als in mydata[] vorhanden?
		count = strnlen(mydata,sizeof(mydata)); 	// wenn ja, count entsprechend dezimieren
	__copy_to_user (buf, mydata, count); 	// Daten aus Kernel- in Userspace kopieren
	mydata[0]=0;	// und lokale Daten "loeschen" womit naechstes read() EOF
	return count;	// Zurueckmelden wieviele Bytes effektiv geliefert werden
}

static ssize_t my_write(struct file *filp, const char *buf, size_t count, loff_t *f_pos) {
	if (sizeof(mydata)<count) 	// mydata[] Platz fuer gelieferte Daten?
		count = sizeof(mydata); 	// wenn nicht entsprehchend begrenzen!
	__copy_from_user(mydata, buf, count);	// Daten aus User- in Kernel-space kopieren
	return count;	// Zurückmelden wieviele Bytes effektiv konsumiert wurden
}


module_init(module_start);
module_exit(module_end);
