# cdev
An example of Linux kernel module of character device, addition logical device. 

/*example*/
$echo 1 > /dev/asyoulike
$cat /dev/asyoulike
1
--the quite natural result
--again
$echo 1>/dev/asyoulike
$cat /dev/asyoulike
2
--the strange result
--once again
$echo 5>/dev/asyoulike
$cat /dev/asyoulike
7
--it seems to return the total of the written numbers.
--So, I try to write a charactor which is not a number.
$echo X>/dev/asyoulike
$cat /dev/asyoulike
You are so silly
--got a scolging.
--send a greeting
$echo Hello>/dev/asyoulike
$cat /dev/asyoulike
Good day!
