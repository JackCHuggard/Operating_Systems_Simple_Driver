#include<linux/init.h>
#include<linux/module.h>
#include<linux/fs.h>
#include<linux/uaccess.h>
#include<asm/uaccess.h>
#include<linux/slab.h>
MODULE_LICENSE("GPL");

#define BUFFERSIZE  1024
// device name is simple_char_driver



static int MajorNumber;
char *bufferPoint;

// open counter
int openCount = 0;
int closeCount = 0;

//used to open the file for the device
static int driveMe_open (struct inode *pinode, struct file *pfile)
{
// increase counter and print to info
openCount++;
printk(KERN_INFO "Device file opened %d times", openCount);

return 0;
}

static int driveMe_close (struct inode *pinode, struct file *pfile)
{
closeCount++;
//to allow access back to the file.
printk(KERN_INFO "Device file has been exited %d times", closeCount);
return 0;
}

loff_t driveMe_llseek (struct file *pfile, loff_t offset, int whence)
{
        // calc  new possition from 
        loff_t posLeft = bufferPoint + BUFFERSIZE;
        loff_t posCalc = bufferPoint;//always set to beginning of file if specifications are bad
        
// case 0 set current to possition off set
// case 1 increment  current possition by offset
// case 2 set current possition to offset before bufferend

printk(KERN_INFO"inside %s function\n", __FUNCTION__);


                switch(whence)
        {
                case 0:
       if (offset > BUFFERSIZE)
                {
                        printk(KERN_ALERT "The offset is larger then the Buffer_Size");
                        break;
                }
                posCalc = posCalc + offset;
                break;
                case 1:
                if ( pfile -> f_pos + offset > posLeft)
                {
                        printk(KERN_ALERT "The offset is larger then the Buffer_Size");
                        break;


                }
                else
                if (pfile -> f_pos + offset < bufferPoint)
                {
                        printk(KERN_ALERT "The offset is less then zero when added");
                        break;
                }
                else
                {
                //we are all good to set posCalc
                posCalc = posCalc + pfile->f_pos + offset;
                }


                break;
                case 2:
                if (offset > BUFFERSIZE)
                {
                        printk(KERN_ALERT "The  offset is larger then the Buffer_Size");
                        break;
                }
                posCalc = bufferPoint + BUFFERSIZE - offset;
                break;
        }



//changing value of f_pos

pfile -> f_pos = posCalc;
posCalc = posCalc - bufferPoint;
printk(KERN_ALERT "The new file possition is %d", posCalc);

return pfile -> f_pos;
}


ssize_t driveMe_read (struct file *pfile, char __user *buffer, size_t length, loff_t *offset)
{

int readbytes = 0;
printk(KERN_INFO"inside %s function\n", __FUNCTION__);    
// check to make sure our read is in the bounds
        if (length + *offset > BUFFERSIZE)
        {
		readbytes = BUFFERSIZE - *offset;
		readbytes = readbytes - copy_to_user(buffer, bufferPoint + *offset, sizeof(readbytes));
                //if it fails to write any it will keep track
		printk(KERN_ALERT "Read out of bounds, larger then buffer");
                printk(KERN_ALERT "bytes read %d", readbytes);
                *offset += readbytes;
		return readbytes;
        }
        readbytes = BUFFERSIZE - *offset;
        readbytes = readbytes - copy_to_user(buffer, bufferPoint + *offset, length);
	*offset += readbytes;
	printk(KERN_ALERT "bytes read %d", readbytes);
        return readbytes;
}

ssize_t driveMe_write (struct file *pfile, const char __user *buffer, size_t length, loff_t *offset)
{
int writebytes = 0;
printk(KERN_ALERT "inside %s func\n",__FUNCTION__);
	if (*offset + length > BUFFERSIZE)
	{
	writebytes = BUFFERSIZE - *offset;
	printk(KERN_ALERT "Write out of bounds, larger then buffer");
	writebytes = writebytes -  copy_from_user(bufferPoint + *offset, buffer, writebytes);
	printk(KERN_ALERT "Wrote %d bytes to device file \n", writebytes);
	*offset = *offset + writebytes;
	return writebytes;
	}
	writebytes = length;
	writebytes = writebytes - copy_from_user(bufferPoint + *offset, buffer, writebytes);
	printk(KERN_ALERT "Wrote %d bytes to device file \n", writebytes);
	
	*offset += writebytes;
	return writebytes;
}


struct file_operations driveMe_operations =
{
        .owner = THIS_MODULE,
        .open = driveMe_open,
        .release = driveMe_close,
        .llseek = driveMe_llseek,
        .read = driveMe_read,
        .write = driveMe_write,
};

static int driveMe_init(void)
{
printk(KERN_ALERT "Device Successfuly open\n");
   
//when the value to register_chrdev is 0 the computer allocates it to the next $
        //buffer allocation of memory
        bufferPoint = kmalloc(BUFFERSIZE, GFP_KERNEL);
        // memset will wipe any trash that is in memory
        memset(bufferPoint, 0 , BUFFERSIZE);
        MajorNumber = register_chrdev(0,"simple_char_driver",&driveMe_operations);

        if (MajorNumber <= 0)
                printk(KERN_ALERT "Failed to Initialize driver\n");

        return 0;
}


static void driveMe_exit(void)
{
        //free the memory of the driver
        kfree(bufferPoint);
        // global assignment for MajorNumber is done in the init
        unregister_chrdev(MajorNumber,"driveMe");
        printk(KERN_INFO "Device unregister success\n");
}


module_init(driveMe_init);
module_exit(driveMe_exit);
