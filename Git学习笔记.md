## 前言

本文档是我的关于廖雪峰Git教程的学习笔记。

[廖雪峰的官方网站]: https://www.liaoxuefeng.com/

## 1 时空穿梭

### 创建仓库

在当前目录初始化仓库（repository）。当前目录下会出现一个名为`.git`的隐藏文件夹。

```shell
git init
```

将文件\<file\>添加到暂存区

```shell
git add <file>
```

将暂存区的内容提交到仓库，”content“是本次提交的说明。

```shell
git commit -m "content"
```

查看仓库当前的状态，例如哪些文件被修改过、合并冲突

```shell
git status
```

查看文件\<file\>具体被改动了哪些内容。

```shell
git diff <file>
```

### 版本回退

显示提交日志

```shell
git log
```

回退到上一次提交，HEAD指向当前commit的版本，HEAD^指向上一次，HEAD^^指向上一次的上一次，依此类推。或者，也可以不用HEAD，而直接使用想要回退到的提交的ID。

```shell
git reset --hard HEAD^
```

显示历史上的每一次命令。这样，即使已经回退到了以前的某个版本，也可以通过这些历史记录，重新回到较新的版本（重返未来）。

```shell
git reflog
```

### 工作区与暂存区

`.git`目录所在的目录为工作区，`.git`目录本身为版本库。版本库里存了很多东西，其中最重要的就是称为stage（或者叫index）的暂存区，还有Git自动创建的第一个分支master，以及指向master的一个指针叫HEAD。

前面把文件往Git版本库里添加的时候，分两步执行。第一步是用`git add`把文件添加进去，实际上就是把文件修改添加到暂存区；第二步是用`git commit`提交更改，实际上就是把暂存区的所有内容提交到当前分支。因为创建Git版本库时，Git自动创建了唯一一个master分支，所以现在`git commit`就是往master分支上提交更改。可以简单理解为，需要提交的文件修改通通放到暂存区，然后，一次性提交暂存区的所有修改。

![git-repo](./images/0.jfif)

### 撤销修改

丢弃对文\<file\>在工作区的修改。也就是说，将工作退回到与暂存区一致的状态。

```shell
git checkout --<file>
```

丢弃对文件\<file\>在暂存区的修改。也就是说，将暂存区退回到与HEAD所指向的分支（在这里就是master分支）一致的状态。

```shell
git reset HEAD <file>
```

### 删除文件

从仓库中删除文件。先`git rm <file>`，然后`git commit`。

```shell
git rm <file>
```

如果误将工作区中的\<file\>删除，可以使用此命令从仓库中恢复文件\<file\>到工作区，前提是\<file\>之前提交过。

```shell
git checkout --<file>
```

## 2 远程仓库

创建SSH密钥，生成的密钥位于用户主目录下的`.ssh`目录下。其中，`id_rsa`是私钥，`id_rsa.pub`是公钥。

然后在GitHub的帐户设置中添加SSH公钥。

```shell
ssh-keygen -t rsa -C "youremail@example.com"
```

将本地仓库和远程仓库关联。origin是远程仓库的名字，也可以改成别的，不过origin是默认的叫法。

```shell
git remote add origin git@github.com:<yourname>/<repositoryname>.git
```

将本地的master分支推送到远程仓库origin

```shell
git push origin master
```

查看远程仓库信息

```shell
git remote -v
```

解除本地仓库和远程仓库的关联。

```shell
git remote rm <name>
```

克隆远程仓库到本地

```shell
git clone git@github.com:<yourname>/<repositoryname>.git
```

## 3 分支管理

### 创建和合并分支

查看分支

```shell
git branch
```

创建分支，分支名为\<name\>

```shell
git branch <name>
```

切换分支，以上两条命令任一即可。

```shell
git switch <name>
git checkout <name>
```

将分支\<name\>合并到当前分支

```shell
git merge <name>
```

删除分支

```shell
git branch -d <name>
```

创建并切换分支，以上两条命令任一即可。

```shell
git switch -c <name>
git checkout -b <name>
```

查看分支合并图

```shell
git log --graph
```

