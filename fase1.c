#include <linux/init.h>
#include <linux/module.h>
#include <linux/fs.h>
#include <linux/uaccess.h>
#include <linux/kernel.h>
#include <linux/gpio.h> 
#include <linux/interrupt.h>   
MODULE_LICENSE("GPL");
static unsigned int LedAPin = 20;
static unsigned int LedBPin = 16;
static unsigned int ButtonAPin = 26;
static unsigned int ButtonBPin = 19;
static unsigned int ButtonCPin = 13;
static unsigned int ButtonDPin = 21;
static unsigned int irqNumber;          
static unsigned int numberPresses[4];
static unsigned char irqButtons[4];



int prueva() {
    int i;
    int number = 0;
    if(number == 0){
        char *argv[] = {"/usr/bin/sh","/home/pi/Documents/prueva.sh", NULL};  
        char *envp[] = {"HOME=/","TERM=linux", "PATH=/sbin:/bin:/usr/sbin:/usr/bin", NULL};  
        i=call_usermodehelper(argv[0], argv,envp, UMH_WAIT_PROC); 
    }
    
    if(number == 1){
        char *argv[] = {"/usr/bin/sh","/home/pi/Documents/prueva2.sh", NULL};  
        char *envp[] = {"HOME=/","TERM=linux", "PATH=/sbin:/bin:/usr/sbin:/usr/bin", NULL};  
        i=call_usermodehelper(argv[0], argv,envp, UMH_WAIT_PROC); 
    }
    
    if(number == 2){
        char *argv[] = {"/usr/bin/sh","/home/pi/Documents/prueva3.sh", NULL};  
        char *envp[] = {"HOME=/","TERM=linux", "PATH=/sbin:/bin:/usr/sbin:/usr/bin", NULL};  
        i=call_usermodehelper(argv[0], argv,envp, UMH_WAIT_PROC); 
    }
    
    if(number == 3){
        char *argv[] = {"/usr/bin/sh","/home/pi/Documents/prueva4.sh", NULL};  
        char *envp[] = {"HOME=/","TERM=linux", "PATH=/sbin:/bin:/usr/sbin:/usr/bin", NULL};  
        i=call_usermodehelper(argv[0], argv,envp, UMH_WAIT_PROC); 
    }
    
    return 0; 
}

static irq_handler_t ebbgpio_irq_handler(unsigned int irq, void *dev_id, struct pt_regs *regs){
   if(irq == irqButtons[0]){                         
       gpio_set_value(LedAPin, 1);          
       printk(KERN_INFO "GPIO_TEST: Interrupt! (button state is %d)\n", gpio_get_value(ButtonAPin));
       printk(KERN_NOTICE "TONTO QUE ERES TONTO A");
       numberPresses[0]++;  
       prueva(0);             
   }
    
   if(irq == irqButtons[1]){
       gpio_set_value(LedAPin, 0);          
       printk(KERN_INFO "GPIO_TEST: Interrupt! (button state is %d)\n", gpio_get_value(ButtonBPin));
       numberPresses[1]++;
       printk(KERN_NOTICE "TONTO QUE ERES TONTO B");
       prueva(1);  
   }
   
   if(irq == irqButtons[2]){
       gpio_set_value(LedBPin, 1);          
       printk(KERN_INFO "GPIO_TEST: Interrupt! (button state is %d)\n", gpio_get_value(ButtonCPin));
       numberPresses[2]++;
       printk(KERN_NOTICE "TONTO QUE ERES TONTO C");
       prueva(2);   
   }
   
   if(irq == irqButtons[3]){
       gpio_set_value(LedBPin, 0);          
       printk(KERN_INFO "GPIO_TEST: Interrupt! (button state is %d)\n", gpio_get_value(ButtonDPin));
       numberPresses[3]++;
       printk(KERN_NOTICE "TONTO QUE ERES TONTO D");
       prueva(3); 
   }
   return (irq_handler_t) IRQ_HANDLED;     
}

int register_button(int ButtonPin, int numCreates) {
    int result = 0;
    gpio_request(ButtonPin, "sysfs");        
    gpio_direction_input(ButtonPin);        
    gpio_set_debounce(ButtonPin, 200);     
    gpio_export(ButtonPin, false);
    irqNumber = gpio_to_irq(ButtonPin);
    irqButtons[numCreates] = irqNumber;
   printk(KERN_INFO "GPIO_TEST: The button is mapped to IRQ: %d\n", irqNumber);
   
   
   result = request_irq(irqNumber,            
                        (irq_handler_t) ebbgpio_irq_handler, 
                        IRQF_TRIGGER_RISING, 
                        "ebb_gpio_handler",    
                        NULL);   
    return result;       
}

void unregister_button(int ButtonPin, int numCreates) {
    printk(KERN_NOTICE "Deleting Button in Pin: %d\n",ButtonPin);
    free_irq(irqButtons[numCreates], NULL);
    gpio_unexport(ButtonPin);
    gpio_free(ButtonPin);
}

int register_led(int LedPin) {

    printk(KERN_NOTICE "Inicialization LED in Pin: %d\n",LedPin);
    
    if (!gpio_is_valid(LedPin)) {
        printk(KERN_NOTICE "Pin Led Incorrect");
        return 0;
    }

    gpio_request(LedPin, "sysfs");
    gpio_direction_output(LedPin, 0);
    gpio_export(LedPin, false);

    return 0;
}

void unregister_led(int LedPin) {
    printk(KERN_NOTICE "Deleting LED in Pin: %d\n",LedPin);
    gpio_set_value(LedPin, 0);
    gpio_unexport(LedPin);
    gpio_free(LedPin);
}

static int my_init(void) { 
    printk(KERN_NOTICE "Init LKM\n");
    register_led(LedAPin);
    register_led(LedBPin);
    register_button(ButtonAPin,0);
    register_button(ButtonBPin,1);
    register_button(ButtonCPin,2);
    register_button(ButtonDPin,3);
    return 0;
}

static void my_exit(void) {
    printk(KERN_NOTICE "Exit LKM\n");
    unregister_led(LedAPin);
    unregister_led(LedBPin);
    unregister_button(ButtonAPin,0);
    unregister_button(ButtonBPin,1);
    unregister_button(ButtonCPin,2);
    unregister_button(ButtonDPin,3);
    printk(KERN_INFO "GPIO_TEST: The button was pressed %d times\n", numberPresses[0]);
    printk(KERN_INFO "GPIO_TEST: The button was pressed %d times\n", numberPresses[1]);
    printk(KERN_INFO "GPIO_TEST: The button was pressed %d times\n", numberPresses[2]);
    printk(KERN_INFO "GPIO_TEST: The button was pressed %d times\n", numberPresses[3]);
    return;
}

module_init(my_init);
module_exit(my_exit);
