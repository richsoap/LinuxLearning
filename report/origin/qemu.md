<h1 style="text-align:center"> Linuxʵ�鱨�棺qemu��buildrootʹ��
</h1>
<div class="auther" style="text-align:center"> ������ </div>
<div class="date" style="text-align:center">2017.8.8</div>

## ϵͳ����
OS:Linux-4.10.0-19-generic
## ��������
### ��װqemu��buildroot
#### qemu��װ
```
sudo apt-get install qemu
```
#### buildroot���������װ
buildroot��Ҫ�õ����������������Ҫ��װ�Ķ������ǲ��ٵġ�
```shell
sudo apt-get install bison
sudo apt-get install flex
sudo apt-get install texinfo
sudo apt-get install hgsubversion
sudo apt-get install whois
sudo apt-get install autoconf
sudo apt-get install wget
sudo apt-get install patch
sudo apt-get install zlib
sudo apt-get install dos2unix
sudo apt-get install gawk 
```
#### buildroot���尲װ
������<a href="https://buildroot.org/download.html">https://buildroot.org/download.html</a>������buildroot�İ�װ�����˴�ѡ�����"buildroot-2017.05.2.tar.gz"��֮��ʹ������
```
tar -xvzf buildroot-2017.05.2.tar.gz
```
��ѹ�õ�buildroot���塣
### �ں˼��ļ�ϵͳ������
1. ��buildrootĿ¼�£�ʹ��
```
make list-defconfigs"
```
����г�buildroot�Ѿ����úõ������б�
2. ʹ��
```shell
make qemu_x86_64_defconfig
```
�����qemu_x86_64_defconfig��Ϊbuildroot�������ļ�
3. ��Ȼֻ��һ�������õ�ϵͳ��������Ϊroot�û����������룬ʹ��
```
make menuconfig
```
��������ѡ��˵���ͼ�ν��棬��"////////"����������������Ҫ������
4. ʹ��
```
make -j8
```
�����ں��Լ���Ӧ���ļ�ϵͳ���˴�ʹ��-j8ѡ��ʹ��8�̱߳�����߱����ٶ�
5. ���ɺõ��ļ���"output/images/"Ŀ¼�£�����bzImageΪ�ں��ļ���rootfs.ext2Ϊext2��ʽ���ļ�ϵͳ
### �����ں˺��ļ�ϵͳ
ʹ��
```
qemu-system-x86_64
```
�������ϵͳ����Ӧ��ѡ������
<ul>
<li>-kernel ��Ҫ���ص��ں�����λ�� �˴�Ϊoutput/images/bzImage</li>
<li>-drive ��Ҫ���ص������� �˴���Ϊ���ǵ�ext2�ļ�ϵͳ������Ҫ����format˵���ļ�ϵͳ��ʽ��if˵��/////////</li>
<li>-M �����ں˵�����ƽ̨ �˴�Ϊpc</li>
<li>-m ƽ̨���ڴ��С����λΪM �˴�Ϊ5120</li>
<li>-smp ƽ̨�ĺ�����Ŀ �˴�Ϊ4</li>
<li>-append һЩ���ӵ�����˴���/dev/vda����Ϊroot</li>
</ul>
���ϣ�ʹ��qemu����Linuxϵͳ����������Ϊ
```
qemu-system-x86_64 -m 5120 -smp 4 -M pc -kernel output/images/bzImage -drive file=output/images/rootfs.ext2,if=virtio,format=raw -append root=/dev/vda
```

### ����ϵͳ��鿴�汾��
�������룬����ϵͳ��ʹ��
```
uname -a
```
����鿴����ϵͳ�������汾��Ϊ"////"����ɻ�����Linuxϵͳ����������

## ����������
��buildrootĿ¼�£�ʹ��"make menuconfig"�����ȱ�ٿ�"ncurses"����ֹ��ʹ��
```
sudo apt-get install libncurses5-dev
```
���װ��Ӧ�Ŀ⼴�ɽ�����⡣

