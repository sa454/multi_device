
#include<linux/wait.h>
#include<linux/module.h>
#include<linux/init.h>
#include<linux/kdev_t.h>
#include<linux/fs.h>
#include<linux/cdev.h>
#include<linux/uaccess.h>
#include<linux/delay.h>
#include<linux/wait.h>
#include<linux/sched.h>
#include <linux/circ_buf.h>
#include <linux/slab.h>
#define NMINOR 3
#define SIZE 8

   
MODULE_LICENSE("GPL");
MODULE_AUTHOR("Satyam");
 
int char_open(struct inode *, struct file *);
int char_close(struct inode *,struct file*);
ssize_t char_read(struct file *filp,char __user* ubuff,size_t count,loff_t *off);
ssize_t char_write(struct file *filp,const char __user* ubuff,size_t count,loff_t *off);
int init_check; 
dev_t devno;                                          //device number

                                                      //wait queue structure beacuse it is type def not using struct 
struct cdev _cdev;                                    //declare the char device structure
struct mydev {
	      struct cdev _cdev;
              wait_queue_head_t waitforme; 
              struct circ_buf cbuf;    
             }dev[NMINOR];

struct file_operations fops={
                             .open = char_open,
                             .release = char_close,  
                             .read = char_read,
                             .write = char_write                  
                            };
      

int char_open(struct inode *inode_path, struct file *filp)
{ 
    //printk("cdev in inode of i_cdev %p\n",inode_path->i_cdev);
    //printk("cdev address is in drive %p\n",&_cdev);
    printk("you are in open function\n");
    struct mydev *mdev = container_of(inode_path->i_cdev,struct mydev,_cdev);
    filp->private_data = mdev;
    return 0;
}


   int char_close(struct inode *inode_path, struct file *filp)
{    
    printk("you are in close function\n");
    struct mydev *close = filp->private_data;
    return 0;
}



/*Read function *//////////////////////////////////////////////////////////////////////////////

ssize_t char_read(struct file *filp,char __user* ubuff,size_t count,loff_t *off)
{  
    int ret,j,m;
    printk("you are in read function\n");
    struct mydev *read = filp->private_data;
   
    wait_event_interruptible(read->waitforme,(CIRC_CNT(read->cbuf.head,read->cbuf.tail,SIZE)>=1));//two arguments one waitqueue & condition for left space
   
    m = min(CIRC_CNT(read->cbuf.head,read->cbuf.tail,SIZE),(int)count);
    printk("m:%d\n",&m);
    for(j=0;j<m;j++){
    ret =copy_to_user(ubuff+j,read->cbuf.buf+read->cbuf.tail,1); //copy from user statement maybe error prone return type number of byte remain to copy
    printk("ret:%d\n",ret);   
     if(ret)
       {
         printk("Not copied successfully to user");
           return -ENOMEM;
       }
    read->cbuf.tail = (read->cbuf.tail+1)&(SIZE-1);
      printk("kernel read side :%c\n",read->cbuf.buf[j]); 

     }//end of the for loop read function
    return m;     
  
}


  /*write function *////////////////////////////////////////////////////////////////////////

ssize_t char_write(struct file *filp,const char __user* ubuff,size_t count,loff_t *off)
{
    int ret, i,m;
    struct mydev *write = filp->private_data;
    printk("\nyou are in write function\n");
    m = min(CIRC_SPACE(write->cbuf.head,write->cbuf.tail,SIZE),(int)count); //check which buffer is small

    for(i=0;i<m;i++)
   {
       ret =copy_from_user(write->cbuf.buf + write->cbuf.head,ubuff+i,1);  //return value shows how many byte not copy from 
       if(ret)
       {
          printk("Not copied successfully from user");
          return -ENOMEM;
       }
      printk("kernel write side :%c\n",write->cbuf.buf[write->cbuf.head]); 
       write->cbuf.head = (write->cbuf.head+1)&(SIZE-1);
   }//end of the for loop
    wake_up_interruptible(&write->waitforme);    
   return m;
}//end of the write function

/*init function start*////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
 
static int char_init(void)
 {     
	printk("You are inside init Function");
        int ret,i;
        devno = MKDEV(42,0); 
        ret = register_chrdev_region(devno,NMINOR,"test_first");    //registering the device with the kernel
        if(ret)
        {                                                       //error checking
          printk("Error in register\n");
          return ret;
        }
       

      /*attaching file operation with the cdev structure*/
          
    for(i=0;i<NMINOR;i++) 
    {
          cdev_init(&dev[i]._cdev,&fops);                              //attaching the file operations with the cdev structures   
          init_waitqueue_head(&dev[i].waitforme);                       //wait queue for three different device
          dev[i].cbuf.buf = kmalloc(SIZE,GFP_KERNEL);                   //allocating memory for the three different buffer
          cdev_add(&dev[i]._cdev,MKDEV(42,0+i),1);                     //cdev structure associated with the device number in /dev file 
        
        if(!dev[i].cbuf.buf)
        {                                                            //start of the if 
         printk("Memory not avaiable");
    for(--i;i>=0;i--)
         kfree(dev[i].cbuf.buf);                              //on error free the allocated memory
         unregister_chrdev_region(devno,NMINOR);
         return -ENOMEM;
        } //end of the if condition 
     }//end of the for loop
 	printk("I am initialiized\n");
    return 0;
}//end of the init function



static void char_exit(void)
{
       int i;
     for(i=0;i<NMINOR;i++)  
       cdev_del(&dev[i]._cdev);
       unregister_chrdev_region(devno,NMINOR);
       printk("inside the exit function\n");
}
module_init(char_init);
module_exit(char_exit);




 	
