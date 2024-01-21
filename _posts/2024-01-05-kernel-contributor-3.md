---
layout:     post
title:      Become a Linux kernel contributor - Part 3
date:       2024-01-05 10:00:00
summary:    Submit your first patch upstream safely
tags:       contributor foss kernel linux
---

This is the third and last episode of my series about how to become a Linux kernel contributor. It is a bit longer than the others because it shows step by step the whole submission process. Nowadays few people can still read or pay attanetion for 15 minutes straight, but try your best. Otherwise, be ready to spend hours fixing what you ignored here :wink:

If you are looking for ideas to make your first contribution to the kernel, please take a look at the [first episode](/kernel-contributor-1). If you want to avoid beginner mistakes in your first contribution, the [second episode](/kernel-contributor-2) may be helpful. If you read the two first episodes, or you think you don't need them (<span style="color:darkorange"><b>you will miserably fail!</b></span>), welcome to the last episode.

Getting your changes accepted upstream might be a complex and frustrating process if you don't know how it works. If you are expecting a simple PR on GitHub, this article was made for you.

#### Content:

1. [Understanding the submission process](#1-understanding-the-submission-process)
2. [Warning: only plain text without frills](#2-warning-only-plain-text-without-frills)
3. [git configuration](#3-git-configuration)
4. [First walk-through: formatting and sending patches](#4-first-walk-through-formatting-and-sending-patches)
5. [Getting feedback (or not)](#5-getting-feedback-or-not)
6. [Second walk-through: patch versioning](#6-second-walk-through-patch-versioning)
7. [Code, TEST, submit, feedback, repeat](#7-code-test-submit-feedback-repeat)

### 1. Understanding the submission process

The process to get changes accepted in the mainline kernel is well known and pretty simple once you learn the few steps involved. On the other hand, it is not a state-of-the-art process, and it is not what new generations are used to. That is actually a common critic, especially among young developers. But in the end it has been around for decades with obvious success (just take a look at the number of contributors involved in every release) and it does not seem that it will change any soon. It is it what it is and the sooner you get used to it, the sooner you will be able to get your changes applied.

In a nutshell, the submission process consists of the following steps:

 1. <u>Patch formatting:</u> your changes should consist of a series of git commits, which must be sent via email in a well-defined format. Every commit will become an email, and you will end up with a **series** covering a specific **topic**. If the series contains multiple patches, you will add a **cover letter** (an informative patch to present your series). For a single patch it is not required because the patch should be self explanatory.
 2. <u>Series submission:</u> your series has to be sent upstream, i.e. you will send your emails to a public **mailing list** where everyone will have the chance to review your changes, propose modifications or ask for missing information. The whole communication runs via public emails. <span style="color:green"><b>Hint:</b></span> always 'reply to all'.
 3. <u>Review and feedback:</u> usually you will get feedback from one or more **reviewers,** either to propose modifications and improvements or to give an OK to your patches. It is perfectly normal that some patches of the series are accepted without further discussion and some others require a new version.
 4. <u>Series versioning:</u> if any of the patches requires a new version, you will be expected to send a new version of the series. The patches that were ok should include the code-review trailers you received (typically *Reviewed-by*, sometimes *Acked-by*) and no further modifications, and the rest should include the modifications agreed in the previous discussions, which will have to be documented (more on that later).
 5. <u>Series accepted:</u> if the community and especially the subsystem **maintainer** considers that your patches are good to be merged, you will (not always, but almost) receive an email notifying that they were applied to some branch. That branch will not be the master branch of the mainline kernel for a number of reasons, and your patches will be applied with many others from other developers during the **merge window** to the mainline kernel if Linus Torvalds is willing to accept them.
 6. <u>Patience:</u> in principle, the rest of the process will be transparent to you and will follow the general rules and timings of the kernel life cycle. Annoying the maintainers asking if everything is alright with the patches they accepted from you is obviously not a good idea. Just wait and do whatever you usually do in your daily life. If you annoy maintainers in your daily life, this might help (them): `sudo rm -rf /`

## 2. Warning: only plain text without frills

One of the things that really annoys people is receiving emails with attachments, fancy signatures and in HTML format. Actually, anything beyond a 72-character line, plain-text email will annoy someone subscribed to the mailing list. And they will probably let you know.

You might think that they are overreacting, and you might be right. You might be sending your patches with a company email that automatically adds a signature or HTML content. I think people should be more patient with new contributors, but that is not always the case and don't be surprised if you get a *not so friendly* reply. Most people are polite and will ask you to check your settings, but don't take that for granted.

If that ever happens and your email does not meet the standards, even if you were sure it did before you sent it, keep calm and do not let the discussion escalate. Check if you are really sending plain text and ask for confirmation from the IT department that nothing is added to your emails before they leave the company network.

`git send-email` will not add any HTML content, but your email service provider could use that format by default when you reply to comments in the mailing list (I use Thunderbird and the default setup is NOT what you want to have). Make sure your settings meet requirements and wrap your commit descriptions to meet the 72-char line rule.

I would recommend you to email a personal address outside the company before emailing a list to make sure that everything looks right. Back to basics and 1990 :grin:

### 3. git configuration

The steps to configure git to send emails with your own account will depend on your email service provider. Simply google "git send-email gmail/office365/gmx/whatever" and follow instructions. Usually, you will only need to add a few lines to your **.gitconfig**. Here's an example for office365:

```
[sendemail]
	smtpencryption = tls
	smtpserver = smtp.office365.com
	smtpuser = foo.bar@office365.com
	smtpserverport = 587
```

The configuration for gmail accounts is a bit more tedious because you also have to "allow" git to send emails with your account. I use gmail for my hobbyist-patches and I never had an issue, though.

### 4. First walk-through: formatting and sending patches

The basic tools to format your patches and send them upstream are `git format-patch` and `git send-email`. In my opinion there are more powerful tools that make life easier, and I never use the traditional approach. I use **b4** in my workflow, and I am very happy with it. I will try to convince you that it is way cooler than `git format-patch` and `git send-email` in another post... but for the sake of simplicity, here I will stick to the canonical workflow.

Let's go through a silly example that you can try yourself.

I want to update the kernel *EXTRAVERSION* and *NAME*. You will find these two variables at the beginning of the **Makefile** in the root directory:

```
# SPDX-License-Identifier: GPL-2.0
VERSION = 6
PATCHLEVEL = 7
SUBLEVEL = 0
EXTRAVERSION = -rc8
NAME = Hurr durr I'ma ninja sloth
```

My *EXTRAVERSION* will be <span style="color:green"><i>-rc1</i></span> and the *NAME* will be <span style="color:green"><i>wannabe</i></span>. I will commit the changes separately, so we have an extremely simple **series**:

<figure>
    <img src="/images/posts/2024-01-05-kernel-contributor-3/dummy-series.jpg"
         alt="dummy series">
    <figcaption><i>If you ever send anything like that upstream, a lightning bolt will strike you down.</i></figcaption>
</figure>

Now my **Makefile** looks like this:

```
# SPDX-License-Identifier: GPL-2.0
VERSION = 6
PATCHLEVEL = 7
SUBLEVEL = 0
EXTRAVERSION = -rc1
NAME = wannabeee
```

If you found the bug, good catch... there will be consequences later.

I am happy with my changes and I want to send them upstream because I believe they will help the kernel grow. But first I need to format the patches and add a **cover letter** to introduce my series:

```properties
$ git format-patch --cover-letter \
	--subject-prefix="PATCH" \
	-M HEAD~2 \
	-o patchwork/dummy-kernel/v1/
patchwork/dummy-kernel/v1/0000-cover-letter.patch
patchwork/dummy-kernel/v1/0001-Makefile-update-EXTRAVERSION.patch
patchwork/dummy-kernel/v1/0002-Makefile-update-NAME.patch
```

Are the arguments not clear?
 - cover-letter: create a cover letter (the *0000-* patch, which will not be applied upstream for obvious reasons).
 - subject-prefix="PATCH": the prefix added to the subject of every patch/email. It is the first version, so "PATCH" is enough.
 - -M HEAD~2: the last two commits from HEAD.
 - -o *some_directory*: optional parameter to specify where the patches will be saved. I like having a directory for all my patches, so I can find them fast if I want to apply them somewhere.

As you can see, `git format-patch` created three patches: the cover letter and the two changes I made. Now I can open the cover letter and edit the **subject** and **description**:

```sh
From af9107409bf7b61fbb8b438e26069759eda81b70 Mon Sep 17 00:00:00 2001
From: Javier Carrasco <javier.carrasco.cruz@gmail.com>
Date: Thu, 4 Jan 2024 02:16:24 +0100
Subject: [PATCH 0/2] *** SUBJECT HERE ***

*** BLURB HERE ***

Javier Carrasco (2):
  Makefile: update EXTRAVERSION
  Makefile: update NAME

 Makefile | 4 ++--
 1 file changed, 2 insertions(+), 2 deletions(-)

--
2.39.2
```

Let's check that my patches don't have obvious errors:

```sh
$ ./scripts/checkpatch.pl --strict patchwork/dummy-kernel/v1/*
```
```properties
-------------------------------------------------
patchwork/dummy-kernel/v1/0000-cover-letter.patch
-------------------------------------------------
total: 0 errors, 0 warnings, 0 checks, 0 lines checked

patchwork/dummy-kernel/v1/0000-cover-letter.patch has no obvious style problems and is ready for submission.
-----------------------------------------------------------------
patchwork/dummy-kernel/v1/0001-Makefile-update-EXTRAVERSION.patch
-----------------------------------------------------------------
WARNING: Missing commit description - Add an appropriate one

ERROR: Missing Signed-off-by: line(s)

total: 1 errors, 1 warnings, 0 checks, 8 lines checked

NOTE: For some of the reported defects, checkpatch may be able to
      mechanically convert to the typical style using --fix or --fix-inplace.

patchwork/dummy-kernel/v1/0001-Makefile-update-EXTRAVERSION.patch has style problems, please review.
---------------------------------------------------------
patchwork/dummy-kernel/v1/0002-Makefile-update-NAME.patch
---------------------------------------------------------
WARNING: Missing commit description - Add an appropriate one

ERROR: Missing Signed-off-by: line(s)

total: 1 errors, 1 warnings, 0 checks, 8 lines checked

NOTE: For some of the reported defects, checkpatch may be able to
      mechanically convert to the typical style using --fix or --fix-inplace.

patchwork/dummy-kernel/v1/0002-Makefile-update-NAME.patch has style problems, please review.

NOTE: If any of the errors are false positives, please report
      them to the maintainer, see CHECKPATCH in MAINTAINERS.
```

Oops! If forgot to [sign my work](https://www.kernel.org/doc/html/latest/process/submitting-patches.html?highlight=signed%20off#sign-your-work-the-developer-s-certificate-of-origin) and add a **commit description**, and both are **mandatory**. Use the following command to amend a commit and add the *Signed-off-by:*:

`git commit --amend --signoff`

If you only forgot the sign-off for all your commits, this is a handy command to add it automatically:

`git rebase --signoff HEAD~N` where N is the number of commits.

Cool! Let's send them upstream... but how do I know the email addresses I need? There is a script for that too:

```sh
$ ./scripts/get_maintainer.pl --separator=, --no-rolestats \
patchwork/dummy-kernel/v1/*.patch
```
The `--separator=,` parameter prints all addresses in one line, which is nice to copy them for the next step. `--no-rolestats` gets rid of additional information you don't care about, and the last argument is the directory where the patches were saved.

Ready to submit and wait for feedback:

```sh
git send-email --dry-run \
--to=linux-kbuild@vger.kernel.org,\
linux-kernel@vger.kernel.org \
--cc="Masahiro Yamada <masahiroy@kernel.org>,\
Nathan Chancellor <nathan@kernel.org>,\
Nicolas Schier <nicolas@fjasle.eu>" patchwork/dummy-kernel/v1
```

Note that I used the `--dry-run` argument to simulate the action instead of ruining my career as a Linux kernel developer. If you followed this example, please don't forget `--dry-run`. I would recommend you to always try first with `--dry-run` to make sure that everything is fine (or even send the series to your personal address before populating the command with the real addresses), and then send the series upstream by removing that argument in the next call.

I added the address I got from `get_mainainer` and as you can see, I passed the mailing lists to the `--to` argument and the personal addresses to the `--cc` argument. Actually, there is no reason why you should not do it the other way round (many people do). Just double check that you don't forget any address.

The last parameter is, once again, the directory where you saved the patches.

### 5. Getting feedback (or not)

In theory, you should get some feedback within two weeks, but that varies a lot depending on the subsystem and how busy the reviewers and maintainers are. That feedback will consist of public replies (i.e. to you and to the involved mailing list(s)) with either inline comments or summaries and code-review trailers at the bottom. Your replies should follow that pattern as well, removing irrelevant paragraphs in long emails to avoid that your comments are overlooked.

If the feedback you get asks you to modify your patches, feel free to discuss if you disagree or require additional information. That is always better than sending an unnecessary additional version. On the other hand, if you get a trailer like <span style="color:blue">Reviewed-by: somename &lt;some email&gt;</span>, your patch would be considered good to go. Sometimes a second reviewer finds something that is not completely right, and the first review might not be relevant anymore, so be ready for that as well. But if that does not happen, you can add the trailer to the next version of the patch if the series does not get accepted as it is. Obviously, if the series gets accepted as it is, you do not need to send a new version with the trailers you got and no further changes. In that case, you are basically done.

If you do not get any reply within two weeks (the merge window is a special period of time when you should be more patient, though), do not send your series again. Just send an email replying to your series as a ping. A simple "Gentle ping" message should be enough.

<u>One reminder that could save your life:</u> I have already mentioned that you **could** get some abrupt reply you did not expect. <span style="color:red"><b>NEVER FIRE BACK!</b></span> There is nothing to win and much to lose, especially because you are a newbie that might have made an obvious mistake, something that is documented, and you should know, but ignored. In the end, no one knows you, you are a newcomer with no experience in the kernel and simply nothing to back you up.
If you feel offended, just keep your hands away from the keyboard, drink some water and stay calm. Make sure you did not mess up (you probably did) and always reply politely, but keeping your point if you think you are right. Some maintainers are under stress and pressure (you are involved in a couple of patches, not in hundreds like some of them), and they get fed up with repeating the same things over and over again. Maybe they had a bad day... Who cares, stay focused and work on your next version. You are here to collaborate, not to argue. Even the *not-so-friendly* maintainers are fair, and if your next version is good and useful, they will eventually send you an **applied, thanks**.

### 6. Second walk-through: patch versioning

If your changes are not very simple, you will probably not find the perfect solution that everyone needs right away. There might be use cases you did not consider, more efficient solutions, or just errors in the code you missed. No matter the reason, you may need to modify your changes and send a new version upstream. In that case, there are a couple of things you will have to take into account:

 - The patch title must reference its current version. The **PATCH X/Y** tag should turn into **PATCH v2 X/Y** for every patch of the series, and so on.
 - You are expected to provide a list of the modifications. People want to know what changed and also if you addressed the feedback you got.
 -  Read the reviews carefully. From the previous point, it should be clear that you should address all the points from the reviewers, not just a few.

Let's continue with our little example. The series has been reviewed, and **miraculously** the *EXTRAVERSION* was considered good and a reviewer replied with the following trailer:
`Reviewed-by: Dummy Reviewer <dummy-reviewer@foo.com>`
We were not that lucky with the *NAME* and the reviewer found a mistake: <span style="color:brown">wannabeee</span> instead of <span style="color:green">wannabe</span>. We will have to send a new version.

First, fix the error and **amend the commits**: one with the fix, the other with the trailer. For the `Reviewed-by` trailer, you just have to paste it **under** your *Signed-off-by* trailer.  <span style="color:red">DO NOT</span> add new commits, only amend them.


Now a new patch series can be created. Second version = v2 (third = v3, etc.)

```properties
$ git format-patch --cover-letter \
	--subject-prefix="PATCH v2" \
	-M HEAD~2 \
	-o patchwork/dummy-kernel/v2/
patchwork/dummy-kernel/v2/0000-cover-letter.patch
patchwork/dummy-kernel/v2/0001-Makefile-update-EXTRAVERSION.patch
patchwork/dummy-kernel/v2/0002-Makefile-update-NAME.patch
```
You can recycle the content of your first cover letter and add a new section with your modifications. Many people add per-patch modifications, but I prefer having all together in the cover letter like **b4** does. Both are equally accepted, so do whatever you prefer. The standard approach is adding your changes under the `---` separator right below your *Signed-off-by*.

The new cover letter looks like this:
```properties
From af9107409bf7b61fbb8b438e26069759eda81b70 Mon Sep 17 00:00:00 2001
From: Javier Carrasco <javier.carrasco.cruz@gmail.com>
Date: Thu, 4 Jan 2024 20:25:01 +0100
Subject: [PATCH v2 0/2] Makefile: pretend I am Linus Torvalds

This series updates the EXTRAVERSION and NAME so I can have my custom
kernel, which I want to share with everyone for the sake of it. Enjoy!

Signed-off-by: Javier Carrasco <javier.carrasco.cruz@gmail.com>
---
Changes in v2:
 - Makefile: fix NAME.

Javier Carrasco (2):
  Makefile: update EXTRAVERSION
  Makefile: update NAME

 Makefile | 4 ++--
 1 file changed, 2 insertions(+), 2 deletions(-)

--
2.39.2
```

If you have to send a v3 later, make sure that you add a 'Changes in v3' right above the 'Changes in v2', so everyone can see all changes, but the latest first.

Alright, the patches have been double checked, (hopfeully) no new errors were introduced, and a new `git send-email` will send them upstream. I will keep the `--dry-run` in case you are copying and pasting without paying attention, though...

```sh
git send-email --dry-run \
--to=linux-kbuild@vger.kernel.org,\
linux-kernel@vger.kernel.org \
--cc="Masahiro Yamada <masahiroy@kernel.org>,\
Nathan Chancellor <nathan@kernel.org>,\
Nicolas Schier <nicolas@fjasle.eu>" patchwork/dummy-kernel/v2
```

Will the series finally make it to the mainline kernel? Fingers crossed!

### 7. Code, TEST, submit, feedback, repeat

There is not much left to say about the general submission process. The steps we went through will be the same for every iteration i.e. version until your patches get eventually accepted.

If nothing strange happens and your changes are merged into the mainline kernel during the next merge window, a few weeks later your contribution will be available with the next kernel version. Congrats, now you can call yourself a Linux kernel contributor.

Stay tuned and share knowledge!
