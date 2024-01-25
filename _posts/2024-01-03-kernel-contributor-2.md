---
layout:     post
title:      Become a Linux kernel contributor - Part 2
date:       2024-01-03 10:00:00
summary:    Things to consider while making changes
permalink:  /:title
tags:       contributor foss kernel linux
---

This is the second episode of my series on how to become a Linux kernel contributor, where I will show you some typical mistakes you should avoid while working on your first contributions. If you are looking for ideas to make your first contribution to the kernel, please take a look at the [first episode](/kernel-contributor-1). If you just want to send your changes upstream, go for the [third episode](/kernel-contributor-3)... but read this one fast just in case.

#### Content:

1. [Make sure no on is doing the same](#1-make-sure-no-one-is-doing-the-same)
2. [One change, one commit](#2-one-change-one-commit)
3. [Read the code style guide and subsystem guides](#3-read-the-code-style-guide-and-subsystem-guides)
4. [Does MAINTAINERS need updating? And anything else?](#4-does-maintainers-need-updating-and-anything-else)
5. [Test your changes as if they were not for you](#5-test-your-changes-as-if-they-were-not-for-you)
6. [Bonus: igt post-commit hook](#6-bonus-git-post-commit-hook)
7. [My code is flawless, how do I send it upstream?](#7-my-code-is-flawless-how-do-i-send-it-upstream)

### 1. Make sure no one is doing the same

Working on something that has already been sent upstream is a waste of time you want to avoid. It could even happen that a solution to your problem has been accepted and you did not notice, because you didn't look carefully. Typical example: you aim to fix a bug that has already been fixed and it is available in the subsystem's tree, but not in the mainline kernel yet. Bear in mind that the mainline repo will only get the changes applied in the merge window (unless it is a relevant fix or something urgent).

Checking the mailing lists and the subsystem tree for such collisions will take you a couple of minutes and can save you from working for nothing. You can also take a look at the [patchwork](https://patchwork.kernel.org/) site of the subsystem to see what is going on.

If someone has already sent a solution upstream, but yours is more complete or somehow better, be nice and give the required feedback to improve the proposed change instead of moving on. Collaborations are also possible for more complex changes, and although they are not common, I can confirm they happen. I had been working on a driver for the hdc302x when I saw that someone sent a series to the mailing list to support that device. The proposed solution could benefit from some development I have already finished, so I emailed the author and asked if we could collaborate. Now the driver is in the mainline kernel and we are both co-authors and co-maintainers. Success story!

### 2. One change, one commit

You should always aim for a single commit for every change you make that can be seen as a single unit with a single purpose. That unit or **patch** will have a clear description (use the commit description for that, not just the commit title... you slacker!) and be chronologically coherent with previous and future patches (be careful if you reorder your commits), so you could check out any commit of a series and compile the kernel without breaking anything because it depends on a later commit of the same series.

Ideally, your patches should not be huge, containing thousands of lines and dozens of files. Sometimes a big patch makes sense (e.g. the code of a new driver is a single patch and sometimes thousands of lines are required), but usually it means you missed some level of granularity. Huge commits are problematic in several ways: they are difficult to review, hard to describe properly, and errors are easily overlooked.

Let's see an example: you programmed a driver (2000 lines of code), added some new variable to the subsystem core, and wrote the bindings for the device. You probably guessed that three patches will be required. And you were right:

 - The driver is big, but still a single unit (its code has a single purpose). You will probably have to modify other files like the **Makefile** and the **Kconfig**, but that belongs to the same commit because they are parts of the same logic unit.

 - The bindings describe a device and not a driver, so they require a separate patch.

 - The change in the subsystem core does not depend on the driver and it could be used by any other driver in the future. Therefore, an additional patch is required. Moreover, if you commit everything together and the commit gets reverted, the system will break if any other driver uses your new variable.

Would you commit the three changes in the order I listed them? <span style="color:red"> Of course not!</span> Did you already forget, that the patches must be chronologically coherent? If only the driver patch gets applied, the code will not compile because it requires a variable in the core, which has not been added yet. The bindings should also be added before the driver to get rid of warnings in case the driver uses properties described in the bindings. So let's order the series:

1. Patch for the subsystem core.
2. Bindings.
3. Driver.

### 3. Read the code style guide and subsystem guides

If you follow this guide, you might save a dummy extra version to fix coding-style issues, which is always an opportunity to mess up. So let's get it right from the beginning.

Like most of the projects where code is involved, the Linux kernel has some [coding-style rules](https://docs.kernel.org/process/coding-style.html) that should be observed. Unlike many other projects, those rules may vary among subsystems and might be followed loosely by some of them. Therefore, the easiest way to do things right is following the general coding-style guide and plus any variations required by the subsystem where your patches will be applied. Take a look at existing code in the subsystem to have a grasp of the common patterns (e.g. the variable used to return errors might always be named "error").

#### Short lines and long tabs: clean and readable code

 For anyone who is used to program in languages with 2-space tabs and 100+ symbols per line, this point may not be trivial. The general rule in the kernel code is 8-space tabs and no more than 80 characters per line (which was later extended to 100 characters per line but still 80 is strongly recommended when possible), which some might take for madness. This time I have to disagree because I like the rule.

Sometimes it is a bit challenging if the function you are calling has a long name, but in general it improves readability by avoiding crazy nested statements and endless operations or comparisons in a single line. That is seldom required in the kernel, and usually there is a cleaner solution. Sticking to this coding style will make your code look better, and your changes will be much easier to understand at first sight, which will also increase the chances that they get accepted upstream.

<figure>
    <img src="/images/posts/2024-01-03-kernel-contributor-2/hadouken-indent.jpeg"
         alt="Hadouken indent">
    <figcaption><i>If your code looks like this, you are in trouble</i></figcaption>
</figure>


#### Clang formatting: followed loosely at most

You might have noticed that there is a .clang-format file in the root directory and you might be tempted to apply the rules in there to the whole project. Forget it, your patches are going to be rejected and probably criticized. Those are only general rules that are broken all the time by contributors in general, and maintainers who do not like them in particular. You can still format your code with `clang-format -i *your_file*` to have some clean basis, yet some adjustments might be required (e.g. line breakings). You can compare your code to similar code from the kernel and see what definitely does not follow the clang rules. In my case particular case, I am a **Vim** user and I defined some rules in my *.vimrc* to follow the general rules. I bet there are plugins and other stuff to do the same with Vim or any other editor, so consider using them.

Alright, we are done with this utterly boring topic, let's move on!

### 4. Does MAINTAINERS need updating? And anything else?

`WARNING: added, moved or deleted file(s), does MAINTAINERS need updating?` is a message you will often see when you add new files to the kernel and check your patches with **checkpatch.pl**, which you should always do. That is a reminder that the MAINTAINERS file might require a new entry with your new addition. But that is not the only file you might need to update!

Your changes might need an update in the documentation (typical examples: bindings for a new device or specific documentation required by the subsystem, like **hwmon** does) or new elements in other data structures beyond your code (often obvious if you want your code to compile). Make sure that you do not forget other parts involved by comparing your changes to similar ones that go accepted. Did the series include additional patches? Is there a subsystem documentation you ignored that tells you exactly what you should submit? Make sure you don't miss anything and save an extra version.

### 5. Test your changes as if they were not for you

It is obvious that you should test your changes and make sure that they work fine. But the Linux kernel supports multiple architectures and an almost infinite number of different configurations. That means that your changes might work in your environment, but dramatically break the system under different circumstances. Not only that, your code should be ready for misusage: negative/zero values, long strings, NULL pointers... anything that is not avoided by an upper layer.

It is also true that you might not have many different architectures to test, or the code might be architecture-agnostic. Testing random configurations is not your task (fuzzers to the rescue!) and you will usually get identical results. Thus, you should have some understanding of what is in principle worth testing.

For example, if you are programming a driver for a light sensor, you do not need to test it with different architectures because that will be out of the scope of your driver. If you do, it is fine, but it should make no difference. On the other hand, you should make sure that the driver works fine as a module and also built-in. You might have always tested it built-in and everything was ok, but that does not mean that no one will ever use it as a module, and if it then crashes straight away... Hmm... not cool, right?

One thing I always do before sending patches upstream is building the kernel with **both gcc and clang**, because sometimes they detect different errors For example, gcc complains a lot about missing headers, and clang finds IMO more serious bugs that gcc misses.

### 6. Bonus: git post-commit hook

If you know what git hooks are, you can just take a look at my **post-commit** hook at the end of this section in case you might find it useful for your workflow.

Git hooks are nothing more than scripts that git calls automatically at certain points of the git workflow (pre/post-commit, pre-rebase/push, etc.). Actually, you have samples of all of them in your local repositories under *.git/hooks/*. Here I am only going to focus on the **post-commit** hook to feed it with the script the kernel provides to make some basic patch quality checks: **checkpatch.pl** (you will find it under *scripts/*). The hook will be triggered after every new commit (yeah, post... commit) and checkpatch will make sure that there are no **obvious** mistakes in your changes (basically code-style errors and misspellings, but also some more relevant issues). Note that if you trigger an **interactive rebase** (`git rebase -i`), your commits will get new hashes and can be considered new commits, so the hook will be called for all the commits being affected by the rebase.

You could of course run *checkpatch* right before sending your patches instead, but I don't like last-minute modifications because they have already costed me unnecessary, embarrassing extra versions of a couple of series in the past, and since then I always try to keep my changes clean from the beginning.

This is the post-commit hook I use:

{% highlight bash %}

#!/bin/bash

exec git show --format=email HEAD | \
./scripts/checkpatch.pl --strict --codespell

# Make sure the file is executable:
chmod a+x .git/hooks/post-commit

{% endhighlight %}

Simply save the script as *post-commit* in the *.git/hooks/* directory.

### 7. My code is flawless, how do I send it upstream?

Even flawless code can require modifications and extensions, but you will learn that during the process that you are about to trigger. The [last episode](/kernel-contributor-3) of the series will walk you through the steps you will have to follow. Stay tuned and share knowledge!
