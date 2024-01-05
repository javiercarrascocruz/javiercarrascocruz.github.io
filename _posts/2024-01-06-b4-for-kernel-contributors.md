---
layout:     post
title:      b4 for Linux kernel contributors
date:       2024-01-06 10:00:00
summary:    An awesome tool to speed up the submission process
tags:       b4 contributor foss kernel linux
---

[b4](https://github.com/mricon/b4) is a free and open-source tool I use every day as a Linux kernel contributor. It has made my workflow more consistent and efficient, which I value a lot. Even though I send patches to the kernel mailing lists regularly, it has been long since the last time I called `git format-patch` and `git send-email`. Still reluctant? Here I will try to convince you.

#### Content:

1. [Are git format-patch and git send-email not good enough?](#1-are-git-format-patch-and-git-send-email-not-good-enough)
2. [Installing b4](#2-installing-b4)
3. [A simple example to convince you](#3-a-simple-example-to-convince-you)
4. [Is that all b4 has to offer?](#4-is-that-all-b4-has-to-offer)

### 1. Are git format-patch and git send-email not good enough?

Of course, they are. Actually, **b4** uses `git format-patch` and `git send-email` heavily, often acting as a wrapper with some extra features. So even if you switch to b4, you will still be using the same tools behind the scenes.

A simple call to populate the CC and CCo lists or collect all code-review trailers from the mailing lists is a feature I want to have, but maybe you can live without. Do you like versioning your series yourself? Have fun, but personally I'd rather have some tool that does it for me. And those are only a few examples.

I still think that every kernel contributor should know how to use the canonical tools in case they ever need them, but that does not mean that you should never evolve.

### 2. Installing b4

There are several ways to install b4 and they are very well documented [here](https://b4.docs.kernel.org/en/latest/installing.html). I personally like cloning the [repo](https://github.com/mricon/b4) and creating a link to *b4.sh* (described in the "[Running from the checkout dir](https://b4.docs.kernel.org/en/latest/installing.html#running-from-the-checkout-dir)" section) to get the latest version. Your distro probably has **b4** as a package and it can be installed normally, but you might get an old version without the latest features. If you can live without them, that is the easiest way.

Given that b4 uses `git send-patch` under the hood, if you have already configured git to send emails, everything should be ready to send patches with b4. If you don't even know what I am talking about, google first how to configure `git send-email` for your email service provided (Gmail, Outlook, etc.).

### 3. A simple example to convince you

Let's work on a silly series with some dummy patches that you can easily reproduce, from creating a branch to submitting the series. Obviously I am not going to **send** the silly series to a mailing list; b4 can **reflect** emails, so they will only be sent to myself even though b4 will populate the CC and CCo lists with the right addresses. If you follow this example and forget the `--reflect` argument... well, then you are a fool. But still, cancel the process when b4 asks for confirmation and give yourself a second chance... you fool.

First, I am going to create a new branch for my series:

```properties
$ b4 prep -n kernel-name-extraversion

Created new branch b4/kernel-name-extraversion
Created the default cover letter, you can edit with --edit-cover.
```

b4 automatically created a branch with a first commit: the cover letter. Let's take a look with `b4 prep --edit-cover`:

```properties
$ b4 prep --edit-cover

EDITME: cover title for kernel-name-extraversion

# Describe the purpose of this series. The information you put here
# will be used by the project maintainer to make a decision whether
# your patches should be reviewed, and in what priority order. Please be
# very detailed and link to any relevant discussions or sites that the
# maintainer can review to better understand your proposed changes. If you
# only have a single patch in your series, the contents of the cover
# letter will be appended to the "under-the-cut" portion of the patch.

# Lines starting with # will be removed from the cover letter. You can
# use them to add notes or reminders to yourself. If you want to use
# markdown headers in your cover letter, start the line with ">#".

# You can add trailers to the cover letter. Any email addresses found in
# these trailers will be added to the addresses specified/generated
# during the b4 send stage. You can also run "b4 prep --auto-to-cc" to
# auto-populate the To: and Cc: trailers based on the code being
# modified.

Signed-off-by: Javier Carrasco <javier.carrasco.cruz@gmail.com>
```

I don't need to explain the content, because b4 does it pretty well. It even added a *Signed-off-by* to the cover letter. Is b4 not beginner-friendly? It will even refuse to send the series if you don't edit the cover letter :smiley:

Now let's introduce some changes. If you followed the example from the [third episode](/2024/01/06/kernel-contributor-3) about how to become a Linux kernel contributor, this will look familiar:

![dummy-series](/images/posts/2024-01-06-b4-for-kernel-contributors/dummy-series.jpg)

Now that I added some dummy changes, b4 can populate the CC and CCo lists with the relevant email addresses (call it automated `get_maintainer.pl`):

```properties
$ b4 prep -c

Will collect To: addresses using get_maintainer.pl
Will collect Cc: addresses using get_maintainer.pl
Collecting To/Cc addresses
    + To: Masahiro Yamada <masahiroy@kernel.org>
    + To: Nathan Chancellor <nathan@kernel.org>
    + To: Nicolas Schier <nicolas@fjasle.eu>
    + Cc:  <linux-kbuild@vger.kernel.org>
    + Cc:  <linux-kernel@vger.kernel.org>
---
You can trim/expand this list with: b4 prep --edit-cover
Invoking git-filter-repo to update the cover letter.
New history written in 0.15 seconds...
Completely finished after 0.50 seconds.
```

If you like, you can add addresses to the lists by editing the cover letter again. Oh, sorry, b4 already told you!

What b4 can't do for you is writing the commit title and description or add the trailers you required (at least *Signed-off-by*, maybe some more). But there had to be something you could do yourself, right? :grin:

Alright, let's send the series upstream! But first I will save my patches locally in a directory I <u>previously</u> created:

```properties
$ b4 send -o patchwork/fool/v1

Converted the branch to 3 messages
Will write out messages into patchwork/fool/v1
  0000-makefile-update-name-and-extraversion.eml
  0001-makefile-update-name.eml
  0002-makefile-update-extraversion.eml
---
DRYRUN: Would have sent 3 messages
```

Hmm... What if I overlooked something? I will send the patches only to my email address to make sure everything looks fine:

```
$ b4 send --reflect

Converted the branch to 3 messages
---
To: Masahiro Yamada <masahiroy@kernel.org>
    Nathan Chancellor <nathan@kernel.org>
    Nicolas Schier <nicolas@fjasle.eu>
Cc: linux-kbuild@vger.kernel.org
    linux-kernel@vger.kernel.org
    Javier Carrasco <javier.carrasco.cruz@gmail.com>
---
  [PATCH 0/2] Makefile: update NAME and EXTRAVERSION
  [PATCH 1/2] Makefile: update NAME
  [PATCH 2/2] Makefile: update EXTRAVERSION
---
Ready to:
  - send the above messages to just javier.carrasco.cruz@gmail.com (REFLECT MODE)
  - with envelope-from: javier.carrasco.cruz@gmail.com
  - via SMTP server smtp.gmail.com

REFLECT MODE:
    The To: and Cc: headers will be fully populated, but the only
    address given to the mail server for actual delivery will be
    javier.carrasco.cruz@gmail.com

    Addresses in To: and Cc: headers will NOT receive this series.

Press Enter to proceed or Ctrl-C to abort
Connecting to smtp.gmail.com:587
---
  [PATCH 0/2] Makefile: update NAME and EXTRAVERSION
  [PATCH 1/2] Makefile: update NAME
  [PATCH 2/2] Makefile: update EXTRAVERSION
---
Reflected 3 messages
```

Let's open Thunderbird and see if the series was sent to me, and only to me...

![thunderbird-series](/images/posts/2024-01-06-b4-for-kernel-contributors/thunderbird-series.jpg)
![thunderbird-cover-letter](/images/posts/2024-01-06-b4-for-kernel-contributors/thunderbird-cover-letter.jpg)

Awesome! I can even see all email addresses, even though they were only sent to me. If you are subscribed to a mailing list where the series would have been sent to, you will see that the series (fortunately and of course) does not show up.

Ready to go (<span style="color:red"><b>don't do this</b></span> with my silly example!):

`b4 send`

Just in case, b4 has updated the series version and added a new section to list the changes. That will not happen until you **actually** send the series upstream, so here I will show you a real example from one of my series. In this case I have already sent v2 and b4 automatically prepared the series for v3. You will also find a link to the previous version below the changes:

```properties
Signed-off-by: Javier Carrasco <javier.carrasco.cruz@gmail.com>

---
Changes in v3:
- EDITME: describe what is new in this series revision.
- EDITME: use bulletpoints and terse descriptions.
- Link to v2: https://lore.kernel.org/r/20240103-as7331-v2-0-6f0ad05e0482@gmail.com

Changes in v2:
- as73211.c: Use IIO_VAL_FRACTIONAL to retrieve scales of AS73211.
- as73211.c: simplify device-specific data retrieval in probe function.
- as73211.c: minor coding-style fix (shorter line).
- Link to v1: https://lore.kernel.org/r/20231220-as7331-v1-0-745b73c27703@gmail.com
```

We are not going to find an awful reviewer who gives us a *Reviewed-by* trailer to these patches, but fortunately I received some for one of my series lately, and I can show you the output of a single command that will retrieve all trailers from the mailing lists:

```properties
$ b4 trailers -u

Calculating patch-ids from commits, this may take a moment...
Checking change-id "20230510-feature-ts_virtobj_patch-e267540aae74"
Grabbing search results from lore.kernel.org
Looking for additional code-review trailers on lore.kernel.org
---
  dt-bindings: touchscreen: add touch-overlay property
    + Reviewed-by: Jeff LaBundy <jeff@labundy.com> (✓ DKIM/netorg5796793.onmicrosoft.com)
    + Reviewed-by: Rob Herring <robh@kernel.org> (✓ DKIM/kernel.org)
  dt-bindings: input: touchscreen: st1232: add touch-overlay example
    + Reviewed-by: Rob Herring <robh@kernel.org> (✓ DKIM/kernel.org)
---
Invoking git-filter-repo to update trailers.
New history written in 0.07 seconds...
Completely finished after 0.43 seconds.
Trailers updated.
```

As simple as that, the trailers were found and applied to the patches. Smooth as silk.

### 4. Is that all b4 has to offer?

No, it is not. Here I only showed you some of its features for contributors, the ones that you will be using over and over again. And not to mention that aside from the contributor perspective, b4 also supports many features to make maintainers' lives easier. If you take a look at the b4 project contributors, you might find some names you already know from the Linux kernel, because they are well-known maintainers. The maintainer's perspective is out of the reach of this post (i.e. I am not a maintainer, yet I use b4 to apply patches), but as you can imagine, it simplifies some otherwise tedious tasks.

The project is actively maintained, and new features appear regularly. One recent addition I really like and I wish I have had when I collaborated to write the [hdc302x driver](https://lore.kernel.org/all/20231211193950.7d4d012f@jic23-huawei/) is [\-\-dry-run-to](https://github.com/mricon/b4/commit/4e03211130d5afbf26297bee8b549013fc30e6fc), **recently renamed to [\-\-preview-to](https://github.com/mricon/b4/commit/ba4a25a4baf8a2773f8ff3216fd56dff63ca42ff)** to send series to a specific email address. I bet there are loads of features I don't know yet and some of them might be relevant to you, so keep exploring and let me know!
