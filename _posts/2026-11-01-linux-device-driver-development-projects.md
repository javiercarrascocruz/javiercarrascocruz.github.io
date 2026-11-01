---
layout:     post
categories: tech
title:      "My First Book: Linux Device Driver Development Projects"
date:       2026-11-01 10:00:00 +0100
summary:    A dream come true
permalink:  /:title
image:      /images/ldddp_cover.webp
tags:       books drivers kernel linux qemu
---

My book, **Linux Device Driver Development Projects**, published by Packt, is now available for [pre-order](https://link.amazon/B01C5cytz)!

Since I started publishing articles on this site and mentoring in the Linux Foundation's Linux Kernel Mentorship Program, I have dreamed of writing a book about Linux device drivers to help more people join the Linux developer community through a simple, smooth and structured learning path that helps them avoid beginner mistakes and save time. After several months of work, that dream is getting close to becoming a reality. This is the first time I am sharing it publicly; until now, only a handful of Linux developers I consulted along the way knew about it. The book is already well along, with only a few chapters left to write.

Unlike my other posts about Linux drivers, this book focuses on writing drivers yourself and testing them in a virtual environment. Each chapter builds a driver for a particular Linux subsystem, introducing the relevant frameworks and APIs along the way rather than explaining them one by one. I chose this approach based both on my own experience and on the difficulties I have seen when mentoring people through their first steps in kernel development.

I want the book to be useful whether you are a C programmer writing your first kernel code or you already have some experience and want to go further. And even though the projects don't need physical hardware, the drivers still follow upstream conventions and use the frameworks and APIs you would use for real devices.

One thing I particularly wanted was for **all the projects to work without dedicated hardware**. QEMU and the emulation capabilities available in Linux and its subsystems make that possible. That also means you can follow along if your main computer runs another operating system and you have not made the full transition to Linux yet.

The projects will leave room for you to experiment and come up with your own solutions instead of simply following a recipe. My implementations will be available in an accompanying GitHub repository as a reference, and I intend to keep the code updated as Linux evolves.

There is still work to do. The final book will be longer than the current Packt description suggests, and I will keep expanding and refining the content as I finish the remaining chapters.

Today seems like a particularly good day to share this announcement: the Linux 7.4 merge window has just set a new record with almost 20,000 git commits! For anyone looking to join the growing community of Linux developers, I hope this book will be a useful companion, whether you are taking your first steps or looking to deepen your existing skills.

You can [pre-order Linux Device Driver Development Projects on Amazon](https://link.amazon/B01C5cytz).
