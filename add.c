#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/init.h>
#include <linux/fs.h>
#include <linux/cdev.h>
#include <asm/uaccess.h>
MODULE_LICENSE("GPLv2");
#define MODULE_NAME "add"
#define DEV_COUNT 1
#define ja_silly "おばかさんですね〜"
#define en_silly "You are so silly"
static dev_t dev;
static struct cdev cdev;
static int total=0;
static int greeting=0;
static int error=0;
static int open_add(struct inode *inode,struct file *file);
static int release_add(struct inode *inode,struct file *file);
static ssize_t read_add(
		struct file *file,char __user *ubuf,size_t count,loff_t *offt);
static ssize_t write_add(
		struct file *file,const char __user *ubuff,size_t count,loff_t *offt);


static struct file_operations fo={
	.open=open_add,
	.release=release_add,
	.read=read_add,
	.write=write_add,
	.owner=THIS_MODULE,
};
static int open_add(struct inode *inode,struct file *file)
{
	printk(MODULE_NAME":opened minor=%d\n",iminor(inode));
	return 0;
}
static int release_add(struct inode *inode,struct file *file)
{
	printk(MODULE_NAME":closed minor=%d\n",iminor(inode));
	return 0;
}
static int __init init_add(void)
{
	int err;
	int firstminor=0;
	total=0;
	err=alloc_chrdev_region(&dev,firstminor,DEV_COUNT,MODULE_NAME);
	if(err<0){
		printk(MODULE_NAME":Failed reg\n");
		goto err1;
	}
	cdev_init(&cdev,&fo);
	cdev.owner=THIS_MODULE;
	err=cdev_add(&cdev,dev,DEV_COUNT);
	if(err<0){
		goto err2;
	}
	printk(MODULE_NAME":reg OK maj=%d\n",MAJOR(dev));
	return 0;
err2:
	cdev_del(&cdev);
	unregister_chrdev_region(dev,DEV_COUNT);
err1:
	return err;
}
static void __exit exit_add(void)
{
	int mj=MAJOR(dev);
	cdev_del(&cdev);
	unregister_chrdev_region(dev,DEV_COUNT);
	printk(MODULE_NAME":exit MAJ=%d\n",mj);
}
static ssize_t read_add(
		struct file *file,char __user *ubuf,size_t count,loff_t *offt)
{
	size_t len=0;
	char buff[256];
	const char *ptr="Good day!";
	const char *ptr2=en_silly;
	printk(MODULE_NAME":read count=%ld off=%lld\n",count,*offt);
	if(*offt!=0){
		return 0;
	}
	if(error){
		sprintf(buff,"%s\n",ptr2);
	}else if(greeting){
		sprintf(buff,"%s\n",ptr);
	}else{
		sprintf(buff,"%d\n",total);
	}
	error=greeting=0;
	len=strlen(buff);
	if(count<len){
		len=count;
	}
	if(raw_copy_to_user(ubuf,buff,len)){
		return -EFAULT;
	}
	*offt+=len;
	return (ssize_t)len;
}
static int checkStr(char *ptr)
{
	if(*ptr!='-' && *ptr!='+' && !(*ptr>='0' && *ptr<='9') ){
		printk(MODULE_NAME":NG1 %c\n",*ptr);
		return -1;
	}
	for(ptr++;*ptr;ptr++){
		if( !(*ptr>='0' && *ptr<='9') && *ptr!='\n'){
			printk(MODULE_NAME":NG2 %c\n",*ptr);
			return -1;
		}
	}
	return 0;
}
static ssize_t write_add(
		struct file *file,const char __user *ubuff,size_t count,loff_t *offt)
{
	char buff[256];
	int d;
	printk(MODULE_NAME":write count=%ld off=%lld\n",count,*offt);
	if(count >250){
		return -EFAULT;
	}
	if(raw_copy_from_user(buff,ubuff,count)){
		return -EFAULT;
	}
	buff[count]='\0';
	printk(MODULE_NAME":input|%s|\n",buff);
	*offt+=count;
	if(strncmp(buff,"Hello",5)==0){
		greeting=1;
		printk(MODULE_NAME":said Hello\n");
		return count;
	}
	if(strncmp(buff,"Clear",5)==0){
		total=0;
		printk(MODULE_NAME":said Clear\n");
		return count;
	}else if(checkStr(buff)!=0){
		error=1;
		return count;
	}
	sscanf(buff,"%d",&d);
	printk(MODULE_NAME":input %d\n",d);
	if(d!=0){
		total+=d;
	}else{
		error=1;
	}
	return count;
}
module_init(init_add);
module_exit(exit_add);

