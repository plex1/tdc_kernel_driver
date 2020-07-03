/*
 * File:  mod_hrtimer.c
 * Autor: Matthias Meier
 * Aim:   simple latency test of high res timers
 *
 * Based on an article "Kernel APIs, Part 3: Timers and lists in the 2.6 kernel"
 * by M.Tim Jones:
 * 	http://www.ibm.com/developerworks/linux/library/l-timers-list/
 *
 * Remarks:
 * - The timer subsystem is documented here:
 * http://www.kernel.org/doc/htmldocs/device-drivers/
 * - For a simple test without additional cpu-load enter:
 *   insmod mod_hrtimer.ko; sleep 3; dmesg | tail -25 ; rmmod mod_hrtimer
 * - For additional CPU load use 'hackbench' or (less heavy) by parallel kernel
 * compile (eg. make -j 20)
 */

#include <linux/delay.h>
#include <linux/hrtimer.h>
#include <linux/kthread.h>
#include <linux/module.h>

MODULE_LICENSE("GPL");
MODULE_AUTHOR("M. Tim Jones (IBM)");
MODULE_AUTHOR("Matthias Meier <matthias.meier@fhnw.ch>");

#define INTERVAL_BETWEEN_CALLBACKS (1 * 1000000LL) // 100ms  (scaled in ns)
#define NR_ITERATIONS 10000

static struct hrtimer hr_timer;
static ktime_t ktime_interval;
static s64 starttime_ns;
static s64 log_times[NR_ITERATIONS] = {0};

static enum hrtimer_restart my_hrtimer_callback(struct hrtimer *timer)
{
  static int n = 0, i = 0;
  // static long min = 1000000000, max = 0, sum = 0;
  // long latency;

  s64 now_ns = ktime_to_ns(ktime_get());
  // hrtimer_forward(&hr_timer, hr_timer._softexpires,
  //                ktime_interval); // next call relative to expired timestamp$

  hrtimer_forward_now(timer, ktime_interval);
  log_times[n++] = now_ns;
  

  // calculate some statistics values...

  /*
  latency = now_ns - starttime_ns - n * INTERVAL_BETWEEN_CALLBACKS;
  sum += latency;
  if (min > latency)
    min = latency;
  if (max < latency)
    max = latency;
  */

  /*
    printk("mod_hrtimer: my_hrtimer_callback called after %ldns.\n",
         (long)(now_ns - starttime_ns));
  */

  if (n < NR_ITERATIONS)
    return HRTIMER_RESTART;
  else
  {
    for (i = 0; i < NR_ITERATIONS; i++)
    {
      printk("%lld\n", log_times[i]);
    }

    /*
    printk(
        "mod_hrtimer: my_hrtimer_callback: statistics latences over %d hrtimer "
        "callbacks: "
        "min=%ldns, max=%ldns, mean=%ldns\n",
        n, min, max, sum / n);
    */
    return HRTIMER_NORESTART;
  }
}

static int init_module_hrtimer(void)
{
  printk("mod_hrtimer: installing module...\n");

  // define a ktime variable with the interval time defined on top of this file
  ktime_interval = ktime_set(0, INTERVAL_BETWEEN_CALLBACKS);

  // init a high resolution timer named 'hr_timer'
  hrtimer_init(&hr_timer, CLOCK_MONOTONIC, HRTIMER_MODE_REL);

  // set the callback function for this hr_timer
  hr_timer.function = &my_hrtimer_callback;

  // get the current time as high resolution timestamp, convert it to ns
  starttime_ns = ktime_to_ns(ktime_get());

  // activate the high resolution timer including callback function...
  hrtimer_start(&hr_timer, ktime_interval, HRTIMER_MODE_REL);

  printk(
      "mod_hrtimer: started timer callback function to fire every %lldns "
      "(current jiffies=%ld, HZ=%d)\n",
      INTERVAL_BETWEEN_CALLBACKS, jiffies, HZ);
  return 0;
}

static void cleanup_module_hrtimer(void)
{
  int ret;
  ret = hrtimer_cancel(&hr_timer);
  if (ret)
    printk("mod_hrtimer: The timer was still in use...\n");
  printk("mod_hrtimer: HR Timer module uninstalling\n");
}

module_init(init_module_hrtimer);
module_exit(cleanup_module_hrtimer);
