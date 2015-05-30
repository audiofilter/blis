### Obtaining BLIS source code ###

We have migrated the [BLIS source code repository](https://github.com/flame/blis) to [github](https://github.com/).

If you are an end-user and just looking to download a copy of BLIS, use the command:

```
  git clone https://github.com/flame/blis.git
```

If you are a developer, please visit the [repository page for BLIS](https://github.com/flame/blis) on github. We recommend that you:
  1. [Create](https://github.com/) a github account, if you don't already have one.
  1. [Set up your account](https://help.github.com/set-up-git-redirect), if desired, so that you don't have to repeated enter your password. This is most easily done (in my opinion) via SSH keys.
  1. [Fork](https://help.github.com/fork-a-repo) (clone) the [BLIS repository](https://github.com/flame/blis) so that you have your own remote copy of BLIS on github. Then, you will be able to `git clone` from your remote repository (to your local machine), make modifications, and then `git push` those changes back.
  1. When you are ready to submit changes back to me, use the github web interface to propose "pull requests", which will allow me to review your suggested changes before accepting and merging.

### Recycling/converting/migrating an old BLIS clone ###

If you are a developer **and** you have a clone of BLIS on your local machine that you would like to convert for use with github, follow these steps:

  1. Set up your github account. (Steps 1 and 2 above.)
  1. Clone/fork BLIS from its [new home](https://github.com/flame/blis) at github. (Step 3 above.)
  1. On your development machine, go to the directory where your clone of BLIS resides:
```
  $ cd /path/to/blis
```
  1. Execute the following to change the remote URL associated with your local repository:
```
  # Note I use SSH access syntax here, not HTTPS.
  git remote set-url origin git@github.com:<your-github-username>/blis.git  
```
  1. Pull in the latest changes to your local repository:
```
  $ git pull
```
  1. Push back to your remote repository:
```
  $ git push origin
```

At this point, your BLIS repository should now be set up to track your new remote repository hosted at github.