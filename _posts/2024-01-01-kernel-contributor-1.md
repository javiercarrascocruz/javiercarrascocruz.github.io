---
layout:     post
title:      Become a Linux kernel contributor - Part 1
date:       2024-01-01 10:00:00
summary:    Finding something useful for your first contribution
tags:       contributor foss kernel linux
---

In this first episode of my series about how to become a Linux contributor, I will cover something that is usually overlooked even though it is the most important thing to consider when you want to contribute to any open-source project for the first time: finding something meaningful to do; something you like and the project needs.

The Linux kernel is a **<u>huge</u>** project and to make it more manageable, it is divided into several **subsystems** that encompass some kind of functionality (memory, video, network, USB, etc.). I would recommend you to focus on one (or two, but not ten) of them and start learning how it works. Just take a look at the existing subsystems and pick the one you prefer. [Here](https://docs.kernel.org/subsystem-apis.html) you can find a list with the different subsystems.

If you couldn't figure out what subsystem would suit you, there are some tasks you could pick until you find your way. I will cover some of them too, so you have no excuses.

#### Content:

1. [Prerequisites: nothing fancy](#1-prerequisites-nothing-fancy)
2. [Patches you don't want to send and no one needs](#2-patches-you-dont-want-to-send-and-no-one-needs)
3. [Low-hanging fruit: documentation](#3-low-hanging-fruit-documentation)
4. [Bug fixing: syzkaller works for you](#4-bug-fixing-syzkaller-works-for-you)
5. [Driver development: become a maintainer](#5-driver-development-become-a-maintainer)
6. [Some other options](#6-some-other-options)
7. [I found my way, what's next?](#7-i-found-my-way-whats-next)

### 1. Prerequisites: nothing fancy

You don't need much to start contributing: your preferred **text editor**, **git** and an **email account**.

Being fluent in **C** will make things much easier, but even without knowing C you could still contribute (documentation, scripts, tests, etc.). You might have heard that **Rust** is now an official language in the kernel. That is true, but it is still far from being commonly used. That might change in a few years, though... we will see. You definitely don't need Rust to program new features.

There is another obvious requirement: **the Linux sources and a toolchain to build the kernel**. The toolchain is architecture-specific, so search for the one you require (RISC-V, ARM, x86, etc.) and make sure you are able to compile the kernel at least with the default settings (`make defconfig`). You will find dozens of good articles online for every architecture, but if you want me to write about a specific one, just let me know and I will consider it :grin:

### 2. Patches you don't want to send and no one needs

First things first: your contributions should add some value to the user, and you should always bear that in mind. If you do so, it will become clear that some changes are unnecessary and only add extra workload not only to yourself, but also to the reviewers and maintainers with no real profit.

The most typical examples of unnecessary patches are those that only modify the code from a picky-reader perspective: typos in the name of a variable, trailing whitespaces, whitespaces versus tabs, etc. You do not want to submit patches to "fix" those for a number of reasons:

 - **It does not improve the user experience** in any way. If no one will benefit from it, what's the point?

 - **Reviewers and maintainers have to waste their time** with that instead of reviewing useful stuff. Do not be surprised if some of them only reply to criticize you.

 - **It hides the author of the code.** I like it when `git blame` shows me the author and commit hash of a certain change instead of a bunch of superfluous modifications that will make me look into the history of the file to find what I was looking for. Again, a patch that only increases workload is not cool.

### 3. Low-hanging fruit: documentation

The documentation is a good place to look for tasks within everyone's grasp. You will not learn as much as if you fix bugs or develop new code, but it is an easy entry point to get used to the submission workflow. If you are aiming for more valuable contributions from the beginning and feel confident enough, you can skip this section.

Some people think that the kernel documentation is far from perfect, and I would tend to agree with them. Many improvements would require some considerable effort, but others are straightforward and anyone could get them done. Some of the easiest fixes I could imagine are the following:

 - **Misspellings and grammar errors:** As opposed to their equivalent in the code, they are exposed to the end user and getting rid of them (minimally) improves documentation quality.

- **Undocumented struct members:** some data structures include a documentation section in the code where they are declared (usually right before the structure) and it is fairly common that it does not get updated when new members are added to the structure. To add the missing information, you will need to understand what the new member is used for, i.e. learn something new!

How can you find bugs in the documentation? Easy! 1. compile; 2. check the output. You can redirect the output to a log file and then analyze it to find something you could fix.

 - HTML documentation: `make htmldocs > doc.log 2>&1`

 - PDF documentation: `make pdfdocs > doc.log 2>&1`

#### Bindings

Another kind of documentation that is still a source of a high number of patches is the so-called **device tree bindings**, which you will find under `Documentation/devicetree/bindings`.

Many devices supported by the Linux kernel have (or should have) an associated document that defines its properties and how to define it in a device tree. These files or **bindings** used to be simple .txt files that do not support verification, but they are being converted into YAML files, which is a nice improvement.

There are still loads of bindings in the old format waiting for someone to convert them, and learning the YAML syntax is pretty simple as long as nothing weird is required. There are also many examples of bindings in YAML format upstream. And last, but not least: this approach is the most interesting so far because you will learn things you will need if you ever program device drivers.

### 4. Bug fixing: syzkaller works for you

A good way to make high-value contributions is fixing bugs found by *dynamic analysis* (fuzzing). Bugs found by *static analysis* often fall into the "low impact" category (a good example are unused variables, which will be optimized away by the compiler). Instead, a fix for a bug that happens under certain circumstances and can make the kernel crash is a great patch, and it will be ported to all (affected) stable kernels as soon as possible.

If you want to give it a try, [syzkaller](https://github.com/google/syzkaller) will help you a lot. It is a **fuzzer** to test the kernel under different architectures and configurations thanks to syzbot, which is a higher-level entity that runs multiple instances of syzkaller, collects the bug reports, and tracks the status of the bugs.

When a bug is found, a report is generated and uploaded to the [syzbot report site](https://syzkaller.appspot.com/upstream). The report offers a lot of valuable information such as the setup (arch, .config, last commit), logs and the backtrace with links to the functions in the code. Not only that, it can generate **reproducers**, which are programs to trigger the bug. You can even send patches to syzkaller to test if they fix the issue (actually, if it can't be reproduced anymore). With such amount of resources, you can just focus on the code where the bug resides and test your solution, both locally and upstream by syzbot. The bugs are sorted by subsystem, so you can focus on your favorite subsystem and learn more about it along the way.

I fixed a couple of syzkaller bugs during my time as a mentee from the [**Linux Kernel Mentorship Program**](/lkmp), and I have written [**this article**](/syzbot) that goes step by step throughout the whole process, from interpreting the logs up to fixing a real bug with Qemu/GDB and sending the patch upstream. Check it out!

### 5. Driver development: become a maintainer

You might think that writing a new driver for a piece of hardware would be a massive task for a newbie, but that does not have to be the case. Many drivers copy code from others that make something similar and require the same interfaces. On top of that, a great amount of every driver's code is just boilerplate. In some cases, the complete driver is *just* boilerplate, as it is usually the case for most of the new drivers under 300 lines of code.

If you know something about electronics and have some SoC (Raspi and friends) to test simple pieces of hardware (many sensors are), this approach is in my opinion the most rewarding. You will learn a lot about the subsystem that covers the device, become a driver maintainer (if you thought that by becoming a maintainer I meant a **subsystem** maintainer, you expected too much of both of us) and as the driver will probably require some documentation updates (bindings, ABI, etc.) or modifications in the subsystem core (new helpers, data members, etc.), you will submit several patches at once. Not bad for a first contribution!

If you want me to recommend you a subsystem for this, I would choose iio (Industrial Input Output). Many sensors fall into this subsystem and from my own experience I can say that it is a well-maintained subsystem with nice reviewers. You could look for an unsupported sensor you would like to have for your own projects and start working on its driver. Enjoy!

### 6. Some other options

Should none of the options I gave you satisfy you, there are some other options you could try. For example, another friendly subsystem is **selftests**, which maintains a number of test programs to validate certain parts of the kernel like user interfaces and specific features. Any contribution that increases test coverage is more than welcome, and there is good documentation to learn about how selftests work and how to program your own tests.

There are also some other trivial fixes you can find when compiling the kernel or its tools. I can give you a recent example: after compiling some selftests, I noticed that my working tree was not clean anymore, and some untracked files had appeared. When you add a new selftest that generates binary objects, you should include a **.gitignore** file to exclude them as stated in the [selftests documentation](https://docs.kernel.org/dev-tools/kselftest.html). Sometimes people forget and the fix is as trivial as creating the missing .gitignore and adding the binaries to the list. I can't help it, I am one of those weirdos who get a bit nervous if the working tree is not clean, so maybe the next time you will fix that for me and I will not notice. Or even better, you will review a new test that does not include the .gitignore and there won't be any fix at all :smiley:

What else... oh yeah, if you are a script geek and you think you could improve existing scripts or write valuable scripts that people would like to use, take a look at the *scripts* directory. Sometimes I have the feeling that some scripts are no longer up-to-date, but to be honest I have never delved into that section. But if you do something valuable there, please let me know!

#### A lot of TODOs

If you still have no idea where to start, don't give up! Until you get to know the kernel a bit better, here you have another source of ideas.

A simple `git grep TODO` will show you that there are literally hundreds of pending tasks waiting for someone to take care of them. Most of them will be way too difficult for beginners, but a few may meet your expertise level, whatever it is. There is not much else to say here, just look for pending tasks in a subsystem of your interest and start working on a solution.

Although some subsystems provide lists of TODOs, they are not always up-to-date, and the tasks are often way too difficult for a first contribution. But if you don't feel confident enough, and want someone to tell you what to do instead of choosing by yourself, they might be your way to get into the kernel, which in the end is what really matters.

### 7. I found my way, what's next?

You are not far from becoming a kernel contributor, so just work on your patches until you think they are ready. If you want to avoid some typical and often silly mistakes, [the second episode](/2024/01/03/kernel-contributor-2) will save you some time. Then you will start the submission process to get your changes accepted upstream. Ok, but how do I trigger that process and how does it work? Be patient, that is exactly the topic of the third and last episode :wink:
