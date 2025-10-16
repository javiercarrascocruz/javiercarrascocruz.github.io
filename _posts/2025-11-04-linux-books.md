---
layout:     post
categories: tech
title:      Books to Master Linux
date:       2025-11-04 08:00:00 +0900
summary:    From basic concepts to kernel programming
permalink:  /:title
image:      /images/posts/2025-11-04-linux-books/linux-books.webp
tags:       books linux
---

{% include affiliate_disclaimer.html %}

Although the learning paradigms are moving really fast with the irruption of AI, we have not reached the point where structured material like courses and books has lost its great value. Apart from the obvious mistakes that the AI still makes and its lack of deep details, learning from experts in the field will always give you that edge from their experience that no one else can provide. I am an avid reader, especially of technical books, and even though I have spent many hours reading upstream code, some of those books have been the most important element to learn efficiently, saving a lot of time and hence even money.

In this article, I will give you some links and short reviews of a few of the Linux books that have been released within the last years. Many of them are literally in front of me on my shelves right now! Note that a couple of them are not yet available, but already have enough preview material to give a clear idea of what they offer. I have listed them in my own personal order of preference, which values reliability over price or novelty, giving more weight to revised editions, books supported by public repositories and errata pages, as well as overall reception and community reviews. Let’s see what we’ve got!

---
<h2 class="content-heading">Content:</h2>

* TOC
{:toc}
---

### General Concepts

If you are new to Linux, I strongly recommend picking up one of the following books, as they will help you understand the essential concepts quickly, build a solid foundation, and avoid the trial-and-error learning path that often wastes A LOT of time.

- **How Linux Works, 3rd Edition: What Every Superuser Should Know** by Brian Ward (2021) is one of the very few good books that have been written about general Linux concepts this decade. It’s a sort of Swiss Army knife for understanding how Linux works under the hood — covering everything from the boot process and system management to networking and scripting — without diving into deep programming. It covers key topics like the Linux boot process and system initialization, kernel and device management, networking and firewall configuration, and practical scripting for everyday administration.

{% include affiliate_link.html
  url="https://amzn.to/3WB0lfi"
  image="/images/posts/2025-11-04-linux-books/how-linux-works.webp"
  text="How Linux Works" %}

- **The Linux Bible, 10th Edition** by Christopher Negus (2020) is a legendary book in the Linux community that has introduced thousands of people to the system, now reaching its tenth edition. Although it is already five years old, the fundamental concepts have not changed much, and it remains almost entirely valid today throughout its nearly 900 pages. An eleventh edition would certainly be welcome, but there is no need to wait for it since the updates would (most likely) only bring small changes.

{% include affiliate_link.html
  url="https://amzn.to/4qF87Td"
  image="/images/posts/2025-11-04-linux-books/linux-bible.webp"
  text="The Linux Bible" %}

### Kernel

A few books covering the Linux kernel have been released within the last couple of years, some of them being new editions to update their materials. And most of them are programming- and development-oriented because that's usually the most common audience for such books, but they are often accessible for regular users because they start from the basics. Let's see what we have!

- **Linux Kernel Programming: A comprehensive and practical guide to kernel internals, writing modules, and kernel synchronization** by Kaiwan N. Billimoria (2024) is currently the most recommended book to learn about the Linux kernel from the very basics. It is based on Linux kernel 6.1. And that is not just a random version — it’s one marked as LTS (Long Term Support) until December 2026, so it will stay around for a few years. Moreover, that version has been selected by the Civil Infrastructure Platform (CIP) as an SLTS (Super-Long-Term Stable) kernel, meaning it will be maintained until August 2033. Plenty of time for you to learn how it works through the 800+ pages of this book 😉

{% include affiliate_link.html
  url="https://amzn.to/48VjBvv"
  image="/images/posts/2025-11-04-linux-books/linux-kernel-programming.webp"
  text="Linux Kernel Programming" %}

The book includes an additional digital chapter to set up a virtual machine that will be used for the examples in the book, making it suitable for non-Linux users as well. Not that Linux users should test their kernel development on their workstation's kernel, though! Apart from some exceptions, like fixing a bug you're experiencing, it's seldom a wise decision :laughing:

