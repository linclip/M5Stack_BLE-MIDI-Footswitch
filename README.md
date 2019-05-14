# M5Stack BLE-MIDI Footswitch

M5Stack Wireless Foot switch, Bluetooth MIDI
M5Stackで、MIDI over Bluetooth LE (BLE-MIDI) 接続のフットスイッチを作成可能にします。

## DEMO VIDEO

[M5Stack Wireless Foot switch, Bluetooth MIDI (MIDI over Bluetooth)](https://www.youtube.com/watch?v=CWVB-LN9PEs)


## 対応デバイス

M5Stackシリーズ + DIYフットスイッチ

フットスイッチがなくても、本体のボタンだけで動作を試すことができます。

## 機能

本体のA、B、CボタンおよびGPIO（初期設定では21, 22）につないだフットスイッチのON/OFFを、MIDI信号のノートON/OFFまおよびコントロールチェンジとして、BLE-MIDIで送信します。

送信する信号は、コードを参照してください。

## フットスイッチの作成

市販のフットスイッチをM5Stackに接続できるようにする工作が必要になります。

2スイッチのフットスイッチの場合は、TRSのTipとRingをGPIOの21、22へ接続。SleeveをGNDに接続。

1スイッチの場合は、TSのTipをGPIO21、SleeveをGNDへ。もう1つのスイッチはTSのTipをGPIO22、SleeveをGNDへ。

GPIOの番号は環境などに合わせて適当に。

## 動作環境

iOSおよびWindows 10で動作確認済み。

Windows 10では「設定」でペアリング後、BLE-MIDI対応のUWPアプリ「MIDIberry」を使用してください。
https://t.co/vgX8qsA5UU

いったんM5Stackの電源を切ると、BLEの接続が切れてしまいます。その差異は、MIDIberryをいったん終了、再度起動してください（再ペアリングの必要はありません）。

## インストール

SDUpdater /  LovyanLauncher 対応。

バイナリのダウンロード [Release](https://github.com/linclip/M5Stack_BLE-MIDI-Footswitch/releases)

