![](../../blob/images/top.jpg)

## このプロジェクトについて

アマゾンの自然言語理解アシスタントサービス「Alexa」の公開されている開発環境を使ってIoTをやってみました。  
ボイスコマンドからLINEのメッセージを送ったり、イルミネーションを制御するやり方とソースコードをこちらで公開します。  
費用もあまりかからず、面白いので是非やってみてください。  

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
好きなスピーチをAlexaにさせるなら、Fact Skill templateがオススメです。SDK等いくつかのファイルをzipにまとめてlambda にアップロードする必要がありますが、コンテンツを書き換える部分はシンプルになります。Lambdaに入るコードは[このように]()なります。

---

## 3. IFTTTを使ってボイスコマンドからLINEメッセージを送信

Alexaが[IFTTT](https://ifttt.com/)に対応しているので、LINE、ツイッター、Slackなど様々なアプリと連携できます。説明する必要が無いほど簡単に使えます。  
IFTTTでは、トリガーとアクションを設定しますが、Alexaはトリガーのみサポートしています。下記のように12種類あり、Say a specific phraseというのが合言葉を言うだけのシンプルで、駆動が早いトリガーです。
![](../../blob/images/ifttt_trigger.jpg)

---

## 4. Adafruit IO を使ってボイスコマンドからイルミネーション制御

クリスマスに向けて、魔法の呪文でイルミネーションの色を変えようと思い、下記のような構成を検討しました。  
* Alexa Skills Kit：上記のようにAWSのLambdaにスキルを追加する構成ですが、テンプレをいじってできる感じではなかったので断念。[このサイト] (https://aaroneiche.com/2016/01/03/weekend-project-amazon-echo-led-control/)が近い感じでしたが、難しそう・・・。
* [Smart Home Skill API](https://developer.amazon.com/alexa/smart-home)：イルミネーション側でOAuth2.0に対応しなければいけないということで断念。OAuth2.0に対応するために、イルミネーション側にもそれなりのコンピュータリソースと費用が必要な印象を受けました。
* [Adafruit IO](https://io.adafruit.com/):IFTTT経由でクラウド上のMQTTブローカーにデータを入れて、イルミネーション側に配信する仕組み。これなら安く簡単にできそうだと思ってやってみたら、動作させることができました。そのやり方とソースコードを説明します。

まず、[こちらのAdafruitのガイド](https://learn.adafruit.com/remote-control-with-the-huzzah-plus-adafruit-io)を参考にしました。  
# 必要機材

* 上記のラズパイAlexa
* [ESPr Developer開発ボード](https://www.switch-science.com/catalog/2500/) （半田不要の[ソケット付き](https://www.switch-science.com/catalog/2652/)もあります。）
* NeoPixel [Ring-12連フルカラーシリアルLED](https://www.switch-science.com/catalog/1593/) （違うタイプでもコードを書き換えれば対応可能です。）

# 手順1：Adafruit IOの準備

[Adafruit IO](https://io.adafruit.com/)でアカウントを作り、データを入れる箱「Feed」を作成します。  
Your Feed から、CREATE FEEDをクリック
![](../../blob/images/adafruit_io_1.jpg)
Feedの名前と説明を適当に入れる。
![](../../blob/images/adafruit_io_2.jpg)
VIEW AIO KEYSをクリックしてメモしておく。イルミネーション側がサブスクライブする時に必要になります。
![](../../blob/images/adafruit_io_3.jpg)
ダッシュボードにボタンを作成します。MY DASHBOARDSからプラスマークのCreate a new blockをクリック。A gauge というところでCREATEをクリック。
![](../../blob/images/adafruit_io_4.jpg)
先ほど作ったFeedを選択して、MIN VALUEを0、MAX VALUEを5に設定する。
![](../../blob/images/adafruit_io_5.jpg)

# 手順2：Alexa側 IFTTTの設定


# 手順3：ESPr側 MQTTサブスクライブ


---

## その他参考資料

動画、プレゼン資料

---
