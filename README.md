# 统计行数

## 运行
```bash
./main 
请输入要统计的文件后缀(中间用,隔开)：.vim, .vimrc, .snippets
请输入要忽略的文件夹名(使用,隔开，如果没有则留空,默认读取ignore_count.txt文件):
```

该程序会统计当前目录下所有文件夹中指定后缀的文件的行数，忽略ignore_count.txt中的文件夹或文件。

忽略文件中空白行

## ignore_count.txt填写格式
```
autoload
colors
.gitignore
pack
plugged
swap
.git
ignore_count.txt
undodir
README.md
undodir
```
不论文件还是文件夹，都是一行一个，不需要加路径`./`，只需要写文件名或文件夹名即可。
