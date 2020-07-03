/*
 * File:  mod_readaddr.c
 * Autor: Felix Arnold
 * Aim:   read data from address
 */

#include <linux/module.h> /* Needed by all modules */
#include <linux/io.h>
#include <linux/litex.h>


MODULE_LICENSE("GPL");
MODULE_AUTHOR("Felix Arnold <felix.arnold@gmail.com>");
MODULE_DESCRIPTION("Simple linux kernel module");

#define MY_MODULE_NAME "mod_readaddr_litex"

typedef struct {
  volatile unsigned int reg0;
  volatile unsigned int reg1;
} mask_regs_t;

mask_regs_t *regs;

static int __init readaddr_start(void)
{
  printk(MY_MODULE_NAME ": Loading hello module...\n");
  printk(MY_MODULE_NAME ": Hello universe!\n");
  regs = (mask_regs_t *) ioremap_nocache(0xf0005000, 8);
  printk(MY_MODULE_NAME ": read data from reg0 is %d\n", regs->reg0);
  printk(MY_MODULE_NAME ": read data from reg1 is %d\n", regs->reg1);
  printk(MY_MODULE_NAME ": read data from reg0_l is %d\n", litex_get_reg((void *) &(regs->reg0), 4));
  return 0;
}

static void __exit readaddr_end(void) { printk(MY_MODULE_NAME ": see you around.\n"); }

module_init(readaddr_start);
module_exit(readaddr_end);