I read this book a few months ago, and although it is perfect for complete beginners, medium-level readers will also find a lot of valuable information exposed in a comprehensive and entertaining way. Even though I don't consider myself a kernel expert yet, if you are one, I bet you'll still learn some tricks you didn't know! In my opinion, this book is one of the best ones you can acquire if you want to learn about Linux kernel development.

- **Linux Kernel Programming: Developing kernel architecture and device drivers for character, block, USB, and network interfaces** by Thierry Gayet (2025) is a rather new book (released in June 2025). This book is shorter than the previous one (462 pages) and slightly more affordable, but the most interesting thing is that it places considerable emphasis on device drivers, going a bit deeper into topics such as USB, PCI, and networking, which makes it an ideal choice if you want to learn about the Linux kernel with a stronger focus on device drivers while still covering other important areas.
 

{% include affiliate_link.html
  url="https://amzn.to/4hFkf2t"
  image="/images/posts/2025-11-04-linux-books/linux-kernel-programming-2.webp"
  text="Linux Kernel Programming" %}

### Device Drivers

This is one of my favorite topics, and I have read a few books about it throughout my entire career. I will start with the most relevant at the moment, leaving the most outdated one, yet still the most read one, for a number of reasons, for the end.

- **Linux Device Driver Development: Everything you need to start with device driver development for Linux kernel and embedded Linux, 2nd Edition** by John Madieu (2022). There are not many modern books covering Linux device drivers in detail at the moment, but this one is (still) an exception. It covers a wide range of topics from the most fundamental concepts like the basics of the Linux kernel development or the **Linux Device Model** up to more advanced aspects like the **Device Tree** and the **IRQ Framework**, all with the current upstream implementations (some very new concepts might be missing, but nothing really fundamental). It also discusses some real kernel subsystems like **Industrial Input/Output (IIO)**, **GPIO**, and **Input**, although the book is more about general concepts, and not about a complete analysis of the different driver subsystems.

{% include affiliate_link.html
  url="https://amzn.to/49E8rLI"
  image="/images/posts/2025-11-04-linux-books/linux-device-driver-development.webp"
  text="Linux Device Driver Development" %}

