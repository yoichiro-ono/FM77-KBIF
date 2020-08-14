# FM-77用キーボードインターフェース

FM-77用キーボードインターフェースの回路図とプログラムです。

FM-77とはD-SUB24ピンのコネクタで接続します。

外部に20MHzのクリスタルを接続していますが、内蔵8MHzでも動作すると思います。

**割当てキー**
-[Caps Lock]⇒[CAP]
-[ALT左右]⇒[GRAPH]
-[カナ]⇒[カナ]
-[F12]⇒[BREAK]
-[End]⇒[EL]
-[Home]⇒[HOME]
-[Insert]⇒[INS]
-[Delete]⇒[DEL]
-[Page Up]⇒[CLS]
-[Page Down]⇒[DUP]

**未割当キー**
-[F11]
-[漢字]
-[変換]
-[無変換]
-[Print Screen]
-[Scroll Lock]
-[Pause]
-[Num Lock]

**LEDランプ割当**
-Caps Lock：CAP LED
-Num Lock:カナ LED
-Scroll Lock:INS LED

## 参考にしたもの

KUNINET BLOG
[KZ80マイコンを 偽MSX1へ〜PS/2キーボードMSXアダプタ(2)](https://kuninet.org/2020/03/21/kz80%e3%83%9e%e3%82%a4%e3%82%b3%e3%83%b3%e3%82%92-%e5%81%bdmsx1%e3%81%b8%e3%80%9cps-2%e3%82%ad%e3%83%bc%e3%83%9c%e3%83%bc%e3%83%89msx%e3%82%a2%e3%83%80%e3%83%97%e3%82%bf2/)

odaman68000さん
[FM-77 Keyboard Encoder for ND80](https://github.com/odaman68000/FM7_Keyboard_Encoder_forND80)


