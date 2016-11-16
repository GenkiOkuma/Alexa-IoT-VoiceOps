![](../../blob/images/top.jpg)

## このプロジェクトについて

アマゾンの自然言語理解アシスタントサービス「Alexa」の公開されている開発環境を使ってIoTをやってみました。  
ボイスコマンドからLINEのメッセージを送ったり、イルミネーションを制御するやり方と[ソースコード](../../blob/master/sketch_alexa_neopixel.ino)をこちらで公開します。  
費用もあまりかからず、面白いので是非やってみてください。  

---

## 関連資料

[デモ動画]()もご覧ください。  
[プレゼン資料]()もご覧ください。

---

## ライセンス等に関する重要事項

* This sample app and associated projects are provided for **prototyping purposes only**. Included client code, including earcons (start listening, stop listening, and error sounds) **SHALL NOT** be used in production. For implementation and design guidance for commercial products, please see [Designing for AVS](https://developer.amazon.com/public/solutions/alexa/alexa-voice-service/content/designing-for-the-alexa-voice-service).

* Alexa Voice Service [Terms & Agreements](https://developer.amazon.com/public/solutions/alexa/alexa-voice-service/support/terms-and-agreements)

* **Usage of Sensory & KITT.AI wake word engines**: The wake word engines included with this project (Sensory and KITT.AI) are intended to be used for **prototyping purposes only**. If you are building a commercial product with either solution, please use the contact information below to enquire about commercial licensing -
  * [Contact Sensory](http://www.sensory.com/support/contact/us-sales/) for information on TrulyHandsFree licensing.
  * [Contact KITT.AI](mailto:snowboy@kitt.ai) for information on SnowBoy licensing.

---

## もくじ

このプロジェクトでやってみたことは、下記の4点です。

1. ラズベリーパイで環境構築
2. AWS Lambda で追加スキル開発
3. IFTTTを使ってボイスコマンドからLINEメッセージを送信
4. Adafruit IO を使ってボイスコマンドからイルミネーション制御

---

## 1. ラズベリーパイで環境構築

[Alexa公式の説明](https://github.com/alexa/alexa-avs-sample-app/wiki/Raspberry-Pi) をご参照ください。
英語ですが、ステップバイステップで丁寧に書かれているので迷うことはないと思います。

---

## 2. AWS Lambda で追加スキル開発

まずは、公開されているスキルを使ってみましょう。[自分のアカウント設定](http://alexa.amazon.com/spa/index.html) の「Skills」でいろいろなスキルを追加できます。オススメは、Zoo Keeper、Trivia Japan、Sushi Facts です。  
次に、自分でスキルを作ってみましょう。これにはAWSのアカウントが必要です。個人で使う程度ではよほどのことが無い限り無料で使えますので、この機会にアカウントを作ってみましょう。スキル開発手順の詳細は、[スキルテンプレート](https://developer.amazon.com/public/solutions/alexa/alexa-skills-kit/content/alexa-skills-developer-training)をご参照ください。  
Trivia Skill template というのが作りやすいと思います。Alexaがトナカイに関するマニアックな4択クイズを出題するというスキルで、コピペで作りながらスキルの仕組みが理解できます。  
好きなスピーチをAlexaにさせるなら、Fact Skill templateがオススメです。SDK等いくつかのファイルをzipにまとめてlambda にアップロードする必要がありますが、コンテンツを書き換える部分はシンプルになります。Lambdaに入るコードは、このプロジェクトにアップロードしている[fact_skill.js](../../blob/master/fact_skill.js)のようになります。※SDK等も入れる必要があるので、このコードだけでは動きません。

---

## 3. IFTTTを使ってボイスコマンドからLINEメッセージを送信

Alexaが[IFTTT](https://ifttt.com/)に対応しているので、LINE、ツイッター、Slackなど様々なアプリと連携できます。説明する必要が無いほど簡単に使えます。  
IFTTTでは、トリガーとアクションを設定しますが、Alexaはトリガーのみサポートしています。下記のように12種類あり、Say a specific phraseというのが合言葉を言うだけのシンプルで、駆動が早いトリガーです。
![](../../blob/images/ifttt_trigger2.jpg)

---

## 4. Adafruit IO を使ってボイスコマンドからイルミネーション制御

クリスマスに向けて、魔法の呪文でイルミネーションの色を変えようと思い、下記のような構成を検討しました。  
* Alexa Skills Kit：上記のようにAWSのLambdaにスキルを追加する構成ですが、テンプレをいじってできる感じではなかったので断念。[このサイト] (https://aaroneiche.com/2016/01/03/weekend-project-amazon-echo-led-control/)が近い感じでしたが、難しそう・・・。
* [Smart Home Skill API](https://developer.amazon.com/alexa/smart-home)：イルミネーション側でOAuth2.0に対応しなければいけないということで断念。OAuth2.0に対応するために、イルミネーション側にもそれなりのコンピュータリソースと費用が必要な印象を受けました。IoTで制御はセキュリティをちゃんと考えないといけないので大変ですね^^;
* [Adafruit IO](https://io.adafruit.com/):IFTTT経由でクラウド上のMQTTブローカーにデータを入れて、イルミネーション側に配信する仕組み。これなら安く簡単にできそうだと思ってやってみたら、動作させることができました。そのやり方とソースコードを説明します。

まず、[こちらのAdafruitのガイド](https://learn.adafruit.com/remote-control-with-the-huzzah-plus-adafruit-io)を参考にしました。  
### 必要機材

* 上記のラズパイAlexa
* [ESPr Developer開発ボード](https://www.switch-science.com/catalog/2500/) （半田不要の[ソケット付き](https://www.switch-science.com/catalog/2652/)もあります。）
* NeoPixel [Ring-12連フルカラーシリアルLED](https://www.switch-science.com/catalog/1593/) （違うタイプでもコードを書き換えれば対応可能です。）

### 手順1：Adafruit IOの準備

[Adafruit IO](https://io.adafruit.com/)でアカウントを作り、データを入れる箱「Feed」を作成します。  
Your Feed から、CREATE FEEDをクリック
![](../../blob/images/adafruit_io_1.jpg)

Feedの名前と説明を適当に入れます。

![](../../blob/images/adafruit_io_2.jpg)

VIEW AIO KEYSをクリックしてメモしておく。イルミネーション側がサブスクライブする時に必要になります。
※下記はサンプル画像なので、自分の環境でキーをメモしてください。
![](../../blob/images/adafruit_io_6.jpg)

ダッシュボードにボタンを作成します。MY DASHBOARDSからプラスマークのCreate a new blockをクリック。A gauge というところでCREATEをクリック。（A stream blockというのもログが見れるのでオススメです。後で追加してみてください。）
![](../../blob/images/adafruit_io_7.jpg)

先ほど作ったFeedを選択して、MIN VALUEを0、MAX VALUEを5に設定する。（0:オフ、1:赤、2:緑、3:青、4:虹、5:予備）
![](../../blob/images/adafruit_io_5.jpg)

### 手順2：Alexa側 IFTTTの設定

[IFTTT](https://ifttt.com/)にログインして、New Appletを作成します。  
トリガーにAlexaを選択して、初回はアカウント情報を入力。Say a specific phraseを選択して、「neo pixel red」などと入力。単語でも文章でもOKですが、小文字しか入らないので注意。  
アクションにAdafruitを選択して、初回はアカウント情報を入力。Send data to Adafruit IOを選択して、先ほど作成したFeedを選び、トリガーの言葉に対応する数値を入力。（0:オフ、1:赤、2:緑、3:青、4:虹、5:予備）そして、Create actionをクリック。
![](../../blob/images/ifttt_1.jpg)

できましたか？同様のアプレットを5種作ります。（0:オフ、1:赤、2:緑、3:青、4:虹）

![](../../blob/images/ifttt_2.jpg)

### 手順3：ESPr側 MQTTサブスクライブ

まずは、ESPr Developer開発ボードの環境をセットアップします。[こちらのサイト](https://www.mgo-tec.com/blog-entry-ss-wroom-howto01.html)がとても勉強になります。このサイトにある通り、[Arduino.cc](https://www.arduino.cc/en/Main/Software)のIDEをインストールする必要があり、Arduino.orgのIDEをインストールしてはダメです。こちらのサイトを参考にしてArduinoスケッチをESPr Developerに書き込んでスマホと通信できるくらいまでご確認ください。

Neo Pixelのセットアップは簡単です。VCC、GND、信号線の3本をESPrと繋げるだけです。ワニぐちクリップがあると便利です。本当は、[コンデンサと抵抗を入れないといけない](http://k183.bake-neko.net/densi/page6.html)のですが、めんどくさくなってやめました・・・。ESPrの3Vくらいだったらいいかな^^;（自己責任で）  
信号線はArduinoスケッチで定義するので何番でもOKですが、とりあえず2番にしてみましょう。
![](../../blob/images/ESPr_1.jpg)

Arduino.ccのIDEが準備できたら、スケッチ新規作成で[sketch_alexa_neopixel.ino](../../blob/master/sketch_alexa_neopixel.ino)のコードをコピペしてください。日本語でコメントしている部分は、自分の環境の値に書き換えてください。
![](../../blob/images/arduino_1.jpg)

このスケッチで使うライブラリをArduino IDEに追加します。スケッチ→ライブラリをインクルード→ライブラリを管理　を開いてください。
![](../../blob/images/arduino_2.jpg)

mqttで検索して、Adafruit MQTT Libraryの最新版をインストール。
![](../../blob/images/arduino_3.jpg)

neopixelで検索して、Adafruit NeoPixelの最新版をインストール。
![](../../blob/images/arduino_4.jpg)

ESP8266関連のライブラリは、ESPr Developer開発ボードのセットアップの段階で入っていると思います。  
それでは、スケッチをESPrに書き込んで、動作確認してみましょう。

### 手順4：動作確認

ESPr側を起動した状態で、シリアルモニタを開いてみましょう。ここでESPrのログをモニターすることができます。
FEEDには手動で値を入れられるので、ボイスコマンドの前に手入力で動作確認するのがお手軽です。FEEDのプラスボタンを押して値を入力してください。
![](../../blob/images/adafruit_io_8.jpg)

期待通りに動きましたか？原因不明ですがESPrのシリアルモニタを見てると、しょっちゅうPacket Dropを起こしてAdafruit IOからのデータを拾うことができません。時間を空けてみたりして繰り返しやってると反応してくれます。MQTTのQoS0みたいな状態になっていて、MQTTブローカー（ここではAdafruit IO）から1回しか情報が配信されず、MQTTサブスクライバー（ここではESPr）が受け取ったかどうかを見ていない、MQTTの機能であるWillやRetainが実装されていないということだと推測します。タダで使ってるし、贅沢は言えないですね。

---

