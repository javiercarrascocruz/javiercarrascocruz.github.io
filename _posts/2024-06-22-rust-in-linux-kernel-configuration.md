---
layout:     post
title:      Rust in the Linux Kernel - Configuration
date:       2024-06-22 15:00:00 +0200
summary:    Step by step to success
permalink:  /:title
tags:       kernel linux rust
---

<center><figure>
    <img src="/images/posts/2024-06-22-rust-in-linux-kernel-configuration/rust-for-linux-logo.webp"
         alt="Rust for Linux logo">
</figure></center>

After spending some days learning how some compiler attributes (\_\_cleanup, \_\_counted\_by) work in the Linux kernel, and fixing multiple bugs by using them properly, I have decided to move on, and start learning how Rust is used in the Linux kernel.

I have to admit that I know very little Rust. I [completed the Rustlings](https://github.com/javiercarrascocruz/rustlings) almost a year ago, and since then I have been programming small userspace applications in Rust to learn the basics. But to be honest, that was only an intermediate step before applying Rust to lower-level stuff: microcontrollers and the Linux kernel. I will leave bare-metal stuff for another occasion, because it is a very interesting topic.

In this article, I am going to show you some very simple steps you can follow to configure Rust in the Linux kernel without stumbling upon version incompatibilities. The [official documentation](https://www.kernel.org/doc/html/next/rust/index.html) provides a detailed [Quick Start guide](https://www.kernel.org/doc/html/next/rust/quick-start.html), but it does not cover the installation of 3rd-party tools like LLVM/Clang and Rust itself. If you want to save some time, let me show you how you can get the right tools and versions without getting into too much trouble.

Most of the steps are distro-agnostic, but everything is mainly tailored for myself (Debian/Ubuntu). If you have a non-debian-based distro, simply install the latest builds wherever a script uses *apt* under the hood.  Let's get into it!

#### Content:

1. [Requirements](#1-requirements)
    - [1.1. LLVM/Clang](#11-llvmclang)
    - [1.2. Rust](#12-rust)
    - [1.3. bindgen](#13-bindgen)
    - [1.4. New worktree](#14-new-worktree)
2. [Configuration](#2-configuration)
3. [Build kernel and Rust documentation](#3-build-kernel-and-rust-documentation)
4. [Start playing around](#4-start-playing-around)

### 1. Requirements

I am going to assume that you have installed all requirements to build the Linux kernel without Rust. If you haven't, you are probably putting the cart before the horse. Anyway, please refer to the [official documentation](https://docs.kernel.org/process/changes.html).


#### 1.1. LLVM/Clang

The official documentation states that the LLVM toolchain is better supported than GCC. I have transitioned to LLVM/Clang as my default compiler long time ago, but let's assume we have not installed LLVM/Clang yet<span style="color:green"><b>\*</b></span>. Instead of installing from the distro packages (apt install & co), which are usually too old for what is required here, we are going to use a very convenient [and official](https://apt.llvm.org/) script for it:

```
# this script requires apt. Download and install a release build otherwise!
wget https://apt.llvm.org/llvm.sh
chmod +x llvm.sh
# don't be a Scrooge and get 'all' packages
sudo ./llvm.sh all
```

This will install the latest released version, which at the moment of writing is *LLVM/Clang-18*.

Note that if you run `clang --version`, you will get an error because the script did not create generic symlinks like `sudo apt install clang` would have done. It simply installs an LLVM/Clang version by adding the required repository and calling `apt`, and it is up to you to decide which one to use. But you may want to have generic names of the tools, unless you are willing to tell every program what version it should use (e.g. clang-format). For example, you could build the kernel with LLVM-18 like this: `make LLVM=-18` instead of `make LLVM=1`.

There are multiple options to get generic names like creating symlinks under /usr/bin, or using the `update-alternatives` command. But you will have to do that for multiple binaries, and you could forget some of them. In my opinion the easiest solution is prepending /lib/llvm-18/bin to `$PATH`, because the generic names are already included, and I can choose a different version by editing 1-2 digits.

```
echo 'export PATH=/lib/llvm-18/bin:$PATH' >> ~/.bashrc
source ~/.bashrc
clang --version
Ubuntu clang version 18.1.8 (++20240615103833+3b5b5c1ec4a3-1~exp1~20240615223845.150)
Target: x86_64-pc-linux-gnu
Thread model: posix
InstalledDir: /lib/llvm-18/bin
```

---

<span style="font-size:0.85em;"><span style="color:green"><b>\*</b></span>If you do have LLVM/Clang installed on your machine, prepending the path to the desired version will still work. `update-alternatives` might also make sense if later (and often) you want to switch to a different version quickly, and without editing any file. The advantage of that is that it will also update the links to the man pages: `man clang` instead of `man clang-18`. Not a great deal, but maybe you care about that. Personally, an updated `$PATH` is all I need.</span>

---

#### 1.2. Rust

Once again, let's get the latest version with an official command, this time from [rustup.rs](https://rustup.rs/):

```
curl --proto '=https' --tlsv1.2 -sSf https://sh.rustup.rs | sh
```

You will be prompted with the following menu:

<figure>
    <img src="/images/posts/2024-06-22-rust-in-linux-kernel-configuration/install-rust.webp"
         alt="Install Rust">
    <figcaption><i>Keep it simple: Enter.</i></figcaption>
</figure>

Everything should work fine, and you will be notified at the end:

<figure>
    <img src="/images/posts/2024-06-22-rust-in-linux-kernel-configuration/rust-is-installed.webp"
         alt="Rust is installed. Great!">
    <figcaption><i>Again, keep it simple: restart the shell.</i></figcaption>
</figure>

This script has installed **rustup**, **rustc** and **cargo**.

#### 1.3. bindgen

Apparently, `bindgen` is used to generate the bindings to the C code in the kernel. Good to know, let's install it as stated in the documentation:

```
cargo install --locked --version $(scripts/min-tool-version.sh bindgen) bindgen-cli
```

As long as the `min-tool-version.sh` is up-to-date (it should), we will get the right version. In my case `bindgen-cli v0.65.1` was installed.

#### 1.4. New worktree

We are going to add a new worktree to isolate everything we will do for Rust. This is not a hard requirement, but don't forget that Rust is still experimental, and you probably don't want to add Rust by default to your kernel builds.
Let's add the worktree and move to its root directory:

```
# use the name you prefer instead of rust-playground
git worktree add rust-playground && cd $_
```

Now we are ready to configure the tools and the kernel. It didn't hurt so far, did it?

### 2. Configuration

First, we are going to check if we meet the requirements to use Rust within the kernel:

```
make LLVM=1 rustavailable

***
*** Rust compiler 'rustc' is too new. This may or may not work.
***   Your version:     1.79.0
***   Expected version: 1.78.0
***
***
*** Source code for the 'core' standard library could not be found
*** at '/home/jc/.rustup/toolchains/stable-x86_64-unknown-linux-gnu/lib/rustlib/src/rust/library/core/src/lib.rs'.
***
***
*** Please see Documentation/rust/quick-start.rst for details
*** on how to set up the Rust support.
***
```

Don't panic, that was expected. We are using a *too new* version of the Rust compiler, and we are missing the `rust-src` component.

A "may or may not work" sounds risky, so let's set the expected version with the following command, which **only affects the working directory**, not the version you will get for any other project:

```
rustup override set $(scripts/min-tool-version.sh rustc)
```

What is that `rust-src` component anyway, and how do I know we need that? I didn't make that up, it's in the official documentation:

```
The Rust standard library source is required because the build system will cross-compile core and alloc.
If rustup is being used, run:

rustup component add rust-src
```

Alright, let's do that:


```
rustup component add rust-src
info: downloading component 'rust-src'
info: installing component 'rust-src'
```

At the moment of writing, we don't have to add any other component (rustfmt, clippy, etc. were automatically installed).

And now, let's try again:

```
# consider exporing LLVM=1 for the whole session
make LLVM=1 rustavailable
Rust is available!
```

Great! Are we done? Almost. We have to enable Rust support, and we are good to go:

```
make LLVM=1 menuconfig
```

Select **General setup --->** (first entry in the main menu), and then **Rust support**:
<figure>
    <img src="/images/posts/2024-06-22-rust-in-linux-kernel-configuration/select-rust.webp"
         alt="select Rust">
</figure>

**&lt;<span style="color:red">S</span>ave&gt;**, **&lt;<span style="color:red">E</span>xit&gt;**, <u><b>Enjoy</b></u>.

### 3. Build kernel and Rust documentation

We are done with the *heavy* work. To compile the kernel, simply run:

```
# consider using -jN (N = $(nproc), 8, etc.)
make LLVM=1
# ...
Kernel: arch/x86/boot/bzImage is ready  (#1)
```

We can also build the Rust documentation with a single command:

```
make LLVM=1 rustdoc
```

The generated documentation provides a lot of information about the supported crates, and it can be opened with your web browser:

```
xdg-open Documentation/output/rust/rustdoc/kernel/index.html
```

<figure>
    <img src="/images/posts/2024-06-22-rust-in-linux-kernel-configuration/rustdoc.webp"
         alt="rustdoc">
    <figcaption><i>A lot of valuable information, don't ignore it!</i></figcaption>
</figure>

### 4. Start playing around

It is up to you to decide what comes next. I can only recommend you to read the [official documentation](https://www.kernel.org/doc/html/next/rust/index.html) that I already mentioned, the `rustdoc` we just generated, and take a look at the existing examples under `samples/rust`.

Just a few tips from the official documentation for the sake of completeness before we call it a day:
 - The Rust support code can be found under `rust/`
 - To add sample modules: `Kernel hacking -> Sample kernel code -> Rust samples`
 - Additional configuration (not much yet, though): `Kernel hacking -> Rust hacking`
 - If you use the `rust-analyzer` language server, you probably want to generate the `rust-project.json` configuration file: `make LLVM=1 rust-analyzer`

I still don't know what I am going to do with Rust, but as I like learning in depth before taking action, I will be busy with the existing code for a while. Nonetheless, if you have something in mind where we could collaborate, I will be pleased to learn about your projects.

<br/><br/>
Enjoy and share knowledge!
