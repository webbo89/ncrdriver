#NCR 7167 DRIVER C++
>A framework for writing to the receipt printer over usb serial connection. Will probably work for other NCR models with a little tweaking.

![](https://pbs.twimg.com/media/A719nP1CUAABQgr.jpg:large)


## Features ##
- String printing
- Monochrome Bitmap printing (QR codes)
- QR code generation
- Ticket Cut
- Beep

For our Human Centered Robotics course at university we are making a robot that will print unique leaflets/tickets after "making friends" with a human. So we bought a receipt printer for its rapid printing and cutting capabilities.

The "main focus" of the project is the C++ code to print text files, QR codes and cut.

## Usage ##

Remember to ensure permissions where printer is connected: 

`CHMOD 777 /dev/ttyUSBX`


Setup + arguments (all required): 

./async 

1. int ***port***
1. int/epoch ***time***
1. string ***messagepath***
1. bool ***cut***
1. int ***lines*** (suggested: 0)
1. bool ***beep*** 

Samples: 

`./async 0 1354133999 message.txt true 3 true
./async 0 1354133999 message.txt false 0 false`

by

[maxrev17](https://github.com/maxrev17/)

[webbo89](https://github.com/webbo89/)