I can tell you that this book has been refined and brought to the current Linux development because I have both the first and the second editions. The second edition is better structured, and it follows the current Linux architecture more closely. It also discusses the key concepts in more detail, which is what you should expect from this kind of book, and it has dropped some random subsystems that were covered in the first edition for no apparent reason, like PWM or RTC. Unfortunately, the Regulator Framework has been dropped too, but you can find it deeply covered by me [here](https://www.patreon.com/posts/linux-device-10-136743553?utm_medium=clipboard_copy&utm_source=copyLink&utm_campaign=postshare_creator&utm_content=join_link) and [here](https://www.patreon.com/posts/linux-drivers-11-142623517?utm_medium=clipboard_copy&utm_source=copyLink&utm_campaign=postshare_creator&utm_content=join_link).

This is a rather long book (more than 660 pages) that contains a lot of precious, digested information for those who want to become device driver developers and optimize their learning time, although I have to admit that it could have been much shorter without losing too much value. You will find way too many snippets that have been copied from the mainline kernel, especially structures, to describe every element (some of them trivial) as if they were not described in the code (often they are). But if you ignore those parts and just move on to the next explanation of key concepts, you will enjoy **Linux Device Driver Development, 2nd Edition** and learn a lot along the way.

- **Mastering Linux Device Driver Development: Write custom device drivers to support computer peripherals in Linux operating systems** by John Madieu (2021) is another book by the same author that covers device drivers from a more advanced perspective, focusing more on more complex drivers that involve frameworks like **Video for Linux 2** (V4L2), **Advanced Linux Sound Architecture** (ALSA), or **Multi-Function Device** (MFD).

{% include affiliate_link.html
  url="https://amzn.to/436Yxyn"
  image="/images/posts/2025-11-04-linux-books/mastering-linux-device-driver-development.webp"
  text="Mastering Linux Device Driver Development" %}

I have this book as well, and although it abuses a bit too much of the same code snippets that I mentioned before for his other book, it is great to understand the interactions between the drivers and the frameworks, and how things work together. If you are still a beginner, I would start with the previous book, and then continue with this one. Some topics overlap a little bit, but this book does not spend much time discussing basic stuff, and you might get lost quickly if you don't know them.

- **Linux Device Drivers, 3rd Edition**

Let's be honest, this book was amazing back then, but nowadays, it is pretty much outdated (said by its own authors) and definitely not the best source to learn how to program modern device drivers. I thought of moving it to the **Hall of Fame** you'll find at the end of this article, but many people still like it for a number of reasons, and some of them are, in my opinion, good ones:

 1. The authors are renowned experts. Absolutely, no question about that. What they wrote back then was a masterpiece, but that was unfortunately a long time ago, and they have said multiple times that there will not be a 4th edition. But if they ever change their minds, I will be one of the first ones to buy the new edition for sure.
 2. Key concepts don't change much. That's actually true, and many topics the book covers haven't changed since then, or at least not much. The problem is that you have to know what is still relevant and what is not, and that is time-consuming as well as a possible source of misunderstandings. 
 3. It's free. Many people will not admit that, but if it weren't free, they would not even consider reading such an old book. But I am not here to judge your motivations :laughing: Anyway, it's still a free source to learn the key concepts!

If you are interested in reading this book, or at least some chapters, you can find them [here](https://lwn.net/Kernel/LDD3/).

### Interface and System Programming

The first part of this section refers to the Linux API and the GNU C library. And here there is no contest, and all Linux developers, no matter from kernel or user space, have heard of this book: **The Linux Programming Interface** by Michael Kerrisk. The author is, by the way, the maintainer of the man pages, which can give you an idea of the deep knowledge he has on the matter. This book is definitely not new (it was published in 2010), but given that the Linux API does not change much over time, it is still very relevant and not a *Hall of Famer* just yet. There might be some newer API calls that are not present in the book, but I can tell you that more than 95% of what you will possibly need as a userspace developer will ever need is there. And if you are a kernel developer, having a reliable description of the Linux system calls will help in multiple scenarios.

{% include affiliate_link.html
  url="https://amzn.to/3WKHQVC"
  image="/images/posts/2025-11-04-linux-books/TLPI.webp"
  text="The Linux Programming Interface" %}

I still need to program userspace applications from time to time, and whenever I am not sure about how to proceed with some task, this is the first book I take to find a clean solution... and I usually find one in it :smiley: Apart from the descriptions of more than 500 system calls, there are multiple examples, code snippets and explanations about how to proceed in order to implement an efficient solution. This book is definitely not the cheapest one on the list, but it is, on the other hand, one of the most complete: more than 1500 pages of pure knowledge!

And if you didn't have enough with that monumental book, you can put most of its concepts (and many others) in practice thanks to another rather long (more than 1000 pages) and recent (from October 2025) book: **System Programming in Linux: A Hands-On Introduction** by Stewart N. Weiss (2025). Apart from making use of the Linux interface, it also covers interactions with filesystems, devices and terminals, focusing on a practical approach to program software on Linux.

{% include affiliate_link.html
  url="https://amzn.to/48ZY2Kg"
  image="/images/posts/2025-11-04-linux-books/system-programming.webp"
  text="System Programming in Linux" %}

### eBPF

eBPF (extended Berkeley Packet Filter) is a powerful technology that allows running small, sandboxed programs inside the Linux kernel without modifying its source code. Originally designed for network packet filtering, it has evolved into a general-purpose mechanism used for tracing, monitoring, performance tuning, and even security. Over the past few years, eBPF has gained tremendous popularity not only among kernel developers but also among engineers and system administrators who use it to safely extend and customize kernel behavior. It clearly deserves its own dedicated section, as a growing amount of literature, tools, and community interest continue to emerge around it.

- **Learning eBPF: Programming the Linux Kernel for Enhanced Observability, Networking, and Security** by Liz Rice (2023) is a popular book about this topic because it starts from the most basic concepts to build upon them without turning into a massive book that no one will ever read. If you want to start using eBPF, this 217-page book is a great choice.

{% include affiliate_link.html
  url="https://amzn.to/47DyAbd"
  image="/images/posts/2025-11-04-linux-books/learning-ebpf.webp"
  text="Learning eBPF" %}

- **Learning eBPF: High performance observability, networking, and security programming on Linux** by Michael Kehoe (2025) is a recent book about eBPF that covers similar topics as the previous book from the list. It seems that eBPF is getting so popular that it's become difficult even to find an original name for a book about it :laughing: This one gets slightly more technical, but it shares a lot of similarities with the previous one, including its length (234 pages). I'd pick only one of them to start learning, and if the need arises, then get the other one.

{% include affiliate_link.html
  url="https://amzn.to/47rUoWU"
  image="/images/posts/2025-11-04-linux-books/learning-ebpf-2.webp"
  text="Learning eBPF" %}

### Distributions

This is probably the topic I am less interested in, because I am not a distro-hopper and, for me, as long as there is a command line and some basic tools, I do not care much about the rest. On the other hand, many people do care a lot about the distro they use, and they want to make the most of it. And to my surprise, I learned a couple of interesting tricks after reading a book about one of the distros I have been using for more than a decade, both personally and professionally, Ubuntu (by the way, the other one is Debian), and I have incorporated them into my workflow with very positive results. The book I am talking about is the very recent **The Ultimate Ubuntu Handbook: A complete guide to Ubuntu 24.04, from installation to advanced security and development** by Ken VanDine (2025).

{% include affiliate_link.html
  url="https://amzn.to/4oiUKq2"
  image="/images/posts/2025-11-04-linux-books/ubuntu-handbook.webp"
  text="The Ultimate Ubuntu Handbook" %}

Many concepts are rather basic, so it is suitable for complete beginners, and a few topics are more advanced for those who have been running Ubuntu for some years and want to go a step further in its usage or improve their setup, like I did. It is a rather new book (from August 2025) with more than 350 pages, making it the most complete and up-to-date guide for Ubuntu users right now.

I have to admit that I am not much into Linux servers, and even though I have used them professionally (like almost everyone else in the field), it is not a topic I really know, so I won't cover it in this article. I will just let you know that there is a specific book for the same distro we have just discussed, called **Mastering Ubuntu Server: Explore the versatile, powerful Linux Server distribution Ubuntu 22.04 with this comprehensive guide** by Jay LaCroix (2022), that might help if that is what you are looking for instead of general-purpose distribution concepts.

{% include affiliate_link.html
  url="https://amzn.to/437pJgy"
  image="/images/posts/2025-11-04-linux-books/mastering-ubuntu-server.webp"
  text="Mastering Ubuntu Server" %}

To be fair and cover some other popular distribution, let's provide some good reference for **Red Hat Enterprise Linux (RHEL)** in its 9th version (the latest one at the moment of writing is the 10th, but the 9th is still pretty strong, and that will not change for some time). **Red Hat Enterprise Linux 9 for SysAdmins: A technical guide for building secure production systems using RHEL 9 administration** by Jerome Gotangco and Luca Berton is the book you need if you want to get deeper into this distribution.
 
{% include affiliate_link.html
  url="https://amzn.to/3JH0ZFa"
  image="/images/posts/2025-11-04-linux-books/RHEL9.webp"
  text="Red Hat Enterprise Linux 9 for Sysadmins" %}

### Memory

The great hit at the moment is definitely **The Linux Memory Manager** by Lorenzo Stoakes (among others, maintainer of the memory management in the Linux kernel), even though **it has not been released yet** :laughing: But you seldom have the chance to read a book about such a key topic in the Linux kernel written by the maintainers themselves, and the couple of pages that have been released so far look very promising. It seems that the book will be out in January 2026, and it will have 1300 pages (!!). If you want to be among the first ones to read it, it is possible to pre-order it online:

{% include affiliate_link.html
  url="https://amzn.to/4oouY3Y"
  image="/images/posts/2025-11-04-linux-books/memory-manager.webp"
  text="The Linux Memory Manager" %}

### Debugging

I know, debugging is not a Linux concept, but it has to be on the list anyway. For me, a Linux user (let alone a developer) is often a curious and perfectionist person, sometimes a tinkerer, and knowing how to fix bugs on your Linux systems is an ability you'll never regret having. And although this is a topic that very few people might find interesting enough to read a book about it, mastering it (or at least having a good source to look up!) will get you out of critical situations more often than any other from this list, especially (but not only!) if you are doing any Linux development. Again, there are not many books about this topic, but I can recommend one that I have on my shelf: **Linux Kernel Debugging: Leverage proven tools and advanced techniques to effectively debug Linux kernels and kernel modules**, also by Kaiwan N. Billimoria, from 2022.

{% include affiliate_link.html
  url="https://amzn.to/4qGmd6H"
  image="/images/posts/2025-11-04-linux-books/linux-kernel-debugging.webp"
  text="Linux Kernel Debugging" %}

This book explains many different kinds of bugs that you will experience at some point if you use any kind of software, even if you are just a Linux user. If you want to solve them yourself or report them to the community, understanding what's roughly going on and even being able to provide extra information like logs or results from debugging tools will be a great skill in your repertoire. And I can tell you that this is one of the books that I re-read more often whenever I am working on some annoying, non-trivial kernel bug. Moreover, what you will learn will be easily portable to other environments that are not kernel-related, like apps or userspace, because the concepts will be similar, and just the tools will differ.

### Shell Scripting

If you don't write shell scripts to automate some of your daily tasks, you're wasting a lot of time! And although it is tempting to use your favorite AI to get them done within a few seconds, you will get in a lot of trouble if the task is slightly complex and the AI hallucinates once again. Instead, you can go the reliable (but unfortunately slower) way, and learn how to write your own scripts, and understand what they are actually doing. At the moment, there are two books that stand out in this underrated aspect of Linux that most of us should know better than we do.

- **Linux Command Line and Shell Scripting Bible, 4th Edition** by Richard Blum and Christine Bresnahan (2021) has been a reference in shell scripting for years, and if you think that a book that was written in 2021 might be slightly outdated, think that shell scripts usually stay around for many years once they are finished. And if you still would like to update yours to include the latest features and good practices, don't worry: **the 5th edition is on its way!** It is planned for February 2026, and you can already pre-order it.

{% include affiliate_link.html
  url="https://amzn.to/4qEKnyp"
  image="/images/posts/2025-11-04-linux-books/linux-command-line-shell-scripting-bible-4.webp"
  text="Linux Command Line and Shell Scripting Bible, 4th Edition" %}

{% include affiliate_link.html
  url="https://amzn.to/47GswP3"
  image="/images/posts/2025-11-04-linux-books/linux-command-line-shell-scripting-bible-5.webp"
  text="Linux Command Line and Shell Scripting Bible, 5th Edition (pre-order)" %}

- **The Ultimate Linux Shell Scripting Guide: Automate, Optimize, and Empower tasks with Linux Shell Scripting** by Donald A. Tevault (2024) is another relatively new alternative that covers a lot of ground in its almost 700 pages. If you want to make the most of your shell scripts, this book is for you. It includes a lot of examples, and it even covers different shells like Z Shell and PowerShell, although it's mainly focused on the ubiquitous Bash.

{% include affiliate_link.html
  url="https://amzn.to/4oKWd8r"
  image="/images/posts/2025-11-04-linux-books/ultimate-shell-scripting.webp"
  text="The Ultimate Linux Shell Scripting Guide" %}

### Hall of Fame

The following list includes some of the most popular Linux books that, unfortunately, have become outdated. If you find yourself reading one of them, you can be sure that many things will not be like they used to be when the book was written. They might still be fun to read, good sources if you want to track the evolution of Linux (or even philosophical aspects in some of them), and sometimes still relevant for some key concepts. Nevertheless, don't use them as reliable sources to learn modern Linux or at least don't blame me if then you apply concepts from 20 to 30 years ago as if they were still in use :wink:

- The Linux Bible, Christopher Negus (for versions < 10th)

- Understanding the Linux Kernel, Daniel P. Bovet & Marco Cesati, 2000

- Linux in a Nutshell, Ellen Siever / Stephen Figgins / Robert Love / Arnold Robbins, 2009

- Linux Kernel Development (3rd Edition), Robert Love, 2003

- Ubuntu Hacks, Jonathan Oxer / Kyle Rankin / Bill Childers, 2006

- Rebel Code: Linux and the Open Source Revolution, Glyn Moody, 2001

- The Cathedral & the Bazaar: Musings on Linux and Open Source, Eric S. Raymond, 1999

- The Linux Kernel Book, Rémy Card / Éric Dumas / Franck Mével, 1998

- Linux Server Hacks, Rob Flickenger, 2003

- The Business and Economics of Linux and Open Source, Martin Fink, 2003

- Linux Network Administrator’s Guide, Tony Bautts / Terry Dawson / Gregor N. Purdy, 2005

- The Unix Programming Environment, Brian W. Kernighan & Rob Pike, 1984

---

If you purchase (and of course, read! :laughing:) just a couple of the books I have listed in this article, you will be for sure on your way to becoming a **Master of Linux** :wink:
