---
layout:     post
categories: tech
title:      Smoothing out Communication in FOSS Projects
date:       2024-11-06 10:00:00 +0200
summary:    From newcomers to community leaders
permalink:  /:title
tags:       contributor foss
---

Most of the articles about Open Source (including mine) only focus on technical aspects: coding, statistics, results, etc. But as we are not machines (just yet),
I think that the human aspects involved in such projects should have some visibility as well. In the end, we are interacting with communities and their individuals,
who have their perspectives and communication skills.

What sets communication apart in FOSS projects is the potential number of people involved and the visibility that the conversations can achieve. As long as everything goes well, this communication is wonderful, and a great source of information for the entire community. But when it goes awry, and someone decides to deviate from the cordial and professional tone that one expects, such visibility can lead to someone feeling intimidated, and even humiliated. Since none of us want these things to happen, I'd like to contribute my small share and discuss a few points that could help new as well as more established community members.

First, I would like to emphasize that at least 99.9% of the interactions in FOSS projects are smooth; usually polite and professional, and from time to time even
welcoming and friendly. If only the worst examples reach the public, it's just because they are exceptional... and because that's what sells :joy:
Although such cases are uncommon, one day you might find yourself being the target of someone's rage, or even worse, the one who put a community member
to shame.

I must also emphasize that I am not an expert in communication or personal skills. I am just above average when it comes to being friendly, talkative, and assertive. That might not
be saying much when you are comparing yourself with the average developer :wink: but it has been enough to successfully manage all conflicts I have faced without leaving
any "corpse" behind, and also support a couple of [LKMP](/lkmp) mentees who were a bit overwhelmed with some feedback they got from their first contributions to the Linux kernel. I even attended some Summer courses about soft skills at the university, and oh surprise, I was the only student from *any* engineering faculty.
That's an area where you won't see many techies showing up, although that is changing now that many big tech companies explicitly require such skills.

I will split this article into two sections: a first one for those who just joined a FOSS community, and a second one for experienced community members who might profit
from smoother discussions upstream.

**Content:**

* TOC
{:toc}

### 1. Newcomers

I am not going to show bad examples to keep them from getting publicity, but let's suppose you have sent a patch upstream with the best intentions, and you get a reply that you consider aggressive or even offensive. You might have never received such replies in your professional or academic interactions, and maybe they are almost impossible to happen where you come from. It should have never happened, but there it is. Let's see what you could/should do at this point.

#### 1.1 Don't take it personally

