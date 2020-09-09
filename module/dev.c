#include "dev.h"

struct mystruct{
    unsigned char delay;
    unsigned char repeat;
    unsigned char position;
    unsigned char value;
};

struct text_po{
    int first_start;
    int first_po;
    int second_start;
    int second_po;
    int first_upflag;
    int second_upflag;
};

unsigned char fpga_number[10][10] = {
	{0x3e,0x7f,0x63,0x73,0x73,0x6f,0x67,0x63,0x7f,0x3e}, // 0
	{0x0c,0x1c,0x1c,0x0c,0x0c,0x0c,0x0c,0x0c,0x0c,0x1e}, // 1
	{0x7e,0x7f,0x03,0x03,0x3f,0x7e,0x60,0x60,0x7f,0x7f}, // 2
	{0xfe,0x7f,0x03,0x03,0x7f,0x7f,0x03,0x03,0x7f,0x7e}, // 3
	{0x66,0x66,0x66,0x66,0x66,0x66,0x7f,0x7f,0x06,0x06}, // 4
	{0x7f,0x7f,0x60,0x60,0x7e,0x7f,0x03,0x03,0x7f,0x7e}, // 5
	{0x60,0x60,0x60,0x60,0x7e,0x7f,0x63,0x63,0x7f,0x3e}, // 6
	{0x7f,0x7f,0x63,0x63,0x03,0x03,0x03,0x03,0x03,0x03}, // 7
	{0x3e,0x7f,0x63,0x63,0x7f,0x7f,0x63,0x63,0x7f,0x3e}, // 8
	{0x3e,0x7f,0x63,0x63,0x7f,0x3f,0x03,0x03,0x03,0x03} // 9
};

unsigned char fpga_set_blank[10] = {
	// memset(array,0x00,sizeof(array));
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00
};

unsigned char *my_number = "120190187      ";
unsigned char *my_name = "Yungwoo Ko     ";
//unsigned char *text_input="120190187        Yungwoo Ko     ";
unsigned char *text_blank = "                                ";

static int dev_usage = 0; 
//static unsigned char *iom_driver_addr; // new dev's addr
static unsigned char *iom_led_addr;
static unsigned char *iom_fnd_addr;
static unsigned char *iom_dot_addr;
static unsigned char *iom_text_addr;

static int __init dev_init(void); // module init
static void __exit dev_exit(void); // module exit

static int dev_open(struct inode *, struct file *);
static int dev_release(struct inode *, struct file *);
static ssize_t dev_write(struct file *, const char *, size_t, loff_t *);
static long dev_ioctl(struct file *, unsigned int, unsigned long);
static void loop(unsigned long); // write 안에서 다시 call?

static void fnd_init(void);
static void led_init(void);
static void dot_init(void);
static void text_init(void);

struct timer_list timer;
struct mystruct input; // 여기서 쓸 것!!!
struct text_po text_real;

//static unsigned char cur_fnd; 
//static unsigned char cur_position;

static struct file_operations dev_fops = { // 새로 만들 driver의 fops
    .owner = THIS_MODULE,
    .open = dev_open,
    .write = dev_write,
    .unlocked_ioctl = dev_ioctl,
    .release = dev_release,
    // read는 필요 없음
};
static void fnd_init(void){

    unsigned short int value_short = 0;
   // unsigned char zero = 0;
   // value_short = zero << 12 | zero << 8 |zero << 4 |zero;
    outw(value_short,(unsigned int)iom_fnd_addr);
     printk("Initializing fnd device...\n");
}

static void led_init(void){

    unsigned short _s_value;
    _s_value = (unsigned short)0;
    outw(_s_value, (unsigned int)iom_led_addr);

    printk("Initializing led device...\n");

}

static void dot_init(void){

    int i;
    unsigned short int _s_value = 0;

    for(i=0;i<10;i++){
        
        _s_value = fpga_set_blank[i] & 0x7F;
		outw(_s_value,(unsigned int)iom_dot_addr+i*2);
    }
    printk("Initializing dot device...\n");

}

static void text_init(void){
    int i;
    unsigned short int _s_value = 0;
    
    for(i=0;i<32;i++){
        _s_value = (text_blank[i] & 0xFF) << 8 | (text_blank[i + 1] & 0xFF);
		outw(_s_value,(unsigned int)iom_text_addr+i);
        i++;
    }
    printk("Initializing text device...\n");
}

