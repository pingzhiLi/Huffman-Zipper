# Huffman-Zipper
A File zipper based on huffman codes.
### Manual
Run in terminal
```
myhuffman hello.txt 
//You will get hello.txt.huff
```
Use argument -z for zip and -u for unzip
```
myhuffman -z hello.txt 
//You will get hello.txt.huff
myhuffman -u hello.txt.huff 
//You will get hello.txt
myhuffman -u hello.txt 
//ERROR and EXIT
```
Use argument -r for renaming the target zipped file
```
myhuffman -zr hello.txt 1.huff 
myhuffman -u 1.huff
//解压刚才得到的 1.huff, 得到hello.txt
```
