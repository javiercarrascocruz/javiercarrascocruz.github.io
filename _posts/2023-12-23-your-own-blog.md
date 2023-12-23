---
layout:     post
title:      A free blog within minutes with GitHub Pages
date:       2023-12-23 18:00:00
summary:    Don't reinvent the wheel and spend your time writing good posts
permalink:  /:title
tags: blog foss github-page
---

Welcome to my very first post. I believe in knowledge sharing and I am not planning to keep any secrets, so I will start by showing you how to publish your own blog like this one with zero knowledge, no money and almost no time.

Well, I lied about the zero knowledge... This post will be especially interesting for people who want to make the most of their regular workflow with **git** and build a blog like any other project they know. Therefore, I will assume that you know git and have a minimal understanding of what branches and repos are. I know there are other ways to get a blog live without git, but this is not the right blog form them.

For this tutorial you will only need **git** and a **GitHub** account. Eventually you might want to have **jekyll** and **bundle** installed, but the core stuff can be achieved without it.

I know absolutely nothing about web design and to be honest that is not a topic I am willing to learn. It is just way too far up the development stack! But I needed an easy, medium-quality solution that would give me the chance to write posts right away. Fortunately I found one that suits my needs.

Enough talk, let's get on with it. Just follow these steps and in a few minutes you will have your own blog online. If then you want to get deeper and learn more advanced stuff, you will have a working blog to play around.

### 1. Fork a decent repository

The goal is to avoid that you reinvent the wheel if you don't want to. Please close any tutorial that shows you how to program a blog from a scratch and stick with me. You might have noticed that many blogs look rather similar: they are just forks of the themes that look good and that is exactly what you need. This blog is simply a fork of [pixyll](https://github.com/johno/pixyll) with a few minimal customizations. If this theme suits your needs, fork the repo and if it does not, search for another one on GitHub and fork it.

![Fork](/images/posts/2023-12-23-your-own-blog/fork.png)

For this post I will use [minima](https://github.com/jekyll/minima) (Jekyll's default theme) as an example. The workflow is always the same.

Now let's talk briefly about the fork name and its relation to the blog URL.

### 2. GitHub Pages: choose your URL

GitHub offers free hosting for pages of your repositories. The trick is that your URL will contain 'github.io' in it unless you have a custom domain. But if you can live with that, let's go on.

The name of the fork will be *username.github.io*, where *username* is **obviously** what you would expect: your username on GitHub.

The URL of your blog will be *https://your_github_user_name.github.io/*.

Pretty simple, right? And if you choose another name, your URL will end up being the one I told you + */the_name_you_chose/*.

Two examples: if your username is **foo** and you name the fork *foo.github.io*, your URL will be:

<span style="background-color:blue; color:white">*https://foo.github.io/*</span>

If you name the fork **bar** instead, your URL will be:

<span style="background-color:blue; color:white">*https://foo.github.io/bar/*</span>

As I used the first approach for my real blog, this time I will use the second one:

![Fork-Name](/images/posts/2023-12-23-your-own-blog/fork-name.png)

Easy question: what URL will I get for the new blog? If your answer is not *https://javiercarrascocruz.github.io/test*, please read this section again.


### 3. Select the branch to deploy

If you followed the steps, by now you should have something like this:

![Test-Repo](/images/posts/2023-12-23-your-own-blog/test-repo.png)

We still need to tell GitHub what branch we want to deploy i.e. what branch will contain the blog we want to upload.

I am using a second branch called **deploy** to keep **master** synchronized with the original repo, but that is just a preference and you can use **master** if you like.

Simply click on **Settings** and then on **Pages**. You will then see a menu to select the branch you will be deploying:

![Pages](/images/posts/2023-12-23-your-own-blog/pages.png)

Save and wait until the page is built (you will see the following message: *Your GitHub Pages site is currently being built from the master branch*). Refresh the site and eventually you will see something like this:

![Live](/images/posts/2023-12-23-your-own-blog/live.png)

Now that I think about it, the minima theme is actually great... but I can't be bothered with changing my theme :grin:

Believe it or not, we are almost done and your blog is already live.

### 4. Clone the repo and customize it

Yep, you have reached the last step. You only need to remove the defaults from the template and add your own name, title and posts.

So let's clone the repo and start customizing it.

If you have a look at your boilerplate blog, you will see that there is already some basic structure and a couple of posts that you could recycle. The most general information is in a file named ***_config.yml***. The fields in it are self-explainatory, so you only need to write basic stuff like your name and email address. If you want to have social-media icons, the (surprise!) *show_social_icons* must be set to true. In general, true means that you want to activate a given property, whereas false means... that you don't. Enter your social usernames for the social networks you want to include.

For example: linkedin_username: javiercarrascocruz adds a LinkedIn symbol with a link to my LinkedIn account.

You will also have some sample posts in the ***_posts*** directory. You can either add your own files to the directory or even faster, edit one of the existing files your own titles and text. Just don't forget to update the date. For your second post you can just copy your first one and use it as a template.

Similarly, you can customize the 'About me' section by editing the ***about.md*** file and adding your own bio. When you think you are done, commit your changes and push them. Shortly after your blog will be updated.

Now you are good to go. Of course, you can customize many other things and get as deep as you want, but having a nice-looking blog in a few minutes is pretty cool, right?

### Bonus: recycle code from others

If you find any interesting structure, font, color combination or anything else you would like to use for your posts, just have a look at the code of the post and get whatever you need. If there is anything you would like to mimic from my own blog, just have a look at my repo and copy what you need. You could even fork my repo and change the name, up to you!

That's it. Enjoy your personal blog and stay tuned.