static int dev_open(struct inode *minode, struct file *mfile){

    printk("driver open!\n");
    if(dev_usage != 0){
        return -EBUSY;
    }
    dev_usage = 1;

    
    return 0;
}


static int dev_release(struct inode *minode, struct file *mfile){

    dev_usage = 0;
    return 0;
}

static void loop(unsigned long repeated){

    int i;
    // for fnd write.. // 
    unsigned char fnd_value[4];
    unsigned short int value_short;

    // for led write.. //
    unsigned short led_2;

    // for dot write.. //
    unsigned short dot;

    // for text write.. //
    unsigned short txt;
    unsigned char imsi[33];
    struct mystruct *tmp;
    tmp = (struct mystruct *)repeated;

    // 새로 뭔가 써주기 전에 지울건 지우기
    printk("Initializng devices in loop function!!\n");

    fnd_init();
    led_init();
    dot_init();
    text_init();
    
     printk("Initializng devices in loop function completed!!\n");
    // 디바이스별 동작하는것만 작성하면 된다
    
    if(tmp->repeat >0){
        printk("current fnd value: %d\n",(int)input.value);
        printk("current position %d\n",(int)input.position);
        // 1. fnd device
        printk("writing to fnd device..\n");
        for(i=0;i<4;i++){
            fnd_value[i] = 0;

            if((int)input.position==i){
                fnd_value[i] = input.value-0x30;
                input.value++;
            }

        }

        if(input.value==57){ // when it reaches max value
                input.value = 49;
                input.position++;
        }

        if((int)input.position==4){ // when it reaches max value
                input.position = 0;
        }

        value_short = fnd_value[0] << 12 | fnd_value[1] << 8 |fnd_value[2] << 4 |fnd_value[3];
        outw(value_short,(unsigned int)iom_fnd_addr);

        printk("writing to fnd device.. finished!\n");
        if(input.value == 49) input.value = 57;
        // 2. led device
         printk("writing to led device..\n");
        switch(input.value){
            case 0x32:
                led_2 = 128;
                break;
            case 0x33:
                led_2 = 64;
                break;
            case 0x34:
                led_2 = 32;
                break;
            case 0x35:
                led_2 = 16;
                break;
            case 0x36:
                led_2 = 8;
                break;
            case 0x37:
                led_2 = 4;
                break;
            case 0x38:
                led_2 = 2;
                break;
            case 0x39:
                led_2 = 1;
                break;
            default:
                break;
        }
         
        outw(led_2, (unsigned int)iom_led_addr);

        printk("writing to led device.. finished!\n");

        // 3. dot device
        printk("writing to dot device..\n");
            for(i=0;i<10;i++){
                dot = fpga_number[input.value-0x31][i] & 0x7F;
		        outw(dot,(unsigned int)iom_dot_addr+i*2);
            }
        printk("writing to dot device.. finished!\n");
        if(input.value == 57) input.value = 49;



        // 4. text device

            printk("writing to text device..\n");

            if(text_real.first_upflag == 1){
                text_real.first_start++;
                text_real.first_po++;

                if(text_real.first_po==16){
                    text_real.first_upflag = 0;
                    text_real.first_start--;
                    text_real.first_po--;
                } 
            }

            else{
                text_real.first_start--;
                text_real.first_po--;
                if(text_real.first_start==0) text_real.first_upflag = 1;
            }

            if(text_real.second_upflag == 1){
                text_real.second_start++;
                text_real.second_po++;

                if(text_real.second_po==16){
                    text_real.second_upflag = 0;
                    text_real.second_start--;
                    text_real.second_po--;
                } 
            }

            else{
                text_real.second_start--;
                text_real.second_po--;
                if(text_real.second_start==0) text_real.second_upflag = 1;
            }

            for(i=0;i<16;i++){

                if(text_real.first_upflag == 1){
                    if(i<text_real.first_start){
                        imsi[i] = ' ';
                    }
                    else{
                        imsi[i] = my_number[i - text_real.first_start]; 
                    }
                }

                else{
                    if(i>text_real.first_po){
                        imsi[i] = ' ';
                    }

                    else{
                        if(i<text_real.first_start) imsi[i] = ' ';
                        
                        else{
                            imsi[i] = my_number[i - text_real.first_start];
                        }
                    }

                }
            }

            for(i=0;i<16;i++){
                
                if(text_real.second_upflag == 1){
                    if(i<text_real.second_start){
                        imsi[i+16] = ' ';
                    }
                    else{
                        imsi[i+16] = my_name[i - text_real.second_start];
                    }
                }

                else{
                    if(i>text_real.second_po){
                        imsi[i+16] = ' ';
                    }

                    else{
                        if(i<text_real.second_start) imsi[i+16] = ' ';

                        else{
                        imsi[i+16] = my_name[i - text_real.second_start];
                        }
                    }
                }

            }

            for(i=0;i<32;i++){
                txt = (imsi[i] & 0xFF) << 8 | (imsi[i + 1] & 0xFF);
		        outw(txt,(unsigned int)iom_text_addr+i);
                i++;
            }
            printk("writing to text device.. finished!\n");

    }
    
    // device write end.. // 
    printk("device writing all done\n");

    timer.expires = get_jiffies_64() + ( (int)input.delay * (HZ/10) );
    timer.data = (unsigned long)&input;
    timer.function = loop;

    if(tmp->repeat-- == 0){
        printk("counter -1\n");
        fnd_init();
        led_init();
        dot_init();
        text_init();
        return; 
    } // 반복이 끝나면 다 초기화 시키고 그만 add하기

    add_timer(&timer);
}

