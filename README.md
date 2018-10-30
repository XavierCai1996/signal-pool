# signal-pool
callback framework for C++

## Features

* Provide a template for one argument callback (signal sink)

* Provide a template to realize signal source to bind several signal sinks, once signal source invoked, all sinks hooked on this source will be triggered

* Provide a template to realize signal pool to combine or manager several of signals, differ them by `Identity` such as a `std::string`; so that we can hook a signal sink or invoke a signal source by Identity

* Provide a template base class to encapsulate the interfaces of signal pool to derived classed

* You may think it's too limited to use callback with only ONE argument, but in fact, it's really enough if you try combining this project and [parameter-list](https://github.com/XavierCai1996/parameter-list) together

## Directories

* `./src/` : implement codes

* `./example/` : a simple example of this callback & signal framework

* `./src-parameter-list/` : source codes of my another project [Parameter List](https://github.com/XavierCai1996/parameter-list), it's amazing to combine these two projects together!

* `./example-with-parameter-list/` : a example to use signal pool with parameter list 

## How to run the example

### Simple example

1.Enter to the folder `./example/`.

2.Run the script `./run.ls`.

### A example with parameter list

1.Enter to the folder `./example-with-parameter-lish/`.

2.Run the script `./run.ls`.

## How to use this template in projects

1.Copy the folder `./src/` to your projects.

2.Include the header file `./src/signal-pool.h` in your codes.

if you want to use parameter list just like example in the folder `./example-with-parameter-list/`, you need to :

1.Copy the folder `./src-parameter-list/` to your projects.

2.Include the header file `./src/parameter-lish.h` in your codes.

## Dependencies

The current versions and compile options for the dependencies are:

### Linux
* g++ 4.8.1
* make 3.81
* cmake 2.8.0

## Contact me

Contact me on my mail address cxw39@foxmail.com

and sorry about my poor English XD.