The person who sent you that is neither a relative nor a close friend. It's someone you probably don't know, and not someone who really matters in your life. And the other way round applies too: that person doesn't know you, and they are only replying to some (probably unknown) e-mail address. You are being criticized for some message/piece of work that does not define you personally or professionally. Of course, you could take it personally and believe you have been punched in the face, but your chin doesn't hurt, does it? Instead, you could take it for what it is: an overreaction that is out of place. As usual with virtual interactions, nonverbal communication is missing, there may have been some misinterpretations (not everyone's mother language is English!), and the sender might not even be aware of having gone *a bit* too far.

#### 1.2 Don't reply immediately

I know absolutely nothing about guns, but I have always heard that you should keep guns and ammo in different places, so you have time to think before you shoot. Well, we can apply the bottom line to immediate replies too. If you have the slightest feelings about a public message you just received, <u>DON'T REPLY JUST YET</u>. Put your email client in the background, keep on doing whatever you are doing, and give yourself time to think. Digest what you just read, and get back a few moments later with a more structured reply. Every comment you send to public forums/mailing lists will stay there for a long time, and you might regret later having replied in the heat of the moment.

#### 1.3. Be honest: did you mess up?

Beginners make a lot of mistakes for obvious reasons, and some of them annoy people because they are repetitive. The project documentation points them out, and you should have read about them before going live. Even in that case, a rude reply is not the way to go, but maybe you can still learn something new, or improve your workflow for the next attempt. If you messed up, consider adding a very short apology at the beginning of your next reply. It costs nothing, and then you can move back to the technical discussion. I have a good example of that: the typical HTML + company signature to a mailing list where only plain text is allowed.

I have been contributing to FOSS projects regularly with my professional email address for more than a year, and I made sure that our IT department keeps my emails untouched when they leave the company infrastructure. That worked well until last week I sent my very first patch to [libcamera](https://libcamera.org/), a very cool project where I am an utter beginner. To my surprise, my email reached the mailing list in a fully-featured HTML fashion and a huge signature letting everyone know that my company was joining some event. A filter had been modified, and my address had been removed from it. Of course, I couldn't have foreseen that, as it still worked a couple of days before. Not the best way to introduce yourself to an Open Source community, right? To be honest, I was expecting some unfriendly reply, but I got a polite comment letting me know what I had done wrong, and the rest of the discussion was only technical. I apologized for the trouble, made sure that it won't happen again (or at least, not any soon), and carried on discussing the technical aspects. Here's the [link to the discussion](https://lists.libcamera.org/pipermail/libcamera-devel/2024-October/046159.html).

If you, on the other hand, get criticized after making a technical mistake like giving a completely wrong solution to an issue, don't be too hard on yourself. We all make mistakes, and if you are a beginner, many community members understand things better than you because they have the background and experience you are still missing. Take it as an opportunity to learn from people who know much more than you, and get ready to do better next time. Abandoning the community with your tail between your legs is the worst approach you could take.

#### 1.3. Gather the technical content

Get back to the email and check if there is relevant feedback about the topic you are interested in. If there is none, and keeping the previous section in mind, you can simply ask the emitter to stick to the technical discussion, or ignore such a pointless message. But maybe there is something useful that you could use to turn the discussion in the direction you want, or even improve your original message or piece of code.

It's time to leave the non-technical comments behind: stop tormenting yourself, gather the valuable information and try to work out how that could fit into a better technical solution.
#### 1.4. Neutral, polite, and focused reply

Once you have decided to reply and what you actually want to reply, try to set an example by replying politely and focused on the technical aspects of the matter. The emitter might be a key stakeholder like a project maintainer, and you want to take the discussion back to the issue you are addressing, keeping all doors open for your current contribution and any others you might come up in the future. Read the Code of Conduct of the project (for example, the Linux Kernel Contributor Covenant Code of Conduct [https://docs.kernel.org/process/code-of-conduct.html]), and try to follow the recommendations, which are usually just common sense.

I would recommend you to leave the offending bits of the message you received uncommented unless there is a good reason to address them. Otherwise, the discussion could get stuck at the points you wanted to leave behind.

#### 1.5 It went too far

If the feedback you got clearly violates the Code of Conduct, you are entitled to report it to the project maintainers. Of course, you should not let pass any flagrant abuse. But such cases are really, really rare, so make sure that you are definitely facing one of them first. Usually, the critiques come in the form of questioning your technical skills, your commitment to the project, and similar things that are unnecessary, but not disobeying the Code of Conduct... or at least not so clearly that you should report them.

Keep in mind that FOSS projects are also open for trolls and fake accounts. If you get 'trolled' (e.g. by fake-looking accounts or users with 0 contributions), don't waste any time replying: just ignore the message and move on.

### 2. Community Leaders

No one is going to question that you are an expert in your area, maybe a truly master with unbelievable technical abilities. But now that you have reached that point on the technical side, why don't you make the most of your interactions with FOSS Communities to improve your soft skills? Believe it or not, they could boost your professional career even further, and they will make you reach your technical goals more easily. Such skills only require some practice, which you will have in the discussions upstream anyway, and their principles are way easier to learn than a new programming language or technology. Even for a geek like you :wink:

#### 2.1. Follow "Good Practices"

It goes without saying that you should follow the Code of Conduct of your project, if there is any. If there is not, it's never too late to implement one! But let's suppose there is one. There is usually a list of things that you **should not** do, and you are probably following those. That's great, but often there is also a section about positive standards that are highly recommended. And that section does not have so many followers :laughing: Once again, let's take the Linux kernel documentation as an example:

```
Examples of behavior that contributes to creating a positive environment include:

    Using welcoming and inclusive language

    Being respectful of differing viewpoints and experiences

    Gracefully accepting constructive criticism

    Focusing on what is best for the community

    Showing empathy towards other community members
```

Would you relate? Awesome! Otherwise, you might have found some room for improvement in your interpersonal skills. You wouldn't believe how much better your comment will look if it starts with a "Welcome to the community" instead of "Your patch sucks" right before addressing the issues. Not only that, you will be setting better standards for the rest of the community, as people often mimic what they see from more experienced members.

#### 2.2. You were once a noob, remember?

Maybe it was long ago, but hopefully you still remember when you joined your first FOSS project. How many beginner mistakes did you make until you got where you are now? Did everyone shout at you every time you messed up? Well, now you have a great opportunity to offer a better experience to the new generations. Not just because it will motivate newcomers to stay around and eventually improve your beloved project, but also because being nice to people is a good thing on its own. Now you that you are on the other side, probably in a privileged position within the community, you can help the community grow by making new members feel welcome.

Personally, I don't get offended easily, and unpleasant comments just roll off my back. But every person has different sensitivities, and there is no need to make anyone feel uncomfortable, as if that would make them learn faster. No, it doesn't, they will learn from polite and professional feedback just the same. I have had one case from the LKMP where a mentee would cross out some subsystem because the feedback was always too harsh, and the very same mentee had no problems in other subsystems. Don't miss helping hands!

#### 2.3. Share knowledge

If you really think that someone needs a "punishment", why don't you lecture them by showing them how things really are? Think of that as an investment: that person will have some solid ground to work on a better approach instead of sending you "crap" again, and everyone who will read your public comment will send you better stuff.

I have a couple of good examples for you, from two Linux kernel maintainers I had never interacted with:

  - [This one](https://lore.kernel.org/all/202406261121.2FFD65647@keescook/) where I got lectured about some memory allocation by Kees Cook, explaining why my patch was a fix even though the bug could not be triggered.
  - [This one](https://lore.kernel.org/all/20240930202642.GA1497385@thelio-3990X/) where Nathan Chancellor dissected a misleading bug report from Clang about an unused variable (only used for a sizeof()).

As you can see, these well-known members provided detailed, technical-oriented replies, which helped me learn (I could reference Nathan's comment shortly after for a similar issue), and everyone can access in the future.

#### 2.4 Do you hate people? Automate replies

If you became a software developer to stay away from people and have zero interest in soft skills, that's absolutely respectable. A FOSS project is not a social club, and you might only be interested in the technology that it covers.

But why don't you optimize your time and let bots reply to anything that does not make sense to you? You can set filters, CI/CD pipelines, and combine them with automatic replies... a lot of cool stuff to keep humans out of the way until they come up with something useful. For example, many Linux kernel maintainers have implemented such facilities (not necessarily because they don't like people, but because it saves loads of time), and a neutral, automated reply is better than one from an angry human. A win/win situation!


That's it for today. If you would like to share some positive interactions in a FOSS community, please leave a comment!