static ssize_t dev_write(struct file *mfile, const char *gdata, size_t len, loff_t *offp){

    int remaining;

    printk("copy_from_user_entering...\n");
    
    remaining = copy_from_user(&input, (struct mystruct *)gdata, sizeof(struct mystruct));
    printk("copy from user complete!\n");
    printk("delay: %d, repeat: %d, position: %d, value: %c", input.delay, input.repeat, input.position, input.value);

    //cur_fnd = (int)input.value;
    //cur_position = (int)input.position;

    printk("initial cur_fnd is... %d\n",(int)input.value);
    printk("initial cur_position is... %d\n",(int)input.position);

    text_real.first_start = -1;
    text_real.first_po = 7;
    text_real.second_start = -1;
    text_real.second_po = 8;
    text_real.first_upflag = 1;
    text_real.second_upflag = 1;
    printk("copy from user data initialized!\n");

    del_timer_sync(&timer);
    printk("Timer deleted\n");

    timer.expires = get_jiffies_64() + ( (int)input.delay * (HZ/10) ); // delay: 1~100..
    timer.data = (unsigned long)&input;
    timer.function = loop;


    add_timer(&timer);
    printk("Timer added\n");

    return 1;
}



static long dev_ioctl(struct file *mfile, unsigned int cmd, unsigned long arg){

    switch(cmd){

        case DEVIOC_WRITE: // arg가 write의 gdata로 들어가야 한다!
            dev_write(NULL, (const char *)arg, 0, NULL);
            break;

        default:
            break;
            
    }

    return 1;    

}


static int __init dev_init(void){  // init module

    int result;

    result = register_chrdev(DEV_MAJOR, DEV_NAME, &dev_fops);

    if(result<0){
        printk(KERN_WARNING"Can't get any major\n");
		return result;
    }

    printk("init module, %s major number : %d\n", DEV_NAME, DEV_MAJOR);
    //iom_driver_addr = ioremap(0x08000000, 0x10);
    iom_led_addr = ioremap(LED_ADDRESS, 0x1);
    iom_fnd_addr = ioremap(FND_ADDRESS, 0x4);
    iom_dot_addr = ioremap(DOT_ADDRESS, 0x10);
    iom_text_addr = ioremap(TEXT_ADDRESS, 0x32); // memory mapping to i/o

    printk("checking i/o address..\n");
    printk("%p %p %p %p\n", iom_led_addr, iom_fnd_addr, iom_dot_addr, iom_text_addr);

    init_timer(&timer);
    return 0;
}


static void __exit dev_exit(void){  // exit module

    dev_usage = 0;
    del_timer_sync(&timer);
    
    //iounmap(iom_driver_addr);
    iounmap(iom_led_addr);
    iounmap(iom_fnd_addr);
    iounmap(iom_dot_addr);
    iounmap(iom_text_addr);

    unregister_chrdev(DEV_MAJOR, DEV_NAME);

}


module_init(dev_init);
module_exit(dev_exit);


MODULE_LICENSE("GPL");
MODULE_AUTHOR("You");
