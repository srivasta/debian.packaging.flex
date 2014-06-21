---
layout: post
title: Fix failure to build from source
category: BugFix

excerpt: TeX 2014 requires an additional vuild dependency on cp-super-minimal

---

Build dependency fixes, no code change

Added a missing build depends on vm-super-minimal, reuired fro building
the pdf documentation, (Closes: #752151).

Move flex.pdf to the flex-doc package. This makes it possible for the
flex package's contents to not change if texinfo is not installed,
e.g. in the stage1 build profile.  Thanks to Peter Pentchev
<roam@ringlet.net>

Move the flex-doc build dependencies to B-D-I. Move the TeX Live
dependencies to Build-Depends-Indep and only build the HTML and PDF
documentation if actually requested. This breaks a circular build
dependency by not requiring texlive for the build of the arch-dependent
flex binary packages.  (Closes: #749344).
See [detils here.](https://github.com/srivasta/debian.packaging.flex/commit/d7b859a063a26d2b72592ac238f7915d8ff797b3)
