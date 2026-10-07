# AGENTS.md

## Purpose

This repository contains the source for **hackerbikepacker.com**, my personal website and blog.

When creating a new article, write it as if **I wrote it myself**. The goal is not to produce generic polished blog content, but to reproduce my existing writing style, tone, structure, vocabulary, level of detail, and way of telling stories as closely as possible.

The existing articles in `_posts/` are the primary source of truth for how I write.

## Author voice

Before writing a new article:

1. Read several existing posts from `_posts/`, especially posts that are similar in subject, length, or format to the requested article.
2. Infer my writing style from those posts.
3. Reuse the same general structure and conventions.
4. Write in first person when the article is about my own experiences.
5. Keep the writing natural and personal. It should sound like a real post written by me, not like AI-generated travel writing.
6. Do not make the text unnecessarily polished, literary, dramatic, enthusiastic, or promotional.
7. Do not add generic introductions, conclusions, lessons, or reflections unless they fit the style of the existing posts.
8. Preserve the amount of detail and the balance between facts, observations, anecdotes, and personal opinions found in the existing articles.
9. Do not invent experiences, conversations, places, dates, facts, measurements, feelings, or opinions that I have not provided or that cannot reasonably be inferred from the conversation.
10. If information is missing and it matters to the article, ask me rather than making it up.

The article should feel like another article from the same author, not like an imitation of a stereotypical travel blog.

## Existing article structure

Use the existing `_posts/` articles to determine the exact Markdown and front matter conventions used by this site.

Do not introduce a new front matter format, Markdown convention, HTML structure, heading style, or other publishing convention unless the existing posts already use it.

When possible, follow the structure of the most similar existing articles.

## Creating posts

New articles must be created under:

`_posts/`

Use the date of the conversation as the post date. The current conversation date is the authoritative date for the filename and front matter unless I explicitly specify another publication date.

Follow the filename convention already used by the existing posts, including the date prefix and slug format.

Before creating the file, inspect existing filenames and front matter so that the new post matches the repository's conventions exactly.

Do not modify existing posts unless I explicitly ask you to.

## Images

Images used in new articles will be provided by me during the conversation.

Do not search for, download, generate, or invent images unless I explicitly ask you to.

Use the images I provide and follow the image/reference conventions already present in existing posts.

If an image is provided but its intended placement or filename is unclear, ask me rather than guessing when the choice could materially affect the article.

Do not invent image captions, locations, dates, or descriptions that are not supported by the conversation or the image context I provide.

## Editing and repository safety

You may:

- Read the repository.
- Read existing articles and other files needed to understand the site.
- Create new files under `_posts/`.
- Create or modify other files only when I explicitly ask you to.

You must **never**:

- Create a git commit.
- Amend a commit.
- Rewrite git history.
- Rebase.
- Reset or force-update branches.
- Push to a remote.
- Delete or rewrite existing history.
- Use commands that modify git history.

Creating the article file is the final repository modification required for a normal article-writing task. Leave the working tree with the new post uncommitted so I can review it myself.

## Do not over-edit

When I give you text that I want incorporated into an article, preserve my wording where possible.

Do not rewrite everything just to make it sound more professional or fluent. Small grammatical corrections are fine, but the final text should retain my natural voice.

If I explicitly provide wording, facts, or a particular way of expressing something, treat that as authoritative.

## Conversation context

The conversation itself is part of the source material for the article.

Use information I provide during the conversation, including:

- Experiences and events I describe.
- Places and dates I mention.
- Personal observations.
- Technical or travel details.
- Specific wording I want to keep.
- Images I provide.
- Corrections I make during the drafting process.

Later corrections override earlier statements.

Do not claim to know something about the experience merely because it would be typical for the location or situation.

## Language

Use the language I request for the article.

If I do not specify a language, inspect the existing posts and use the predominant language of the relevant articles.

Do not translate names of places, people, organisations, trails, roads, or other proper nouns unless that is already the convention of the site.

## Final checks

Before considering the article finished:

- Confirm that the file is in `_posts/`.
- Confirm that its filename follows the existing convention.
- Confirm that the date corresponds to the conversation date unless I specified otherwise.
- Confirm that the front matter matches existing posts.
- Confirm that images reference only images I provided or explicitly approved.
- Check that the article reads naturally as something I would have written.
- Check for invented facts or details.
- Check that no existing article was unintentionally modified.
- Do not commit anything.

The most important rule is simple:

**Write like me, using my existing articles as the reference, and do not invent things I did not tell you.**
