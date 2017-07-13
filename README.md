# eeic2017-i1i2i3 (ping電話)

[![Build Status](https://travis-ci.org/hakatashi/eeic2017-i1i2i3.svg?branch=master)](https://travis-ci.org/hakatashi/eeic2017-i1i2i3)

2017年度EEIC 電気電子情報実験演習 I実験課題 (および最終課題発表)

電気系実験のI実験 (情報) で書いたプログラムを保管するためのリポジトリです。`08`ディレクトリには最終課題である「電話プログラムの任意機能実装」のコードが入っています。

## スライド

[![](http://i.imgur.com/5NgvPx7.png)](http://slides.com/hakatashi/ping-phone)

## ビルド

```
sudo apt-get install libopus-dev
cd 08
make
```

## 使い方

### クライアント

```
rec -t raw -b 16 -c 1 -e s -r 24000 - \
  | sudo bin/ping_phone <host_address> \
  | play -t raw -b 16 -c 1 -e s -r 24000
```

### ホスト

```
rec -t raw -b 16 -c 1 -e s -r 24000 - \
  | sudo bin/ping_phone \
  | play -t raw -b 16 -c 1 -e s -r 24000
```
