---
layout:     post
categories: tech
title:      Linux Drivers 3 - Regmap in Detail
date:       2025-02-10 18:00:00 +0100
summary:    Standard API to access registers with very cool features
permalink:  /:title
image:      /images/posts/2025-02-10-ldd3-regmap/ldd3-regmap.webp
tags:       drivers linux soc
---

This article is still only available on Patreon for **Early BYTE Explorers** [here](https://www.patreon.com/posts/linux-drivers-3-121950375?utm_medium=clipboard_copy&utm_source=copyLink&utm_campaign=postshare_creator&utm_content=join_link). If you would like to support my work and get early access to my articles, you can do so on my Patreon profile and start reading this article right now.

I’m fully committed to sharing knowledge with everyone, for free. All my articles will eventually be published for open access because I believe knowledge should be accessible to all. On the other hand, I would like to show my appreciation to my main supporters with in-depth, detailed articles that they can enjoy fresh off the press. That’s why some of my articles remain exclusive to Patreon for some time before being made available to the public.

<p>10% of the donations I receive are donated to free and open-source projects. Thanks to the support I've received, I've been able to cover some expenses for my bikepacking adventure while spreading FOSS along the way. But the content I create wouldn’t be possible without FOSS, and many of these projects are severely underfunded.</p>

<p>Even though 10% of what I get won’t even cover a basic web domain (and in many countries, not even a coffee! 😂), I'll do my best to give these projects visibility, hoping that, with a bit of luck, contributions will grow. I strongly believe in the FOSS community—not just in receiving, but also in giving back.</p>

If you can’t afford a subscription or simply aren’t interested, you can rest assured that the content will eventually be published with no restrictions. However, if you’d like to help me continue creating content, please consider more affordable options like the Byte Rider subscription or buying me a coffee through BuyMeaCoffee (check out [🌟 Support Me](/support)). Your contributions allow me publishing content regularly on this page, keeping it accessible for everyone.

Your support, in any form, means a lot to me!

<b><u>UPDATE:</u></b> The first article of the series has already been released, and it is available for everyone [here](/linux-drivers-hardware). I hope you will find it useful!