### 合并冲突

如果出现合并冲突，需要手动解决冲突。解决冲突就是把Git合并失败的文件手动编辑为我们希望的内容，再提交。

### 分支策略

在合并分支的时候，使用参数`--no-ff`表示禁用Fast Forward的合并模式，Git就会在合并时生成一个新的提交，这样，从分支历史上就可以看出分支信息。

```shell
git merge --no-ff -m "content" <name>
```

而在Fast Forward的模式下，合并分支意味着只是将一个分支的指针指向另一个。合并分支时，加上`--no-ff`参数就可以用普通模式合并，合并后的历史有分支，能看出来曾经做过合并，而Fast Forward合并就看不出来曾经做过合并。

### Bug分支

临时保存工作区现场（没有提交，只是临时保存）

```shell
git stash
```

查看保存的工作现场

```shell
git stash list
```

切换回本分支后，若想要恢复从前的工作现场，有两种方法。

1. ```shell
   git stash apply <name>
   ```

   然后，删除该stash内容

   ```shell
   git stash drop <name>
   ```

2. 恢复的同时删除栈顶stash内容
   
   ```shell
   git stash pop
   ```
   

将\<commit\>应用到当前分支，也就是说，对当前分支也进行一次和\<commit\>一模一样的修改。不过这次修改虽然和\<commit\>修改的内容一样，却仍是两次提交，即有着不同的ID。

```shell
git cherry-pick <commit id>
```

### Feature分支

上文提到的此命令用于删除分支。但是如果某分支还没有被合并过就要被销毁，那么执行此命令时，Git会提示用户此分支还没有被合并，删除失败。

```shell
git branch -d <name>
```

此命令将强制删除分支。

```shell
git branch -D <name>
```

### 多人协作

如果多人对同一远程仓库push，那么他们push的内容可能存在冲突，导致后来push的人push失败。因此需要使用`git pull`命令将最新的提交抓取到本地，在本地解决冲突之后，再push。

```shell
git pull
```

当然，再pull之前需要将本地分支\<name1\>和远程仓库的分支\<name0\>关联起来，否则会pull失败。

```shell
git branch --set-upstream-to=origin/<name0> <name1>
```

### Rebase

rebase操作可以把本地未push的分叉提交历史整理成直线；

```shell
git rebase
```

rebase的目的是使得我们在查看历史提交的变化时更容易，因为分叉的提交需要三方对比。

## 4 标签管理

一个标签对应这一次提交，但是提交的id很长很复杂，

查看所有标签

```shell
git tag
```

在当前分支最新提交上创建名为\<tagname\>的标签

```shell
git tag <tagname>
```

在当前分支的\<commit\>提交上创建名为\<tagname\>的标签

```sh
git tag <tagname> <commit>
```

查看标签信息

```sh
git show <tagname>
```

在当前分支的\<commit\>提交上创建名为\<tagname\>的标签，标签说明为”blablabla...“

```sh
git tag -a <tagname> -m "blablabla..." <commit>
```

删除标签

```sh
git tag -d <tagname>
```

将标签推送到远程

```sh
git push origin <tagname>
```

将本地全部标签推送到远程

```sh
git push origin --tags
```

将标签从远程删除

```sh
git tag -d <tagname>
git push origin :refs/tags/<tagname>
```

## 5 其他

在Git仓库目录下创建文件`.gitignore`，然后把要忽略的文件名填进去，然后提交`.gitignore`，Git就会自动忽略这些文件。比如编译的中间文件、可执行文件等，没有必要提交到Git，因此需要忽略。

不需要从头写`.gitignore`文件，GitHub已经为我们准备了各种配置文件，只需要组合一下就可以使用了。所有配置文件可以直接在线浏览：[](https://github.com/github/gitignore)

```sh
git add -f <name>
```

在文件\<name\>被`.gitignore`包含时，可以使用参数`-f`强制添加\<name\>文件。

也可以在`.gitignore`中添加例外规则将指定文件排除在`.gitignore`之外。把指定文件排除在`.gitignore`规则外的写法就是`!`+文件名，所以，只需把例外文件添加进去即可。
